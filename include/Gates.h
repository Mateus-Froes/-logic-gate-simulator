#ifndef GATES_H
#define GATES_H

#include <cstddef>
#include <stdexcept>
#include <string>
#include <vector>
#include "Component.h"

class LogicGate : public Component {
protected:
    Pin<bool> outputPin;

public:
    /** @brief Constrói uma porta lógica básica com um pino de saída inicializado em falso. */
    LogicGate(int compId, const std::string& compName, float x, float y);

    /** @brief Retorna o ponteiro para o pino de saída da porta lógica. */
    Pin<bool>* getOutputPin() override;
};

class UnaryGate : public LogicGate {
protected:
    Pin<bool> inputPin;

public:
    /** @brief Constrói uma porta lógica unária com um único pino de entrada. */
    UnaryGate(int compId, const std::string& compName, float x, float y);

    /** @brief Retorna o pino de entrada se o índice for 0, ou nullptr caso contrário. */
    Pin<bool>* getInputPin(size_t index = 0) override;

    /** @brief Retorna a quantidade de entradas da porta unária (sempre 1). */
    size_t getInputCount() const override;
};

class MultiInputGate : public LogicGate {
protected:
    std::vector<Pin<bool>> inputPins;

public:
    /** @brief Constrói uma porta lógica com múltiplas entradas (2 ou 3 pinos). */
    MultiInputGate(int compId, const std::string& compName, size_t numInputs, float x, float y);

    /** @brief Retorna o ponteiro para o pino de entrada no índice especificado, ou nullptr se fora do intervalo. */
    Pin<bool>* getInputPin(size_t index = 0) override;

    /** @brief Retorna o número de entradas configuradas na porta. */
    size_t getNumInputs() const;

    /** @brief Retorna a quantidade total de pinos de entrada da porta. */
    size_t getInputCount() const override;
};

class NotGate : public UnaryGate {
public:
    /** @brief Constrói uma porta lógica inversora (NOT). */
    NotGate(int compId, float x = 0.0f, float y = 0.0f);

    /** @brief Inverte o valor lógico da entrada e armazena no pino de saída. */
    void evaluate() override;

    /** @brief Retorna uma cópia alocada dinamicamente desta porta NOT. */
    Component* clone() const override;
};

class AndGate : public MultiInputGate {
public:
    /** @brief Constrói uma porta lógica AND com 2 ou 3 entradas. */
    AndGate(int compId, size_t numInputs = 2, float x = 0.0f, float y = 0.0f);

    /** @brief Define a saída como verdadeira somente se todas as entradas forem verdadeiras. */
    void evaluate() override;

    /** @brief Retorna uma cópia alocada dinamicamente desta porta AND. */
    Component* clone() const override;
};

class NandGate : public MultiInputGate {
public:
    /** @brief Constrói uma porta lógica NAND com 2 ou 3 entradas. */
    NandGate(int compId, size_t numInputs = 2, float x = 0.0f, float y = 0.0f);

    /** @brief Define a saída como o inverso da operação AND sobre todas as entradas. */
    void evaluate() override;

    /** @brief Retorna uma cópia alocada dinamicamente desta porta NAND. */
    Component* clone() const override;
};

class OrGate : public MultiInputGate {
public:
    /** @brief Constrói uma porta lógica OR com 2 ou 3 entradas. */
    OrGate(int compId, size_t numInputs = 2, float x = 0.0f, float y = 0.0f);

    /** @brief Define a saída como verdadeira se ao menos uma das entradas for verdadeira. */
    void evaluate() override;

    /** @brief Retorna uma cópia alocada dinamicamente desta porta OR. */
    Component* clone() const override;
};

class NorGate : public MultiInputGate {
public:
    /** @brief Constrói uma porta lógica NOR com 2 ou 3 entradas. */
    NorGate(int compId, size_t numInputs = 2, float x = 0.0f, float y = 0.0f);

    /** @brief Define a saída como verdadeira somente se todas as entradas forem falsas. */
    void evaluate() override;

    /** @brief Retorna uma cópia alocada dinamicamente desta porta NOR. */
    Component* clone() const override;
};

class XorGate : public MultiInputGate {
public:
    /** @brief Constrói uma porta lógica XOR com 2 ou 3 entradas. */
    XorGate(int compId, size_t numInputs = 2, float x = 0.0f, float y = 0.0f);

    /** @brief Define a saída como verdadeira se uma quantidade ímpar de entradas for verdadeira. */
    void evaluate() override;

    /** @brief Retorna uma cópia alocada dinamicamente desta porta XOR. */
    Component* clone() const override;
};

class InputSwitch : public Component {
private:
    Pin<bool> outputPin;

public:
    /** @brief Constrói uma chave de entrada manual com estado lógico inicial configurável. */
    InputSwitch(int compId, const std::string& compName, bool initialState = false,
                float x = 0.0f, float y = 0.0f);

    /** @brief Alterna o estado lógico atual da chave entre verdadeiro e falso. */
    void toggle();

    /** @brief Define explicitamente o estado lógico de saída da chave. */
    void setState(bool state);

    /** @brief Mantém o estado atual da chave durante o ciclo de avaliação. */
    void evaluate() override;

    /** @brief Retorna uma cópia alocada dinamicamente desta chave de entrada. */
    Component* clone() const override;

    /** @brief Retorna o ponteiro para o pino de saída da chave. */
    Pin<bool>* getOutputPin() override;

    /** @brief Retorna nullptr, pois chaves de entrada não possuem pinos de entrada. */
    Pin<bool>* getInputPin(size_t index = 0) override;

    /** @brief Retorna 0, indicando que a chave não possui entradas. */
    size_t getInputCount() const override;
};

class OutputLED : public Component {
private:
    Pin<bool> inputPin;

public:
    /** @brief Constrói um LED indicador de saída. */
    OutputLED(int compId, const std::string& compName, float x = 0.0f, float y = 0.0f);

    /** @brief Retorna true se o sinal recebido no pino de entrada estiver em nível lógico alto. */
    bool isLit() const;

    /** @brief Mantém o estado recebido na entrada durante o ciclo de avaliação. */
    void evaluate() override;

    /** @brief Retorna uma cópia alocada dinamicamente deste LED. */
    Component* clone() const override;

    /** @brief Retorna nullptr, pois o LED não possui pino de saída. */
    Pin<bool>* getOutputPin() override;

    /** @brief Retorna o ponteiro para o único pino de entrada do LED (índice 0). */
    Pin<bool>* getInputPin(size_t index = 0) override;

    /** @brief Retorna 1, indicando que o LED possui apenas uma entrada. */
    size_t getInputCount() const override;
};

#endif
