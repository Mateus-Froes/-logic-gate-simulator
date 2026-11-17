#define LOGIC_SIM_UNITY_INCLUDED

#include <exception>
#include <iostream>
#include <string>
#if __has_include("CircuitBoard.h")
#include "CircuitBoard.h"
#include "gui/GuiApplication.h"
#else
#include "../../include/CircuitBoard.h"
#include "../../include/gui/GuiApplication.h"
#endif

/** @brief Ponto de entrada da aplicação gráfica do simulador de portas lógicas. */
int main(int argc, char* argv[]) {
    try {
        CircuitBoard board;
        GuiApplication app(board);

        if (argc >= 3 && std::string(argv[1]) == "--snapshot") {
            return app.saveSnapshot(argv[2]) ? 0 : 1;
        }

        app.run();
    } catch (const std::exception& e) {
        std::cerr << "Erro: " << e.what() << "\n";
        return 1;
    }
    return 0;
}

#ifndef MULTI_FILE_BUILD
#include "../Component.cpp"
#include "../Wire.cpp"
#include "../Gates.cpp"
#include "../CircuitBoard.cpp"
#include "GuiRenderer.cpp"
#include "Sidebar.cpp"
#include "GuiApplication.cpp"
#endif
