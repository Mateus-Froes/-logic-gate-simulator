#ifndef LOGIC_SIM_SIDEBAR_CPP
#define LOGIC_SIM_SIDEBAR_CPP

#if __has_include("gui/Sidebar.h")
#include "gui/Sidebar.h"
#else
#include "../../include/gui/Sidebar.h"
#endif

namespace {
const sf::Color COLOR_PANEL(28, 30, 36);
const sf::Color COLOR_BUTTON(52, 56, 66);
const sf::Color COLOR_BUTTON_HOVER(72, 78, 92);
const sf::Color COLOR_BUTTON_SELECTED(40, 110, 200);
const sf::Color COLOR_BUTTON_DANGER(185, 55, 55);
const sf::Color COLOR_BORDER(90, 95, 110);
const sf::Color COLOR_SIDEBAR_TEXT(235, 235, 240);
const sf::Color COLOR_TITLE(150, 190, 255);
}

/** @brief Constrói a barra lateral e posiciona todos os botões de ferramentas. */
Sidebar::Sidebar() : selectedInputs(2), deleteMode(false) {
    float y = 50.0f;
    const float h = 34.0f;
    const float gap = 8.0f;

    addButton(y, h, "AND", ToolAction::AddAnd);   y += h + gap;
    addButton(y, h, "NAND", ToolAction::AddNand); y += h + gap;
    addButton(y, h, "OR", ToolAction::AddOr);     y += h + gap;
    addButton(y, h, "NOR", ToolAction::AddNor);   y += h + gap;
    addButton(y, h, "XOR", ToolAction::AddXor);   y += h + gap;
    addButton(y, h, "NOT", ToolAction::AddNot);   y += h + gap;

    y += 30.0f;
    const float half = (WIDTH - 30.0f - gap) / 2.0f;
    addButton(y, h, "2", ToolAction::SetInputs2, 15.0f, half);
    addButton(y, h, "3", ToolAction::SetInputs3, 15.0f + half + gap, half);
    y += h + gap + 30.0f;

    addButton(y, h, "SWITCH", ToolAction::AddSwitch); y += h + gap;
    addButton(y, h, "LED", ToolAction::AddLed);       y += h + gap + 30.0f;

    addButton(y, h, "EXCLUIR (OFF)", ToolAction::ToggleDeleteMode); y += h + gap;
    addButton(y, h, "LIMPAR PLACA", ToolAction::ClearAll);
}

/** @brief Adiciona um novo botão à lista interna da barra lateral. */
void Sidebar::addButton(float y, float height, const std::string& label, ToolAction action,
                        float x, float width) {
    buttons.push_back(Button{sf::FloatRect({x, y}, {width, height}), label, action});
}

/** @brief Verifica se um botão de seleção de número de entradas está atualmente selecionado. */
bool Sidebar::isSelected(const Button& button) const {
    return (button.action == ToolAction::SetInputs2 && selectedInputs == 2) ||
           (button.action == ToolAction::SetInputs3 && selectedInputs == 3);
}

/** @brief Processa um clique na barra lateral e retorna a ação correspondente ao botão atingido. */
ToolAction Sidebar::handleClick(sf::Vector2f point) {
    for (Button& button : buttons) {
        if (button.area.contains(point)) {
            if (button.action == ToolAction::SetInputs2) selectedInputs = 2;
            if (button.action == ToolAction::SetInputs3) selectedInputs = 3;
            if (button.action == ToolAction::ToggleDeleteMode) {
                setDeleteMode(!deleteMode);
            } else if (button.action != ToolAction::ClearAll) {
                setDeleteMode(false);
            }
            return button.action;
        }
    }
    return ToolAction::None;
}

/** @brief Verifica se o ponto informado está dentro dos limites da barra lateral. */
bool Sidebar::contains(sf::Vector2f point) const {
    return point.x >= 0.0f && point.x < WIDTH;
}

/** @brief Retorna a quantidade de entradas selecionada (2 ou 3) para novas portas lógicas. */
size_t Sidebar::getSelectedInputs() const { return selectedInputs; }

/** @brief Retorna true se o modo de exclusão estiver ativo. */
bool Sidebar::isDeleteMode() const { return deleteMode; }

/** @brief Ativa ou desativa o modo de exclusão e atualiza o rótulo do botão correspondente. */
void Sidebar::setDeleteMode(bool enabled) {
    deleteMode = enabled;
    for (Button& button : buttons) {
        if (button.action == ToolAction::ToggleDeleteMode) {
            button.label = deleteMode ? "EXCLUIR (ON)" : "EXCLUIR (OFF)";
        }
    }
}

/** @brief Desenha o fundo da barra lateral, os títulos das seções e todos os botões. */
void Sidebar::draw(sf::RenderTarget& target, const GuiRenderer& renderer,
                   sf::Vector2f mouse, float height) const {
    sf::RectangleShape panel({WIDTH, height});
    panel.setFillColor(COLOR_PANEL);
    target.draw(panel);

    renderer.drawText(target, "Portas lógicas", {15.0f, 18.0f}, 15, COLOR_TITLE);
    renderer.drawText(target, "Entradas (AND..XOR)", {15.0f, buttons[6].area.position.y - 24.0f},
                      12, COLOR_TITLE);
    renderer.drawText(target, "Entrada / Saída", {15.0f, buttons[8].area.position.y - 24.0f},
                      15, COLOR_TITLE);
    renderer.drawText(target, "Edição", {15.0f, buttons[10].area.position.y - 24.0f},
                      15, COLOR_TITLE);

    for (const Button& button : buttons) {
        sf::Color fill = COLOR_BUTTON;
        if (button.action == ToolAction::ToggleDeleteMode && deleteMode) {
            fill = COLOR_BUTTON_DANGER;
        } else if (isSelected(button)) {
            fill = COLOR_BUTTON_SELECTED;
        } else if (button.area.contains(mouse)) {
            fill = COLOR_BUTTON_HOVER;
        }

        sf::RectangleShape shape(button.area.size);
        shape.setPosition(button.area.position);
        shape.setFillColor(fill);
        shape.setOutlineThickness(1.0f);
        shape.setOutlineColor(COLOR_BORDER);
        target.draw(shape);

        renderer.drawCenteredText(target, button.label, button.area.getCenter(), 15, COLOR_SIDEBAR_TEXT);
    }
}

#if !defined(MULTI_FILE_BUILD) && !defined(LOGIC_SIM_UNITY_INCLUDED)
#include "main_gui.cpp"
#endif

#endif
