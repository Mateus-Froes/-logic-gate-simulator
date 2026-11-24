#ifndef LOGIC_SIM_GUIRENDERER_CPP
#define LOGIC_SIM_GUIRENDERER_CPP

#if __has_include("gui/GuiRenderer.h")
#include "gui/GuiRenderer.h"
#include "Gates.h"
#else
#include "../../include/gui/GuiRenderer.h"
#include "../../include/Gates.h"
#endif

#include <cmath>

namespace {
const sf::Color COR_FIO_ON(60, 220, 90);
const sf::Color COR_FIO_OFF(120, 124, 132);
const sf::Color COR_PORTA(40, 70, 130);
const sf::Color COR_CHAVE_OFF(70, 74, 82);
const sf::Color COR_CHAVE_ON(30, 140, 70);
const sf::Color COR_CORPO_LED(55, 45, 70);
const sf::Color COR_LED_APAGADO(90, 90, 90);
const sf::Color COR_LED_ACESO(170, 255, 80);
const sf::Color COR_BORDA(200, 205, 215);
const sf::Color COR_TEXTO(240, 240, 245);
const sf::Color COR_DESTAQUE(255, 220, 60);
const sf::Color COR_FIO_HOVER(255, 110, 90);

// Calcula a distância entre dois pontos 2D
float distancia(sf::Vector2f a, sf::Vector2f b) {
    const sf::Vector2f diff = b - a;
    return std::sqrt(diff.x * diff.x + diff.y * diff.y);
}

// Menor distância de um ponto até um segmento de reta (usado pra clicar no fio)
float distPontoSegmento(sf::Vector2f p, sf::Vector2f a, sf::Vector2f b) {
    const sf::Vector2f ab = b - a;
    const float tamQuad = ab.x * ab.x + ab.y * ab.y;
    if (tamQuad < 0.0001f) return distancia(p, a);
    float t = ((p.x - a.x) * ab.x + (p.y - a.y) * ab.y) / tamQuad;
    if (t < 0.0f) t = 0.0f;
    if (t > 1.0f) t = 1.0f;
    const sf::Vector2f proj{a.x + t * ab.x, a.y + t * ab.y};
    return distancia(p, proj);
}
}

// Converte string UTF-8 pra sf::String não quebrar os acentos
sf::String toSfString(const std::string& utf8) {
    return sf::String::fromUtf8(utf8.begin(), utf8.end());
}

// Inicializa o renderizador guardando a fonte
Desenhista::Desenhista(const sf::Font& fonteRef) : fonte(fonteRef) {}

// Retorna a caixa (retângulo) onde fica o corpo do componente
sf::FloatRect Desenhista::bodyRect(const Componente& comp) {
    return sf::FloatRect({comp.getX(), comp.getY()}, {BODY_WIDTH, BODY_HEIGHT});
}

// Posição do pino de saída (no meio do lado direito)
sf::Vector2f Desenhista::outputPinPos(const Componente& comp) {
    return {comp.getX() + BODY_WIDTH, comp.getY() + BODY_HEIGHT / 2.0f};
}

// Posição de cada pino de entrada (distribuídos no lado esquerdo)
sf::Vector2f Desenhista::inputPinPos(const Componente& comp, size_t index) {
    const float qtd = static_cast<float>(comp.getInputCount());
    const float passo = BODY_HEIGHT / (qtd + 1.0f);
    return {comp.getX(), comp.getY() + passo * static_cast<float>(index + 1)};
}

// Vê se o ponto está dentro do raio do pino de saída
bool Desenhista::hitOutputPin(Componente& comp, sf::Vector2f ponto) {
    return comp.getOutputPin() != nullptr &&
           distancia(ponto, outputPinPos(comp)) <= PIN_HIT_RADIUS;
}

