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
    // Porta lógica genérica, já começa com a saída em 0 (false)
    LogicGate(int compId, const std::string& compName, float x, float y);

    // Pega o pino de saída da porta
    Pin<bool>* getOutputPin() override;
};

class UnaryGate : public LogicGate {
protected:
    Pin<bool> inputPin;

public:
    // Porta de 1 entrada só (usada como base pro NOT)
    UnaryGate(int compId, const std::string& compName, float x, float y);

    // Só tem o pino 0; qualquer outro índice retorna nullptr
    Pin<bool>* getInputPin(size_t index = 0) override;

    // Sempre tem 1 entrada
    size_t getInputCount() const override;
};

class MultiInputGate : public LogicGate {
protected:
    std::vector<Pin<bool>> inputPins;

public:
    // Porta com várias entradas (aceita 2 ou 3 pinos)
    MultiInputGate(int compId, const std::string& compName, size_t numInputs, float x, float y);

    // Pega o pino de entrada pelo índice, se for válido
    Pin<bool>* getInputPin(size_t index = 0) override;

    // Quantas entradas foram configuradas nessa porta
    size_t getNumInputs() const;

    // Total de pinos de entrada
    size_t getInputCount() const override;
};

class NotGate : public UnaryGate {
public:
    // Cria uma porta NOT (inversora)
    NotGate(int compId, float x = 0.0f, float y = 0.0f);

    // Inverte o valor que está na entrada
    void evaluate() override;

    // Clona a porta NOT
    Component* clone() const override;
};

class AndGate : public MultiInputGate {
public:
    // Cria uma porta AND de 2 ou 3 entradas
    AndGate(int compId, size_t numInputs = 2, float x = 0.0f, float y = 0.0f);

    // Saída só dá 1 se todas as entradas forem 1
    void evaluate() override;

    // Clona a porta AND
    Component* clone() const override;
};

class NandGate : public MultiInputGate {
public:
    // Cria uma porta NAND de 2 ou 3 entradas
    NandGate(int compId, size_t numInputs = 2, float x = 0.0f, float y = 0.0f);

    // Faz o inverso da AND (só dá 0 se tudo for 1)
    void evaluate() override;

    // Clona a porta NAND
    Component* clone() const override;
};

class OrGate : public MultiInputGate {
public:
    // Cria uma porta OR de 2 ou 3 entradas
    OrGate(int compId, size_t numInputs = 2, float x = 0.0f, float y = 0.0f);

    // Saída dá 1 se pelo menos uma entrada for 1
    void evaluate() override;

    // Clona a porta OR
    Component* clone() const override;
};

class NorGate : public MultiInputGate {
public:
    // Cria uma porta NOR de 2 ou 3 entradas
    NorGate(int compId, size_t numInputs = 2, float x = 0.0f, float y = 0.0f);

    // Inverso da OR: só dá 1 quando todas as entradas são 0
    void evaluate() override;

    // Clona a porta NOR
    Component* clone() const override;
};

class XorGate : public MultiInputGate {
public:
    // Cria uma porta XOR de 2 ou 3 entradas
    XorGate(int compId, size_t numInputs = 2, float x = 0.0f, float y = 0.0f);

    // Dá 1 quando tem uma quantidade ímpar de entradas ligadas
    void evaluate() override;

    // Clona a porta XOR
    Component* clone() const override;
};

class InputSwitch : public Component {
private:
    Pin<bool> outputPin;

public:
    // Chave de entrada que o usuário pode ligar/desligar
    InputSwitch(int compId, const std::string& compName, bool initialState = false,
                float x = 0.0f, float y = 0.0f);

    // Inverte o estado atual da chave (0 <-> 1)
    void toggle();

    // Força um estado específico na chave
    void setState(bool state);

    // A chave não precisa calcular nada, só mantém o valor dela
    void evaluate() override;

    // Clona a chave
    Component* clone() const override;

    // Pega o pino de saída da chave
    Pin<bool>* getOutputPin() override;

    // Chave não tem entrada, então retorna nullptr
    Pin<bool>* getInputPin(size_t index = 0) override;

    // 0 entradas
    size_t getInputCount() const override;
};

class OutputLED : public Component {
private:
    Pin<bool> inputPin;

public:
    // LED pra mostrar o resultado de saída do circuito
    OutputLED(int compId, const std::string& compName, float x = 0.0f, float y = 0.0f);

    // Diz se o LED está aceso (entrada em 1)
    bool isLit() const;

    // O LED só lê a entrada, não tem saída pra calcular
    void evaluate() override;

    // Clona o LED
    Component* clone() const override;

    // LED não tem pino de saída
    Pin<bool>* getOutputPin() override;

    // Pega o pino de entrada do LED (índice 0)
    Pin<bool>* getInputPin(size_t index = 0) override;

    // LED tem só 1 entrada
    size_t getInputCount() const override;
};

#endif
