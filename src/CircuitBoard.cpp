#ifndef LOGIC_SIM_CIRCUITBOARD_CPP
#define LOGIC_SIM_CIRCUITBOARD_CPP

#if __has_include("CircuitBoard.h")
#include "CircuitBoard.h"
#else
#include "../include/CircuitBoard.h"
#endif
#include <iostream>

// Aloca os arrays de componentes e fios com a capacidade inicial
CircuitBoard::CircuitBoard(size_t initialCapacity)
    : components(nullptr), compCount(0), compCapacity(initialCapacity),
      wires(nullptr), wireCount(0), wireCapacity(initialCapacity) {
    components = new Component*[compCapacity];
    wires = new Wire*[wireCapacity];
}

// Construtor de cópia: cria uma placa nova copiando tudo da outra
CircuitBoard::CircuitBoard(const CircuitBoard& other)
    : components(nullptr), compCount(0), compCapacity(0),
      wires(nullptr), wireCount(0), wireCapacity(0) {
    copyFrom(other);
}

// Operador de atribuição: limpa o que tinha antes e copia da outra placa
CircuitBoard& CircuitBoard::operator=(const CircuitBoard& other) {
    if (this != &other) {
        clearMemory();
        copyFrom(other);
    }
    return *this;
}

// Destrutor: apaga toda a memória alocada dinamicamente
CircuitBoard::~CircuitBoard() {
    clearMemory();
}

// Deleta cada fio e cada componente e depois libera os vetores
void CircuitBoard::clearMemory() {
    for (size_t i = 0; i < wireCount; ++i) {
        delete wires[i];
    }
    delete[] wires;

    for (size_t i = 0; i < compCount; ++i) {
        delete components[i];
    }
    delete[] components;

    components = nullptr;
    wires = nullptr;
    compCount = 0;
    wireCount = 0;
    compCapacity = 0;
    wireCapacity = 0;
}

// Clona os componentes da outra placa e reconecta os fios nos clones novos
void CircuitBoard::copyFrom(const CircuitBoard& other) {
    compCapacity = other.compCapacity;
    compCount = other.compCount;
    components = new Component*[compCapacity];
    for (size_t i = 0; i < compCount; ++i) {
        components[i] = other.components[i]->clone();
    }

    wireCapacity = other.wireCapacity;
    wireCount = other.wireCount;
    wires = new Wire*[wireCapacity];
    for (size_t i = 0; i < wireCount; ++i) {
        const Wire* oldWire = other.wires[i];
        Component* newSrc = findComponent(oldWire->getSourceId());
        Component* newDst = findComponent(oldWire->getDestId());

        wires[i] = new Wire(
            oldWire->getSourceId(),
            newSrc ? newSrc->getOutputPin() : nullptr,
            oldWire->getDestId(),
            newDst ? newDst->getInputPin(oldWire->getDestPinIndex()) : nullptr,
            oldWire->getDestPinIndex());
    }
}

// Dobra o espaço do array de componentes
void CircuitBoard::resizeComponents() {
    growArray(components, compCount, compCapacity);
}

// Dobra o espaço do array de fios
void CircuitBoard::resizeWires() {
    growArray(wires, wireCount, wireCapacity);
}

// Procura um componente pelo ID no array
Component* CircuitBoard::findComponent(int id) const {
    for (size_t i = 0; i < compCount; ++i) {
        if (components[i]->getId() == id) return components[i];
    }
    return nullptr;
}

// Cria um fio ligando a saída de src na entrada dstPinIndex de dst
bool CircuitBoard::connect(Component* src, Component* dst, size_t dstPinIndex) {
    if (!src || !dst || src == dst) return false;

    Pin<bool>* outPin = src->getOutputPin();
    Pin<bool>* inPin = dst->getInputPin(dstPinIndex);
    if (!outPin || !inPin) return false;

    const Wire candidate(src->getId(), nullptr, dst->getId(), nullptr, dstPinIndex);
    for (size_t i = 0; i < wireCount; ++i) {
        if (*wires[i] == candidate) {
            return false;
        }
    }

    if (wireCount >= wireCapacity) resizeWires();
    wires[wireCount++] = new Wire(src->getId(), outPin, dst->getId(), inPin, dstPinIndex);
    return true;
}

// Faz a conexão buscando os componentes pelo ID
bool CircuitBoard::connect(int srcId, int dstId, size_t dstPinIndex) {
    return connect(findComponent(srcId), findComponent(dstId), dstPinIndex);
}

