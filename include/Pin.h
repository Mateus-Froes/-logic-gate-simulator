#ifndef PIN_H
#define PIN_H

template <typename T>
class Pin {
private:
    T state;
    bool connected;

public:
    // Começa o pino desconectado e com o estado inicial passado
    explicit Pin(T initialState = T()) : state(initialState), connected(false) {}

    // Muda o valor do sinal no pino
    void setState(T newState) { state = newState; }

    // Pega o valor atual do sinal
    T getState() const { return state; }

    // Marca se o pino está conectado a algum fio ou não
    void setConnected(bool status) { connected = status; }

    // Diz se tem fio ligado nesse pino
    bool isConnected() const { return connected; }

    // Compara se dois pinos têm o mesmo estado e conexão
    bool operator==(const Pin<T>& other) const {
        return (state == other.state) && (connected == other.connected);
    }

    // Compara se dois pinos são diferentes
    bool operator!=(const Pin<T>& other) const {
        return !(*this == other);
    }
};

#endif
