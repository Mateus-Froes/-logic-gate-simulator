#ifndef PIN_H
#define PIN_H

template <typename T>
class Pino {
private:
    T valor;
    bool ligado;

public:
    // Começa o pino desligado e com o valor inicial passado
    explicit Pino(T valorInicial = T()) : valor(valorInicial), ligado(false) {}

    // Muda o valor do sinal no pino
    void setState(T novoValor) { valor = novoValor; }

    // Pega o valor atual do sinal
    T getState() const { return valor; }

    // Marca se o pino está ligado a algum fio ou não
    void setConnected(bool status) { ligado = status; }

    // Diz se tem fio ligado nesse pino
    bool isConnected() const { return ligado; }

    // Compara se dois pinos têm o mesmo valor e conexão
    bool operator==(const Pino<T>& outro) const {
        return (valor == outro.valor) && (ligado == outro.ligado);
    }

    // Compara se dois pinos são diferentes
    bool operator!=(const Pino<T>& outro) const {
        return !(*this == outro);
    }
};

#endif
