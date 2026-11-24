#ifndef LOGIC_SIM_GATES_CPP
#define LOGIC_SIM_GATES_CPP

#if __has_include("Gates.h")
#include "Gates.h"
#else
#include "../include/Gates.h"
#endif

// Porta lógica básica, começa com a saída desligada
PortaLogica::PortaLogica(int compId, const std::string& compNome, float x, float y)
    : Componente(compId, compNome, x, y), pinoSaida(false) {}

// Retorna o endereço do pino de saída
Pino<bool>* PortaLogica::getOutputPin() { return &pinoSaida; }

// Porta de uma entrada só, começa em false
PortaSimples::PortaSimples(int compId, const std::string& compNome, float x, float y)
    : PortaLogica(compId, compNome, x, y), pinoEntrada(false) {}

// Só retorna o pino se pedir o índice 0
Pino<bool>* PortaSimples::getInputPin(size_t index) {
    return (index == 0) ? &pinoEntrada : nullptr;
}

// Porta unária tem sempre 1 entrada
size_t PortaSimples::getInputCount() const { return 1; }

// Cria a porta de várias entradas e valida se tem 2 ou 3 pinos
PortaMulti::PortaMulti(int compId, const std::string& compNome, size_t qtdEntradas, float x, float y)
    : PortaLogica(compId, compNome, x, y) {
    if (qtdEntradas < 2 || qtdEntradas > 3) {
        throw std::invalid_argument("Portas multi-entrada devem ter estritamente 2 ou 3 pinos.");
    }
    pinosEntrada.resize(qtdEntradas, Pino<bool>(false));
}

// Retorna o pino de entrada pedido ou nullptr se passar do limite
Pino<bool>* PortaMulti::getInputPin(size_t index) {
    if (index >= pinosEntrada.size()) return nullptr;
    return &pinosEntrada[index];
}

// Quantidade de entradas configuradas
size_t PortaMulti::getNumInputs() const { return pinosEntrada.size(); }

// Total de entradas no vetor
size_t PortaMulti::getInputCount() const { return pinosEntrada.size(); }

// Construtor da porta NOT
PortaNot::PortaNot(int compId, float x, float y) : PortaSimples(compId, "NOT", x, y) {}

// Inverte o sinal que chegou na entrada
void PortaNot::evaluate() { pinoSaida.setState(!pinoEntrada.getState()); }

// Cria uma cópia da NOT
Componente* PortaNot::clone() const { return new PortaNot(*this); }

// Construtor da AND (já coloca o número de entradas no nome, tipo AND_2)
PortaAnd::PortaAnd(int compId, size_t qtdEntradas, float x, float y)
    : PortaMulti(compId, "AND_" + std::to_string(qtdEntradas), qtdEntradas, x, y) {}

// Faz o E lógico de todas as entradas
void PortaAnd::evaluate() {
    bool res = true;
    for (const auto& p : pinosEntrada) res = res && p.getState();
    pinoSaida.setState(res);
}

// Cria uma cópia da AND
Componente* PortaAnd::clone() const { return new PortaAnd(*this); }

// Construtor da NAND
PortaNand::PortaNand(int compId, size_t qtdEntradas, float x, float y)
    : PortaMulti(compId, "NAND_" + std::to_string(qtdEntradas), qtdEntradas, x, y) {}

// Faz o AND de tudo e inverte no final
void PortaNand::evaluate() {
    bool res = true;
    for (const auto& p : pinosEntrada) res = res && p.getState();
    pinoSaida.setState(!res);
}

// Cria uma cópia da NAND
Componente* PortaNand::clone() const { return new PortaNand(*this); }

// Construtor da OR
PortaOr::PortaOr(int compId, size_t qtdEntradas, float x, float y)
    : PortaMulti(compId, "OR_" + std::to_string(qtdEntradas), qtdEntradas, x, y) {}

// Faz o OU lógico (basta uma entrada ser true pra sair true)
void PortaOr::evaluate() {
    bool res = false;
    for (const auto& p : pinosEntrada) res = res || p.getState();
    pinoSaida.setState(res);
}

// Cria uma cópia da OR
Componente* PortaOr::clone() const { return new PortaOr(*this); }

// Construtor da NOR
PortaNor::PortaNor(int compId, size_t qtdEntradas, float x, float y)
    : PortaMulti(compId, "NOR_" + std::to_string(qtdEntradas), qtdEntradas, x, y) {}

// Faz o OR das entradas e inverte o resultado
void PortaNor::evaluate() {
    bool res = false;
    for (const auto& p : pinosEntrada) res = res || p.getState();
    pinoSaida.setState(!res);
}

// Cria uma cópia da NOR
Componente* PortaNor::clone() const { return new PortaNor(*this); }

// Construtor da XOR
PortaXor::PortaXor(int compId, size_t qtdEntradas, float x, float y)
    : PortaMulti(compId, "XOR_" + std::to_string(qtdEntradas), qtdEntradas, x, y) {}

// OU exclusivo: vai alternando o resultado a cada entrada ligada
void PortaXor::evaluate() {
    bool res = false;
    for (const auto& p : pinosEntrada) res = (res != p.getState());
    pinoSaida.setState(res);
}

// Cria uma cópia da XOR
Componente* PortaXor::clone() const { return new PortaXor(*this); }

// Cria a chave de entrada já com o estado inicial escolhido
Chave::Chave(int compId, const std::string& compNome, bool estadoInicial, float x, float y)
    : Componente(compId, compNome, x, y), pinoSaida(estadoInicial) {}

// Muda a chave de 0 pra 1 ou de 1 pra 0
void Chave::toggle() { pinoSaida.setState(!pinoSaida.getState()); }

// Muda o estado da chave diretamente
void Chave::setState(bool estado) { pinoSaida.setState(estado); }

// Não faz nada no evaluate porque o valor da chave quem controla é o usuário
void Chave::evaluate() {}

// Cria uma cópia da chave
Componente* Chave::clone() const { return new Chave(*this); }

// Retorna o pino de saída da chave
Pino<bool>* Chave::getOutputPin() { return &pinoSaida; }

// Chave não tem pino de entrada
Pino<bool>* Chave::getInputPin(size_t) { return nullptr; }

// 0 entradas
size_t Chave::getInputCount() const { return 0; }

// Cria o LED de saída (começa apagado)
Led::Led(int compId, const std::string& compNome, float x, float y)
    : Componente(compId, compNome, x, y), pinoEntrada(false) {}

// Verifica se está chegando sinal 1 no LED
bool Led::isLit() const { return pinoEntrada.getState(); }

// O LED só mostra o que chegou no pino de entrada, então não calcula nada aqui
void Led::evaluate() {}

// Cria uma cópia do LED
Componente* Led::clone() const { return new Led(*this); }

// LED não tem saída
Pino<bool>* Led::getOutputPin() { return nullptr; }

// Retorna o único pino de entrada do LED (índice 0)
Pino<bool>* Led::getInputPin(size_t index) { return (index == 0) ? &pinoEntrada : nullptr; }

// LED só tem 1 entrada
size_t Led::getInputCount() const { return 1; }

#if !defined(MULTI_FILE_BUILD) && !defined(LOGIC_SIM_UNITY_INCLUDED)
#include "gui/main_gui.cpp"
#endif

#endif
