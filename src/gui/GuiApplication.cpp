#ifndef LOGIC_SIM_GUIAPPLICATION_CPP
#define LOGIC_SIM_GUIAPPLICATION_CPP

#if __has_include("gui/GuiApplication.h")
#include "gui/GuiApplication.h"
#include "Gates.h"
#else
#include "../../include/gui/GuiApplication.h"
#include "../../include/Gates.h"
#endif

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <optional>
#include <stdexcept>
#include <utility>

namespace {
const sf::Color COLOR_BACKGROUND(18, 20, 26);
const sf::Color COLOR_GRID(30, 33, 41);
const sf::Color COLOR_STATUS_BAR(28, 30, 36);
const sf::Color COLOR_STATUS_TEXT(220, 225, 235);
const sf::Color COLOR_HINT(130, 136, 150);
const sf::Color COLOR_GHOST_WIRE(255, 220, 60);

// Ativa o antialiasing pra deixar as linhas e bolinhas mais suaves
sf::ContextSettings makeContextSettings() {
    sf::ContextSettings settings;
    settings.antiAliasingLevel = 4;
    return settings;
}

// Monta uma string tipo "AND_2#1" pra identificar o componente nos avisos
std::string describe(const Component& comp) {
    return comp.getName() + "#" + std::to_string(comp.getId());
}
}

// Tenta achar o arquivo da fonte em várias pastas relativas diferentes
sf::Font GuiApplication::loadFont() {
    const std::filesystem::path sourceDir = std::filesystem::path(__FILE__).parent_path();
    const std::filesystem::path candidates[] = {
        "assets/fonts/DejaVuSans.ttf",
        "../assets/fonts/DejaVuSans.ttf",
        "../../assets/fonts/DejaVuSans.ttf",
        "../../../assets/fonts/DejaVuSans.ttf",
        "../../../../assets/fonts/DejaVuSans.ttf",
        "LOGIC GATE SIMULATOR/assets/fonts/DejaVuSans.ttf",
        "../LOGIC GATE SIMULATOR/assets/fonts/DejaVuSans.ttf",
        sourceDir / "../../assets/fonts/DejaVuSans.ttf",
    };
    sf::Font loaded;
    std::error_code ec;
    for (const auto& path : candidates) {
        if (std::filesystem::exists(path, ec) && loaded.openFromFile(path)) {
            return loaded;
        }
    }
    throw std::runtime_error(
        "Fonte 'assets/fonts/DejaVuSans.ttf' nao encontrada. "
        "Execute o programa a partir da raiz do projeto.");
}

// Configura a janela principal, carrega a fonte e prepara a simulação
GuiApplication::GuiApplication(CircuitBoard& circuit)
    : board(circuit),
      font(loadFont()),
      window(sf::VideoMode({1280u, 800u}), "Simulador de Portas Logicas",
             sf::Style::Default, sf::State::Windowed, makeContextSettings()),
      renderer(font),
      sidebar(),
      mode(Mode::Idle),
      wiringSource(nullptr),
      dragTarget(nullptr),
      dragOffset(0.0f, 0.0f),
      pressPosition(0.0f, 0.0f),
      dragMoved(false),
      mousePosition(0.0f, 0.0f),
      statusMessage("Pronto. Use a barra lateral para adicionar portas/chaves/LEDs, ou botão direito para excluir.") {
    window.setFramerateLimit(60);
    refreshSimulation();
}

// Mantém a janela aberta lendo eventos e desenhando a cada frame
void GuiApplication::run() {
    while (window.isOpen()) {
        processEvents();
        render();
    }
}

// Renderiza a tela uma vez e salva em PNG (útil pra tirar print automático)
bool GuiApplication::saveSnapshot(const std::string& path) {
    processEvents();
    render();
    sf::Texture texture(window.getSize());
    texture.update(window);
    return texture.copyToImage().saveToFile(path);
}

// Lê a fila de eventos do SFML (fechar janela, resize, cliques e tecla ESC)
void GuiApplication::processEvents() {
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window.close();
        } else if (const auto* resized = event->getIf<sf::Event::Resized>()) {
            const sf::Vector2f size(static_cast<float>(resized->size.x),
                                    static_cast<float>(resized->size.y));
            window.setView(sf::View(sf::FloatRect({0.0f, 0.0f}, size)));
        } else if (const auto* pressed = event->getIf<sf::Event::MouseButtonPressed>()) {
            const sf::Vector2f point = window.mapPixelToCoords(pressed->position);
            if (pressed->button == sf::Mouse::Button::Left) {
                onMousePressed(point);
            } else if (pressed->button == sf::Mouse::Button::Right) {
                onRightMousePressed(point);
            }
        } else if (const auto* released = event->getIf<sf::Event::MouseButtonReleased>()) {
            if (released->button == sf::Mouse::Button::Left) {
                onMouseReleased();
            }
        } else if (const auto* moved = event->getIf<sf::Event::MouseMoved>()) {
            onMouseMoved(window.mapPixelToCoords(moved->position));
        } else if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
            if (key->code == sf::Keyboard::Key::Escape) {
                if (mode == Mode::Wiring) {
                    cancelWiring("Ligação cancelada.");
                } else if (sidebar.isDeleteMode()) {
                    sidebar.setDeleteMode(false);
                    statusMessage = "Modo de exclusão desativado.";
                }
            }
        }
    }
}

