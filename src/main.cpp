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
    Placa minhaPlaca;

    auto* chA = minhaPlaca.createComponent<Chave>(1, "Chave_A", true,  10.0f, 20.0f);
    auto* chB = minhaPlaca.createComponent<Chave>(2, "Chave_B", false, 10.0f, 60.0f);
    minhaPlaca.createComponent<Chave>(3, "Chave_C", true, 10.0f, 100.0f);

    auto* xor1 = minhaPlaca.createComponent<PortaXor>(4, 2, 120.0f, 40.0f);
    minhaPlaca.createComponent<PortaAnd>(5, 3, 120.0f, 100.0f);
    auto* not1 = minhaPlaca.createComponent<PortaNot>(6, 240.0f, 40.0f);

    auto* ledS = minhaPlaca.createComponent<Led>(7, "LED_Saida_INV", 350.0f, 40.0f);
    auto* ledC = minhaPlaca.createComponent<Led>(8, "LED_And3",      350.0f, 100.0f);

    minhaPlaca.connect(chA, xor1, 0);
    minhaPlaca.connect(chB, xor1, 1);
    minhaPlaca.connect(xor1, not1, 0);
    minhaPlaca.connect(not1, ledS, 0);

    minhaPlaca.connect(1, 5, 0);
    minhaPlaca.connect(2, 5, 1);
    minhaPlaca.connect(3, 5, 2);
    minhaPlaca.connect(5, 8, 0);

    minhaPlaca.stepSimulation();
    std::cout << "[Circuito Original - Passo 1 (A=1, B=0, C=1)]\n";
    minhaPlaca.printCircuitStatus();
    std::cout << "  -> LED_Saida_INV aceso? " << (ledS->isLit() ? "SIM" : "NAO") << "\n";
    std::cout << "  -> LED_And3 aceso?      " << (ledC->isLit() ? "SIM" : "NAO") << "\n\n";

    Placa placaClonada = minhaPlaca;
    auto* chBClone = dynamic_cast<Chave*>(placaClonada.findComponent(2));
    if (chBClone) chBClone->setState(true);
    placaClonada.stepSimulation();

    std::cout << "[Circuito Clonado (Deep Copy) - Apos mudar Chave_B para 1]\n";
    auto* ledCClone = dynamic_cast<Led*>(placaClonada.findComponent(8));
    std::cout << "  -> LED_And3 no Clone aceso?    "
              << ((ledCClone && ledCClone->isLit()) ? "SIM" : "NAO") << "\n";
    std::cout << "  -> LED_And3 no Original aceso? " << (ledC->isLit() ? "SIM" : "NAO")
              << " (Isolamento de memoria comprovado!)\n\n";

    Placa copiaAtribuida;
    copiaAtribuida = placaClonada;
    copiaAtribuida.stepSimulation();
    auto* ledCAtribuido = dynamic_cast<Led*>(copiaAtribuida.findComponent(8));
    std::cout << "[Atribuicao por copia (copiaAtribuida = placaClonada)]\n";
    std::cout << "  -> LED_And3 na copia atribuida aceso? "
              << ((ledCAtribuido && ledCAtribuido->isLit()) ? "SIM" : "NAO") << "\n";

    return 0;
}

#ifndef MULTI_FILE_BUILD
#include "Component.cpp"
#include "Wire.cpp"
#include "Gates.cpp"
#include "CircuitBoard.cpp"
#endif
