#ifndef LOGIC_SIM_WIRE_CPP
#define LOGIC_SIM_WIRE_CPP

#if __has_include("Wire.h")
#include "Wire.h"
#else
#include "../include/Wire.h"
#endif

// Cria o fio e já marca os pinos de origem e destino como conectados
Wire::Wire(int srcId, Pin<bool>* src, int dstId, Pin<bool>* dst, size_t dstIdx)
    : sourceCompId(srcId), destCompId(dstId), destPinIndex(dstIdx),
      sourcePin(src), destPin(dst) {
    if (sourcePin) sourcePin->setConnected(true);
    if (destPin) destPin->setConnected(true);
}

// Copia o sinal da saída de origem para a entrada de destino
void Wire::propagate() {
    if (sourcePin && destPin) {
        destPin->setState(sourcePin->getState());
    }
}

// Solta o pino de entrada e volta o estado dele pra false
void Wire::disconnectInputPin() {
    if (destPin) {
        destPin->setState(false);
        destPin->setConnected(false);
    }
}

// Religa os ponteiros pros pinos dos novos componentes
void Wire::rebind(Component* srcComp, Component* dstComp) {
    if (srcComp && dstComp) {
        sourcePin = srcComp->getOutputPin();
        destPin = dstComp->getInputPin(destPinIndex);
        if (sourcePin) sourcePin->setConnected(true);
        if (destPin) destPin->setConnected(true);
    }
}

// Retorna o ID do componente de origem
int Wire::getSourceId() const { return sourceCompId; }

// Retorna o ID do componente de destino
int Wire::getDestId() const { return destCompId; }

// Retorna o índice do pino de entrada no destino
size_t Wire::getDestPinIndex() const { return destPinIndex; }

// Vê qual valor lógico está passando no fio agora
bool Wire::getWireState() const {
    return sourcePin ? sourcePin->getState() : false;
}

// Checa se os dois fios ocupam exatamente a mesma entrada
bool Wire::operator==(const Wire& other) const {
    return (destCompId == other.destCompId) && (destPinIndex == other.destPinIndex);
}

#if !defined(MULTI_FILE_BUILD) && !defined(LOGIC_SIM_UNITY_INCLUDED)
#include "gui/main_gui.cpp"
#endif

#endif
