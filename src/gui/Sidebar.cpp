#ifndef LOGIC_SIM_SIDEBAR_CPP
#define LOGIC_SIM_SIDEBAR_CPP

#if __has_include("gui/Sidebar.h")
#include "gui/Sidebar.h"
#else
#include "../../include/gui/Sidebar.h"
#endif

namespace {
const sf::Color COR_PAINEL(28, 30, 36);
const sf::Color COR_BOTAO(52, 56, 66);
const sf::Color COR_BOTAO_HOVER(72, 78, 92);
const sf::Color COR_BOTAO_SELECIONADO(40, 110, 200);
const sf::Color COR_BOTAO_PERIGO(185, 55, 55);
const sf::Color COR_BORDA_PAINEL(90, 95, 110);
const sf::Color COR_TEXTO_BOTAO(235, 235, 240);
const sf::Color COR_TITULO_SECAO(150, 190, 255);
}

// Cria a barra lateral e adiciona todos os botões na ordem certa
BarraLateral::BarraLateral() : entradasMarcadas(2), modoApagar(false) {
    float y = 50.0f;
    const float alt = 34.0f;
    const float espaco = 8.0f;

    addButton(y, alt, "AND", ToolAction::AddAnd);   y += alt + espaco;
    addButton(y, alt, "NAND", ToolAction::AddNand); y += alt + espaco;
    addButton(y, alt, "OR", ToolAction::AddOr);     y += alt + espaco;
    addButton(y, alt, "NOR", ToolAction::AddNor);   y += alt + espaco;
    addButton(y, alt, "XOR", ToolAction::AddXor);   y += alt + espaco;
    addButton(y, alt, "NOT", ToolAction::AddNot);   y += alt + espaco;

    y += 30.0f;
    const float metade = (WIDTH - 30.0f - espaco) / 2.0f;
    addButton(y, alt, "2", ToolAction::SetInputs2, 15.0f, metade);
    addButton(y, alt, "3", ToolAction::SetInputs3, 15.0f + metade + espaco, metade);
    y += alt + espaco + 30.0f;

    addButton(y, alt, "SWITCH", ToolAction::AddSwitch); y += alt + espaco;
    addButton(y, alt, "LED", ToolAction::AddLed);       y += alt + espaco + 30.0f;

    addButton(y, alt, "EXCLUIR (OFF)", ToolAction::ToggleDeleteMode); y += alt + espaco;
    addButton(y, alt, "LIMPAR PLACA", ToolAction::ClearAll);
}

// Guarda um botão novo no vetor da barra lateral
void BarraLateral::addButton(float y, float altura, const std::string& texto, ToolAction acao,
                             float x, float largura) {
    botoes.push_back(Botao{sf::FloatRect({x, y}, {largura, altura}), texto, acao});
}

// Vê se o botão de 2 ou 3 entradas está ativo
bool BarraLateral::isSelected(const Botao& botao) const {
    return (botao.acao == ToolAction::SetInputs2 && entradasMarcadas == 2) ||
           (botao.acao == ToolAction::SetInputs3 && entradasMarcadas == 3);
}

// Verifica qual botão foi clicado e já atualiza o estado da barra lateral
ToolAction BarraLateral::handleClick(sf::Vector2f ponto) {
    for (Botao& botao : botoes) {
        if (botao.area.contains(ponto)) {
            if (botao.acao == ToolAction::SetInputs2) entradasMarcadas = 2;
            if (botao.acao == ToolAction::SetInputs3) entradasMarcadas = 3;
            if (botao.acao == ToolAction::ToggleDeleteMode) {
                setDeleteMode(!modoApagar);
            } else if (botao.acao != ToolAction::ClearAll) {
                setDeleteMode(false);
            }
            return botao.acao;
        }
    }
    return ToolAction::None;
}

// Retorna true se a posição X cair dentro da barra lateral
bool BarraLateral::contains(sf::Vector2f ponto) const {
    return ponto.x >= 0.0f && ponto.x < WIDTH;
}

// Quantidade de entradas escolhida (2 ou 3)
size_t BarraLateral::getSelectedInputs() const { return entradasMarcadas; }

// Retorna se o modo de exclusão está ativo
bool BarraLateral::isDeleteMode() const { return modoApagar; }

// Liga ou desliga o modo excluir e troca o texto do botão
void BarraLateral::setDeleteMode(bool ativo) {
    modoApagar = ativo;
    for (Botao& botao : botoes) {
        if (botao.acao == ToolAction::ToggleDeleteMode) {
            botao.texto = modoApagar ? "EXCLUIR (ON)" : "EXCLUIR (OFF)";
        }
    }
}

// Desenha o painel lateral, os títulos das seções e cada botão
void BarraLateral::draw(sf::RenderTarget& target, const Desenhista& desenhista,
                        sf::Vector2f mouse, float altura) const {
    sf::RectangleShape painel({WIDTH, altura});
    painel.setFillColor(COR_PAINEL);
    target.draw(painel);

    desenhista.drawText(target, "Portas lógicas", {15.0f, 18.0f}, 15, COR_TITULO_SECAO);
    desenhista.drawText(target, "Entradas (AND..XOR)", {15.0f, botoes[6].area.position.y - 24.0f},
                        12, COR_TITULO_SECAO);
    desenhista.drawText(target, "Entrada / Saída", {15.0f, botoes[8].area.position.y - 24.0f},
                        15, COR_TITULO_SECAO);
    desenhista.drawText(target, "Edição", {15.0f, botoes[10].area.position.y - 24.0f},
                        15, COR_TITULO_SECAO);

    for (const Botao& botao : botoes) {
        sf::Color fundo = COR_BOTAO;
        if (botao.acao == ToolAction::ToggleDeleteMode && modoApagar) {
            fundo = COR_BOTAO_PERIGO;
        } else if (isSelected(botao)) {
            fundo = COR_BOTAO_SELECIONADO;
        } else if (botao.area.contains(mouse)) {
            fundo = COR_BOTAO_HOVER;
        }

        sf::RectangleShape forma(botao.area.size);
        forma.setPosition(botao.area.position);
        forma.setFillColor(fundo);
        forma.setOutlineThickness(1.0f);
        forma.setOutlineColor(COR_BORDA_PAINEL);
        target.draw(forma);

        desenhista.drawCenteredText(target, botao.texto, botao.area.getCenter(), 15, COR_TEXTO_BOTAO);
    }
}

#if !defined(MULTI_FILE_BUILD) && !defined(LOGIC_SIM_UNITY_INCLUDED)
#include "main_gui.cpp"
#endif

#endif
