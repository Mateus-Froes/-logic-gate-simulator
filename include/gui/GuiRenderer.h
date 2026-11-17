#ifndef GUI_RENDERER_H
#define GUI_RENDERER_H

#include <cstddef>
#include <string>
#include <vector>
#include <SFML/Graphics.hpp>
#if __has_include("CircuitBoard.h")
#include "CircuitBoard.h"
#include "Component.h"
#else
#include "../CircuitBoard.h"
#include "../Component.h"
#endif

/** @brief Converte uma string codificada em UTF-8 para sf::String. */
sf::String toSfString(const std::string& utf8);

class GuiRenderer {
public:
    static constexpr float BODY_WIDTH     = 110.0f;
    static constexpr float BODY_HEIGHT    = 64.0f;
    static constexpr float PIN_RADIUS     = 7.0f;
    static constexpr float PIN_HIT_RADIUS = 11.0f;
    static constexpr float WIRE_THICKNESS = 3.0f;

    /** @brief Inicializa o renderizador associando a fonte utilizada nos textos. */
    explicit GuiRenderer(const sf::Font& font);

    GuiRenderer(const GuiRenderer&) = delete;
    GuiRenderer& operator=(const GuiRenderer&) = delete;

    /** @brief Desenha todos os fios e componentes da placa no alvo de renderização. */
    void drawBoard(sf::RenderTarget& target, const CircuitBoard& board, sf::Vector2f mouse) const;

    /** @brief Desenha um fio ortogonal entre dois pontos com a cor especificada. */
    void drawWire(sf::RenderTarget& target, sf::Vector2f from, sf::Vector2f to,
                  sf::Color color, int lane = -1) const;

    /** @brief Desenha um texto centralizado na posição informada. */
    void drawCenteredText(sf::RenderTarget& target, const std::string& text,
                          sf::Vector2f center, unsigned int size, sf::Color color) const;

    /** @brief Desenha um texto alinhado pelo canto superior esquerdo na posição informada. */
    void drawText(sf::RenderTarget& target, const std::string& text,
                  sf::Vector2f topLeft, unsigned int size, sf::Color color) const;

    /** @brief Retorna o retângulo delimitador do corpo de um componente. */
    static sf::FloatRect bodyRect(const Component& comp);

    /** @brief Calcula e retorna a posição central do pino de saída de um componente. */
    static sf::Vector2f outputPinPos(const Component& comp);

    /** @brief Calcula e retorna a posição central de um pino de entrada pelo seu índice. */
    static sf::Vector2f inputPinPos(const Component& comp, size_t index);

    /** @brief Verifica se o ponto informado está sobre o pino de saída do componente. */
    static bool hitOutputPin(Component& comp, sf::Vector2f point);

    /** @brief Retorna o índice do pino de entrada sob o ponto informado, ou -1 se nenhum for atingido. */
    static int hitInputPin(const Component& comp, sf::Vector2f point);

    /** @brief Retorna o índice do fio cujo traçado passa sob o ponto informado, ou -1 se nenhum for atingido. */
    static int hitWire(const CircuitBoard& board, sf::Vector2f point);

    /** @brief Calcula e retorna os vértices da polilinha ortogonal que compõe o traçado de um fio. */
    static std::vector<sf::Vector2f> wirePolyline(sf::Vector2f from, sf::Vector2f to, int lane = -1);

private:
    const sf::Font& font;

    /** @brief Desenha o corpo, os rótulos e os pinos de um único componente. */
    void drawComponent(sf::RenderTarget& target, Component& comp, sf::Vector2f mouse) const;

    /** @brief Desenha um pino circular indicando seu estado lógico e destaque de cursor. */
    void drawPin(sf::RenderTarget& target, sf::Vector2f center, bool state, bool hovered) const;

    /** @brief Desenha um segmento de reta espesso entre dois pontos. */
    void drawSegment(sf::RenderTarget& target, sf::Vector2f a, sf::Vector2f b, sf::Color color) const;
};

#endif