// Trata o clique esquerdo: botões da barra lateral, puxar fios ou arrastar portas
void GuiApplication::onMousePressed(sf::Vector2f point) {
    if (sidebar.contains(point)) {
        if (mode == Mode::Wiring) {
            mode = Mode::Idle;
            wiringSource = nullptr;
        }
        handleToolAction(sidebar.handleClick(point));
        return;
    }

    if (sidebar.isDeleteMode()) {
        if (!deleteAt(point)) {
            statusMessage = "Modo EXCLUIR ativo: clique sobre um componente, pino ou fio para removê-lo.";
        }
        return;
    }

    if (mode == Mode::Wiring) {
        for (size_t i = board.getComponentCount(); i-- > 0;) {
            Component* comp = board.getComponent(i);
            const int pin = GuiRenderer::hitInputPin(*comp, point);
            if (pin >= 0) {
                tryConnect(*wiringSource, *comp, static_cast<size_t>(pin));
                mode = Mode::Idle;
                wiringSource = nullptr;
                return;
            }
        }
        cancelWiring("Ligação cancelada (clique fora de um pino de entrada).");
        return;
    }

    for (size_t i = board.getComponentCount(); i-- > 0;) {
        Component* comp = board.getComponent(i);
        if (GuiRenderer::hitOutputPin(*comp, point)) {
            mode = Mode::Wiring;
            wiringSource = comp;
            statusMessage = "Fio a partir de " + describe(*comp) +
                            ": clique em um pino de ENTRADA (ESC ou botão direito cancela).";
            return;
        }
        if (GuiRenderer::hitInputPin(*comp, point) >= 0) {
            statusMessage = "Para ligar, clique primeiro no pino de SAÍDA (lado direito). Botão direito remove a conexão.";
            return;
        }
    }

    if (Component* comp = componentAt(point)) {
        mode = Mode::Dragging;
        dragTarget = comp;
        dragOffset = point - sf::Vector2f(comp->getX(), comp->getY());
        pressPosition = point;
        dragMoved = false;
    }
}

// Clique direito serve tanto pra cancelar um fio quanto pra apagar coisas rápido
void GuiApplication::onRightMousePressed(sf::Vector2f point) {
    if (mode == Mode::Wiring) {
        cancelWiring("Ligação cancelada.");
        return;
    }
    if (sidebar.contains(point)) return;

    if (!deleteAt(point)) {
        statusMessage = "Botão direito: clique sobre uma porta/chave/LED para excluí-la ou sobre um fio/pino para desconectar.";
    }
}

// Tenta apagar o que estiver debaixo do mouse (primeiro pinos, depois componente, depois fio)
bool GuiApplication::deleteAt(sf::Vector2f point) {
    for (size_t i = board.getComponentCount(); i-- > 0;) {
        Component* comp = board.getComponent(i);
        const int inPin = GuiRenderer::hitInputPin(*comp, point);
        if (inPin >= 0) {
            if (board.disconnect(comp, static_cast<size_t>(inPin))) {
                refreshSimulation();
                statusMessage = "Conexão removida da entrada " + std::to_string(inPin) +
                                " de " + describe(*comp) + ".";
                return true;
            }
            return false;
        }
        if (GuiRenderer::hitOutputPin(*comp, point)) {
            const size_t removed = board.disconnectOutput(comp->getId());
            if (removed > 0) {
                refreshSimulation();
                statusMessage = std::to_string(removed) + " conexão(ões) removida(s) da saída de " +
                                describe(*comp) + ".";
                return true;
            }
            return false;
        }
    }

    if (Component* comp = componentAt(point)) {
        const std::string label = describe(*comp);
        if (dragTarget == comp) {
            dragTarget = nullptr;
            mode = Mode::Idle;
        }
        if (wiringSource == comp) {
            wiringSource = nullptr;
            mode = Mode::Idle;
        }
        if (board.removeComponent(comp)) {
            refreshSimulation();
            statusMessage = "Componente " + label + " e suas conexões foram excluídos.";
            return true;
        }
    }

    const int wireIdx = GuiRenderer::hitWire(board, point);
    if (wireIdx >= 0) {
        if (board.removeWireAt(static_cast<size_t>(wireIdx))) {
            refreshSimulation();
            statusMessage = "Conexão (fio) removida.";
            return true;
        }
    }

    return false;
}

