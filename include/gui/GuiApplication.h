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

class GuiApplication {
public:
    // Cria a janela do SFML e conecta com a placa do circuito
    explicit GuiApplication(CircuitBoard& circuit);

    GuiApplication(const GuiApplication&) = delete;
    GuiApplication& operator=(const GuiApplication&) = delete;

    // Loop principal: lê os eventos e desenha tudo até fechar a janela
    void run();

    // Desenha um frame e salva como imagem (usado na flag --snapshot)
    bool saveSnapshot(const std::string& path);

private:
    enum class Mode {
        Idle,
        Wiring,
        Dragging
    };

    static constexpr float STATUS_BAR_HEIGHT = 30.0f;
    static constexpr float DRAG_THRESHOLD = 4.0f;

    CircuitBoard& board;
    sf::Font font;
    sf::RenderWindow window;
    GuiRenderer renderer;
    Sidebar sidebar;

    Mode mode;
    Component* wiringSource;
    Component* dragTarget;
    sf::Vector2f dragOffset;
    sf::Vector2f pressPosition;
    bool dragMoved;
    sf::Vector2f mousePosition;
    std::string statusMessage;

    // Procura e carrega a fonte usada na interface
    static sf::Font loadFont();

    // Processa os eventos da janela (mouse, teclado, fechar, redimensionar)
    void processEvents();

    // Clique esquerdo: interage com a barra lateral ou com os componentes na tela
    void onMousePressed(sf::Vector2f point);

    // Clique direito: cancela o fio atual ou apaga componentes/conexões
    void onRightMousePressed(sf::Vector2f point);

    // Quando solta o botão do mouse: finaliza o arrasto ou alterna um switch
    void onMouseReleased();

    // Atualiza a posição do mouse e move o componente se estiver arrastando
    void onMouseMoved(sf::Vector2f point);

    // Cancela o fio que estava sendo puxado e mostra o motivo na barra de status
    void cancelWiring(const std::string& reason);

    // Executa a ação do botão clicado na barra lateral
    void handleToolAction(ToolAction action);

    // Tenta ligar a saída de um componente na entrada de outro
    void tryConnect(Component& source, Component& destination, size_t pinIndex);

    // Apaga o pino conectado, componente ou fio que estiver na posição clicada
    bool deleteAt(sf::Vector2f point);

    // Retorna qual componente está debaixo do ponto passado (ou nullptr)
    Component* componentAt(sf::Vector2f point) const;

    // Acha um lugar livre na área de desenho pra colocar um componente novo
    sf::Vector2f findFreeSpot() const;

    // Cria uma porta de várias entradas usando a quantidade marcada na barra lateral
    template <typename GateType>
    void spawnMultiInputGate();

    // Roda a simulação pra atualizar os estados do circuito
    void refreshSimulation();

    // Limpa a janela e desenha a grade, o circuito, a barra lateral e os avisos
    void render();

    // Desenha as dicas no topo e a barra de status no rodapé
    void drawOverlay();
};

#endif
