#SET( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR} )
#set(EXECUTABLE_OUTPUT_PATH bin/${CMAKE_BUILD_TYPE})

#Activer le support C++0x
if(CMAKE_VERSION VERSION_LESS "3.1")
  set(CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS}")
else ()
  set (CMAKE_CXX_STANDARD 98)
endif ()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -W -Wshadow -Wunused-variable -Wunused-parameter -Wunused-function -Wunused -Wno-system-headers -Wno-deprecated -Woverloaded-virtual") # various warning flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s -O2") # no debug, active optimisation

include_directories(../dependency)

#Génération de la liste des fichiers sources
file(
  GLOB_RECURSE
  Plateforms_files
  ../dependency/Plateforms/*
)

file(
  GLOB_RECURSE
  SafeFunctions_files
  ../dependency/SafeFunctions/*
)

file(
  GLOB_RECURSE
  Service_files
  ../dependency/Service/*
)

file(
  GLOB_RECURSE
  SimpleFolders_files
  ../dependency/SimpleFolders/*
)

file(
  GLOB_RECURSE
  SimpleIni_files
  ../dependency/SimpleIni/*
)

file(
  GLOB_RECURSE
  SimpleLog_files
  ../dependency/SimpleLog/*
)

file(
  GLOB_RECURSE
  SimpleSock_files
  ../dependency/SimpleSock/*
)

file(
  GLOB_RECURSE
  StringTools_files
  ../dependency/StringTools/*
)

file(
  GLOB_RECURSE
  xPLLib_files
  ../dependency/xPLLib/*
)

if(CMAKE_VERSION VERSION_GREATER "3.0")
  set(THREADS_PREFER_PTHREAD_FLAG ON)
endif ()
find_package(Threads)

find_package(MySQL)
link_directories(${MYSQL_ADD_LIBRARY})
 
#Déclaration de l'exécutable
add_executable(
  xPLDatabase
  ${Plateforms_files}
  ${SafeFunctions_files}
  ${Service_files}
  ${SimpleFolders_files}
  ${SimpleIni_files}
  ${SimpleLog_files}
  ${SimpleSock_files}
  ${StringTools_files}
  ${xPLLib_files}
  ../dependency/SQLite/sqlite3.c
  xPLDatabase.cpp
  CacheFile.cpp
  CacheMemory.cpp
  DateHelper.cpp
  DbMysql.cpp
  DbSqlite.cpp
  Filter.cpp
  RequestValue.cpp
  main.cpp
)

target_link_libraries(xPLDatabase ${CMAKE_DL_LIBS} ${CMAKE_THREAD_LIBS_INIT} ${MYSQL_LIBRARIES})

#Spécification WIN32
if(WIN32)
  target_link_libraries(xPLDatabase ws2_32)
endif()