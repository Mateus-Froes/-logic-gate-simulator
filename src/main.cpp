#define LOGIC_SIM_UNITY_INCLUDED

#include <iostream>
#if __has_include("CircuitBoard.h")
#include "CircuitBoard.h"
#include "Gates.h"
#else
#include "../include/CircuitBoard.h"
#include "../include/Gates.h"
#endif

// Main de teste pelo terminal: monta um circuito e testa a cópia profunda da placa
int main() {
    CircuitBoard board;

    auto* swA  = board.createComponent<InputSwitch>(1, "Switch_A", true,  10.0f, 20.0f);
    auto* swB  = board.createComponent<InputSwitch>(2, "Switch_B", false, 10.0f, 60.0f);
    board.createComponent<InputSwitch>(3, "Switch_C", true, 10.0f, 100.0f);

    auto* xor1 = board.createComponent<XorGate>(4, 2, 120.0f, 40.0f);
    board.createComponent<AndGate>(5, 3, 120.0f, 100.0f);
    auto* not1 = board.createComponent<NotGate>(6, 240.0f, 40.0f);

    auto* ledS = board.createComponent<OutputLED>(7, "LED_Saida_INV", 350.0f, 40.0f);
    auto* ledC = board.createComponent<OutputLED>(8, "LED_And3",      350.0f, 100.0f);

    board.connect(swA, xor1, 0);
    board.connect(swB, xor1, 1);
    board.connect(xor1, not1, 0);
    board.connect(not1, ledS, 0);

    board.connect(1, 5, 0);
    board.connect(2, 5, 1);
    board.connect(3, 5, 2);
    board.connect(5, 8, 0);

    board.stepSimulation();
    std::cout << "[Circuito Original - Passo 1 (A=1, B=0, C=1)]\n";
    board.printCircuitStatus();
    std::cout << "  -> LED_Saida_INV aceso? " << (ledS->isLit() ? "SIM" : "NAO") << "\n";
    std::cout << "  -> LED_And3 aceso?      " << (ledC->isLit() ? "SIM" : "NAO") << "\n\n";

    CircuitBoard clonedBoard = board;
    auto* clonedSwB = dynamic_cast<InputSwitch*>(clonedBoard.findComponent(2));
    if (clonedSwB) clonedSwB->setState(true);
    clonedBoard.stepSimulation();

    std::cout << "[Circuito Clonado (Deep Copy) - Apos mudar Switch_B para 1]\n";
    auto* clonedLedC = dynamic_cast<OutputLED*>(clonedBoard.findComponent(8));
    std::cout << "  -> LED_And3 no Clone aceso?    "
              << ((clonedLedC && clonedLedC->isLit()) ? "SIM" : "NAO") << "\n";
    std::cout << "  -> LED_And3 no Original aceso? " << (ledC->isLit() ? "SIM" : "NAO")
              << " (Isolamento de memoria comprovado!)\n\n";

    CircuitBoard assigned;
    assigned = clonedBoard;
    assigned.stepSimulation();
    auto* assignedLedC = dynamic_cast<OutputLED*>(assigned.findComponent(8));
    std::cout << "[Atribuicao por copia (assigned = clonedBoard)]\n";
    std::cout << "  -> LED_And3 na copia atribuida aceso? "
              << ((assignedLedC && assignedLedC->isLit()) ? "SIM" : "NAO") << "\n";

    return 0;
}

#ifndef MULTI_FILE_BUILD
#include "Component.cpp"
#include "Wire.cpp"
#include "Gates.cpp"
#include "CircuitBoard.cpp"
#endif
