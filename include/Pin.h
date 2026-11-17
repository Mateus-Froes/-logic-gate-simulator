#ifndef PIN_H
#define PIN_H

template <typename T>
class Pin {
private:
    T state;
    bool connected;

public:
    /** @brief Constrói o pino com um estado inicial e desconectado. */
    explicit Pin(T initialState = T()) : state(initialState), connected(false) {}

    /** @brief Define o valor atual do sinal no pino. */
    void setState(T newState) { state = newState; }

    /** @brief Retorna o valor atual do sinal no pino. */
    T getState() const { return state; }

    /** @brief Define se o pino está conectado a um fio. */
    void setConnected(bool status) { connected = status; }

    /** @brief Retorna true se o pino estiver conectado a um fio. */
    bool isConnected() const { return connected; }

    /** @brief Verifica se dois pinos possuem o mesmo estado e status de conexão. */
    bool operator==(const Pin<T>& other) const {
        return (state == other.state) && (connected == other.connected);
    }

    /** @brief Verifica se dois pinos diferem em estado ou status de conexão. */
    bool operator!=(const Pin<T>& other) const {
        return !(*this == other);
    }
};

#endif
