#ifndef WIRE_H
#define WIRE_H

#include <cstddef>
#include "Component.h"
#include "Pin.h"

class Fio {
private:
    int idOrigem;
    int idDestino;
    size_t indicePinoDestino;
    Pino<bool>* pinoOrigem;
    Pino<bool>* pinoDestino;

public:
    // Cria um fio ligando o pino de saída de um componente na entrada de outro
    Fio(int srcId, Pino<bool>* src, int dstId, Pino<bool>* dst, size_t dstIdx);

    // Passa o sinal do pino de origem pro pino de destino
    void propagate();

    // Zera a entrada de destino e marca que não tem mais fio conectado nela
    void disconnectInputPin();

    // Atualiza os ponteiros dos pinos (útil depois de clonar a placa)
    void rebind(Componente* srcComp, Componente* dstComp);

    // Pega o ID do componente de onde o fio sai
    int getSourceId() const;

    // Pega o ID do componente onde o fio chega
    int getDestId() const;

    // Qual pino de entrada do destino esse fio está ligado
    size_t getDestPinIndex() const;

    // Retorna o sinal (0 ou 1) que está passando pelo fio
    bool getWireState() const;

    // Dois fios são considerados iguais se vão para a mesma entrada do mesmo componente
    bool operator==(const Fio& outro) const;
};

#endif
