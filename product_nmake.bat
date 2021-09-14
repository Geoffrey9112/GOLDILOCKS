@ECHO OFF

SET PROGRAM_NAME=%0

:PARAM_BEGIN

IF /I "%1" == "debug"   SET BUILD_TYPE=Debug& GOTO :PARAM_END
IF /I "%1" == "release" SET BUILD_TYPE=Release& GOTO :PARAM_END
IF /I "%1" == "help"    GOTO :PRINT_USAGE

ECHO ERROR: unknown parameter %1& GOTO :PRINT_USAGE

:PARAM_END

SHIFT

:OPTION_BEGIN

IF /I "%1" == "" GOTO :OPTION_END

IF /I "%1" == "bit" (
  IF "%2" == "32" SET BUILD_BIT=32& GOTO :BIT_END
  IF "%2" == "64" SET BUILD_BIT=64& GOTO :BIT_END
  
  ECHO ERROR: unknown options %2& GOTO :PRINT_USAGE
  
  :BIT_END
  SHIFT
  GOTO :NEXT_OPTION
)

ECHO ERROR: invalid options %2& GOTO :PRINT_USAGE

:NEXT_OPTION
SHIFT
GOTO :OPTION_BEGIN

:OPTION_END

@ECHO ON
cmake -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=%BUILD_TYPE% -DCMAKE_TARGET_HOST_TYPE=Client -DCMAKE_BITS=%BUILD_BIT% .
@ECHO OFF

nmake clean
nmake

EXIT /B

:PRINT_USAGE
ECHO.
ECHO USAGE: %PROGRAM_NAME% parameter [options]
ECHO.
ECHO   parameters :
ECHO       debug
ECHO       release
ECHO.
ECHO   options :
ECHO       bit {32 or 64}    : compile bit
ECHO.

EXIT /B