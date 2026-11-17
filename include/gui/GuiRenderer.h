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

// Converte std::string (UTF-8) pra sf::String pro SFML mostrar acentos direito
sf::String toSfString(const std::string& utf8);

class GuiRenderer {
public:
    static constexpr float BODY_WIDTH     = 110.0f;
    static constexpr float BODY_HEIGHT    = 64.0f;
    static constexpr float PIN_RADIUS     = 7.0f;
    static constexpr float PIN_HIT_RADIUS = 11.0f;
    static constexpr float WIRE_THICKNESS = 3.0f;

    // Guarda a referência da fonte usada nos textos
    explicit GuiRenderer(const sf::Font& font);

    GuiRenderer(const GuiRenderer&) = delete;
    GuiRenderer& operator=(const GuiRenderer&) = delete;

    // Desenha todos os fios e componentes da placa na tela
    void drawBoard(sf::RenderTarget& target, const CircuitBoard& board, sf::Vector2f mouse) const;

    // Desenha um fio fazendo curvas em 90 graus entre dois pontos
    void drawWire(sf::RenderTarget& target, sf::Vector2f from, sf::Vector2f to,
                  sf::Color color, int lane = -1) const;

    // Desenha um texto centralizado na posição passada
    void drawCenteredText(sf::RenderTarget& target, const std::string& text,
                          sf::Vector2f center, unsigned int size, sf::Color color) const;

    // Desenha um texto alinhado pelo canto superior esquerdo
    void drawText(sf::RenderTarget& target, const std::string& text,
                  sf::Vector2f topLeft, unsigned int size, sf::Color color) const;

    // Retorna o retângulo do corpo de um componente
    static sf::FloatRect bodyRect(const Component& comp);

    // Calcula onde fica o pino de saída do componente na tela
    static sf::Vector2f outputPinPos(const Component& comp);

    // Calcula onde fica um pino de entrada pelo índice dele
    static sf::Vector2f inputPinPos(const Component& comp, size_t index);

    // Vê se o ponto clicado está em cima do pino de saída
    static bool hitOutputPin(Component& comp, sf::Vector2f point);

    // Vê se o ponto está em cima de algum pino de entrada (retorna o índice ou -1)
    static int hitInputPin(const Component& comp, sf::Vector2f point);

    // Descobre se o ponto está em cima de algum fio (retorna o índice ou -1)
    static int hitWire(const CircuitBoard& board, sf::Vector2f point);

    // Monta os pontos do caminho ortogonal que o fio faz na tela
    static std::vector<sf::Vector2f> wirePolyline(sf::Vector2f from, sf::Vector2f to, int lane = -1);

private:
    const sf::Font& font;

    // Desenha a caixinha do componente, os textos e os pinos
    void drawComponent(sf::RenderTarget& target, Component& comp, sf::Vector2f mouse) const;

    // Desenha a bolinha de um pino (muda de cor conforme o estado e o mouse)
    void drawPin(sf::RenderTarget& target, sf::Vector2f center, bool state, bool hovered) const;

    // Desenha um trecho reto do fio entre dois pontos
    void drawSegment(sf::RenderTarget& target, sf::Vector2f a, sf::Vector2f b, sf::Color color) const;
};

#endif