// Segue o movimento do mouse e arrasta o componente se passou do limite mínimo
void GuiApplication::onMouseMoved(sf::Vector2f point) {
    mousePosition = point;
    if (mode != Mode::Dragging || dragTarget == nullptr) return;

    const sf::Vector2f delta = point - pressPosition;
    if (!dragMoved && std::sqrt(delta.x * delta.x + delta.y * delta.y) > DRAG_THRESHOLD) {
        dragMoved = true;
    }
    if (dragMoved) {
        sf::Vector2f target = point - dragOffset;
        if (target.x < Sidebar::WIDTH + 10.0f) target.x = Sidebar::WIDTH + 10.0f;
        if (target.y < 5.0f) target.y = 5.0f;
        dragTarget->setPosition(target.x, target.y);
    }
}

// Soltou o mouse: se nem mexeu e era um switch, inverte o valor dele
void GuiApplication::onMouseReleased() {
    if (mode != Mode::Dragging) return;

    if (!dragMoved && dragTarget != nullptr) {
        if (auto* sw = dynamic_cast<InputSwitch*>(dragTarget)) {
            sw->toggle();
            refreshSimulation();
            statusMessage = describe(*sw) + " = " +
                            (sw->getOutputPin()->getState() ? "1" : "0");
        }
    } else if (dragTarget != nullptr) {
        dragTarget->setPosition(std::make_pair(dragTarget->getX(), dragTarget->getY()));
        statusMessage = describe(*dragTarget) + " movido.";
    }
    mode = Mode::Idle;
    dragTarget = nullptr;
}

// Sai do modo de criação de fio e mostra a mensagem passada
void GuiApplication::cancelWiring(const std::string& reason) {
    mode = Mode::Idle;
    wiringSource = nullptr;
    statusMessage = reason;
}

// Procura de trás pra frente qual componente contém o ponto clicado
Component* GuiApplication::componentAt(sf::Vector2f point) const {
    for (size_t i = board.getComponentCount(); i-- > 0;) {
        Component* comp = board.getComponent(i);
        if (GuiRenderer::bodyRect(*comp).contains(point)) return comp;
    }
    return nullptr;
}

// Varre a tela em grade procurando um lugar vazio pra não jogar um componente em cima do outro
sf::Vector2f GuiApplication::findFreeSpot() const {
    const float padding = 20.0f;
    const float cellW = GuiRenderer::BODY_WIDTH + 50.0f;
    const float cellH = GuiRenderer::BODY_HEIGHT + 36.0f;
    const float startX = Sidebar::WIDTH + 40.0f;
    const float startY = 50.0f;
    const sf::Vector2f area = window.getView().getSize();
    const int cols = std::max(1, static_cast<int>((area.x - startX) / cellW));
    const int rows = std::max(1, static_cast<int>((area.y - startY - STATUS_BAR_HEIGHT) / cellH));

    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            const sf::Vector2f origin(startX + c * cellW, startY + r * cellH);
            const sf::FloatRect padded(origin - sf::Vector2f(padding, padding),
                                       {GuiRenderer::BODY_WIDTH + 2.0f * padding,
                                        GuiRenderer::BODY_HEIGHT + 2.0f * padding});
            bool occupied = false;
            for (size_t i = 0; i < board.getComponentCount() && !occupied; ++i) {
                occupied = GuiRenderer::bodyRect(*board.getComponent(i)).findIntersection(padded).has_value();
            }
            if (!occupied) return origin;
        }
    }
    return {startX, startY};
}

// Template pra criar portas de 2 ou 3 entradas sem repetir código no switch
template <typename GateType>
void GuiApplication::spawnMultiInputGate() {
    const sf::Vector2f pos = findFreeSpot();
    GateType* gate = board.createComponent<GateType>(board.nextFreeId(), sidebar.getSelectedInputs(),
                                                     pos.x, pos.y);
    refreshSimulation();
    statusMessage = describe(*gate) + " criado.";
}

