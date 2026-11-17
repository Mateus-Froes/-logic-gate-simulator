#ifndef LOGIC_SIM_COMPONENT_CPP
#define LOGIC_SIM_COMPONENT_CPP

#if __has_include("Component.h")
#include "Component.h"
#else
#include "../include/Component.h"
#endif

// Inicializa o componente com id, nome e posição
Component::Component(int compId, const std::string& compName, float x, float y)
    : id(compId), name(compName), posX(x), posY(y) {}

// Destrutor padrão
Component::~Component() = default;

// Retorna o ID
int Component::getId() const { return id; }

// Retorna o nome
std::string Component::getName() const { return name; }

// Retorna a posição X
float Component::getX() const { return posX; }

// Retorna a posição Y
float Component::getY() const { return posY; }

// Muda a posição X e Y na placa
void Component::setPosition(float x, float y) {
    posX = x;
    posY = y;
}

// Muda a posição usando um std::pair
void Component::setPosition(const std::pair<float, float>& pos) {
    setPosition(pos.first, pos.second);
}

// Imprime o componente formatado (id, nome, posição e saída)
std::ostream& operator<<(std::ostream& os, const Component& comp) {
    os << "[" << comp.id << "] " << comp.name
       << " @(" << comp.posX << ", " << comp.posY << ")";

    Component& mutableComp = const_cast<Component&>(comp);
    const Pin<bool>* out = mutableComp.getOutputPin();
    if (out != nullptr) {
        os << " -> Out: " << (out->getState() ? "HIGH (1)" : "LOW (0)");
    }
    return os;
}

#if !defined(MULTI_FILE_BUILD) && !defined(LOGIC_SIM_UNITY_INCLUDED)
#include "gui/main_gui.cpp"
#endif

#endif
