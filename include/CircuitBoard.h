#ifndef CIRCUIT_BOARD_H
#define CIRCUIT_BOARD_H

#include <cstddef>
#include <utility>
#include "Component.h"
#include "Wire.h"

// Função auxiliar pra dobrar o tamanho dos arrays dinâmicos quando lotam
template <typename T>
void aumentarArray(T**& vetor, size_t qtd, size_t& capacidade) {
    capacidade = (capacidade == 0) ? 1 : capacidade * 2;
    T** maior = new T*[capacidade];
    for (size_t i = 0; i < qtd; ++i) {
        maior[i] = vetor[i];
    }
    delete[] vetor;
    vetor = maior;
}

class Placa {
private:
    Componente** listaComp;
    size_t qtdComp;
    size_t capComp;

    Fio** listaFios;
    size_t qtdFios;
    size_t capFios;

    // Aumenta o array de componentes
    void resizeComponents();

    // Aumenta o array de fios
    void resizeWires();

    // Deleta todos os componentes e fios e libera os arrays
    void clearMemory();

    // Faz a cópia profunda (deep copy) de outra placa pra esta
    void copyFrom(const Placa& outra);

public:
    // Cria a placa já alocando espaço inicial pros arrays
    explicit Placa(size_t capacidadeInicial = 10);

    // Construtor de cópia
    Placa(const Placa& outra);

    // Operador de atribuição (limpa a placa atual e copia a outra)
    Placa& operator=(const Placa& outra);

    // Destrutor: libera toda a memória usada pela placa
    ~Placa();

    // Cria um componente novo na placa e retorna o ponteiro pra ele
    template <typename T, typename... Args>
    T* createComponent(Args&&... args) {
        if (qtdComp >= capComp) {
            resizeComponents();
        }
        T* novoComp = new T(std::forward<Args>(args)...);
        listaComp[qtdComp++] = novoComp;
        return novoComp;
    }

    // Conecta a saída de src na entrada de dst
    bool connect(Componente* src, Componente* dst, size_t dstPinIndex = 0);

    // Conecta dois componentes procurando eles pelo ID
    bool connect(int srcId, int dstId, size_t dstPinIndex = 0);

    // Remove o fio ligado nessa entrada específica (pelo ID)
    bool disconnect(int dstId, size_t dstPinIndex = 0);

    // Remove o fio ligado nessa entrada (pelo ponteiro)
    bool disconnect(Componente* dst, size_t dstPinIndex = 0);

    // Tira todos os fios que estão saindo desse componente
    size_t disconnectOutput(int srcId);

    // Remove um fio pelo índice dele no vetor
    bool removeWireAt(size_t wireIndex);

    // Apaga o componente com esse ID e todos os fios ligados nele
    bool removeComponent(int id);

    // Apaga o componente passado e os fios dele
    bool removeComponent(Componente* comp);

    // Limpa a placa inteira (apaga componentes e fios)
    void clear();

    // Procura um componente pelo ID (retorna nullptr se não achar)
    Componente* findComponent(int id) const;

    // Roda a simulação várias vezes até os sinais estabilizarem no circuito
    void stepSimulation();

    // Mostra no terminal como está cada componente da placa
    void printCircuitStatus() const;

    // Quantos componentes existem na placa
    size_t getComponentCount() const;

    // Pega um componente pela posição no array
    Componente* getComponent(size_t index) const;

    // Quantos fios existem na placa
    size_t getWireCount() const;

    // Pega um fio pela posição no array
    const Fio* getWire(size_t index) const;

    // Descobre o próximo ID livre pra usar num componente novo
    int nextFreeId() const;

    // Checa se já tem um fio chegando nesse pino de entrada
    bool isInputConnected(int dstId, size_t pinIndex) const;
};

#endif
