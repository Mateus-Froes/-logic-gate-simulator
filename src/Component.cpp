#ifndef LOGIC_SIM_COMPONENT_CPP
#define LOGIC_SIM_COMPONENT_CPP

#if __has_include("Component.h")
#include "Component.h"
#else
#include "../include/Component.h"
#endif

// Inicializa o componente com id, nome e posição
Componente::Componente(int compId, const std::string& compNome, float x, float y)
    : id(compId), nome(compNome), posX(x), posY(y) {}

// Destrutor padrão
Componente::~Componente() = default;

// Retorna o ID
int Componente::getId() const { return id; }

// Retorna o nome
std::string Componente::getName() const { return nome; }

// Retorna a posição X
float Componente::getX() const { return posX; }

// Retorna a posição Y
float Componente::getY() const { return posY; }

// Muda a posição X e Y na placa
void Componente::setPosition(float x, float y) {
    posX = x;
    posY = y;
}

// Muda a posição usando um std::pair
void Componente::setPosition(const std::pair<float, float>& pos) {
    setPosition(pos.first, pos.second);
}

// Imprime o componente formatado (id, nome, posição e saída)
std::ostream& operator<<(std::ostream& os, const Componente& comp) {
    os << "[" << comp.id << "] " << comp.nome
       << " @(" << comp.posX << ", " << comp.posY << ")";

    Componente& compMutavel = const_cast<Componente&>(comp);
    const Pino<bool>* saida = compMutavel.getOutputPin();
    if (saida != nullptr) {
        os << " -> Out: " << (saida->getState() ? "HIGH (1)" : "LOW (0)");
    }
    return os;
}

#if !defined(MULTI_FILE_BUILD) && !defined(LOGIC_SIM_UNITY_INCLUDED)
#include "gui/main_gui.cpp"
#endif

#endif
