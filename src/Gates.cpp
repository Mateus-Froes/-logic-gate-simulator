#ifndef LOGIC_SIM_GATES_CPP
#define LOGIC_SIM_GATES_CPP

#if __has_include("Gates.h")
#include "Gates.h"
#else
#include "../include/Gates.h"
#endif

// Porta lógica básica, começa com a saída desligada
LogicGate::LogicGate(int compId, const std::string& compName, float x, float y)
    : Component(compId, compName, x, y), outputPin(false) {}

// Retorna o endereço do pino de saída
Pin<bool>* LogicGate::getOutputPin() { return &outputPin; }

// Porta de uma entrada só, começa em false
UnaryGate::UnaryGate(int compId, const std::string& compName, float x, float y)
    : LogicGate(compId, compName, x, y), inputPin(false) {}

// Só retorna o pino se pedir o índice 0
Pin<bool>* UnaryGate::getInputPin(size_t index) {
    return (index == 0) ? &inputPin : nullptr;
}

// Porta unária tem sempre 1 entrada
size_t UnaryGate::getInputCount() const { return 1; }

// Cria a porta de várias entradas e valida se tem 2 ou 3 pinos
MultiInputGate::MultiInputGate(int compId, const std::string& compName, size_t numInputs, float x, float y)
    : LogicGate(compId, compName, x, y) {
    if (numInputs < 2 || numInputs > 3) {
        throw std::invalid_argument("Portas multi-entrada devem ter estritamente 2 ou 3 pinos.");
    }
    inputPins.resize(numInputs, Pin<bool>(false));
}

// Retorna o pino de entrada pedido ou nullptr se passar do limite
Pin<bool>* MultiInputGate::getInputPin(size_t index) {
    if (index >= inputPins.size()) return nullptr;
    return &inputPins[index];
}

// Quantidade de entradas configuradas
size_t MultiInputGate::getNumInputs() const { return inputPins.size(); }

// Total de entradas no vetor
size_t MultiInputGate::getInputCount() const { return inputPins.size(); }

// Construtor da porta NOT
NotGate::NotGate(int compId, float x, float y) : UnaryGate(compId, "NOT", x, y) {}

// Inverte o sinal que chegou na entrada
void NotGate::evaluate() { outputPin.setState(!inputPin.getState()); }

// Cria uma cópia da NOT
Component* NotGate::clone() const { return new NotGate(*this); }

// Construtor da AND (já coloca o número de entradas no nome, tipo AND_2)
AndGate::AndGate(int compId, size_t numInputs, float x, float y)
    : MultiInputGate(compId, "AND_" + std::to_string(numInputs), numInputs, x, y) {}

// Faz o E lógico de todas as entradas
void AndGate::evaluate() {
    bool result = true;
    for (const auto& pin : inputPins) result = result && pin.getState();
    outputPin.setState(result);
}

// Cria uma cópia da AND
Component* AndGate::clone() const { return new AndGate(*this); }

// Construtor da NAND
NandGate::NandGate(int compId, size_t numInputs, float x, float y)
    : MultiInputGate(compId, "NAND_" + std::to_string(numInputs), numInputs, x, y) {}

// Faz o AND de tudo e inverte no final
void NandGate::evaluate() {
    bool result = true;
    for (const auto& pin : inputPins) result = result && pin.getState();
    outputPin.setState(!result);
}

// Cria uma cópia da NAND
Component* NandGate::clone() const { return new NandGate(*this); }

// Construtor da OR
OrGate::OrGate(int compId, size_t numInputs, float x, float y)
    : MultiInputGate(compId, "OR_" + std::to_string(numInputs), numInputs, x, y) {}

// Faz o OU lógico (basta uma entrada ser true pra sair true)
void OrGate::evaluate() {
    bool result = false;
    for (const auto& pin : inputPins) result = result || pin.getState();
    outputPin.setState(result);
}

// Cria uma cópia da OR
Component* OrGate::clone() const { return new OrGate(*this); }

// Construtor da NOR
NorGate::NorGate(int compId, size_t numInputs, float x, float y)
    : MultiInputGate(compId, "NOR_" + std::to_string(numInputs), numInputs, x, y) {}

// Faz o OR das entradas e inverte o resultado
void NorGate::evaluate() {
    bool result = false;
    for (const auto& pin : inputPins) result = result || pin.getState();
    outputPin.setState(!result);
}

// Cria uma cópia da NOR
Component* NorGate::clone() const { return new NorGate(*this); }

// Construtor da XOR
XorGate::XorGate(int compId, size_t numInputs, float x, float y)
    : MultiInputGate(compId, "XOR_" + std::to_string(numInputs), numInputs, x, y) {}

// OU exclusivo: vai alternando o resultado a cada entrada ligada
void XorGate::evaluate() {
    bool result = false;
    for (const auto& pin : inputPins) result = (result != pin.getState());
    outputPin.setState(result);
}

// Cria uma cópia da XOR
Component* XorGate::clone() const { return new XorGate(*this); }

// Cria a chave de entrada já com o estado inicial escolhido
InputSwitch::InputSwitch(int compId, const std::string& compName, bool initialState, float x, float y)
    : Component(compId, compName, x, y), outputPin(initialState) {}

// Muda a chave de 0 pra 1 ou de 1 pra 0
void InputSwitch::toggle() { outputPin.setState(!outputPin.getState()); }

// Muda o estado da chave diretamente
void InputSwitch::setState(bool state) { outputPin.setState(state); }

// Não faz nada no evaluate porque o valor da chave quem controla é o usuário
void InputSwitch::evaluate() {}

// Cria uma cópia da chave
Component* InputSwitch::clone() const { return new InputSwitch(*this); }

// Retorna o pino de saída da chave
Pin<bool>* InputSwitch::getOutputPin() { return &outputPin; }

// Chave não tem pino de entrada
Pin<bool>* InputSwitch::getInputPin(size_t) { return nullptr; }

// 0 entradas
size_t InputSwitch::getInputCount() const { return 0; }

// Cria o LED de saída (começa apagado)
OutputLED::OutputLED(int compId, const std::string& compName, float x, float y)
    : Component(compId, compName, x, y), inputPin(false) {}

// Verifica se está chegando sinal 1 no LED
bool OutputLED::isLit() const { return inputPin.getState(); }

// O LED só mostra o que chegou no pino de entrada, então não calcula nada aqui
void OutputLED::evaluate() {}

// Cria uma cópia do LED
Component* OutputLED::clone() const { return new OutputLED(*this); }

// LED não tem saída
Pin<bool>* OutputLED::getOutputPin() { return nullptr; }

// Retorna o único pino de entrada do LED (índice 0)
Pin<bool>* OutputLED::getInputPin(size_t index) { return (index == 0) ? &inputPin : nullptr; }

// LED só tem 1 entrada
size_t OutputLED::getInputCount() const { return 1; }

#if !defined(MULTI_FILE_BUILD) && !defined(LOGIC_SIM_UNITY_INCLUDED)
#include "gui/main_gui.cpp"
#endif

#endif
