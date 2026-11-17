#ifndef LOGIC_SIM_WIRE_CPP
#define LOGIC_SIM_WIRE_CPP

#if __has_include("Wire.h")
#include "Wire.h"
#else
#include "../include/Wire.h"
#endif

/** @brief Constrói um fio conectando o pino de saída de origem ao pino de entrada de destino. */
Wire::Wire(int srcId, Pin<bool>* src, int dstId, Pin<bool>* dst, size_t dstIdx)
    : sourceCompId(srcId), destCompId(dstId), destPinIndex(dstIdx),
      sourcePin(src), destPin(dst) {
    if (sourcePin) sourcePin->setConnected(true);
    if (destPin) destPin->setConnected(true);
}

/** @brief Copia o estado lógico do pino de origem para o pino de destino. */
void Wire::propagate() {
    if (sourcePin && destPin) {
        destPin->setState(sourcePin->getState());
    }
}

/** @brief Zera o estado e marca como desconectado o pino de entrada de destino. */
void Wire::disconnectInputPin() {
    if (destPin) {
        destPin->setState(false);
        destPin->setConnected(false);
    }
}

/** @brief Religa os ponteiros de pino do fio aos componentes informados. */
void Wire::rebind(Component* srcComp, Component* dstComp) {
    if (srcComp && dstComp) {
        sourcePin = srcComp->getOutputPin();
        destPin = dstComp->getInputPin(destPinIndex);
        if (sourcePin) sourcePin->setConnected(true);
        if (destPin) destPin->setConnected(true);
    }
}

/** @brief Retorna o ID do componente de origem. */
int Wire::getSourceId() const { return sourceCompId; }

/** @brief Retorna o ID do componente de destino. */
int Wire::getDestId() const { return destCompId; }

/** @brief Retorna o índice do pino de entrada no componente de destino. */
size_t Wire::getDestPinIndex() const { return destPinIndex; }

/** @brief Retorna o estado lógico atual transportado pelo fio. */
bool Wire::getWireState() const {
    return sourcePin ? sourcePin->getState() : false;
}

/** @brief Compara se dois fios conectam ao mesmo pino de entrada do mesmo componente de destino. */
bool Wire::operator==(const Wire& other) const {
    return (destCompId == other.destCompId) && (destPinIndex == other.destPinIndex);
}

#if !defined(MULTI_FILE_BUILD) && !defined(LOGIC_SIM_UNITY_INCLUDED)
#include "gui/main_gui.cpp"
#endif

#endif
