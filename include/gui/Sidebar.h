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

class BarraLateral {
public:
    static constexpr float WIDTH = 170.0f;

    // Monta a barra lateral e posiciona todos os botões
    BarraLateral();

    // Vê em qual botão o usuário clicou e retorna a ação dele
    ToolAction handleClick(sf::Vector2f ponto);

    // Checa se o ponto está dentro da área da barra lateral
    bool contains(sf::Vector2f ponto) const;

    // Retorna se está marcado 2 ou 3 entradas pras portas novas
    size_t getSelectedInputs() const;

    // Diz se o modo de excluir está ligado
    bool isDeleteMode() const;

    // Liga ou desliga o modo de excluir e muda o texto do botão
    void setDeleteMode(bool ativo);

    // Desenha o fundo da barra lateral, os títulos e os botões
    void draw(sf::RenderTarget& target, const Desenhista& desenhista,
              sf::Vector2f mouse, float altura) const;

private:
    struct Botao {
        sf::FloatRect area;
        std::string texto;
        ToolAction acao;
    };

    std::vector<Botao> botoes;
    size_t entradasMarcadas;
    bool modoApagar;

    // Cadastra um botão novo na barra lateral
    void addButton(float y, float altura, const std::string& texto, ToolAction acao,
                   float x = 15.0f, float largura = WIDTH - 30.0f);

    // Vê se o botão de 2 ou 3 entradas é o que está selecionado agora
    bool isSelected(const Botao& botao) const;
};

#endif
