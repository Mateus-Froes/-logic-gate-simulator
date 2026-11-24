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

class Desenhista {
public:
    static constexpr float BODY_WIDTH     = 110.0f;
    static constexpr float BODY_HEIGHT    = 64.0f;
    static constexpr float PIN_RADIUS     = 7.0f;
    static constexpr float PIN_HIT_RADIUS = 11.0f;
    static constexpr float WIRE_THICKNESS = 3.0f;

    // Guarda a referência da fonte usada nos textos
    explicit Desenhista(const sf::Font& fonteRef);

    Desenhista(const Desenhista&) = delete;
    Desenhista& operator=(const Desenhista&) = delete;

    // Desenha todos os fios e componentes da placa na tela
    void drawBoard(sf::RenderTarget& target, const Placa& placa, sf::Vector2f mouse) const;

    // Desenha um fio fazendo curvas em 90 graus entre dois pontos
    void drawWire(sf::RenderTarget& target, sf::Vector2f de, sf::Vector2f para,
                  sf::Color cor, int trilha = -1) const;

    // Desenha um texto centralizado na posição passada
    void drawCenteredText(sf::RenderTarget& target, const std::string& texto,
                          sf::Vector2f centro, unsigned int tamanho, sf::Color cor) const;

    // Desenha um texto alinhado pelo canto superior esquerdo
    void drawText(sf::RenderTarget& target, const std::string& texto,
                  sf::Vector2f topoEsq, unsigned int tamanho, sf::Color cor) const;

    // Retorna o retângulo do corpo de um componente
    static sf::FloatRect bodyRect(const Componente& comp);

    // Calcula onde fica o pino de saída do componente na tela
    static sf::Vector2f outputPinPos(const Componente& comp);

    // Calcula onde fica um pino de entrada pelo índice dele
    static sf::Vector2f inputPinPos(const Componente& comp, size_t index);

    // Vê se o ponto clicado está em cima do pino de saída
    static bool hitOutputPin(Componente& comp, sf::Vector2f ponto);

    // Vê se o ponto está em cima de algum pino de entrada (retorna o índice ou -1)
    static int hitInputPin(const Componente& comp, sf::Vector2f ponto);

    // Descobre se o ponto está em cima de algum fio (retorna o índice ou -1)
    static int hitWire(const Placa& placa, sf::Vector2f ponto);

    // Monta os pontos do caminho ortogonal que o fio faz na tela
    static std::vector<sf::Vector2f> wirePolyline(sf::Vector2f de, sf::Vector2f para, int trilha = -1);

private:
    const sf::Font& fonte;

    // Desenha a caixinha do componente, os textos e os pinos
    void drawComponent(sf::RenderTarget& target, Componente& comp, sf::Vector2f mouse) const;

    // Desenha a bolinha de um pino (muda de cor conforme o estado e o mouse)
    void drawPin(sf::RenderTarget& target, sf::Vector2f centro, bool estado, bool mouseEmCima) const;

    // Desenha um trecho reto do fio entre dois pontos
    void drawSegment(sf::RenderTarget& target, sf::Vector2f a, sf::Vector2f b, sf::Color cor) const;
};

#endif
