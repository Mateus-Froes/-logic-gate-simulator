cmake_minimum_required(VERSION 3.16)
project(LogicGateSimulator VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_definitions(MULTI_FILE_BUILD)

if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

add_library(logic_core STATIC
    src/Component.cpp
    src/Wire.cpp
    src/Gates.cpp
    src/CircuitBoard.cpp
)
target_include_directories(logic_core PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

add_executable(simulador src/main.cpp)
target_link_libraries(simulador PRIVATE logic_core)

find_package(SFML 3 COMPONENTS Graphics Window System QUIET)

if(SFML_FOUND)
    message(STATUS "SFML 3 encontrado - habilitando alvo 'simulador_gui'.")
    add_executable(simulador_gui
        src/gui/GuiRenderer.cpp
        src/gui/Sidebar.cpp
        src/gui/GuiApplication.cpp
        src/gui/main_gui.cpp
    )
    target_link_libraries(simulador_gui PRIVATE
        logic_core
        SFML::Graphics
        SFML::Window
        SFML::System
    )

    add_custom_command(TARGET simulador_gui POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/assets"
            "$<TARGET_FILE_DIR:simulador_gui>/assets"
        COMMENT "Copiando assets/ para o diretório de build..."
    )
else()
    message(WARNING "SFML 3 não encontrado - alvo 'simulador_gui' ignorado (apenas 'simulador' será compilado).")
endif()
