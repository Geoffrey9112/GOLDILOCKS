#!/bin/sh

#######################################
# Parsing Arguments
#######################################

CMAKE_FLAGS=""
CPU_JOB_NUM=1

CMAKE_TEST_TYPE="none"          # none, warning
CMAKE_COMIPILE_TYPE="none"      # none, binary, doc
CMAKE_TARGET_HOST_TYPE="server" # server, client
CMAKE_BITS="64"                 # none, server, client

print_usage()
{
      echo ""
      echo "USAGE: $0 parameter [options]"
      echo ""
      echo "  parameters : "
      echo "       debug "
      echo "       release "
      echo "       profile "
      echo "       warning "
      echo "       doc"
      echo "       help"
      echo ""
      echo "  options : "
      echo "       parallel       : enable parallel build"
      echo "       cpp            : compile with c++ compiler"
      echo "       gsql-cpp-odbc  : enable gsql cpp odbc"
      echo "       odbc-legacy-64 : compile odbc with 32-bit SQLLEN/SQLULEN types" 
      echo "       bit {32|64}    : compile bit"
      echo "       client         : compile for client"
      echo "       server         : compile for server"
      echo ""
}

#######################################
# choose make
# we prefer to gnu make
#######################################

case `uname -s` in 
  Linux)
    MAKE=make
    ;;
  HP-UX)
    MAKE=gmake
    ;;
  SunOS)
    MAKE=gmake
    ;;
  *)
    MAKE=make
    ;;
esac

#######################################
# parse parameter
#######################################

case $1 in
  debug)
    CMAKE_FLAGS="${CMAKE_FLAGS} -DCMAKE_BUILD_TYPE=Debug"
    CMAKE_COMIPILE_TYPE="binary"
    ;;
  release)
    CMAKE_FLAGS="${CMAKE_FLAGS} -DCMAKE_BUILD_TYPE=Release"
    CMAKE_COMIPILE_TYPE="binary"
    ;;
  warning)
    CMAKE_FLAGS="${CMAKE_FLAGS} -DCMAKE_BUILD_TYPE=Release"
    CMAKE_COMIPILE_TYPE="binary"
    CMAKE_TEST_TYPE="warning"
    ;;
  validate)
    CMAKE_FLAGS="${CMAKE_FLAGS} -DCMAKE_BUILD_TYPE=Validate"
    CMAKE_COMIPILE_TYPE="binary"
    ;;
  profile)
    CMAKE_FLAGS="${CMAKE_FLAGS} -DCMAKE_BUILD_TYPE=Profile"
    CMAKE_COMIPILE_TYPE="binary"
    ;;
  doc)
    CMAKE_FLAGS="${CMAKE_FLAGS} -DCMAKE_BUILD_TYPE=Debug -DBUILD_DOCUMENTATION=ON"
    CMAKE_COMIPILE_TYPE="DOC"
    ;;
  jdbc)
    CMAKE_COMIPILE_TYPE="jdbc"
    ;;
  help)
    print_usage
    exit 1
    ;;
  *)
    echo "ERROR: unknown parameter \"$1\""
    print_usage
    exit 1
    ;;
esac
shift

#######################################
# parse options
#######################################

while [ "$1" != "" ]; do
  case $1 in
    parallel)
      case `uname -s` in 
        Linux)
          CPU_JOB_NUM=`grep processor /proc/cpuinfo | wc -l`
          ;;
        *)
          CPU_JOB_NUM=0
          ;;
      esac
      ;;
    client)
      CMAKE_TARGET_HOST_TYPE="client"
      ;;
    server)
      CMAKE_TARGET_HOST_TYPE="server"
      ;;
    bit)
      case $2 in
        32)
          CMAKE_BITS="32"
          ;;
        64)
          CMAKE_BITS="64"
          ;;
        *)
          echo "ERROR: unknown options \"$2\""
          print_usage
          exit 1
          ;;
      esac
      shift
      ;;
    cpp)
      CMAKE_FLAGS="${CMAKE_FLAGS} -DCMAKE_CPP_COMPILER=1"
      ;;
    gsql-cpp-odbc)
      CMAKE_FLAGS="${CMAKE_FLAGS} -DCMAKE_SUPPORT_CPP_ODBC=1"
      ;;
    odbc-legacy-64)
      CMAKE_FLAGS="${CMAKE_FLAGS} -DODBC_LEGACY_64BIT=1"
      ;;
    *)
      echo "ERROR: unknown options \"$1\""
      print_usage
      exit 1
      ;;
  esac
  shift
done

CMAKE_FLAGS="${CMAKE_FLAGS} -DCMAKE_TARGET_HOST_TYPE=${CMAKE_TARGET_HOST_TYPE}"
CMAKE_FLAGS="${CMAKE_FLAGS} -DCMAKE_BITS=${CMAKE_BITS}"


#######################################
# Build Makefile
#######################################

cmake ${CMAKE_FLAGS} .

#######################################
# Compile Source
#######################################

case ${CMAKE_COMIPILE_TYPE} in
  "binary")
      ${MAKE} etags
      ${MAKE} clean;
      if test ${CPU_JOB_NUM} = 0; then
        ${MAKE} -j
      elif test ${CPU_JOB_NUM} = 1; then
        ${MAKE}
      else
        ${MAKE} -j ${CPU_JOB_NUM}
      fi
      if [ -n "$JAVA5" ]; then
        export JAVA_HOME=$JAVA5;
        export PATH=$JAVA_HOME/bin:$PATH;
        export CLASSPATH=.:$JAVA_HOME/jre/lib/rt.jar;
        cd layer/GlieseLibrary/JDBC/ver4;
        cmake .
        ${MAKE} clean;
        ${MAKE};
        cd ../ver5;
        cmake .
        ${MAKE} clean;
        ${MAKE};
        cd ../../../../;
      fi
    ;;
  "jdbc")
      cd layer/GlieseLibrary/JDBC;
      ${MAKE} clean;
      ${MAKE};
      cd ../../..;
      if [ -n "$JAVA5" ]; then
        export JAVA_HOME=$JAVA5;
        export PATH=$JAVA_HOME/bin:$PATH;
        export CLASSPATH=.:$JAVA_HOME/jre/lib/rt.jar;
        cd layer/GlieseLibrary/JDBC/ver4;
        cmake .
        ${MAKE} clean;
        ${MAKE};
        cd ../ver5;
        cmake .
        ${MAKE} clean;
        ${MAKE};
        cd ../../../../;
      fi
    ;;
  "doc")
    ${MAKE} Doxygen_Standard_API
    ${MAKE} Doxygen_DataType_API
    ${MAKE} Doxygen_Kernel_API
    ${MAKE} Doxygen_SM_API
    ${MAKE} Doxygen_Execution_API
    ${MAKE} Doxygen_Gliese_API
    ${MAKE} Doxygen_SqlProcessor_API
    ${MAKE} Doxygen_ZT_API
    ;;
  "none")
    ;;
esac

#######################################
# Test Now
#######################################

case ${CMAKE_TEST_TYPE} in
  "warning")
    cd layer; sh warning.sh; cd ..
    ;;
  "none")
    ;;
esac