// Apaga um fio do array e atualiza o status dos pinos que estavam nele
bool CircuitBoard::removeWireAt(size_t wireIndex) {
    if (wireIndex >= wireCount) return false;

    const int srcId = wires[wireIndex]->getSourceId();
    wires[wireIndex]->disconnectInputPin();
    delete wires[wireIndex];

    for (size_t j = wireIndex; j + 1 < wireCount; ++j) {
        wires[j] = wires[j + 1];
    }
    --wireCount;

    if (Component* src = findComponent(srcId)) {
        if (Pin<bool>* outPin = src->getOutputPin()) {
            bool stillConnected = false;
            for (size_t i = 0; i < wireCount; ++i) {
                if (wires[i]->getSourceId() == srcId) {
                    stillConnected = true;
                    break;
                }
            }
            outPin->setConnected(stillConnected);
        }
    }
    return true;
}

// Acha e remove o fio ligado nessa entrada específica do componente
bool CircuitBoard::disconnect(int dstId, size_t dstPinIndex) {
    for (size_t i = 0; i < wireCount; ++i) {
        if (wires[i]->getDestId() == dstId && wires[i]->getDestPinIndex() == dstPinIndex) {
            return removeWireAt(i);
        }
    }
    return false;
}

// Desconecta a entrada passando o ponteiro do componente
bool CircuitBoard::disconnect(Component* dst, size_t dstPinIndex) {
    return dst ? disconnect(dst->getId(), dstPinIndex) : false;
}

// Remove todos os fios que saem do componente com esse ID
size_t CircuitBoard::disconnectOutput(int srcId) {
    size_t removed = 0;
    for (size_t i = wireCount; i-- > 0;) {
        if (wires[i]->getSourceId() == srcId) {
            removeWireAt(i);
            ++removed;
        }
    }
    return removed;
}

// Tira todos os fios ligados no componente e depois deleta ele do array
bool CircuitBoard::removeComponent(int id) {
    size_t compIndex = compCount;
    for (size_t i = 0; i < compCount; ++i) {
        if (components[i]->getId() == id) {
            compIndex = i;
            break;
        }
    }
    if (compIndex >= compCount) return false;

    for (size_t i = wireCount; i-- > 0;) {
        if (wires[i]->getSourceId() == id || wires[i]->getDestId() == id) {
            removeWireAt(i);
        }
    }

    delete components[compIndex];
    for (size_t i = compIndex; i + 1 < compCount; ++i) {
        components[i] = components[i + 1];
    }
    --compCount;
    return true;
}

// Remove o componente passando o ponteiro dele
bool CircuitBoard::removeComponent(Component* comp) {
    return comp ? removeComponent(comp->getId()) : false;
}

// Apaga todos os componentes e fios, zerando as contagens da placa
void CircuitBoard::clear() {
    for (size_t i = 0; i < wireCount; ++i) {
        delete wires[i];
    }
    wireCount = 0;

    for (size_t i = 0; i < compCount; ++i) {
        delete components[i];
    }
    compCount = 0;
}

// Avalia as portas e propaga os fios várias vezes até o circuito todo atualizar
void CircuitBoard::stepSimulation() {
    for (size_t pass = 0; pass < compCount; ++pass) {
        for (size_t i = 0; i < compCount; ++i) {
            components[i]->evaluate();
        }
        for (size_t i = 0; i < wireCount; ++i) {
            wires[i]->propagate();
        }
    }
}

// Imprime todos os componentes e o resumo da placa no cout
void CircuitBoard::printCircuitStatus() const {
    std::cout << "=== Estado da Placa (" << compCount << " componentes, "
              << wireCount << " trilhas) ===\n";
    for (size_t i = 0; i < compCount; ++i) {
        std::cout << "  " << *components[i] << "\n";
    }
}

// Retorna o total de componentes
size_t CircuitBoard::getComponentCount() const { return compCount; }

// Retorna o componente na posição index (ou nullptr se for inválido)
Component* CircuitBoard::getComponent(size_t index) const {
    return (index < compCount) ? components[index] : nullptr;
}

// Retorna o total de fios
size_t CircuitBoard::getWireCount() const { return wireCount; }

// Retorna o fio na posição index (ou nullptr se for inválido)
const Wire* CircuitBoard::getWire(size_t index) const {
    return (index < wireCount) ? wires[index] : nullptr;
}

// Pega o maior ID usado até agora e soma 1 pra gerar um novo
int CircuitBoard::nextFreeId() const {
    int maxId = 0;
    for (size_t i = 0; i < compCount; ++i) {
        if (components[i]->getId() > maxId) maxId = components[i]->getId();
    }
    return maxId + 1;
}

// Verifica se já existe algum fio ligado nesse pino de entrada
bool CircuitBoard::isInputConnected(int dstId, size_t pinIndex) const {
    for (size_t i = 0; i < wireCount; ++i) {
        if (wires[i]->getDestId() == dstId && wires[i]->getDestPinIndex() == pinIndex) {
            return true;
        }
    }
    return false;
}

#if !defined(MULTI_FILE_BUILD) && !defined(LOGIC_SIM_UNITY_INCLUDED)
#include "gui/main_gui.cpp"
#endif

#endif
