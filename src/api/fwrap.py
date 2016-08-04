#!/usr/bin/env python 

import re

PARAM_TYPE_COLUMN = 43

def formatParameter(typ, name):
    global PARAM_TYPE_COLUMN 
    space = ' ' * (PARAM_TYPE_COLUMN - len(typ)) 
    return typ + space + ':: ' + name

def declarations(source_cc = 'odbql.cc'):
    decls = [line for line in [l.strip() for l in open(source_cc).read().splitlines()]
         if line.find('odbql_') <> -1 
             and not line.startswith('//')
             and line.find('return') == -1
             and line.find('typedef') == -1]
    return decls

def constants(source_h = 'odbql.h'):

    lines = [line.strip() for line in open(source_h).read().splitlines()
            if line.startswith('#define ODBQL_') 
               and len(line.split()) > 2]

    defs = [line.split(None, 2)[1:] for line in lines]
    return defs

def translate_value_and_comment(value_and_possibly_comment):
    value = value_and_possibly_comment.split('/*')[0].strip()
    comment = ''

    if len(value_and_possibly_comment.split('/*')) > 1:
        comment = value_and_possibly_comment.split('/*')[1].split('*/')[0]

    if value.find('|') <> -1:
        comment = value + ' ' + comment
        l,r = [x.strip(' ()') for x in value.split('|')]
        i, shift = [x.strip() for x in r.split('<<')]
        value = 'IOR(%s, LSHIFT(%s,%s))' % (l, i, shift)
    
    if value.startswith('0x'):
        comment = value + ' ' + comment
        value = eval(value)

    return value, '! ' + comment 

def generateParameter(define):
    name, value_and_possibly_comment = define

    typ = 'integer'
    if value_and_possibly_comment.find('"') <> -1:
        typ = 'character(len=*)'

    if name == 'ODBQL_TRANSIENT':
        value, comment = '-1', " ! ((odbql_destructor_type)-1)"
    elif name == 'ODBQL_STATIC':
        value, comment = '0', ' ! ((odbql_destructor_type)0)'
    else:
        value, comment = translate_value_and_comment(value_and_possibly_comment) 

    return '%s, parameter :: %s = %s %s' % (typ, name, value, comment)

def generateParameters(defs):
    return """
module odbql_constants
  implicit none

""" + '\n'.join(generateParameter(d) for d in defs) + """

end module odbql_constants
""" 

def normalize_type(t): return re.sub(' [*]', '*', t)

def parseParam(p):
    p = p.strip()

    # Hack for the destructor type, used in sqlite3 to let user pass
    # a function that will release memory occupied by strings and blobs'
    # No idea how to handle this in Fortran at this point, not sure
    # if we immediately need this functionality anyway.
    ## 
    if p == 'void(*d)(void*)': return 'void(*)(void*)', 'd'

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

    # filter out void from parameterless functions e.g: const char * odbql_libversion(void)
    if len(params) == 1 and (len(params[0][0]) == 0 and params[0][1] == 'void'):
        params = []
    return (decl, (return_type, fun_name, params))

def translate_type_for_binding(t):
    #print 'translate_type_for_binding:', t
    if t == 'const char*':           return 'character(kind=C_CHAR), dimension(*)'
    if t == 'const char**':          return 'character(kind=C_CHAR), dimension(*)' # TODO
    if t == 'double':                return 'real(kind=C_DOUBLE), value'
    if t == 'int':                   return 'integer(kind=C_INT), value'
    if t == 'odbql*':                return 'type(C_PTR), VALUE'
    if t == 'odbql**':               return 'type(C_PTR)'
    if t == 'odbql_stmt*':           return 'type(C_PTR), VALUE'
    if t == 'odbql_stmt**':          return 'type(C_PTR)'
    if t == 'void(*)(void*)':        return 'type(C_PTR), VALUE'

    raise Exception("Don't know how to translate '" + t + "'")


def translate_type_for_binding_return(t):
    #print 'translate_type_for_binding_return:', t
    if t == 'const char*':           return 'type(C_PTR)'
    if t == 'const unsigned char*':  return 'type(C_PTR)'
    if t == 'int':                   return 'integer(kind=C_INT)'
    if t == 'odbql_value*':          return 'type(C_PTR)'
    if t == 'error_code_t':          return 'integer(kind=C_INT)'

    raise Exception("Don't know how to translate '" + t + "'")


