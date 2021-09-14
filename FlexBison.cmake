FIND_PROGRAM(FLEX_EXECUTABLE flex DOC "path to the flex executable")
MARK_AS_ADVANCED(FLEX_EXECUTABLE)

FIND_LIBRARY(FL_LIBRARY NAMES fl
  DOC "path to the fl library")
MARK_AS_ADVANCED(FL_LIBRARY)
SET(FLEX_LIBRARIES ${FL_LIBRARY})

IF(FLEX_EXECUTABLE)

  EXECUTE_PROCESS(COMMAND ${FLEX_EXECUTABLE} --version
    OUTPUT_VARIABLE FLEX_version_output
    ERROR_VARIABLE FLEX_version_error
    RESULT_VARIABLE FLEX_version_result
    OUTPUT_STRIP_TRAILING_WHITESPACE)
  IF(NOT ${FLEX_version_result} EQUAL 0)
    MESSAGE(SEND_ERROR "Command \"${FLEX_EXECUTABLE} --version\" failed with output:\n${FLEX_version_error}")
  ELSE()
    STRING(REGEX REPLACE "^flex (.*)$" "\\1"
      FLEX_VERSION "${FLEX_version_output}")
  ENDIF()

  #============================================================
  # STL_FLEX_TARGET (public macro)
  #============================================================
  #
  MACRO(STL_FLEX_TARGET Name Input Output)
    SET(STL_FLEX_TARGET_usage "STL_FLEX_TARGET(<Name> <Input> <Output> [COMPILE_FLAGS <string>]")
    IF(${ARGC} GREATER 3)
      IF(${ARGC} EQUAL 5)
        IF("${ARGV3}" STREQUAL "COMPILE_FLAGS")
          SET(FLEX_EXECUTABLE_opts  "${ARGV4}")
          SEPARATE_ARGUMENTS(FLEX_EXECUTABLE_opts)
        ELSE()
          MESSAGE(SEND_ERROR ${FLEX_TARGET_usage})
        ENDIF()
      ELSE()
        MESSAGE(SEND_ERROR ${FLEX_TARGET_usage})
      ENDIF()
    ENDIF()

    ADD_CUSTOM_COMMAND(OUTPUT ${Output}
      COMMAND ${FLEX_EXECUTABLE}
      ARGS ${FLEX_EXECUTABLE_opts} -o${Output} ${Input}
      DEPENDS ${Input}
      COMMENT "[FLEX][${Name}] Building scanner with flex ${FLEX_VERSION}"
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

    SET(STL_FLEX_${Name}_DEFINED TRUE)
    SET(STL_FLEX_${Name}_OUTPUTS ${Output})
    SET(STL_FLEX_${Name}_INPUT ${Input})
    SET(STL_FLEX_${Name}_COMPILE_FLAGS ${FLEX_EXECUTABLE_opts})
    SET(GENERATED_FLEX_FILES ${GENERATED_FLEX_FILES} ${Output} PARENT_SCOPE )
 ENDMACRO(STL_FLEX_TARGET)
  #============================================================


  #============================================================
  # ADD_FLEX_BISON_DEPENDENCY (public macro)
  #============================================================
  #
  MACRO(ADD_FLEX_BISON_DEPENDENCY FlexTarget BisonTarget)

    IF(NOT FLEX_${FlexTarget}_OUTPUTS)
      MESSAGE(SEND_ERROR "Flex target `${FlexTarget}' does not exists.")
    ENDIF()

    IF(NOT BISON_${BisonTarget}_OUTPUT_HEADER)
      MESSAGE(SEND_ERROR "Bison target `${BisonTarget}' does not exists.")
    ENDIF()

    SET_SOURCE_FILES_PROPERTIES(${FLEX_${FlexTarget}_OUTPUTS}
      PROPERTIES OBJECT_DEPENDS ${BISON_${BisonTarget}_OUTPUT_HEADER})
  ENDMACRO(ADD_FLEX_BISON_DEPENDENCY)
  #============================================================

ENDIF(FLEX_EXECUTABLE)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(FLEX DEFAULT_MSG FLEX_EXECUTABLE)

FIND_PROGRAM(BISON_EXECUTABLE bison DOC "path to the bison executable")
MARK_AS_ADVANCED(BISON_EXECUTABLE)

IF(BISON_EXECUTABLE)

  EXECUTE_PROCESS(COMMAND ${BISON_EXECUTABLE} --version
    OUTPUT_VARIABLE BISON_version_output
    ERROR_VARIABLE BISON_version_error
    RESULT_VARIABLE BISON_version_result
    OUTPUT_STRIP_TRAILING_WHITESPACE)
  IF(NOT ${BISON_version_result} EQUAL 0)
    MESSAGE(SEND_ERROR "Command \"${BISON_EXECUTABLE} --version\" failed with output:\n${BISON_version_error}")
  ELSE()
    STRING(REGEX REPLACE "^bison \\(GNU Bison\\) ([^\n]+)\n.*" "\\1"
      BISON_VERSION "${BISON_version_output}")
  ENDIF()

  # internal macro
  MACRO(BISON_TARGET_option_verbose Name BisonOutput filename)
    LIST(APPEND BISON_TARGET_cmdopt "--verbose")
    GET_FILENAME_COMPONENT(BISON_TARGET_output_path "${BisonOutput}" PATH)
    GET_FILENAME_COMPONENT(BISON_TARGET_output_name "${BisonOutput}" NAME_WE)
    ADD_CUSTOM_COMMAND(OUTPUT ${filename}
      COMMAND ${CMAKE_COMMAND}
      ARGS -E copy
      "${BISON_TARGET_output_path}/${BISON_TARGET_output_name}.output"
      "${filename}"
      DEPENDS
      "${BISON_TARGET_output_path}/${BISON_TARGET_output_name}.output"
      COMMENT "[BISON][${Name}] Copying bison verbose table to ${filename}"
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
    SET(BISON_${Name}_VERBOSE_FILE ${filename})
    LIST(APPEND BISON_TARGET_extraoutputs
      "${BISON_TARGET_output_path}/${BISON_TARGET_output_name}.output")
  ENDMACRO(BISON_TARGET_option_verbose)

  # internal macro
  MACRO(BISON_TARGET_option_extraopts Options)
    SET(BISON_TARGET_extraopts "${Options}")
    SEPARATE_ARGUMENTS(BISON_TARGET_extraopts)
    LIST(APPEND BISON_TARGET_cmdopt ${BISON_TARGET_extraopts})
  ENDMACRO(BISON_TARGET_option_extraopts)

  #============================================================
  # BISON_TARGET (public macro)
  #============================================================
  #
  MACRO(STL_BISON_TARGET Name BisonInput BisonOutput)
    SET(BISON_TARGET_output_header "")
    SET(BISON_TARGET_command_opt "")
    SET(BISON_TARGET_outputs "${BisonOutput}")
    IF(NOT ${ARGC} EQUAL 3 AND NOT ${ARGC} EQUAL 5 AND NOT ${ARGC} EQUAL 7)
      MESSAGE(SEND_ERROR "Usage")
    ELSE()
      # Parsing parameters
      IF(${ARGC} GREATER 5 OR ${ARGC} EQUAL 5)
        IF("${ARGV3}" STREQUAL "VERBOSE")
          BISON_TARGET_option_verbose(${Name} ${BisonOutput} "${ARGV4}")
        ENDIF()
        IF("${ARGV3}" STREQUAL "COMPILE_FLAGS")
          BISON_TARGET_option_extraopts("${ARGV4}")
        ENDIF()
      ENDIF()

      IF(${ARGC} EQUAL 7)
        IF("${ARGV5}" STREQUAL "VERBOSE")
          BISON_TARGET_option_verbose(${Name} ${BisonOutput} "${ARGV6}")
        ENDIF()
      
        IF("${ARGV5}" STREQUAL "COMPILE_FLAGS")
          BISON_TARGET_option_extraopts("${ARGV6}")
        ENDIF()
      ENDIF()

      # Header's name generated by bison (see option -d)
      LIST(APPEND BISON_TARGET_cmdopt "-d")
      STRING(REGEX REPLACE "^(.*)(\\.[^.]*)$" "\\2" _fileext "${ARGV2}")
      STRING(REPLACE "c" "h" _fileext ${_fileext})
      STRING(REGEX REPLACE "^(.*)(\\.[^.]*)$" "\\1${_fileext}" 
          BISON_${Name}_OUTPUT_HEADER "${ARGV2}")
      LIST(APPEND BISON_TARGET_outputs "${BISON_${Name}_OUTPUT_HEADER}")
        
      ADD_CUSTOM_COMMAND(OUTPUT ${BISON_TARGET_outputs}
        ${BISON_TARGET_extraoutputs}
        COMMAND ${BISON_EXECUTABLE}
        ARGS ${BISON_TARGET_cmdopt} -o ${ARGV2} ${ARGV1}
        DEPENDS ${ARGV1}
        COMMENT "[BISON][${Name}] Building parser with bison ${BISON_VERSION}"
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    
      # define target variables
      SET(STL_BISON_${Name}_DEFINED TRUE)
      SET(STL_BISON_${Name}_INPUT ${ARGV1})
      SET(STL_BISON_${Name}_OUTPUTS ${BISON_TARGET_outputs})
      SET(STL_BISON_${Name}_COMPILE_FLAGS ${BISON_TARGET_cmdopt})
      SET(STL_BISON_${Name}_OUTPUT_SOURCE "${BisonOutput}")
      SET(GENERATED_BISON_FILES ${GENERATED_BISON_FILES} ${BISON_TARGET_outputs} PARENT_SCOPE )

    ENDIF(NOT ${ARGC} EQUAL 3 AND NOT ${ARGC} EQUAL 5 AND NOT ${ARGC} EQUAL 7)
  ENDMACRO(STL_BISON_TARGET)
  #
  #============================================================

ENDIF(BISON_EXECUTABLE)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(BISON DEFAULT_MSG BISON_EXECUTABLE)
