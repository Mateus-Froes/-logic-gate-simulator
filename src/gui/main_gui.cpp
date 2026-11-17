#include <exception>
#include <iostream>
#include <string>
#include "CircuitBoard.h"
#include "gui/GuiApplication.h"

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