def translate_type_for_fortran(t):
    #print 'translate_type_for_fortran:', t
    if t == 'const char*':           return 'character(len=*), intent(in)'
    if t == 'const char**':          return 'character(len=*), intent(out)'
    if t == 'double':                return 'real(kind=C_DOUBLE), value'
    if t == 'int':                   return 'integer(kind=C_INT), value'
    if t == 'odbql*':                return 'type(odbql), value'
    if t == 'odbql**':               return 'type(odbql)'
    if t == 'odbql_stmt*':           return 'type(odbql_stmt), value'
    if t == 'odbql_stmt**':          return 'type(odbql_stmt)'
    if t == 'void(*)(void*)':        return 'type(C_PTR), value'

    raise Exception("Don't know how to translate '" + t + "'")


def translate_type_for_fortran_return(t):
    #print 'translate_type_for_fortran_return:', t
    if t == 'const char*':           return 'character(len=*), intent(out)'
    if t == 'const unsigned char*':  return 'character(len=*), intent(out)' # TODO: think about it
    if t == 'int':                   return 'integer(kind=C_INT)'
    if t == 'odbql_value*':          return 'logical'
    if t == 'error_code_t':          return 'integer(kind=C_INT), intent(out), optional'

    raise Exception("Don't know how to translate '" + t + "'")

def fortranParamTypeDeclaration(p, translate_type = translate_type_for_binding):
    typ, parameter_name = p
    fortran_type = translate_type(typ)
    return formatParameter(fortran_type, parameter_name)

nl_indent = '\n     '

helper_functions = """

!> Helper function to convert C pointer to logical:

    function c_ptr_to_logical(ptr)

      use, intrinsic :: iso_c_binding, only: c_ptr

      type(c_ptr), intent(in)                       :: ptr
      logical                                       :: c_ptr_to_logical

      c_ptr_to_logical = c_associated(ptr)

    end function c_ptr_to_logical


!> Helper function to convert C '\\0' terminated strings to Fortran strings

    function C_to_F_string(c_string_pointer) result(f_string)
      use, intrinsic :: iso_c_binding, only: c_ptr,c_f_pointer,c_char,c_null_char
      type(c_ptr), intent(in)                       :: c_string_pointer
      character(len=:), allocatable                 :: f_string
      character(kind=c_char), dimension(:), pointer :: char_array_pointer => null()
      character(len=255)                            :: aux_string
      integer                                       :: i,length

      call c_f_pointer(c_string_pointer,char_array_pointer,[255])
      if (.not.associated(char_array_pointer)) then
          allocate(character(len=4)::f_string)
          f_string = "NULL"
          return
      end if
      aux_string = " "
      do i=1,255
        if (char_array_pointer(i)==c_null_char) then
          length=i-1; exit
        end if
        aux_string(i:i)=char_array_pointer(i)
      end do
      allocate(character(len=length)::f_string)
      f_string = aux_string(1:length)
    end function C_to_F_string

"""

status_handling_code = """
if (present(status)) then
    status = rc ! let user handle the error
else
    if (rc /= ODBQL_OK .and. rc /= ODBQL_ROW .and. rc /= ODBQL_DONE) then
        write (0,*) 'Error in %(function_name)s'
        stop
    end if
end if
"""
status_handling_code = nl_indent.join (status_handling_code.splitlines())

def parameter_type(p): return p[0]
def parameter_name(p): return p[1]

def actual_parameter(p):
    if p[1] == 'iCol': return p[1] + '-1'
    if p[0] == 'const char*': return p[1] + '_tmp'
    if p[0] == 'odbql*': return p[1] + '%this'
    if p[0] == 'odbql**': return p[1] + '%this'
    if p[0] == 'odbql_stmt*': return p[1] + '%this'
    if p[0] == 'odbql_stmt**': return p[1] + '%this'
    return p[1]

