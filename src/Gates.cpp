#ifndef LOGIC_SIM_GATES_CPP
#define LOGIC_SIM_GATES_CPP

#if __has_include("Gates.h")
#include "Gates.h"
#else
#include "../include/Gates.h"
#endif

/** @brief Constrói uma porta lógica básica com um pino de saída inicializado em falso. */
LogicGate::LogicGate(int compId, const std::string& compName, float x, float y)
    : Component(compId, compName, x, y), outputPin(false) {}

/** @brief Retorna o ponteiro para o pino de saída da porta lógica. */
Pin<bool>* LogicGate::getOutputPin() { return &outputPin; }

/** @brief Constrói uma porta lógica unária com um único pino de entrada. */
UnaryGate::UnaryGate(int compId, const std::string& compName, float x, float y)
    : LogicGate(compId, compName, x, y), inputPin(false) {}

/** @brief Retorna o pino de entrada se o índice for 0, ou nullptr caso contrário. */
Pin<bool>* UnaryGate::getInputPin(size_t index) {
    return (index == 0) ? &inputPin : nullptr;
}

/** @brief Retorna a quantidade de entradas da porta unária (sempre 1). */
size_t UnaryGate::getInputCount() const { return 1; }

/** @brief Constrói uma porta lógica com múltiplas entradas (2 ou 3 pinos). */
MultiInputGate::MultiInputGate(int compId, const std::string& compName, size_t numInputs, float x, float y)
    : LogicGate(compId, compName, x, y) {
    if (numInputs < 2 || numInputs > 3) {
        throw std::invalid_argument("Portas multi-entrada devem ter estritamente 2 ou 3 pinos.");
    }
    inputPins.resize(numInputs, Pin<bool>(false));
}

/** @brief Retorna o ponteiro para o pino de entrada no índice especificado, ou nullptr se fora do intervalo. */
Pin<bool>* MultiInputGate::getInputPin(size_t index) {
    if (index >= inputPins.size()) return nullptr;
    return &inputPins[index];
}

/** @brief Retorna o número de entradas configuradas na porta. */
size_t MultiInputGate::getNumInputs() const { return inputPins.size(); }

/** @brief Retorna a quantidade total de pinos de entrada da porta. */
size_t MultiInputGate::getInputCount() const { return inputPins.size(); }

/** @brief Constrói uma porta lógica inversora (NOT). */
NotGate::NotGate(int compId, float x, float y) : UnaryGate(compId, "NOT", x, y) {}

/** @brief Inverte o valor lógico da entrada e armazena no pino de saída. */
void NotGate::evaluate() { outputPin.setState(!inputPin.getState()); }

/** @brief Retorna uma cópia alocada dinamicamente desta porta NOT. */
Component* NotGate::clone() const { return new NotGate(*this); }

/** @brief Constrói uma porta lógica AND com 2 ou 3 entradas. */
AndGate::AndGate(int compId, size_t numInputs, float x, float y)
    : MultiInputGate(compId, "AND_" + std::to_string(numInputs), numInputs, x, y) {}

/** @brief Define a saída como verdadeira somente se todas as entradas forem verdadeiras. */
void AndGate::evaluate() {
    bool result = true;
    for (const auto& pin : inputPins) result = result && pin.getState();
    outputPin.setState(result);
}

/** @brief Retorna uma cópia alocada dinamicamente desta porta AND. */
Component* AndGate::clone() const { return new AndGate(*this); }

/** @brief Constrói uma porta lógica NAND com 2 ou 3 entradas. */
NandGate::NandGate(int compId, size_t numInputs, float x, float y)
    : MultiInputGate(compId, "NAND_" + std::to_string(numInputs), numInputs, x, y) {}

/** @brief Define a saída como o inverso da operação AND sobre todas as entradas. */
void NandGate::evaluate() {
    bool result = true;
    for (const auto& pin : inputPins) result = result && pin.getState();
    outputPin.setState(!result);
}

/** @brief Retorna uma cópia alocada dinamicamente desta porta NAND. */
Component* NandGate::clone() const { return new NandGate(*this); }

/** @brief Constrói uma porta lógica OR com 2 ou 3 entradas. */
OrGate::OrGate(int compId, size_t numInputs, float x, float y)
    : MultiInputGate(compId, "OR_" + std::to_string(numInputs), numInputs, x, y) {}

