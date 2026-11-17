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
const sf::Color COLOR_WIRE_ON(60, 220, 90);
const sf::Color COLOR_WIRE_OFF(120, 124, 132);
const sf::Color COLOR_GATE_BODY(40, 70, 130);
const sf::Color COLOR_SWITCH_OFF(70, 74, 82);
const sf::Color COLOR_SWITCH_ON(30, 140, 70);
const sf::Color COLOR_LED_BODY(55, 45, 70);
const sf::Color COLOR_LED_OFF(90, 90, 90);
const sf::Color COLOR_LED_ON(170, 255, 80);
const sf::Color COLOR_OUTLINE(200, 205, 215);
const sf::Color COLOR_TEXT(240, 240, 245);
const sf::Color COLOR_HOVER(255, 220, 60);
const sf::Color COLOR_WIRE_HOVER(255, 110, 90);

/** @brief Calcula a distância euclidiana entre dois pontos 2D. */
float distance(sf::Vector2f a, sf::Vector2f b) {
    const sf::Vector2f d = b - a;
    return std::sqrt(d.x * d.x + d.y * d.y);
}

/** @brief Calcula a menor distância entre um ponto e um segmento de reta. */
float pointToSegmentDistance(sf::Vector2f p, sf::Vector2f a, sf::Vector2f b) {
    const sf::Vector2f ab = b - a;
    const float lenSq = ab.x * ab.x + ab.y * ab.y;
    if (lenSq < 0.0001f) return distance(p, a);
    float t = ((p.x - a.x) * ab.x + (p.y - a.y) * ab.y) / lenSq;
    if (t < 0.0f) t = 0.0f;
    if (t > 1.0f) t = 1.0f;
    const sf::Vector2f proj{a.x + t * ab.x, a.y + t * ab.y};
    return distance(p, proj);
}
}

/** @brief Converte uma string codificada em UTF-8 para sf::String. */
sf::String toSfString(const std::string& utf8) {
    return sf::String::fromUtf8(utf8.begin(), utf8.end());
}

/** @brief Inicializa o renderizador associando a fonte utilizada nos textos. */
GuiRenderer::GuiRenderer(const sf::Font& fontRef) : font(fontRef) {}

/** @brief Retorna o retângulo delimitador do corpo de um componente. */
sf::FloatRect GuiRenderer::bodyRect(const Component& comp) {
    return sf::FloatRect({comp.getX(), comp.getY()}, {BODY_WIDTH, BODY_HEIGHT});
}

/** @brief Calcula e retorna a posição central do pino de saída de um componente. */
sf::Vector2f GuiRenderer::outputPinPos(const Component& comp) {
    return {comp.getX() + BODY_WIDTH, comp.getY() + BODY_HEIGHT / 2.0f};
}

/** @brief Calcula e retorna a posição central de um pino de entrada pelo seu índice. */
sf::Vector2f GuiRenderer::inputPinPos(const Component& comp, size_t index) {
    const float count = static_cast<float>(comp.getInputCount());
    const float step = BODY_HEIGHT / (count + 1.0f);
    return {comp.getX(), comp.getY() + step * static_cast<float>(index + 1)};
}

/** @brief Verifica se o ponto informado está sobre o pino de saída do componente. */
bool GuiRenderer::hitOutputPin(Component& comp, sf::Vector2f point) {
    return comp.getOutputPin() != nullptr &&
           distance(point, outputPinPos(comp)) <= PIN_HIT_RADIUS;
}

