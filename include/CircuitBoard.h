#ifndef CIRCUIT_BOARD_H
#define CIRCUIT_BOARD_H

#include <cstddef>
#include <utility>
#include "Component.h"
#include "Wire.h"

// Função auxiliar pra dobrar o tamanho dos arrays dinâmicos quando lotam
template <typename T>
void growArray(T**& array, size_t count, size_t& capacity) {
    capacity = (capacity == 0) ? 1 : capacity * 2;
    T** bigger = new T*[capacity];
    for (size_t i = 0; i < count; ++i) {
        bigger[i] = array[i];
    }
    delete[] array;
    array = bigger;
}

class CircuitBoard {
private:
    Component** components;
    size_t compCount;
    size_t compCapacity;

    Wire** wires;
    size_t wireCount;
    size_t wireCapacity;

    // Aumenta o array de componentes
    void resizeComponents();

    // Aumenta o array de fios
    void resizeWires();

    // Deleta todos os componentes e fios e libera os arrays
    void clearMemory();

    // Faz a cópia profunda (deep copy) de outra placa pra esta
    void copyFrom(const CircuitBoard& other);

public:
    // Cria a placa já alocando espaço inicial pros arrays
    explicit CircuitBoard(size_t initialCapacity = 10);

    // Construtor de cópia
    CircuitBoard(const CircuitBoard& other);

    // Operador de atribuição (limpa a placa atual e copia a outra)
    CircuitBoard& operator=(const CircuitBoard& other);

    // Destrutor: libera toda a memória usada pela placa
    ~CircuitBoard();

    // Cria um componente novo na placa e retorna o ponteiro pra ele
    template <typename T, typename... Args>
    T* createComponent(Args&&... args) {
        if (compCount >= compCapacity) {
            resizeComponents();
        }
        T* newComp = new T(std::forward<Args>(args)...);
        components[compCount++] = newComp;
        return newComp;
    }

    // Conecta a saída de src na entrada de dst
    bool connect(Component* src, Component* dst, size_t dstPinIndex = 0);

    // Conecta dois componentes procurando eles pelo ID
    bool connect(int srcId, int dstId, size_t dstPinIndex = 0);

    // Remove o fio ligado nessa entrada específica (pelo ID)
    bool disconnect(int dstId, size_t dstPinIndex = 0);

    // Remove o fio ligado nessa entrada (pelo ponteiro)
    bool disconnect(Component* dst, size_t dstPinIndex = 0);

    // Tira todos os fios que estão saindo desse componente
    size_t disconnectOutput(int srcId);

    // Remove um fio pelo índice dele no vetor
    bool removeWireAt(size_t wireIndex);

    // Apaga o componente com esse ID e todos os fios ligados nele
    bool removeComponent(int id);

    // Apaga o componente passado e os fios dele
    bool removeComponent(Component* comp);

    // Limpa a placa inteira (apaga componentes e fios)
    void clear();

    // Procura um componente pelo ID (retorna nullptr se não achar)
    Component* findComponent(int id) const;

    // Roda a simulação várias vezes até os sinais estabilizarem no circuito
    void stepSimulation();

    // Mostra no terminal como está cada componente da placa
    void printCircuitStatus() const;

    // Quantos componentes existem na placa
    size_t getComponentCount() const;

    // Pega um componente pela posição no array
    Component* getComponent(size_t index) const;

    // Quantos fios existem na placa
    size_t getWireCount() const;

    // Pega um fio pela posição no array
    const Wire* getWire(size_t index) const;

    // Descobre o próximo ID livre pra usar num componente novo
    int nextFreeId() const;

    // Checa se já tem um fio chegando nesse pino de entrada
    bool isInputConnected(int dstId, size_t pinIndex) const;
};

#endif
