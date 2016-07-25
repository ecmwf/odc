import re

PARAM_TYPE_COLUMN = 37

decls = [line for line in [l.strip() for l in open('odbql.cc').read().splitlines()]
         if line.find('odbql_') <> -1 
            and not line.startswith('//')
            and line.find('return') == -1
            and line.find('typedef') == -1]

def parseParam(p):
    p = p.strip()
    if p == 'void(*d)(void*)':
        return 'void(*)(void*)', 'd'
    param_name = re.findall(r"[\w']+", p)[-1]
    return p[:len(p) - len(param_name)].strip(), param_name.strip()
    

def parseDeclaration(decl):
    ret_and_fun_name = decl.split('(', 1)[0].strip()
    fun_name = re.findall(r"[\w']+", ret_and_fun_name)[-1]
    return_type = ret_and_fun_name[:-len(fun_name)].strip()

    params = decl.split('(',1)[1].strip().rsplit(')', 1)[0].strip()
    params = tuple([parseParam(p) for p in params.split(',')])
    return (decl, (return_type, fun_name, params))

def translate_type(t):
    if t == 'int':                  return 'integer(kind=C_INT),VALUE'
    if t == 'double':               return 'real(kind=C_DOUBLE),VALUE'
    if t == 'const char *':         return 'character(kind=C_CHAR),dimension(*)'
    if t == 'const char*':          return 'character(kind=C_CHAR),dimension(*)'
    if t == 'const unsigned char*': return 'character(kind=C_CHAR),dimension(*)' # TODO same as signed?
    if t == 'const unsigned char *': return 'character(kind=C_CHAR),dimension(*)' # TODO same as signed?
    if t == 'const char **':        return 'character(kind=C_CHAR),dimension(*)' # TODO
    if t == 'odbql*':               return 'type(C_PTR), VALUE'
    if t == 'odbql *':              return 'type(C_PTR), VALUE'
    if t == 'odbql **':             return 'real(kind=C_DOUBLE)' # TODO
    if t == 'odbql_stmt*':          return 'type(C_PTR), VALUE'
    if t == 'odbql_stmt *':         return 'type(C_PTR), VALUE'
    if t == 'odbql_stmt **':        return 'real(kind=C_DOUBLE)' # TODO
    if t == 'void(*)(void*)':       return 'real(kind=C_DOUBLE)' # TODO

    raise Exception("Don't know how to translate '" + t + "'")

def fortranParamTypeDeclaration(p):
    global PARAM_TYPE_COLUMN
    typ, parameter_name = p
    fortran_type = translate_type(typ)
    space = ' ' * (PARAM_TYPE_COLUMN - len(fortran_type))
    return  fortran_type + space + ':: ' + parameter_name


def generateWrapper(d, comment):
    global PARAM_TYPE_COLUMN 

    return_type, function_name, params = d

    parameter_declarations = [p for p in params
                                if not (p[1] == 'void' and len(p[0]) == 0)]
    parameter_declarations = '\n     '.join([fortranParamTypeDeclaration(p) 
                                        for p in parameter_declarations])

    parameter_list = ','.join([p[1] for p in params])
    if parameter_list: parameter_list = '(' + parameter_list + ')'

    space = ' ' * (PARAM_TYPE_COLUMN - len('use, intrinsic')) 

    fortran_return_type = translate_type(return_type)
    return_type_declaration = fortran_return_type + (' ' * (PARAM_TYPE_COLUMN - len(fortran_return_type))) + ":: " + function_name 

    template = """
!> %(comment)s

    function %(function_name)s %(parameter_list)s bind(C, "%(function_name)s")
     use, intrinsic%(space)s:: iso_c_binding
     %(parameter_declarations)s
     %(return_type_declaration)s
    end function %(function_name)s

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
    print s
    with open('odbql_binding.f90', 'w') as f: f.write(s)

