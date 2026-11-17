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
    // Cria um fio ligando o pino de saída de um componente na entrada de outro
    Wire(int srcId, Pin<bool>* src, int dstId, Pin<bool>* dst, size_t dstIdx);

    // Passa o sinal do pino de origem pro pino de destino
    void propagate();

    // Zera a entrada de destino e marca que não tem mais fio conectado nela
    void disconnectInputPin();

    // Atualiza os ponteiros dos pinos (útil depois de clonar a placa)
    void rebind(Component* srcComp, Component* dstComp);

    // Pega o ID do componente de onde o fio sai
    int getSourceId() const;

    // Pega o ID do componente onde o fio chega
    int getDestId() const;

    // Qual pino de entrada do destino esse fio está ligado
    size_t getDestPinIndex() const;

    // Retorna o sinal (0 ou 1) que está passando pelo fio
    bool getWireState() const;

    // Dois fios são considerados iguais se vão para a mesma entrada do mesmo componente
    bool operator==(const Wire& other) const;
};

#endif
