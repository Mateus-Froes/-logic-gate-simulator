#ifndef CIRCUIT_BOARD_H
#define CIRCUIT_BOARD_H

#include <cstddef>
#include <utility>
#include "Component.h"
#include "Wire.h"

/** @brief Dobra a capacidade de um array dinâmico de ponteiros preservando os elementos existentes. */
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

    /** @brief Dobra a capacidade do array interno de componentes. */
    void resizeComponents();

    /** @brief Dobra a capacidade do array interno de fios. */
    void resizeWires();

    /** @brief Libera toda a memória alocada para componentes e fios. */
    void clearMemory();

    /** @brief Copia profundamente todos os componentes e fios de outra placa. */
    void copyFrom(const CircuitBoard& other);

public:
    /** @brief Inicializa a placa de circuito com a capacidade inicial informada. */
    explicit CircuitBoard(size_t initialCapacity = 10);

    /** @brief Constrói uma nova placa como cópia profunda de outra placa existente. */
    CircuitBoard(const CircuitBoard& other);

    /** @brief Substitui o conteúdo desta placa por uma cópia profunda de outra placa. */
    CircuitBoard& operator=(const CircuitBoard& other);

    /** @brief Destrói a placa e libera todos os componentes e fios alocados. */
    ~CircuitBoard();

    /** @brief Instancia um novo componente na placa e retorna seu ponteiro. */
    template <typename T, typename... Args>
    T* createComponent(Args&&... args) {
        if (compCount >= compCapacity) {
            resizeComponents();
        }
        T* newComp = new T(std::forward<Args>(args)...);
        components[compCount++] = newComp;
        return newComp;
    }

    /** @brief Cria um fio conectando a saída do componente de origem à entrada do componente de destino. */
    bool connect(Component* src, Component* dst, size_t dstPinIndex = 0);

    /** @brief Cria um fio conectando dois componentes identificados pelos seus IDs. */
    bool connect(int srcId, int dstId, size_t dstPinIndex = 0);

    /** @brief Remove o fio conectado ao pino de entrada especificado de um componente pelo ID. */
    bool disconnect(int dstId, size_t dstPinIndex = 0);

    /** @brief Remove o fio conectado ao pino de entrada especificado de um componente pelo ponteiro. */
    bool disconnect(Component* dst, size_t dstPinIndex = 0);

    /** @brief Remove todos os fios que partem do pino de saída do componente informado. */
    size_t disconnectOutput(int srcId);

    /** @brief Remove um fio específico a partir do seu índice no array de fios. */
    bool removeWireAt(size_t wireIndex);

    /** @brief Remove um componente pelo ID juntamente com todos os fios conectados a ele. */
    bool removeComponent(int id);

    /** @brief Remove um componente pelo ponteiro juntamente com todos os fios conectados a ele. */
    bool removeComponent(Component* comp);

    /** @brief Remove todos os componentes e fios, deixando a placa vazia. */
    void clear();

    /** @brief Busca e retorna um componente pelo seu ID, ou nullptr se não encontrado. */
    Component* findComponent(int id) const;

    /** @brief Executa a avaliação dos componentes e a propagação de sinais pelos fios até estabilizar o circuito. */
    void stepSimulation();

    /** @brief Exibe no console o estado atual de todos os componentes da placa. */
    void printCircuitStatus() const;

    /** @brief Retorna a quantidade atual de componentes na placa. */
    size_t getComponentCount() const;

    /** @brief Retorna o componente armazenado no índice informado, ou nullptr se fora do intervalo. */
    Component* getComponent(size_t index) const;

    /** @brief Retorna a quantidade atual de fios na placa. */
    size_t getWireCount() const;

    /** @brief Retorna o fio armazenado no índice informado, ou nullptr se fora do intervalo. */
    const Wire* getWire(size_t index) const;

    /** @brief Retorna o próximo identificador numérico disponível para criação de componente. */
    int nextFreeId() const;

    /** @brief Verifica se o pino de entrada especificado de um componente já possui um fio conectado. */
    bool isInputConnected(int dstId, size_t pinIndex) const;
};

#endif
