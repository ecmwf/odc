import re

PARAM_TYPE_COLUMN = 37

def formatParameter(typ, name):
    global PARAM_TYPE_COLUMN 
    space = ' ' * (PARAM_TYPE_COLUMN - len(typ)) 
    return typ + space + ':: ' + name

decls = [line for line in [l.strip() for l in open('odbql.cc').read().splitlines()]
         if line.find('odbql_') <> -1 
            and not line.startswith('//')
            and line.find('return') == -1
            and line.find('typedef') == -1]

def normalize_type(t): return re.sub(' [*]', '*', t)

def parseParam(p):
    p = p.strip()

    if p == 'void(*d)(void*)':
        return 'void(*)(void*)', 'd'

    param_name = re.findall(r"[\w']+", p)[-1]
    param_type = p[:len(p) - len(param_name)].strip()
    param_type = normalize_type(param_type)

    return param_type, param_name
    

def parseDeclaration(decl):
    ret_and_fun_name = decl.split('(', 1)[0].strip()
    fun_name = re.findall(r"[\w']+", ret_and_fun_name)[-1]
    return_type = ret_and_fun_name[:-len(fun_name)].strip()
    return_type = normalize_type(return_type)

    params = decl.split('(',1)[1].strip().rsplit(')', 1)[0].strip()
    params = list([parseParam(p) for p in params.split(',')])
    return (decl, (return_type, fun_name, params))

def translate_type_for_binding(t):
    #print 'translate_type_for_binding:', t
    if t == 'const char*':           return 'character(kind=C_CHAR),dimension(*)'
    if t == 'const char**':          return 'character(kind=C_CHAR),dimension(*)' # TODO
    if t == 'double':                return 'real(kind=C_DOUBLE)'
    if t == 'int':                   return 'integer(kind=C_INT)'
    if t == 'odbql*':                return 'type(C_PTR), VALUE'
    if t == 'odbql**':               return 'real(kind=C_DOUBLE)' # TODO
    if t == 'odbql_stmt*':           return 'type(C_PTR), VALUE'
    if t == 'odbql_stmt**':          return 'real(kind=C_DOUBLE)' # TODO
    if t == 'void(*)(void*)':        return 'real(kind=C_DOUBLE)' # TODO

    raise Exception("Don't know how to translate '" + t + "'")


def translate_type_for_binding_return(t):
    #print 'translate_type_for_binding_return:', t
    if t == 'const char*':           return 'type(C_PTR)'
    if t == 'const unsigned char*':  return 'type(C_PTR)'
    if t == 'int':                   return 'integer(kind=C_INT)'

    raise Exception("Don't know how to translate '" + t + "'")


def translate_type_for_fortran(t):
    #print 'translate_type_for_fortran:', t
    if t == 'const char*':           return 'character(len=*),intent(in)'
    if t == 'const char**':          return 'character(len=*),intent(out)'
    if t == 'double':                return 'real(kind=C_DOUBLE)'
    if t == 'int':                   return 'integer(kind=C_INT)'
    if t == 'odbql*':                return 'type(C_PTR), VALUE'
    if t == 'odbql**':               return 'type(C_PTR)'
    if t == 'odbql_stmt*':           return 'type(C_PTR), VALUE'
    if t == 'odbql_stmt**':          return 'type(C_PTR)'
    if t == 'void(*)(void*)':        return 'real(kind=C_DOUBLE)' # TODO

    raise Exception("Don't know how to translate '" + t + "'")


def translate_type_for_fortran_return(t):
    #print 'translate_type_for_fortran_return:', t
    if t == 'const char*':           return 'character(len=*),intent(out)'
    if t == 'const unsigned char*':  return 'character(len=*),intent(out)' # TODO: think about it
    if t == 'int':                   return 'integer(kind=C_INT)'

    raise Exception("Don't know how to translate '" + t + "'")

def fortranParamTypeDeclaration(p, translate_type = translate_type_for_binding):
    global PARAM_TYPE_COLUMN
    typ, parameter_name = p
    fortran_type = translate_type(typ)
    return formatParameter(fortran_type, parameter_name)

def generateWrapper(declaration, comment):

    procedure_keyword = 'function'

    return_type, function_name, params = declaration

    print 'generateWrapper: ', declaration, comment

    print 'params:', params

    output_parameter = function_name

    # filter out void from parameterless fucntions (subroutines?) e.g: const char * odbql_libversion(void)
    if len(params) == 1 and (len(params[0][0]) == 0 and params[0][1] == 'void'):
        params = []
    params = [p for p in params if not p[1] == 'void(*)(void*)']

    fortran_params = params[:]

    if return_type == 'const char*' or return_type == 'const unsigned char*':
        procedure_keyword = 'subroutine'
        output_parameter = 'return_value'
        fortran_params.append( (return_type, output_parameter) )

    parameter_list = ','.join([p[1] for p in params])
    parameter_list = '(' + parameter_list + ')'

    fortran_parameter_list = ','.join([p[1] for p in fortran_params])
    fortran_parameter_list = '(' + fortran_parameter_list + ')'

    binding_parameters_declarations = '\n     '.join([fortranParamTypeDeclaration(p) for p in params])
    binding_return_type_declaration = formatParameter(translate_type_for_binding_return(return_type), function_name + '_c')

    fortran_parameters_declarations = '\n     '.join([fortranParamTypeDeclaration(p, translate_type = translate_type_for_fortran) for p in params])
    fortran_return_type_declaration = formatParameter(translate_type_for_fortran_return(return_type), output_parameter)

    use_intrinsic = formatParameter('use, intrinsic', 'iso_c_binding')

    template = """
!> %(comment)s

    function %(function_name)s_c %(parameter_list)s bind(C, name="%(function_name)s")
     %(use_intrinsic)s
     %(binding_parameters_declarations)s
     %(binding_return_type_declaration)s
    end function %(function_name)s_c

    %(procedure_keyword)s %(function_name)s %(fortran_parameter_list)s 
     %(use_intrinsic)s
     %(fortran_parameters_declarations)s
     %(fortran_return_type_declaration)s

!>     %(function_name)s = %(function_name)s_c %(parameter_list)s

    end %(procedure_keyword)s %(function_name)s

    """ 
    return template % locals()

def generateWrappers(decls):
    s = """
module odbql_binding
  use iso_c_binding
  use, intrinsic :: iso_c_binding
  implicit none
interface
"""
    for original, ast in [parseDeclaration(decl) for decl in decls]: 
        s += generateWrapper(ast, original) + '\n'

    s += """
end interface
end module odbql_binding
"""
    return s

if __name__ == '__main__':
    s = generateWrappers(decls)
    with open('odbql_binding.f90', 'w') as f: 
        f.write(s)
