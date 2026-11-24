#ifndef GATES_H
#define GATES_H

#include <cstddef>
#include <stdexcept>
#include <string>
#include <vector>
#include "Component.h"

class PortaLogica : public Componente {
protected:
    Pino<bool> pinoSaida;

public:
    // Porta lógica genérica, já começa com a saída em 0 (false)
    PortaLogica(int compId, const std::string& compNome, float x, float y);

    // Pega o pino de saída da porta
    Pino<bool>* getOutputPin() override;
};

class PortaSimples : public PortaLogica {
protected:
    Pino<bool> pinoEntrada;

public:
    // Porta de 1 entrada só (usada como base pro NOT)
    PortaSimples(int compId, const std::string& compNome, float x, float y);

    // Só tem o pino 0; qualquer outro índice retorna nullptr
    Pino<bool>* getInputPin(size_t index = 0) override;

    // Sempre tem 1 entrada
    size_t getInputCount() const override;
};

class PortaMulti : public PortaLogica {
protected:
    std::vector<Pino<bool>> pinosEntrada;

public:
    // Porta com várias entradas (aceita 2 ou 3 pinos)
    PortaMulti(int compId, const std::string& compNome, size_t qtdEntradas, float x, float y);

    // Pega o pino de entrada pelo índice, se for válido
    Pino<bool>* getInputPin(size_t index = 0) override;

    // Quantas entradas foram configuradas nessa porta
    size_t getNumInputs() const;

    // Total de pinos de entrada
    size_t getInputCount() const override;
};

class PortaNot : public PortaSimples {
public:
    // Cria uma porta NOT (inversora)
    PortaNot(int compId, float x = 0.0f, float y = 0.0f);

    // Inverte o valor que está na entrada
    void evaluate() override;

    // Clona a porta NOT
    Componente* clone() const override;
};

class PortaAnd : public PortaMulti {
public:
    // Cria uma porta AND de 2 ou 3 entradas
    PortaAnd(int compId, size_t qtdEntradas = 2, float x = 0.0f, float y = 0.0f);

    // Saída só dá 1 se todas as entradas forem 1
    void evaluate() override;

    // Clona a porta AND
    Componente* clone() const override;
};

class PortaNand : public PortaMulti {
public:
    // Cria uma porta NAND de 2 ou 3 entradas
    PortaNand(int compId, size_t qtdEntradas = 2, float x = 0.0f, float y = 0.0f);

    // Faz o inverso da AND (só dá 0 se tudo for 1)
    void evaluate() override;

    // Clona a porta NAND
    Componente* clone() const override;
};

class PortaOr : public PortaMulti {
public:
    // Cria uma porta OR de 2 ou 3 entradas
    PortaOr(int compId, size_t qtdEntradas = 2, float x = 0.0f, float y = 0.0f);

    // Saída dá 1 se pelo menos uma entrada for 1
    void evaluate() override;

    // Clona a porta OR
    Componente* clone() const override;
};

class PortaNor : public PortaMulti {
public:
    // Cria uma porta NOR de 2 ou 3 entradas
    PortaNor(int compId, size_t qtdEntradas = 2, float x = 0.0f, float y = 0.0f);

    // Inverso da OR: só dá 1 quando todas as entradas são 0
    void evaluate() override;

    // Clona a porta NOR
    Componente* clone() const override;
};

class PortaXor : public PortaMulti {
public:
    // Cria uma porta XOR de 2 ou 3 entradas
    PortaXor(int compId, size_t qtdEntradas = 2, float x = 0.0f, float y = 0.0f);

    // Dá 1 quando tem uma quantidade ímpar de entradas ligadas
    void evaluate() override;

    // Clona a porta XOR
    Componente* clone() const override;
};

class Chave : public Componente {
private:
    Pino<bool> pinoSaida;

public:
    // Chave de entrada que o usuário pode ligar/desligar
    Chave(int compId, const std::string& compNome, bool estadoInicial = false,
          float x = 0.0f, float y = 0.0f);

    // Inverte o estado atual da chave (0 <-> 1)
    void toggle();

    // Força um estado específico na chave
    void setState(bool estado);

    // A chave não precisa calcular nada, só mantém o valor dela
    void evaluate() override;

    // Clona a chave
    Componente* clone() const override;

    // Pega o pino de saída da chave
    Pino<bool>* getOutputPin() override;

    // Chave não tem entrada, então retorna nullptr
    Pino<bool>* getInputPin(size_t index = 0) override;

    // 0 entradas
    size_t getInputCount() const override;
};

class Led : public Componente {
private:
    Pino<bool> pinoEntrada;

public:
    // LED pra mostrar o resultado de saída do circuito
    Led(int compId, const std::string& compNome, float x = 0.0f, float y = 0.0f);

    // Diz se o LED está aceso (entrada em 1)
    bool isLit() const;

    // O LED só lê a entrada, não tem saída pra calcular
    void evaluate() override;

    // Clona o LED
    Componente* clone() const override;

    // LED não tem pino de saída
    Pino<bool>* getOutputPin() override;

    // Pega o pino de entrada do LED (índice 0)
    Pino<bool>* getInputPin(size_t index = 0) override;

    // LED tem só 1 entrada
    size_t getInputCount() const override;
};

#endif
