#ifndef COMPONENT_H
#define COMPONENT_H

#include <cstddef>
#include <iostream>
#include <string>
#include <utility>
#include "Pin.h"

class Componente {
protected:
    int id;
    std::string nome;
    float posX;
    float posY;

public:
    // Construtor base: guarda id, nome e posição na placa
    Componente(int compId, const std::string& compNome, float x = 0.0f, float y = 0.0f);

    // Destrutor virtual por ser classe base
    virtual ~Componente();

    // Calcula a saída do componente com base nas entradas
    virtual void evaluate() = 0;

    // Faz uma cópia dinâmica do componente (usado no deep copy da placa)
    virtual Componente* clone() const = 0;

    // Retorna o pino de saída (ou nullptr se não tiver, tipo o LED)
    virtual Pino<bool>* getOutputPin() = 0;

    // Retorna o pino de entrada pelo índice (ou nullptr se não existir)
    virtual Pino<bool>* getInputPin(size_t index = 0) = 0;

    // Quantidade de pinos de entrada que o componente tem
    virtual size_t getInputCount() const = 0;

    // Pega o ID do componente
    int getId() const;

    // Pega o nome do componente
    std::string getName() const;

    // Pega a posição X
    float getX() const;

    // Pega a posição Y
    float getY() const;

    // Atualiza a posição (x, y) do componente
    void setPosition(float x, float y);

    // Atualiza a posição recebendo um pair (x, y)
    void setPosition(const std::pair<float, float>& pos);

    // Sobrecarga do << pra imprimir os dados do componente no cout
    friend std::ostream& operator<<(std::ostream& os, const Componente& comp);
};

#endif
