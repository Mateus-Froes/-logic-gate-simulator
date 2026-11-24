#ifndef GUI_APPLICATION_H
#define GUI_APPLICATION_H

#include <string>
#include <SFML/Graphics.hpp>
#if __has_include("CircuitBoard.h")
#include "CircuitBoard.h"
#else
#include "../CircuitBoard.h"
#endif
#include "GuiRenderer.h"
#include "Sidebar.h"

class JanelaApp {
public:
    // Cria a janela do SFML e conecta com a placa do circuito
    explicit JanelaApp(Placa& circuito);

    JanelaApp(const JanelaApp&) = delete;
    JanelaApp& operator=(const JanelaApp&) = delete;

    // Loop principal: lê os eventos e desenha tudo até fechar a janela
    void run();

    // Desenha um frame e salva como imagem (usado na flag --snapshot)
    bool saveSnapshot(const std::string& caminho);

private:
    enum class Modo {
        Parado,
        LigandoFio,
        Arrastando
    };

    static constexpr float STATUS_BAR_HEIGHT = 30.0f;
    static constexpr float DRAG_THRESHOLD = 4.0f;

    Placa& placa;
    sf::Font fonte;
    sf::RenderWindow janela;
    Desenhista desenhista;
    BarraLateral barra;

    Modo modo;
    Componente* origemFio;
    Componente* alvoArraste;
    sf::Vector2f offsetArraste;
    sf::Vector2f posClique;
    bool moveuArraste;
    sf::Vector2f posMouse;
    std::string msgStatus;

    // Procura e carrega a fonte usada na interface
    static sf::Font loadFont();

    // Processa os eventos da janela (mouse, teclado, fechar, redimensionar)
    void processEvents();

    // Clique esquerdo: interage com a barra lateral ou com os componentes na tela
    void onMousePressed(sf::Vector2f ponto);

    // Clique direito: cancela o fio atual ou apaga componentes/conexões
    void onRightMousePressed(sf::Vector2f ponto);

    // Quando solta o botão do mouse: finaliza o arrasto ou alterna um switch
    void onMouseReleased();

    // Atualiza a posição do mouse e move o componente se estiver arrastando
    void onMouseMoved(sf::Vector2f ponto);

    // Cancela o fio que estava sendo puxado e mostra o motivo na barra de status
    void cancelWiring(const std::string& motivo);

    // Executa a ação do botão clicado na barra lateral
    void handleToolAction(ToolAction acao);

    // Tenta ligar a saída de um componente na entrada de outro
    void tryConnect(Componente& origem, Componente& destino, size_t pinoIdx);

    // Apaga o pino conectado, componente ou fio que estiver na posição clicada
    bool deleteAt(sf::Vector2f ponto);

    // Retorna qual componente está debaixo do ponto passado (ou nullptr)
    Componente* componentAt(sf::Vector2f ponto) const;

    // Acha um lugar livre na área de desenho pra colocar um componente novo
    sf::Vector2f findFreeSpot() const;

    // Cria uma porta de várias entradas usando a quantidade marcada na barra lateral
    template <typename TipoPorta>
    void spawnMultiInputGate();

    // Roda a simulação pra atualizar os estados do circuito
    void refreshSimulation();

    // Limpa a janela e desenha a grade, o circuito, a barra lateral e os avisos
    void render();

    // Desenha as dicas no topo e a barra de status no rodapé
    void drawOverlay();
};

#endif
