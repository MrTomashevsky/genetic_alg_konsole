cmake_minimum_required(VERSION 3.16)

project(genetic_alg_konsole LANGUAGES CXX)

add_compile_definitions(FATHER_DIR="genetic_alg_konsole")
add_compile_definitions(DEBUG_ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

add_executable(genetic_alg_konsole

    # mainwindow.hpp
    # mainwindow.cpp
    # mainwindow.ui

    include/tomatik_clah.hpp
    include/tomatik_clah.cpp
    include/get_file.hpp
    include/array_digits.hpp
    include/byte_code.hpp
    include/byte_code.cpp
    include/byte_code_genetic.hpp
    include/byte_code_genetic.cpp
    include/genetic.hpp
    include/genetic.cpp
    include/random.hpp
    include/random.cpp
    include/fitness.hpp
    include/fitness.cpp
    include/global.hpp
    include/global.cpp

    include/tomatik_degree_of_completion.hpp

    main.cpp
    main_program.hpp
    main_program.cpp





)
target_link_libraries(genetic_alg_konsole Qt${QT_VERSION_MAJOR}::Core)

include(GNUInstallDirs)
install(TARGETS genetic_alg_konsole
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
