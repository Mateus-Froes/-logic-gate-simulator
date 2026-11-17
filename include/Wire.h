#ifndef WIRE_H
#define WIRE_H

#include <cstddef>
#include "Component.h"
#include "Pin.h"

class Wire {
private:
    int sourceCompId;
    int destCompId;
    size_t destPinIndex;
    Pin<bool>* sourcePin;
    Pin<bool>* destPin;

public:
    /** @brief Constrói um fio conectando o pino de saída de origem ao pino de entrada de destino. */
    Wire(int srcId, Pin<bool>* src, int dstId, Pin<bool>* dst, size_t dstIdx);

    /** @brief Copia o estado lógico do pino de origem para o pino de destino. */
    void propagate();

    /** @brief Zera o estado e marca como desconectado o pino de entrada de destino. */
    void disconnectInputPin();

    /** @brief Religa os ponteiros de pino do fio aos componentes informados. */
    void rebind(Component* srcComp, Component* dstComp);

    /** @brief Retorna o ID do componente de origem. */
    int getSourceId() const;

    /** @brief Retorna o ID do componente de destino. */
    int getDestId() const;

    /** @brief Retorna o índice do pino de entrada no componente de destino. */
    size_t getDestPinIndex() const;

    /** @brief Retorna o estado lógico atual transportado pelo fio. */
    bool getWireState() const;

    /** @brief Compara se dois fios conectam ao mesmo pino de entrada do mesmo componente de destino. */
    bool operator==(const Wire& other) const;
};

#endif
