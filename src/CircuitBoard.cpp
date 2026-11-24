#ifndef LOGIC_SIM_CIRCUITBOARD_CPP
#define LOGIC_SIM_CIRCUITBOARD_CPP

#if __has_include("CircuitBoard.h")
#include "CircuitBoard.h"
#else
#include "../include/CircuitBoard.h"
#endif
#include <iostream>

// Aloca os arrays de componentes e fios com a capacidade inicial
Placa::Placa(size_t capacidadeInicial)
    : listaComp(nullptr), qtdComp(0), capComp(capacidadeInicial),
      listaFios(nullptr), qtdFios(0), capFios(capacidadeInicial) {
    listaComp = new Componente*[capComp];
    listaFios = new Fio*[capFios];
}

// Construtor de cópia: cria uma placa nova copiando tudo da outra
Placa::Placa(const Placa& outra)
    : listaComp(nullptr), qtdComp(0), capComp(0),
      listaFios(nullptr), qtdFios(0), capFios(0) {
    copyFrom(outra);
}

// Operador de atribuição: limpa o que tinha antes e copia da outra placa
Placa& Placa::operator=(const Placa& outra) {
    if (this != &outra) {
        clearMemory();
        copyFrom(outra);
    }
    return *this;
}

// Destrutor: apaga toda a memória alocada dinamicamente
Placa::~Placa() {
    clearMemory();
}

// Deleta cada fio e cada componente e depois libera os vetores
void Placa::clearMemory() {
    for (size_t i = 0; i < qtdFios; ++i) {
        delete listaFios[i];
    }
    delete[] listaFios;

    for (size_t i = 0; i < qtdComp; ++i) {
        delete listaComp[i];
    }
    delete[] listaComp;

    listaComp = nullptr;
    listaFios = nullptr;
    qtdComp = 0;
    qtdFios = 0;
    capComp = 0;
    capFios = 0;
}

// Clona os componentes da outra placa e reconecta os fios nos clones novos
void Placa::copyFrom(const Placa& outra) {
    capComp = outra.capComp;
    qtdComp = outra.qtdComp;
    listaComp = new Componente*[capComp];
    for (size_t i = 0; i < qtdComp; ++i) {
        listaComp[i] = outra.listaComp[i]->clone();
    }

    capFios = outra.capFios;
    qtdFios = outra.qtdFios;
    listaFios = new Fio*[capFios];
    for (size_t i = 0; i < qtdFios; ++i) {
        const Fio* fioAntigo = outra.listaFios[i];
        Componente* novaOrigem = findComponent(fioAntigo->getSourceId());
        Componente* novoDestino = findComponent(fioAntigo->getDestId());

        listaFios[i] = new Fio(
            fioAntigo->getSourceId(),
            novaOrigem ? novaOrigem->getOutputPin() : nullptr,
            fioAntigo->getDestId(),
            novoDestino ? novoDestino->getInputPin(fioAntigo->getDestPinIndex()) : nullptr,
            fioAntigo->getDestPinIndex());
    }
}

// Dobra o espaço do array de componentes
void Placa::resizeComponents() {
    aumentarArray(listaComp, qtdComp, capComp);
}

// Dobra o espaço do array de fios
void Placa::resizeWires() {
    aumentarArray(listaFios, qtdFios, capFios);
}

// Procura um componente pelo ID no array
Componente* Placa::findComponent(int id) const {
    for (size_t i = 0; i < qtdComp; ++i) {
        if (listaComp[i]->getId() == id) return listaComp[i];
    }
    return nullptr;
}

// Cria um fio ligando a saída de src na entrada dstPinIndex de dst
bool Placa::connect(Componente* src, Componente* dst, size_t dstPinIndex) {
    if (!src || !dst || src == dst) return false;

    Pino<bool>* pinoOut = src->getOutputPin();
    Pino<bool>* pinoIn = dst->getInputPin(dstPinIndex);
    if (!pinoOut || !pinoIn) return false;

    const Fio candidato(src->getId(), nullptr, dst->getId(), nullptr, dstPinIndex);
    for (size_t i = 0; i < qtdFios; ++i) {
        if (*listaFios[i] == candidato) {
            return false;
        }
    }

    if (qtdFios >= capFios) resizeWires();
    listaFios[qtdFios++] = new Fio(src->getId(), pinoOut, dst->getId(), pinoIn, dstPinIndex);
    return true;
}

// Faz a conexão buscando os componentes pelo ID
bool Placa::connect(int srcId, int dstId, size_t dstPinIndex) {
    return connect(findComponent(srcId), findComponent(dstId), dstPinIndex);
}

