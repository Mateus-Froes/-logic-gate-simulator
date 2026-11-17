#ifndef LOGIC_SIM_COMPONENT_CPP
#define LOGIC_SIM_COMPONENT_CPP

#if __has_include("Component.h")
#include "Component.h"
#else
#include "../include/Component.h"
#endif

/** @brief Inicializa um componente com identificador, nome e coordenadas na placa. */
Component::Component(int compId, const std::string& compName, float x, float y)
    : id(compId), name(compName), posX(x), posY(y) {}

/** @brief Destrói o componente. */
Component::~Component() = default;

/** @brief Retorna o identificador único do componente. */
int Component::getId() const { return id; }

/** @brief Retorna o nome do componente. */
std::string Component::getName() const { return name; }

/** @brief Retorna a coordenada X do componente. */
float Component::getX() const { return posX; }

/** @brief Retorna a coordenada Y do componente. */
float Component::getY() const { return posY; }

/** @brief Define a posição do componente usando coordenadas X e Y. */
void Component::setPosition(float x, float y) {
    posX = x;
    posY = y;
}

/** @brief Define a posição do componente a partir de um par de coordenadas (X, Y). */
void Component::setPosition(const std::pair<float, float>& pos) {
    setPosition(pos.first, pos.second);
}

/** @brief Formata e insere as informações do componente em uma stream de saída. */
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
