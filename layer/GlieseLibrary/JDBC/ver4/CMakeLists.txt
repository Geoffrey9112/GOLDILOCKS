CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
# Avoid warnings in higher versions
IF("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" GREATER 2.6)
 CMAKE_POLICY(VERSION 2.8)
ENDIF()

INCLUDE(FindJava.cmake)
INCLUDE(UseJava.cmake)

SET(GLIESE_HOME_LIB          ${CMAKE_CURRENT_SOURCE_DIR}/../../../../Gliese/home/lib)
SET(INSTALL_LIBDIR           lib )

SET(CMAKE_JAVA_COMPILE_FLAGS -target jsr14 -source 1.5 -sourcepath dev/src)
SET(CMAKE_JAVA_MANIFEST_FILE ../META-INF/MANIFEST.MF)
SET(CMAKE_JAVA_RESOURCE_FILE META-INF/services/java.sql.Driver)
ADD_JAR(goldilocks4 ${GLIESE_HOME_LIB} 
              src/sunje/goldilocks/jdbc/*.java
              ../dev/src/sunje/goldilocks/sql/*.java
              ../dev/src/sunje/goldilocks/jdbc/cm/*.java
              ../dev/src/sunje/goldilocks/jdbc/dt/*.java
              ../dev/src/sunje/goldilocks/jdbc/ex/*.java
              ../dev/src/sunje/goldilocks/jdbc/internal/*.java
              ../dev/src/sunje/goldilocks/jdbc/env/*.java
              ../dev/src/sunje/goldilocks/jdbc/util/*.java
              ../dev/src/sunje/goldilocks/jdbc/core4/*.java)

INSTALL(FILES ${GLIESE_HOME_LIB}/igoldilocks4.jar             DESTINATION ${INSTALL_LIBDIR})