// Apaga um fio do array e atualiza o status dos pinos que estavam nele
bool Placa::removeWireAt(size_t wireIndex) {
    if (wireIndex >= qtdFios) return false;

    const int srcId = listaFios[wireIndex]->getSourceId();
    listaFios[wireIndex]->disconnectInputPin();
    delete listaFios[wireIndex];

    for (size_t j = wireIndex; j + 1 < qtdFios; ++j) {
        listaFios[j] = listaFios[j + 1];
    }
    --qtdFios;

    if (Componente* src = findComponent(srcId)) {
        if (Pino<bool>* pinoOut = src->getOutputPin()) {
            bool aindaLigado = false;
            for (size_t i = 0; i < qtdFios; ++i) {
                if (listaFios[i]->getSourceId() == srcId) {
                    aindaLigado = true;
                    break;
                }
            }
            pinoOut->setConnected(aindaLigado);
        }
    }
    return true;
}

// Acha e remove o fio ligado nessa entrada específica do componente
bool Placa::disconnect(int dstId, size_t dstPinIndex) {
    for (size_t i = 0; i < qtdFios; ++i) {
        if (listaFios[i]->getDestId() == dstId && listaFios[i]->getDestPinIndex() == dstPinIndex) {
            return removeWireAt(i);
        }
    }
    return false;
}

// Desconecta a entrada passando o ponteiro do componente
bool Placa::disconnect(Componente* dst, size_t dstPinIndex) {
    return dst ? disconnect(dst->getId(), dstPinIndex) : false;
}

// Remove todos os fios que saem do componente com esse ID
size_t Placa::disconnectOutput(int srcId) {
    size_t removidos = 0;
    for (size_t i = qtdFios; i-- > 0;) {
        if (listaFios[i]->getSourceId() == srcId) {
            removeWireAt(i);
            ++removidos;
        }
    }
    return removidos;
}

// Tira todos os fios ligados no componente e depois deleta ele do array
bool Placa::removeComponent(int id) {
    size_t posComp = qtdComp;
    for (size_t i = 0; i < qtdComp; ++i) {
        if (listaComp[i]->getId() == id) {
            posComp = i;
            break;
        }
    }
    if (posComp >= qtdComp) return false;

    for (size_t i = qtdFios; i-- > 0;) {
        if (listaFios[i]->getSourceId() == id || listaFios[i]->getDestId() == id) {
            removeWireAt(i);
        }
    }

    delete listaComp[posComp];
    for (size_t i = posComp; i + 1 < qtdComp; ++i) {
        listaComp[i] = listaComp[i + 1];
    }
    --qtdComp;
    return true;
}

// Remove o componente passando o ponteiro dele
bool Placa::removeComponent(Componente* comp) {
    return comp ? removeComponent(comp->getId()) : false;
}

// Apaga todos os componentes e fios, zerando as contagens da placa
void Placa::clear() {
    for (size_t i = 0; i < qtdFios; ++i) {
        delete listaFios[i];
    }
    qtdFios = 0;

    for (size_t i = 0; i < qtdComp; ++i) {
        delete listaComp[i];
    }
    qtdComp = 0;
}

// Avalia as portas e propaga os fios várias vezes até o circuito todo atualizar
void Placa::stepSimulation() {
    for (size_t rodada = 0; rodada < qtdComp; ++rodada) {
        for (size_t i = 0; i < qtdComp; ++i) {
            listaComp[i]->evaluate();
        }
        for (size_t i = 0; i < qtdFios; ++i) {
            listaFios[i]->propagate();
        }
    }
}

// Imprime todos os componentes e o resumo da placa no cout
void Placa::printCircuitStatus() const {
    std::cout << "=== Estado da Placa (" << qtdComp << " componentes, "
              << qtdFios << " trilhas) ===\n";
    for (size_t i = 0; i < qtdComp; ++i) {
        std::cout << "  " << *listaComp[i] << "\n";
    }
}

// Retorna o total de componentes
size_t Placa::getComponentCount() const { return qtdComp; }

// Retorna o componente na posição index (ou nullptr se for inválido)
Componente* Placa::getComponent(size_t index) const {
    return (index < qtdComp) ? listaComp[index] : nullptr;
}

// Retorna o total de fios
size_t Placa::getWireCount() const { return qtdFios; }

// Retorna o fio na posição index (ou nullptr se for inválido)
const Fio* Placa::getWire(size_t index) const {
    return (index < qtdFios) ? listaFios[index] : nullptr;
}

// Pega o maior ID usado até agora e soma 1 pra gerar um novo
int Placa::nextFreeId() const {
    int maiorId = 0;
    for (size_t i = 0; i < qtdComp; ++i) {
        if (listaComp[i]->getId() > maiorId) maiorId = listaComp[i]->getId();
    }
    return maiorId + 1;
}

// Verifica se já existe algum fio ligado nesse pino de entrada
bool Placa::isInputConnected(int dstId, size_t pinIndex) const {
    for (size_t i = 0; i < qtdFios; ++i) {
        if (listaFios[i]->getDestId() == dstId && listaFios[i]->getDestPinIndex() == pinIndex) {
            return true;
        }
    }
    return false;
}

#if !defined(MULTI_FILE_BUILD) && !defined(LOGIC_SIM_UNITY_INCLUDED)
#include "gui/main_gui.cpp"
#endif

#endif
