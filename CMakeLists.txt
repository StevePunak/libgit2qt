cmake_minimum_required(VERSION 3.16)
include(GNUInstallDirs)

set(PROJ git2qt)
project(${PROJ})

find_package(Qt6 REQUIRED COMPONENTS Network)
qt_standard_project_setup()

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(KANOOP_GIT_VERSION "0.1.2")

file(GLOB_RECURSE KANOOP_GIT_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB_RECURSE KANOOP_GIT_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

qt_add_library(
    ${PROJ} ${KANOOP_GIT_SOURCES} ${KANOOP_GIT_HEADERS}
)

target_compile_options(${PROJ} PRIVATE -Wextra -Wall -Werror -Wno-missing-field-initializers -Wno-documentation)

set_target_properties(${PROJ} PROPERTIES VERSION "${KANOOP_GIT_VERSION}")
set_target_properties(${PROJ} PROPERTIES SOVERSION 1)
# configure_file("${CMAKE_CURRENT_SOURCE_DIR}/libKanoopGit.pc.in" "libKanoopGit.pc" @ONLY)

target_include_directories(${PROJ} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(${PROJ} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_include_directories(${PROJ} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/git2qt)

# Temp include Kanoop
get_directory_property(PARENT_DIR PARENT_DIRECTORY)
target_include_directories(${PROJ} PRIVATE ${PARENT_DIR}/KanoopCommon/include)

# target_link_directories(${PROJ} PRIVATE "C:/Program Files (x86)/libgit2/lib/")
target_link_libraries(${PROJ} PRIVATE Qt6::Network git2 KanoopCommon)
# end temp Kanoop

target_include_directories(${PROJ} PRIVATE ${PARENT_DIR}/libgit2/include)
target_link_directories(${PROJ} PRIVATE ${CMAKE_BINARY_DIR}/libgit2)

add_compile_definitions(GIT2QT_LIBRARY)
add_compile_definitions(QT_DEPRECATED_WARNINGS)
add_compile_definitions(QT_DISABLE_DEPRECATED_BEFORE=0x060000)  # Disables all the APIs deprecated before Qt 6.0.0

# Set install prefix.  This is necessary when installing without KYBER_OUTPUT_PREFIX set as this causes the path to start with two
# forward-slashes which CMake interprets to mean a network path.  Thus, we only include the "/" if there is something ahead of it.
if (DEFINED $ENV{KYBER_OUTPUT_PREFIX})
    set(KANOOP_INSTALL_PREFIX "$ENV{KYBER_OUTPUT_PREFIX}/")
endif()

install(TARGETS ${PROJ} LIBRARY DESTINATION "${KANOOP_INSTALL_PREFIX}${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/Kanoop" DESTINATION "${KANOOP_INSTALL_PREFIX}${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR}" FILES_MATCHING PATTERN "*.h")
