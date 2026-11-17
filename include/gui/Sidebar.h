#ifndef SIDEBAR_H
#define SIDEBAR_H

#include <cstddef>
#include <string>
#include <vector>
#include <SFML/Graphics.hpp>
#include "GuiRenderer.h"

enum class ToolAction {
    None,
    AddAnd, AddNand, AddOr, AddNor, AddXor, AddNot,
    AddSwitch, AddLed,
    SetInputs2, SetInputs3,
    ToggleDeleteMode, ClearAll
};

class Sidebar {
public:
    static constexpr float WIDTH = 170.0f;

    /** @brief Constrói a barra lateral e posiciona todos os botões de ferramentas. */
    Sidebar();

    /** @brief Processa um clique na barra lateral e retorna a ação correspondente ao botão atingido. */
    ToolAction handleClick(sf::Vector2f point);

    /** @brief Verifica se o ponto informado está dentro dos limites da barra lateral. */
    bool contains(sf::Vector2f point) const;

    /** @brief Retorna a quantidade de entradas selecionada (2 ou 3) para novas portas lógicas. */
    size_t getSelectedInputs() const;

    /** @brief Retorna true se o modo de exclusão estiver ativo. */
    bool isDeleteMode() const;

    /** @brief Ativa ou desativa o modo de exclusão e atualiza o rótulo do botão correspondente. */
    void setDeleteMode(bool enabled);

    /** @brief Desenha o fundo da barra lateral, os títulos das seções e todos os botões. */
    void draw(sf::RenderTarget& target, const GuiRenderer& renderer,
              sf::Vector2f mouse, float height) const;

private:
    struct Button {
        sf::FloatRect area;
        std::string label;
        ToolAction action;
    };

    std::vector<Button> buttons;
    size_t selectedInputs;
    bool deleteMode;

    /** @brief Adiciona um novo botão à lista interna da barra lateral. */
    void addButton(float y, float height, const std::string& label, ToolAction action,
                   float x = 15.0f, float width = WIDTH - 30.0f);

    /** @brief Verifica se um botão de seleção de número de entradas está atualmente selecionado. */
    bool isSelected(const Button& button) const;
};

#endif
