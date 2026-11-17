#ifndef LOGIC_SIM_CIRCUITBOARD_CPP
#define LOGIC_SIM_CIRCUITBOARD_CPP

#if __has_include("CircuitBoard.h")
#include "CircuitBoard.h"
#else
#include "../include/CircuitBoard.h"
#endif
#include <iostream>

/** @brief Inicializa a placa de circuito com a capacidade inicial informada. */
CircuitBoard::CircuitBoard(size_t initialCapacity)
    : components(nullptr), compCount(0), compCapacity(initialCapacity),
      wires(nullptr), wireCount(0), wireCapacity(initialCapacity) {
    components = new Component*[compCapacity];
    wires = new Wire*[wireCapacity];
}

/** @brief Constrói uma nova placa como cópia profunda de outra placa existente. */
CircuitBoard::CircuitBoard(const CircuitBoard& other)
    : components(nullptr), compCount(0), compCapacity(0),
      wires(nullptr), wireCount(0), wireCapacity(0) {
    copyFrom(other);
}

/** @brief Substitui o conteúdo desta placa por uma cópia profunda de outra placa. */
CircuitBoard& CircuitBoard::operator=(const CircuitBoard& other) {
    if (this != &other) {
        clearMemory();
        copyFrom(other);
    }
    return *this;
}

/** @brief Destrói a placa e libera todos os componentes e fios alocados. */
CircuitBoard::~CircuitBoard() {
    clearMemory();
}

/** @brief Libera toda a memória alocada para componentes e fios. */
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

/** @brief Copia profundamente todos os componentes e fios de outra placa. */
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

/** @brief Dobra a capacidade do array interno de componentes. */
void CircuitBoard::resizeComponents() {
    growArray(components, compCount, compCapacity);
}

/** @brief Dobra a capacidade do array interno de fios. */
void CircuitBoard::resizeWires() {
    growArray(wires, wireCount, wireCapacity);
}

/** @brief Busca e retorna um componente pelo seu ID, ou nullptr se não encontrado. */
Component* CircuitBoard::findComponent(int id) const {
    for (size_t i = 0; i < compCount; ++i) {
        if (components[i]->getId() == id) return components[i];
    }
    return nullptr;
}

/** @brief Cria um fio conectando a saída do componente de origem à entrada do componente de destino. */
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

/** @brief Cria um fio conectando dois componentes identificados pelos seus IDs. */
bool CircuitBoard::connect(int srcId, int dstId, size_t dstPinIndex) {
    return connect(findComponent(srcId), findComponent(dstId), dstPinIndex);
}

/** @brief Remove um fio específico a partir do seu índice no array de fios. */
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

/** @brief Remove o fio conectado ao pino de entrada especificado de um componente pelo ID. */
bool CircuitBoard::disconnect(int dstId, size_t dstPinIndex) {
    for (size_t i = 0; i < wireCount; ++i) {
        if (wires[i]->getDestId() == dstId && wires[i]->getDestPinIndex() == dstPinIndex) {
            return removeWireAt(i);
        }
    }
    return false;
}

/** @brief Remove o fio conectado ao pino de entrada especificado de um componente pelo ponteiro. */
bool CircuitBoard::disconnect(Component* dst, size_t dstPinIndex) {
    return dst ? disconnect(dst->getId(), dstPinIndex) : false;
}

/** @brief Remove todos os fios que partem do pino de saída do componente informado. */
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

/** @brief Remove um componente pelo ID juntamente com todos os fios conectados a ele. */
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

/** @brief Remove um componente pelo ponteiro juntamente com todos os fios conectados a ele. */
bool CircuitBoard::removeComponent(Component* comp) {
    return comp ? removeComponent(comp->getId()) : false;
}

/** @brief Remove todos os componentes e fios, deixando a placa vazia. */
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

/** @brief Executa a avaliação dos componentes e a propagação de sinais pelos fios até estabilizar o circuito. */
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

/** @brief Exibe no console o estado atual de todos os componentes da placa. */
void CircuitBoard::printCircuitStatus() const {
    std::cout << "=== Estado da Placa (" << compCount << " componentes, "
              << wireCount << " trilhas) ===\n";
    for (size_t i = 0; i < compCount; ++i) {
        std::cout << "  " << *components[i] << "\n";
    }
}

/** @brief Retorna a quantidade atual de componentes na placa. */
size_t CircuitBoard::getComponentCount() const { return compCount; }

/** @brief Retorna o componente armazenado no índice informado, ou nullptr se fora do intervalo. */
Component* CircuitBoard::getComponent(size_t index) const {
    return (index < compCount) ? components[index] : nullptr;
}

/** @brief Retorna a quantidade atual de fios na placa. */
size_t CircuitBoard::getWireCount() const { return wireCount; }

/** @brief Retorna o fio armazenado no índice informado, ou nullptr se fora do intervalo. */
const Wire* CircuitBoard::getWire(size_t index) const {
    return (index < wireCount) ? wires[index] : nullptr;
}

/** @brief Retorna o próximo identificador numérico disponível para criação de componente. */
int CircuitBoard::nextFreeId() const {
    int maxId = 0;
    for (size_t i = 0; i < compCount; ++i) {
        if (components[i]->getId() > maxId) maxId = components[i]->getId();
    }
    return maxId + 1;
}

/** @brief Verifica se o pino de entrada especificado de um componente já possui um fio conectado. */
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
