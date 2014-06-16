cmake_minimum_required(VERSION 2.6)
set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_FLAGS_DEBUG ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "bin/")

project(crypto-alg)

set(BOOST_USE_STATIC_LIBS ON)
set(BOOST_USE_MULTITHREADED ON)
set(BOOST_USE_STATIC_RUNTIME OFF)
find_package(Boost 1.49 COMPONENTS filesystem program_options system)

file(GLOB_RECURSE headers "${PROJECT_SOURCE_DIR}/lib/include/*.hpp")

file(GLOB grid_ncrypt "${PROJECT_SOURCE_DIR}/lib/src/grid_ncrypt.cc")
file(GLOB public_ncrypt "${PROJECT_SOURCE_DIR}/lib/src/public_ncrypt.cc")
file(GLOB fragment_ncrypt "${PROJECT_SOURCE_DIR}/lib/src/fragment_ncrypt.cc")
file(GLOB chat_message "${PROJECT_SOURCE_DIR}/lib/src/chat_message.cc")

file(GLOB grid_ncrypt_x "${PROJECT_SOURCE_DIR}/src/grid_ncrypt.cxx")
file(GLOB public_ncrypt_x "${PROJECT_SOURCE_DIR}/src/public_ncrypt.cxx")
file(GLOB fragment_ncrypt_x "${PROJECT_SOURCE_DIR}/src/fragment_ncrypt.cxx")

file(GLOB client "${PROJECT_SOURCE_DIR}/src/client.cxx")
file(GLOB server "${PROJECT_SOURCE_DIR}/src/server.cxx")

source_group("Header Files" FILES ${headers})

include_directories(${PROJECT_SOURCE_DIR}/lib/include)

add_executable(grid_ncrypt ${grid_ncrypt_x} ${grid_ncrypt} ${headers})
add_executable(public_ncrypt ${public_ncrypt_x} ${public_ncrypt} ${headers})
add_executable(fragment_ncrypt ${fragment_ncrypt_x} ${fragment_ncrypt} ${headers})

add_executable(client ${client} ${chat_message} ${public_ncrypt})
target_link_libraries(client pthread boost_system boost_thread boost_program_options)

add_executable(server ${server} ${chat_message} ${public_ncrypt})
target_link_libraries(server pthread boost_system boost_thread boost_program_options)