/** @brief Retorna o índice do pino de entrada sob o ponto informado, ou -1 se nenhum for atingido. */
int GuiRenderer::hitInputPin(const Component& comp, sf::Vector2f point) {
    for (size_t i = 0; i < comp.getInputCount(); ++i) {
        if (distance(point, inputPinPos(comp, i)) <= PIN_HIT_RADIUS) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

/** @brief Calcula e retorna os vértices da polilinha ortogonal que compõe o traçado de um fio. */
std::vector<sf::Vector2f> GuiRenderer::wirePolyline(sf::Vector2f from, sf::Vector2f to, int lane) {
    const float margin = 18.0f;
    if (to.x < from.x + 2.0f * margin) {
        const float outX = from.x + margin;
        const float inX = to.x - margin - 8.0f * static_cast<float>(lane < 0 ? 0 : lane);
        const float midY = (from.y + to.y) / 2.0f;
        return {from, {outX, from.y}, {outX, midY}, {inX, midY}, {inX, to.y}, to};
    }

    float midX = (from.x + to.x) / 2.0f;
    const float laneX = to.x - 22.0f - 14.0f * static_cast<float>(lane);
    if (lane >= 0 && laneX > from.x + 15.0f) {
        midX = laneX;
    }
    return {from, {midX, from.y}, {midX, to.y}, to};
}

/** @brief Retorna o índice do fio cujo traçado passa sob o ponto informado, ou -1 se nenhum for atingido. */
int GuiRenderer::hitWire(const CircuitBoard& board, sf::Vector2f point) {
    constexpr float HIT_TOLERANCE = 6.5f;
    for (size_t i = board.getWireCount(); i-- > 0;) {
        const Wire* wire = board.getWire(i);
        const Component* src = board.findComponent(wire->getSourceId());
        const Component* dst = board.findComponent(wire->getDestId());
        if (!src || !dst) continue;

        const auto pts = wirePolyline(outputPinPos(*src),
                                      inputPinPos(*dst, wire->getDestPinIndex()),
                                      static_cast<int>(wire->getDestPinIndex()));
        for (size_t s = 0; s + 1 < pts.size(); ++s) {
            if (pointToSegmentDistance(point, pts[s], pts[s + 1]) <= HIT_TOLERANCE) {
                return static_cast<int>(i);
            }
        }
    }
    return -1;
}

/** @brief Desenha um segmento de reta espesso entre dois pontos. */
void GuiRenderer::drawSegment(sf::RenderTarget& target, sf::Vector2f a, sf::Vector2f b,
                              sf::Color color) const {
    const sf::Vector2f d = b - a;
    const float length = distance(a, b);
    if (length < 0.01f) return;

    sf::RectangleShape line({length, WIRE_THICKNESS});
    line.setOrigin({0.0f, WIRE_THICKNESS / 2.0f});
    line.setPosition(a);
    line.setRotation(sf::radians(std::atan2(d.y, d.x)));
    line.setFillColor(color);
    target.draw(line);

    sf::CircleShape joint(WIRE_THICKNESS / 2.0f);
    joint.setOrigin({WIRE_THICKNESS / 2.0f, WIRE_THICKNESS / 2.0f});
    joint.setPosition(b);
    joint.setFillColor(color);
    target.draw(joint);
}

/** @brief Desenha um fio ortogonal entre dois pontos com a cor especificada. */
void GuiRenderer::drawWire(sf::RenderTarget& target, sf::Vector2f from, sf::Vector2f to,
                           sf::Color color, int lane) const {
    const auto pts = wirePolyline(from, to, lane);
    for (size_t s = 0; s + 1 < pts.size(); ++s) {
        drawSegment(target, pts[s], pts[s + 1], color);
    }
}

/** @brief Desenha um texto alinhado pelo canto superior esquerdo na posição informada. */
void GuiRenderer::drawText(sf::RenderTarget& target, const std::string& text,
                           sf::Vector2f topLeft, unsigned int size, sf::Color color) const {
    sf::Text label(font, toSfString(text), size);
    label.setFillColor(color);
    label.setPosition(topLeft);
    target.draw(label);
}

/** @brief Desenha um texto centralizado na posição informada. */
void GuiRenderer::drawCenteredText(sf::RenderTarget& target, const std::string& text,
                                   sf::Vector2f center, unsigned int size, sf::Color color) const {
    sf::Text label(font, toSfString(text), size);
    label.setFillColor(color);
    const sf::FloatRect bounds = label.getLocalBounds();
    label.setOrigin({bounds.position.x + bounds.size.x / 2.0f,
                     bounds.position.y + bounds.size.y / 2.0f});
    label.setPosition(center);
    target.draw(label);
}

/** @brief Desenha um pino circular indicando seu estado lógico e destaque de cursor. */
void GuiRenderer::drawPin(sf::RenderTarget& target, sf::Vector2f center, bool state,
                          bool hovered) const {
    const float radius = hovered ? PIN_RADIUS + 2.0f : PIN_RADIUS;
    sf::CircleShape pin(radius);
    pin.setOrigin({radius, radius});
    pin.setPosition(center);
    pin.setFillColor(state ? COLOR_WIRE_ON : COLOR_WIRE_OFF);
    pin.setOutlineThickness(2.0f);
    pin.setOutlineColor(hovered ? COLOR_HOVER : COLOR_OUTLINE);
    target.draw(pin);
}

/** @brief Desenha o corpo, os rótulos e os pinos de um único componente. */
void GuiRenderer::drawComponent(sf::RenderTarget& target, Component& comp,
                                sf::Vector2f mouse) const {
    const sf::FloatRect rect = bodyRect(comp);

    const auto* sw = dynamic_cast<const InputSwitch*>(&comp);
    const auto* led = dynamic_cast<const OutputLED*>(&comp);

    sf::Color bodyColor = COLOR_GATE_BODY;
    if (sw) {
        bodyColor = comp.getOutputPin()->getState() ? COLOR_SWITCH_ON : COLOR_SWITCH_OFF;
    } else if (led) {
        bodyColor = COLOR_LED_BODY;
    }

    sf::RectangleShape body(rect.size);
    body.setPosition(rect.position);
    body.setFillColor(bodyColor);
    body.setOutlineThickness(2.0f);
    body.setOutlineColor(COLOR_OUTLINE);
    target.draw(body);

    const sf::Vector2f center = rect.getCenter();
    const std::string idLabel = "#" + std::to_string(comp.getId());

    if (led) {
        const float r = 15.0f;
        sf::CircleShape bulb(r);
        bulb.setOrigin({r, r});
        bulb.setPosition({center.x + 12.0f, center.y});
        bulb.setFillColor(led->isLit() ? COLOR_LED_ON : COLOR_LED_OFF);
        bulb.setOutlineThickness(2.0f);
        bulb.setOutlineColor(COLOR_OUTLINE);
        target.draw(bulb);
        drawCenteredText(target, comp.getName(), {center.x, rect.position.y - 12.0f}, 13, COLOR_TEXT);
        drawCenteredText(target, idLabel, {rect.position.x + 28.0f, center.y}, 12, COLOR_TEXT);
    } else if (sw) {
        const bool on = comp.getOutputPin()->getState();
        drawCenteredText(target, comp.getName(), {center.x, center.y - 14.0f}, 12, COLOR_TEXT);
        drawCenteredText(target, on ? "1  (ON)" : "0  (OFF)", {center.x, center.y + 4.0f}, 14, COLOR_TEXT);
        drawCenteredText(target, idLabel, {center.x, center.y + 21.0f}, 10, COLOR_TEXT);
    } else {
        drawCenteredText(target, comp.getName(), {center.x, center.y - 8.0f}, 16, COLOR_TEXT);
        drawCenteredText(target, idLabel, {center.x, center.y + 14.0f}, 11, COLOR_TEXT);
    }

    for (size_t i = 0; i < comp.getInputCount(); ++i) {
        const Pin<bool>* in = comp.getInputPin(i);
        const sf::Vector2f pos = inputPinPos(comp, i);
        drawPin(target, pos, in && in->getState(), distance(mouse, pos) <= PIN_HIT_RADIUS);
    }

    if (const Pin<bool>* out = comp.getOutputPin()) {
        const sf::Vector2f pos = outputPinPos(comp);
        drawPin(target, pos, out->getState(), distance(mouse, pos) <= PIN_HIT_RADIUS);
    }
}

/** @brief Desenha todos os fios e componentes da placa no alvo de renderização. */
void GuiRenderer::drawBoard(sf::RenderTarget& target, const CircuitBoard& board,
                            sf::Vector2f mouse) const {
    const int hoveredWire = hitWire(board, mouse);

    for (size_t i = 0; i < board.getWireCount(); ++i) {
        const Wire* wire = board.getWire(i);
        const Component* src = board.findComponent(wire->getSourceId());
        const Component* dst = board.findComponent(wire->getDestId());
        if (!src || !dst) continue;

        sf::Color wireColor = wire->getWireState() ? COLOR_WIRE_ON : COLOR_WIRE_OFF;
        if (static_cast<int>(i) == hoveredWire) {
            wireColor = COLOR_WIRE_HOVER;
        }
        drawWire(target, outputPinPos(*src), inputPinPos(*dst, wire->getDestPinIndex()),
                 wireColor, static_cast<int>(wire->getDestPinIndex()));
    }

    for (size_t i = 0; i < board.getComponentCount(); ++i) {
        drawComponent(target, *board.getComponent(i), mouse);
    }
}

#if !defined(MULTI_FILE_BUILD) && !defined(LOGIC_SIM_UNITY_INCLUDED)
#include "main_gui.cpp"
#endif

#endif
