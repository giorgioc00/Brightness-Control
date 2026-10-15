cmake_minimum_required(VERSION 3.10)
project(brightness-control)

set(CMAKE_CXX_STANDARD 17)

# Find required packages
find_package(PkgConfig REQUIRED)

# Try to find GTK3
pkg_check_modules(GTK REQUIRED gtk+-3.0)

# Try to find ayatana-appindicator
pkg_check_modules(APPINDICATOR ayatana-appindicator3-0.1)

if(NOT APPINDICATOR_FOUND)
    message(FATAL_ERROR "ayatana-appindicator3-0.1 not found. Please install libayatana-appindicator3-dev")
endif()

add_executable(brightness-control brightness.cpp)

target_link_libraries(brightness-control ${GTK_LIBRARIES} ${APPINDICATOR_LIBRARIES})
target_include_directories(brightness-control PRIVATE ${GTK_INCLUDE_DIRS} ${APPINDICATOR_INCLUDE_DIRS})
target_compile_options(brightness-control PRIVATE ${GTK_CFLAGS_OTHER} ${APPINDICATOR_CFLAGS_OTHER})

include(GNUInstallDirs)
install(TARGETS brightness-control RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES brightness-control.desktop DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)
