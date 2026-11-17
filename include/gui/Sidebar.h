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

    // Monta a barra lateral e posiciona todos os botões
    Sidebar();

    // Vê em qual botão o usuário clicou e retorna a ação dele
    ToolAction handleClick(sf::Vector2f point);

    // Checa se o ponto está dentro da área da barra lateral
    bool contains(sf::Vector2f point) const;

    // Retorna se está marcado 2 ou 3 entradas pras portas novas
    size_t getSelectedInputs() const;

    // Diz se o modo de excluir está ligado
    bool isDeleteMode() const;

    // Liga ou desliga o modo de excluir e muda o texto do botão
    void setDeleteMode(bool enabled);

    // Desenha o fundo da barra lateral, os títulos e os botões
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

    // Cadastra um botão novo na barra lateral
    void addButton(float y, float height, const std::string& label, ToolAction action,
                   float x = 15.0f, float width = WIDTH - 30.0f);

    // Vê se o botão de 2 ou 3 entradas é o que está selecionado agora
    bool isSelected(const Button& button) const;
};

#endif
