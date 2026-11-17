CXX      ?= g++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -Wpedantic -Iinclude

CORE_SRCS := src/Component.cpp src/Wire.cpp src/Gates.cpp src/CircuitBoard.cpp
CLI_MAIN  := src/main.cpp
GUI_SRCS  := src/gui/GuiRenderer.cpp src/gui/Sidebar.cpp src/gui/GuiApplication.cpp src/gui/main_gui.cpp
SFML_LIBS := -lsfml-graphics -lsfml-window -lsfml-system

ifeq ($(OS),Windows_NT)
    EXE_EXT := .exe
else
    EXE_EXT :=
endif

CLI_TARGET := build/simulador$(EXE_EXT)
GUI_TARGET := build/simulador_gui$(EXE_EXT)

.PHONY: all core gui clean run run-gui

all: core gui

core: $(CLI_TARGET)

gui: $(GUI_TARGET)

$(CLI_TARGET): $(CORE_SRCS) $(CLI_MAIN)
	$(CXX) $(CXXFLAGS) $(CORE_SRCS) $(CLI_MAIN) -o $(CLI_TARGET)

$(GUI_TARGET): $(CORE_SRCS) $(GUI_SRCS)
	$(CXX) $(CXXFLAGS) $(CORE_SRCS) $(GUI_SRCS) -o $(GUI_TARGET) $(SFML_LIBS)

run: $(CLI_TARGET)
	./$(CLI_TARGET)

run-gui: $(GUI_TARGET)
	./$(GUI_TARGET)

clean:
	rm -f build/simulador build/simulador.exe build/simulador_gui build/simulador_gui.exe
