# The minimum required version hasn't been tested. Feel free to adjust
# downwards if necessary.
cmake_minimum_required(VERSION 3.15.3)
project("merge2Db" C)

find_package(PkgConfig)
pkg_check_modules(MySQL REQUIRED mysqlclient)

add_executable(test main.c config.h)
target_include_directories(test PUBLIC ${MySQL_INCLUDE_DIRS})
target_link_libraries(test PUBLIC ${MySQL_LIBRARIES})