def generateWrapper(signature, comment, template):
    global status_handling_code

    return_type, function_name, params = signature
    procedure_keyword = 'function'

    output_parameter = function_name

    # filter out user supplied destructor functions (not supported now)
    fortran_params = [p for p in params if not parameter_type(p) == 'void(*)(void*)']
    fortran_params_excluding_return_parameter = fortran_params[:]

    binding_parameter_list = '(' + ','.join([p[1] for p in params]) + ')'
    actual_binding_parameter_list = '(' + ','.join([actual_parameter(p) for p in params]) + ')'

    temporary_variables_declarations = nl_indent.join(
        [formatParameter('character(len=len_trim('+p[1]+')+1)', p[1] + '_tmp') for p in params if p[0] == 'const char*']
        + [formatParameter('type(C_PTR)', p[1]) for p in params if p[0] == 'void(*)(void*)'])
    temporary_variables_assignments   = nl_indent.join(p[1] + '_tmp = ' + p[1] + '//achar(0)'
                                                      for p in params if p[0] == 'const char*')
    call_binding = function_name + '_c' + actual_binding_parameter_list

    error_handling = ''
    return_value_tmp = None

    if return_type == 'const char*' or return_type == 'const unsigned char*':
        procedure_keyword = 'subroutine'
        output_parameter = 'return_value'
        fortran_params.append( (return_type, output_parameter) )
        call_binding = 'C_to_F_string(' + call_binding  + ')'

    if return_type == 'odbql_value*':
        call_binding = 'c_ptr_to_logical(' + call_binding  + ')'

    if return_type == 'error_code_t':
        procedure_keyword = 'subroutine'
        output_parameter = 'status'
        return_value_tmp = 'rc'
        temporary_variables_declarations = nl_indent.join(
            [temporary_variables_declarations, 
             formatParameter('integer(kind=c_int)', 'rc')])
        fortran_params.append( (return_type, output_parameter) )
        error_handling = status_handling_code % locals()

    return_value_assignment = (return_value_tmp or output_parameter) + ' = ' + call_binding

    binding_parameters_declarations = nl_indent.join([fortranParamTypeDeclaration(p) for p in params])
    binding_return_type_declaration = formatParameter(translate_type_for_binding_return(return_type), function_name + '_c')

    fortran_parameter_list = '(' + ','.join([p[1] for p in fortran_params]) + ')'
    fortran_parameters_declarations = nl_indent.join(
        [fortranParamTypeDeclaration(p, translate_type = translate_type_for_fortran)
         for p in fortran_params_excluding_return_parameter])
    fortran_return_type_declaration = formatParameter(translate_type_for_fortran_return(return_type), output_parameter)

    use_intrinsic = formatParameter('use, intrinsic', 'iso_c_binding')

    return template % locals()

def generateWrappers(decls, header, footer, template):
    s = header
    for original, ast in [parseDeclaration(decl) for decl in decls]: 
        s += generateWrapper(ast, original, template) + '\n'

    s += footer
    return s



def generateBindings(source_cc = '../odb_api/odbql.cc',
                     source_h = '../odb_api/odbql.h',
                     binding_f90 = 'odbql_binding.f90',
                     wrappers_f90 = 'odbql_wrappers.f90',
                     constants_f90 = 'odbql_constants.f90'):

    decls = declarations(source_cc)
    defs = constants(source_h)
    with open(constants_f90, 'w') as f: 
        f.write(generateParameters(defs))

    ########## odbql_binding.f90: declarations of ISO C bindings for the new ODB API
    with open(binding_f90, 'w') as f: 
        f.write(generateWrappers(decls, header = """

!!!!! THIS FILE WAS AUTOMATICALLY GENERATED. DO NOT EDIT MANUALLY !!!!!

module odbql_binding
  use iso_c_binding
  use, intrinsic :: iso_c_binding
  implicit none
interface
""", footer = """
end interface
end module odbql_binding
""", template = """
!> %(comment)s

    function %(function_name)s_c %(binding_parameter_list)s bind(C, name="%(function_name)s")
     %(use_intrinsic)s
     %(binding_parameters_declarations)s
     %(binding_return_type_declaration)s
    end function %(function_name)s_c


    """))

    ############ odbql_wrappers.f90  The Fortran user interface

    with open(wrappers_f90, 'w') as f: 
        f.write(generateWrappers(decls, header = """

!!!!! THIS FILE WAS AUTOMATICALLY GENERATED. DO NOT EDIT MANUALLY !!!!!

module odbql_wrappers
  use odbql_binding
  use odbql_constants
  implicit none
  
  type odbql
    type(c_ptr) :: this
  end type

  type odbql_stmt
    type(c_ptr) :: this
  end type

contains

%(helper_functions)s

""" % globals(), footer = """
end module odbql_wrappers
""", template = """
!> %(comment)s

    %(procedure_keyword)s %(function_name)s %(fortran_parameter_list)s 
     use odbql_binding
     %(use_intrinsic)s
     %(fortran_parameters_declarations)s
     %(fortran_return_type_declaration)s

     %(temporary_variables_declarations)s

     %(temporary_variables_assignments)s
     %(return_value_assignment)s
     %(error_handling)s

    end %(procedure_keyword)s %(function_name)s

    """ 
    ))

if __name__ == '__main__': generateBindings()