/** @brief Define a saída como verdadeira se ao menos uma das entradas for verdadeira. */
void OrGate::evaluate() {
    bool result = false;
    for (const auto& pin : inputPins) result = result || pin.getState();
    outputPin.setState(result);
}

/** @brief Retorna uma cópia alocada dinamicamente desta porta OR. */
Component* OrGate::clone() const { return new OrGate(*this); }

/** @brief Constrói uma porta lógica NOR com 2 ou 3 entradas. */
NorGate::NorGate(int compId, size_t numInputs, float x, float y)
    : MultiInputGate(compId, "NOR_" + std::to_string(numInputs), numInputs, x, y) {}

/** @brief Define a saída como verdadeira somente se todas as entradas forem falsas. */
void NorGate::evaluate() {
    bool result = false;
    for (const auto& pin : inputPins) result = result || pin.getState();
    outputPin.setState(!result);
}

/** @brief Retorna uma cópia alocada dinamicamente desta porta NOR. */
Component* NorGate::clone() const { return new NorGate(*this); }

/** @brief Constrói uma porta lógica XOR com 2 ou 3 entradas. */
XorGate::XorGate(int compId, size_t numInputs, float x, float y)
    : MultiInputGate(compId, "XOR_" + std::to_string(numInputs), numInputs, x, y) {}

/** @brief Define a saída como verdadeira se uma quantidade ímpar de entradas for verdadeira. */
void XorGate::evaluate() {
    bool result = false;
    for (const auto& pin : inputPins) result = (result != pin.getState());
    outputPin.setState(result);
}

/** @brief Retorna uma cópia alocada dinamicamente desta porta XOR. */
Component* XorGate::clone() const { return new XorGate(*this); }

/** @brief Constrói uma chave de entrada manual com estado lógico inicial configurável. */
InputSwitch::InputSwitch(int compId, const std::string& compName, bool initialState, float x, float y)
    : Component(compId, compName, x, y), outputPin(initialState) {}

/** @brief Alterna o estado lógico atual da chave entre verdadeiro e falso. */
void InputSwitch::toggle() { outputPin.setState(!outputPin.getState()); }

/** @brief Define explicitamente o estado lógico de saída da chave. */
void InputSwitch::setState(bool state) { outputPin.setState(state); }

/** @brief Mantém o estado atual da chave durante o ciclo de avaliação. */
void InputSwitch::evaluate() {}

/** @brief Retorna uma cópia alocada dinamicamente desta chave de entrada. */
Component* InputSwitch::clone() const { return new InputSwitch(*this); }

/** @brief Retorna o ponteiro para o pino de saída da chave. */
Pin<bool>* InputSwitch::getOutputPin() { return &outputPin; }

/** @brief Retorna nullptr, pois chaves de entrada não possuem pinos de entrada. */
Pin<bool>* InputSwitch::getInputPin(size_t) { return nullptr; }

/** @brief Retorna 0, indicando que a chave não possui entradas. */
size_t InputSwitch::getInputCount() const { return 0; }

/** @brief Constrói um LED indicador de saída. */
OutputLED::OutputLED(int compId, const std::string& compName, float x, float y)
    : Component(compId, compName, x, y), inputPin(false) {}

/** @brief Retorna true se o sinal recebido no pino de entrada estiver em nível lógico alto. */
bool OutputLED::isLit() const { return inputPin.getState(); }

/** @brief Mantém o estado recebido na entrada durante o ciclo de avaliação. */
void OutputLED::evaluate() {}

/** @brief Retorna uma cópia alocada dinamicamente deste LED. */
Component* OutputLED::clone() const { return new OutputLED(*this); }

/** @brief Retorna nullptr, pois o LED não possui pino de saída. */
Pin<bool>* OutputLED::getOutputPin() { return nullptr; }

/** @brief Retorna o ponteiro para o único pino de entrada do LED (índice 0). */
Pin<bool>* OutputLED::getInputPin(size_t index) { return (index == 0) ? &inputPin : nullptr; }

/** @brief Retorna 1, indicando que o LED possui apenas uma entrada. */
size_t OutputLED::getInputCount() const { return 1; }

#if !defined(MULTI_FILE_BUILD) && !defined(LOGIC_SIM_UNITY_INCLUDED)
#include "gui/main_gui.cpp"
#endif

#endif
