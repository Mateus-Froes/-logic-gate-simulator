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
    /** @brief Inicializa a janela gráfica e associa a placa de circuito ao controlador. */
    explicit GuiApplication(CircuitBoard& circuit);

    GuiApplication(const GuiApplication&) = delete;
    GuiApplication& operator=(const GuiApplication&) = delete;

    /** @brief Executa o laço principal de eventos e renderização até a janela ser fechada. */
    void run();

    /** @brief Renderiza um único quadro da interface e salva o resultado em um arquivo de imagem. */
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

    /** @brief Carrega e retorna a fonte tipográfica utilizada na interface gráfica. */
    static sf::Font loadFont();

    /** @brief Processa todos os eventos pendentes da janela (teclado, mouse e redimensionamento). */
    void processEvents();

    /** @brief Trata o clique com o botão esquerdo do mouse na barra lateral ou na área de desenho. */
    void onMousePressed(sf::Vector2f point);

    /** @brief Trata o clique com o botão direito do mouse para cancelar conexões ou excluir elementos. */
    void onRightMousePressed(sf::Vector2f point);

    /** @brief Finaliza o arrasto de um componente ou alterna o estado de uma chave ao soltar o mouse. */
    void onMouseReleased();

    /** @brief Atualiza a posição do cursor e move o componente selecionado durante um arrasto. */
    void onMouseMoved(sf::Vector2f point);

    /** @brief Cancela a criação de um fio em andamento e atualiza a mensagem da barra de status. */
    void cancelWiring(const std::string& reason);

    /** @brief Executa a ação correspondente ao botão clicado na barra lateral. */
    void handleToolAction(ToolAction action);

    /** @brief Tenta conectar a saída de um componente à entrada de outro e atualiza a simulação. */
    void tryConnect(Component& source, Component& destination, size_t pinIndex);

    /** @brief Remove o pino conectado, componente ou fio localizado sob o ponto informado. */
    bool deleteAt(sf::Vector2f point);

    /** @brief Retorna o componente cujo corpo contém o ponto informado, ou nullptr se nenhum for atingido. */
    Component* componentAt(sf::Vector2f point) const;

    /** @brief Procura e retorna uma posição livre na área de desenho para posicionar um novo componente. */
    sf::Vector2f findFreeSpot() const;

    /** @brief Cria uma porta lógica multi-entrada na placa utilizando o número de entradas selecionado na barra lateral. */
    template <typename GateType>
    void spawnMultiInputGate();

    /** @brief Recalcula os estados lógicos de todo o circuito. */
    void refreshSimulation();

    /** @brief Limpa a janela e desenha a grade, a placa, a barra lateral e a barra de status. */
    void render();

    /** @brief Desenha as instruções de uso no topo e a barra de status no rodapé da janela. */
    void drawOverlay();
};

#endif
