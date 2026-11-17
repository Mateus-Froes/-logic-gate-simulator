#ifndef COMPONENT_H
#define COMPONENT_H

#include <cstddef>
#include <iostream>
#include <string>
#include <utility>
#include "Pin.h"

class Component {
protected:
    int id;
    std::string name;
    float posX;
    float posY;

public:
    /** @brief Inicializa um componente com identificador, nome e coordenadas na placa. */
    Component(int compId, const std::string& compName, float x = 0.0f, float y = 0.0f);

    /** @brief Destrói o componente. */
    virtual ~Component();

    /** @brief Recalcula o estado de saída do componente a partir de suas entradas. */
    virtual void evaluate() = 0;

    /** @brief Cria e retorna uma cópia alocada dinamicamente deste componente. */
    virtual Component* clone() const = 0;

    /** @brief Retorna o ponteiro para o pino de saída, ou nullptr se não houver saída. */
    virtual Pin<bool>* getOutputPin() = 0;

    /** @brief Retorna o ponteiro para o pino de entrada no índice informado, ou nullptr se inválido. */
    virtual Pin<bool>* getInputPin(size_t index = 0) = 0;

    /** @brief Retorna a quantidade total de pinos de entrada do componente. */
    virtual size_t getInputCount() const = 0;

    /** @brief Retorna o identificador único do componente. */
    int getId() const;

    /** @brief Retorna o nome do componente. */
    std::string getName() const;

    /** @brief Retorna a coordenada X do componente. */
    float getX() const;

    /** @brief Retorna a coordenada Y do componente. */
    float getY() const;

    /** @brief Define a posição do componente usando coordenadas X e Y. */
    void setPosition(float x, float y);

    /** @brief Define a posição do componente a partir de um par de coordenadas (X, Y). */
    void setPosition(const std::pair<float, float>& pos);

    /** @brief Formata e insere as informações do componente em uma stream de saída. */
    friend std::ostream& operator<<(std::ostream& os, const Component& comp);
};

#endif
