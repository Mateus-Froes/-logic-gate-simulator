#ifndef LOGIC_SIM_WIRE_CPP
#define LOGIC_SIM_WIRE_CPP

#if __has_include("Wire.h")
#include "Wire.h"
#else
#include "../include/Wire.h"
#endif

// Cria o fio e já marca os pinos de origem e destino como conectados
Fio::Fio(int srcId, Pino<bool>* src, int dstId, Pino<bool>* dst, size_t dstIdx)
    : idOrigem(srcId), idDestino(dstId), indicePinoDestino(dstIdx),
      pinoOrigem(src), pinoDestino(dst) {
    if (pinoOrigem) pinoOrigem->setConnected(true);
    if (pinoDestino) pinoDestino->setConnected(true);
}

// Copia o sinal da saída de origem para a entrada de destino
void Fio::propagate() {
    if (pinoOrigem && pinoDestino) {
        pinoDestino->setState(pinoOrigem->getState());
    }
}

// Solta o pino de entrada e volta o estado dele pra false
void Fio::disconnectInputPin() {
    if (pinoDestino) {
        pinoDestino->setState(false);
        pinoDestino->setConnected(false);
    }
}

// Religa os ponteiros pros pinos dos novos componentes
void Fio::rebind(Componente* srcComp, Componente* dstComp) {
    if (srcComp && dstComp) {
        pinoOrigem = srcComp->getOutputPin();
        pinoDestino = dstComp->getInputPin(indicePinoDestino);
        if (pinoOrigem) pinoOrigem->setConnected(true);
        if (pinoDestino) pinoDestino->setConnected(true);
    }
}

// Retorna o ID do componente de origem
int Fio::getSourceId() const { return idOrigem; }

// Retorna o ID do componente de destino
int Fio::getDestId() const { return idDestino; }

// Retorna o índice do pino de entrada no destino
size_t Fio::getDestPinIndex() const { return indicePinoDestino; }

// Vê qual valor lógico está passando no fio agora
bool Fio::getWireState() const {
    return pinoOrigem ? pinoOrigem->getState() : false;
}

// Checa se os dois fios ocupam exatamente a mesma entrada
bool Fio::operator==(const Fio& outro) const {
    return (idDestino == outro.idDestino) && (indicePinoDestino == outro.indicePinoDestino);
}

#if !defined(MULTI_FILE_BUILD) && !defined(LOGIC_SIM_UNITY_INCLUDED)
#include "gui/main_gui.cpp"
#endif

#endif