// Testa se o ponto bateu em algum pino de entrada e retorna qual foi (ou -1)
int Desenhista::hitInputPin(const Componente& comp, sf::Vector2f ponto) {
    for (size_t i = 0; i < comp.getInputCount(); ++i) {
        if (distancia(ponto, inputPinPos(comp, i)) <= PIN_HIT_RADIUS) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

// Cria os pontos do caminho dobrado em 90 graus pro fio ligar dois pinos
std::vector<sf::Vector2f> Desenhista::wirePolyline(sf::Vector2f de, sf::Vector2f para, int trilha) {
    const float margem = 18.0f;
    if (para.x < de.x + 2.0f * margem) {
        const float saidaX = de.x + margem;
        const float entradaX = para.x - margem - 8.0f * static_cast<float>(trilha < 0 ? 0 : trilha);
        const float meioY = (de.y + para.y) / 2.0f;
        return {de, {saidaX, de.y}, {saidaX, meioY}, {entradaX, meioY}, {entradaX, para.y}, para};
    }

    float meioX = (de.x + para.x) / 2.0f;
    const float trilhaX = para.x - 22.0f - 14.0f * static_cast<float>(trilha);
    if (trilha >= 0 && trilhaX > de.x + 15.0f) {
        meioX = trilhaX;
    }
    return {de, {meioX, de.y}, {meioX, para.y}, para};
}

// Checa se o mouse está perto o suficiente de algum trecho de fio
int Desenhista::hitWire(const Placa& placa, sf::Vector2f ponto) {
    constexpr float TOLERANCIA = 6.5f;
    for (size_t i = placa.getWireCount(); i-- > 0;) {
        const Fio* fio = placa.getWire(i);
        const Componente* orig = placa.findComponent(fio->getSourceId());
        const Componente* dest = placa.findComponent(fio->getDestId());
        if (!orig || !dest) continue;

        const auto pts = wirePolyline(outputPinPos(*orig),
                                      inputPinPos(*dest, fio->getDestPinIndex()),
                                      static_cast<int>(fio->getDestPinIndex()));
        for (size_t s = 0; s + 1 < pts.size(); ++s) {
            if (distPontoSegmento(ponto, pts[s], pts[s + 1]) <= TOLERANCIA) {
                return static_cast<int>(i);
            }
        }
    }
    return -1;
}

// Desenha uma linha grossa entre os pontos A e B
void Desenhista::drawSegment(sf::RenderTarget& target, sf::Vector2f a, sf::Vector2f b,
                             sf::Color cor) const {
    const sf::Vector2f d = b - a;
    const float comp = distancia(a, b);
    if (comp < 0.01f) return;

    sf::RectangleShape linha({comp, WIRE_THICKNESS});
    linha.setOrigin({0.0f, WIRE_THICKNESS / 2.0f});
    linha.setPosition(a);
    linha.setRotation(sf::radians(std::atan2(d.y, d.x)));
    linha.setFillColor(cor);
    target.draw(linha);

    sf::CircleShape dobra(WIRE_THICKNESS / 2.0f);
    dobra.setOrigin({WIRE_THICKNESS / 2.0f, WIRE_THICKNESS / 2.0f});
    dobra.setPosition(b);
    dobra.setFillColor(cor);
    target.draw(dobra);
}

// Desenha o fio completo ligando todos os segmentos dele
void Desenhista::drawWire(sf::RenderTarget& target, sf::Vector2f de, sf::Vector2f para,
                          sf::Color cor, int trilha) const {
    const auto pts = wirePolyline(de, para, trilha);
    for (size_t s = 0; s + 1 < pts.size(); ++s) {
        drawSegment(target, pts[s], pts[s + 1], cor);
    }
}

// Desenha um texto na posição (x, y) normal
void Desenhista::drawText(sf::RenderTarget& target, const std::string& texto,
                          sf::Vector2f topoEsq, unsigned int tamanho, sf::Color cor) const {
    sf::Text rotulo(fonte, toSfString(texto), tamanho);
    rotulo.setFillColor(cor);
    rotulo.setPosition(topoEsq);
    target.draw(rotulo);
}

// Desenha um texto centralizado no ponto informado
void Desenhista::drawCenteredText(sf::RenderTarget& target, const std::string& texto,
                                  sf::Vector2f centro, unsigned int tamanho, sf::Color cor) const {
    sf::Text rotulo(fonte, toSfString(texto), tamanho);
    rotulo.setFillColor(cor);
    const sf::FloatRect limites = rotulo.getLocalBounds();
    rotulo.setOrigin({limites.position.x + limites.size.x / 2.0f,
                      limites.position.y + limites.size.y / 2.0f});
    rotulo.setPosition(centro);
    target.draw(rotulo);
}

// Desenha o círculo de um pino (aumenta um pouco se o mouse estiver em cima)
void Desenhista::drawPin(sf::RenderTarget& target, sf::Vector2f centro, bool estado,
                         bool mouseEmCima) const {
    const float raio = mouseEmCima ? PIN_RADIUS + 2.0f : PIN_RADIUS;
    sf::CircleShape pino(raio);
    pino.setOrigin({raio, raio});
    pino.setPosition(centro);
    pino.setFillColor(estado ? COR_FIO_ON : COR_FIO_OFF);
    pino.setOutlineThickness(2.0f);
    pino.setOutlineColor(mouseEmCima ? COR_DESTAQUE : COR_BORDA);
    target.draw(pino);
}

// Desenha o componente inteiro (corpo, textos, LED/chave e pinos)
void Desenhista::drawComponent(sf::RenderTarget& target, Componente& comp,
                               sf::Vector2f mouse) const {
    const sf::FloatRect caixa = bodyRect(comp);

    const auto* ch = dynamic_cast<const Chave*>(&comp);
    const auto* led = dynamic_cast<const Led*>(&comp);

    sf::Color corCorpo = COR_PORTA;
    if (ch) {
        corCorpo = comp.getOutputPin()->getState() ? COR_CHAVE_ON : COR_CHAVE_OFF;
    } else if (led) {
        corCorpo = COR_CORPO_LED;
    }

    sf::RectangleShape corpo(caixa.size);
    corpo.setPosition(caixa.position);
    corpo.setFillColor(corCorpo);
    corpo.setOutlineThickness(2.0f);
    corpo.setOutlineColor(COR_BORDA);
    target.draw(corpo);

    const sf::Vector2f centro = caixa.getCenter();
    const std::string tagId = "#" + std::to_string(comp.getId());

    if (led) {
        const float r = 15.0f;
        sf::CircleShape lampada(r);
        lampada.setOrigin({r, r});
        lampada.setPosition({centro.x + 12.0f, centro.y});
        lampada.setFillColor(led->isLit() ? COR_LED_ACESO : COR_LED_APAGADO);
        lampada.setOutlineThickness(2.0f);
        lampada.setOutlineColor(COR_BORDA);
        target.draw(lampada);
        drawCenteredText(target, comp.getName(), {centro.x, caixa.position.y - 12.0f}, 13, COR_TEXTO);
        drawCenteredText(target, tagId, {caixa.position.x + 28.0f, centro.y}, 12, COR_TEXTO);
    } else if (ch) {
        const bool ligada = comp.getOutputPin()->getState();
        drawCenteredText(target, comp.getName(), {centro.x, centro.y - 14.0f}, 12, COR_TEXTO);
        drawCenteredText(target, ligada ? "1  (ON)" : "0  (OFF)", {centro.x, centro.y + 4.0f}, 14, COR_TEXTO);
        drawCenteredText(target, tagId, {centro.x, centro.y + 21.0f}, 10, COR_TEXTO);
    } else {
        drawCenteredText(target, comp.getName(), {centro.x, centro.y - 8.0f}, 16, COR_TEXTO);
        drawCenteredText(target, tagId, {centro.x, centro.y + 14.0f}, 11, COR_TEXTO);
    }

    for (size_t i = 0; i < comp.getInputCount(); ++i) {
        const Pino<bool>* pinoIn = comp.getInputPin(i);
        const sf::Vector2f pos = inputPinPos(comp, i);
        drawPin(target, pos, pinoIn && pinoIn->getState(), distancia(mouse, pos) <= PIN_HIT_RADIUS);
    }

    if (const Pino<bool>* pinoOut = comp.getOutputPin()) {
        const sf::Vector2f pos = outputPinPos(comp);
        drawPin(target, pos, pinoOut->getState(), distancia(mouse, pos) <= PIN_HIT_RADIUS);
    }
}

// Desenha todos os fios primeiro e depois os componentes por cima
void Desenhista::drawBoard(sf::RenderTarget& target, const Placa& placa,
                           sf::Vector2f mouse) const {
    const int fioHover = hitWire(placa, mouse);

    for (size_t i = 0; i < placa.getWireCount(); ++i) {
        const Fio* fio = placa.getWire(i);
        const Componente* orig = placa.findComponent(fio->getSourceId());
        const Componente* dest = placa.findComponent(fio->getDestId());
        if (!orig || !dest) continue;

        sf::Color corFio = fio->getWireState() ? COR_FIO_ON : COR_FIO_OFF;
        if (static_cast<int>(i) == fioHover) {
            corFio = COR_FIO_HOVER;
        }
        drawWire(target, outputPinPos(*orig), inputPinPos(*dest, fio->getDestPinIndex()),
                 corFio, static_cast<int>(fio->getDestPinIndex()));
    }

    for (size_t i = 0; i < placa.getComponentCount(); ++i) {
        drawComponent(target, *placa.getComponent(i), mouse);
    }
}

#if !defined(MULTI_FILE_BUILD) && !defined(LOGIC_SIM_UNITY_INCLUDED)
#include "main_gui.cpp"
#endif

#endif
