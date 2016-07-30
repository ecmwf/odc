#!/usr/bin/env python 

import re

PARAM_TYPE_COLUMN = 37

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
    return (decl, (return_type, fun_name, params))

def translate_type_for_binding(t):
    #print 'translate_type_for_binding:', t
    if t == 'const char*':           return 'character(kind=C_CHAR),dimension(*)'
    if t == 'const char**':          return 'character(kind=C_CHAR),dimension(*)' # TODO
    if t == 'double':                return 'real(kind=C_DOUBLE), VALUE'
    if t == 'int':                   return 'integer(kind=C_INT), VALUE'
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

    raise Exception("Don't know how to translate '" + t + "'")


def translate_type_for_fortran(t):
    #print 'translate_type_for_fortran:', t
    if t == 'const char*':           return 'character(len=*),intent(in)'
    if t == 'const char**':          return 'character(len=*),intent(out)'
    if t == 'double':                return 'real(kind=C_DOUBLE), VALUE'
    if t == 'int':                   return 'integer(kind=C_INT), VALUE'
    if t == 'odbql*':                return 'type(odbql), VALUE'
    if t == 'odbql**':               return 'type(odbql)'
    if t == 'odbql_stmt*':           return 'type(odbql_stmt), VALUE'
    if t == 'odbql_stmt**':          return 'type(odbql_stmt)'
    if t == 'void(*)(void*)':        return 'type(C_PTR), VALUE'

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


helper_functions = """

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

def actual_parameter(p):
    if p[1] == 'iCol': return p[1] + '-1'
    if p[0] == 'const char*': return p[1] + '_tmp'
    if p[0] == 'odbql*': return p[1] + '%this'
    if p[0] == 'odbql**': return p[1] + '%this'
    if p[0] == 'odbql_stmt*': return p[1] + '%this'
    if p[0] == 'odbql_stmt**': return p[1] + '%this'
    return p[1]

def generateWrapper(declaration, comment, template):
    procedure_keyword = 'function'

    return_type, function_name, params = declaration

    ###print 'generateWrapper: ', declaration, comment

    #print 'params:', params  # [(name,type), ...]

    output_parameter = function_name

    # filter out void from parameterless functions e.g: const char * odbql_libversion(void)
    if len(params) == 1 and (len(params[0][0]) == 0 and params[0][1] == 'void'):
        params = []
    # filter out user supplied destructor functions (not supported now)
    params = [p for p in params if not p[1] == 'void(*)(void*)']

    fortran_params = params[:]

    binding_parameter_list = '(' + ','.join([p[1] for p in params]) + ')'
    actual_binding_parameter_list = '(' + ','.join([actual_parameter(p) for p in params]) + ')'
    # character(len=len_trim(filename) + 1)     :: filename_tmp
    temporary_variables_declarations = '\n     '.join(formatParameter('character(len=len_trim('+p[1]+')+1)', p[1] + '_tmp')
                                                      for p in params if p[0] == 'const char*')
    temporary_variables_assignments   = '\n     '.join(p[1] + '_tmp = ' + p[1] + '//achar(0)'
                                                      for p in params if p[0] == 'const char*')

    call_binding = output_parameter + ' = ' + function_name + '_c' + actual_binding_parameter_list
    if return_type == 'const char*' or return_type == 'const unsigned char*':
        procedure_keyword = 'subroutine'
        output_parameter = 'return_value'
        fortran_params.append( (return_type, output_parameter) )
        call_binding = output_parameter + ' = C_to_F_string(' + function_name + '_c' + actual_binding_parameter_list + ')'

    fortran_parameter_list = '(' + ','.join([p[1] for p in fortran_params]) + ')'

    binding_parameters_declarations = '\n     '.join([fortranParamTypeDeclaration(p) for p in params])
    binding_return_type_declaration = formatParameter(translate_type_for_binding_return(return_type), function_name + '_c')

    fortran_parameters_declarations = '\n     '.join([fortranParamTypeDeclaration(p, translate_type = translate_type_for_fortran) for p in params])
    fortran_return_type_declaration = formatParameter(translate_type_for_fortran_return(return_type), output_parameter)

    use_intrinsic = formatParameter('use, intrinsic', 'iso_c_binding')

    return template % locals()

def generateWrappers(decls, header, footer, template):
    s = header
    for original, ast in [parseDeclaration(decl) for decl in decls]: 
        s += generateWrapper(ast, original, template) + '\n'

    s += footer
    return s


def generateBindings(source_cc = 'odbql.cc',
                     binding_f90 = 'odbql_binding.f90',
                     wrappers_f90 = 'odbql_wrappers.f90'):

    decls = declarations(source_cc)
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

     %(call_binding)s

    end %(procedure_keyword)s %(function_name)s

    """ 
    ))

if __name__ == '__main__':
    generateBindings()