// Faz o que o botão da barra lateral pediu (adicionar porta, switch, LED, limpar, etc.)
void GuiApplication::handleToolAction(ToolAction action) {
    const sf::Vector2f pos = findFreeSpot();
    const int id = board.nextFreeId();
    Component* created = nullptr;

    switch (action) {
        case ToolAction::AddAnd:  spawnMultiInputGate<AndGate>();  return;
        case ToolAction::AddNand: spawnMultiInputGate<NandGate>(); return;
        case ToolAction::AddOr:   spawnMultiInputGate<OrGate>();   return;
        case ToolAction::AddNor:  spawnMultiInputGate<NorGate>();  return;
        case ToolAction::AddXor:  spawnMultiInputGate<XorGate>();  return;
        case ToolAction::AddNot:
            created = board.createComponent<NotGate>(id, pos.x, pos.y);
            break;
        case ToolAction::AddSwitch:
            created = board.createComponent<InputSwitch>(id, "SW_" + std::to_string(id), false,
                                                         pos.x, pos.y);
            break;
        case ToolAction::AddLed:
            created = board.createComponent<OutputLED>(id, "LED_" + std::to_string(id),
                                                       pos.x, pos.y);
            break;
        case ToolAction::SetInputs2:
        case ToolAction::SetInputs3:
            statusMessage = "Novas portas multi-entrada terão " +
                            std::to_string(sidebar.getSelectedInputs()) + " entradas.";
            return;
        case ToolAction::ToggleDeleteMode:
            statusMessage = sidebar.isDeleteMode()
                ? "Modo EXCLUIR ativado: clique sobre qualquer porta, chave, LED ou fio para removê-lo."
                : "Modo EXCLUIR desativado.";
            return;
        case ToolAction::ClearAll:
            mode = Mode::Idle;
            wiringSource = nullptr;
            dragTarget = nullptr;
            sidebar.setDeleteMode(false);
            board.clear();
            statusMessage = "Placa limpa (todos os componentes e conexões foram removidos).";
            return;
        case ToolAction::None:
            return;
    }

    if (created) {
        refreshSimulation();
        statusMessage = describe(*created) + " criado.";
    }
}

// Valida e conecta dois componentes na placa, atualizando a mensagem lá embaixo
void GuiApplication::tryConnect(Component& source, Component& destination, size_t pinIndex) {
    if (&source == &destination) {
        statusMessage = "Um componente não pode ser ligado a si mesmo.";
        return;
    }
    if (board.isInputConnected(destination.getId(), pinIndex)) {
        statusMessage = "Pino de entrada " + std::to_string(pinIndex) + " de " +
                        describe(destination) + " já está ocupado.";
        return;
    }
    if (board.connect(&source, &destination, pinIndex)) {
        refreshSimulation();
        statusMessage = "Conectado " + describe(source) + " -> " + describe(destination) +
                        " (pino " + std::to_string(pinIndex) + ").";
    } else {
        statusMessage = "Ligação inválida.";
    }
}

// Roda um passo completo da simulação da placa
void GuiApplication::refreshSimulation() {
    board.stepSimulation();
}

// Desenha tudo na janela: grade de fundo, circuito, fio fantasma e interface
void GuiApplication::render() {
    window.clear(COLOR_BACKGROUND);

    const sf::Vector2f area = window.getView().getSize();
    const float spacing = 25.0f;
    sf::RectangleShape line;
    line.setFillColor(COLOR_GRID);
    for (float x = Sidebar::WIDTH; x < area.x; x += spacing) {
        line.setSize({1.0f, area.y});
        line.setPosition({x, 0.0f});
        window.draw(line);
    }
    for (float y = 0.0f; y < area.y; y += spacing) {
        line.setSize({area.x, 1.0f});
        line.setPosition({Sidebar::WIDTH, y});
        window.draw(line);
    }

    renderer.drawBoard(window, board, mousePosition);

    if (mode == Mode::Wiring && wiringSource != nullptr) {
        renderer.drawWire(window, GuiRenderer::outputPinPos(*wiringSource), mousePosition,
                          COLOR_GHOST_WIRE);
    }

    sidebar.draw(window, renderer, mousePosition, area.y);
    drawOverlay();
    window.display();
}

// Desenha o texto de ajuda lá em cima e a barra de status no rodapé
void GuiApplication::drawOverlay() {
    const sf::Vector2f area = window.getView().getSize();

    renderer.drawText(window,
                      "Clique na chave: alterna  |  Saída -> Entrada: liga  |  Arraste: move  |  Botão direito ou EXCLUIR: remove",
                      {Sidebar::WIDTH + 15.0f, 12.0f}, 13, COLOR_HINT);

    sf::RectangleShape bar({area.x, STATUS_BAR_HEIGHT});
    bar.setPosition({0.0f, area.y - STATUS_BAR_HEIGHT});
    bar.setFillColor(COLOR_STATUS_BAR);
    window.draw(bar);

    const std::string counts = "Componentes: " + std::to_string(board.getComponentCount()) +
                               "   Fios: " + std::to_string(board.getWireCount()) + "   |   ";
    renderer.drawText(window, counts + statusMessage, {12.0f, area.y - STATUS_BAR_HEIGHT + 6.0f},
                      14, COLOR_STATUS_TEXT);
}

#if !defined(MULTI_FILE_BUILD) && !defined(LOGIC_SIM_UNITY_INCLUDED)
#include "main_gui.cpp"
#endif

#endif
