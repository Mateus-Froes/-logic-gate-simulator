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
const sf::Color COR_FUNDO(18, 20, 26);
const sf::Color COR_GRADE(30, 33, 41);
const sf::Color COR_BARRA_STATUS(28, 30, 36);
const sf::Color COR_TEXTO_STATUS(220, 225, 235);
const sf::Color COR_DICA(130, 136, 150);
const sf::Color COR_FIO_FANTASMA(255, 220, 60);

// Ativa o antialiasing pra deixar as linhas e bolinhas mais suaves
sf::ContextSettings criaConfigSuavizacao() {
    sf::ContextSettings config;
    config.antiAliasingLevel = 4;
    return config;
}

// Monta uma string tipo "AND_2#1" pra identificar o componente nos avisos
std::string descreveComp(const Componente& comp) {
    return comp.getName() + "#" + std::to_string(comp.getId());
}
}

// Tenta achar o arquivo da fonte em várias pastas relativas diferentes
sf::Font JanelaApp::loadFont() {
    const std::filesystem::path pastaFonte = std::filesystem::path(__FILE__).parent_path();
    const std::filesystem::path caminhosPossiveis[] = {
        "assets/fonts/DejaVuSans.ttf",
        "../assets/fonts/DejaVuSans.ttf",
        "../../assets/fonts/DejaVuSans.ttf",
        "../../../assets/fonts/DejaVuSans.ttf",
        "../../../../assets/fonts/DejaVuSans.ttf",
        "LOGIC GATE SIMULATOR/assets/fonts/DejaVuSans.ttf",
        "../LOGIC GATE SIMULATOR/assets/fonts/DejaVuSans.ttf",
        pastaFonte / "../../assets/fonts/DejaVuSans.ttf",
    };
    sf::Font carregada;
    std::error_code ec;
    for (const auto& caminho : caminhosPossiveis) {
        if (std::filesystem::exists(caminho, ec) && carregada.openFromFile(caminho)) {
            return carregada;
        }
    }
    throw std::runtime_error(
        "Fonte 'assets/fonts/DejaVuSans.ttf' nao encontrada. "
        "Execute o programa a partir da raiz do projeto.");
}

// Configura a janela principal, carrega a fonte e prepara a simulação
JanelaApp::JanelaApp(Placa& circuito)
    : placa(circuito),
      fonte(loadFont()),
      janela(sf::VideoMode({1280u, 800u}), "Simulador de Portas Logicas",
             sf::Style::Default, sf::State::Windowed, criaConfigSuavizacao()),
      desenhista(fonte),
      barra(),
      modo(Modo::Parado),
      origemFio(nullptr),
      alvoArraste(nullptr),
      offsetArraste(0.0f, 0.0f),
      posClique(0.0f, 0.0f),
      moveuArraste(false),
      posMouse(0.0f, 0.0f),
      msgStatus("Pronto. Use a barra lateral para adicionar portas/chaves/LEDs, ou botão direito para excluir.") {
    janela.setFramerateLimit(60);
    refreshSimulation();
}

// Mantém a janela aberta lendo eventos e desenhando a cada frame
void JanelaApp::run() {
    while (janela.isOpen()) {
        processEvents();
        render();
    }
}

// Renderiza a tela uma vez e salva em PNG (útil pra tirar print automático)
bool JanelaApp::saveSnapshot(const std::string& caminho) {
    processEvents();
    render();
    sf::Texture textura(janela.getSize());
    textura.update(janela);
    return textura.copyToImage().saveToFile(caminho);
}

// Lê a fila de eventos do SFML (fechar janela, resize, cliques e tecla ESC)
void JanelaApp::processEvents() {
    while (const std::optional evento = janela.pollEvent()) {
        if (evento->is<sf::Event::Closed>()) {
            janela.close();
        } else if (const auto* redim = evento->getIf<sf::Event::Resized>()) {
            const sf::Vector2f tam(static_cast<float>(redim->size.x),
                                   static_cast<float>(redim->size.y));
            janela.setView(sf::View(sf::FloatRect({0.0f, 0.0f}, tam)));
        } else if (const auto* clique = evento->getIf<sf::Event::MouseButtonPressed>()) {
            const sf::Vector2f ponto = janela.mapPixelToCoords(clique->position);
            if (clique->button == sf::Mouse::Button::Left) {
                onMousePressed(ponto);
            } else if (clique->button == sf::Mouse::Button::Right) {
                onRightMousePressed(ponto);
            }
        } else if (const auto* soltou = evento->getIf<sf::Event::MouseButtonReleased>()) {
            if (soltou->button == sf::Mouse::Button::Left) {
                onMouseReleased();
            }
        } else if (const auto* moveu = evento->getIf<sf::Event::MouseMoved>()) {
            onMouseMoved(janela.mapPixelToCoords(moveu->position));
        } else if (const auto* tecla = evento->getIf<sf::Event::KeyPressed>()) {
            if (tecla->code == sf::Keyboard::Key::Escape) {
                if (modo == Modo::LigandoFio) {
                    cancelWiring("Ligação cancelada.");
                } else if (barra.isDeleteMode()) {
                    barra.setDeleteMode(false);
                    msgStatus = "Modo de exclusão desativado.";
                }
            }
        }
    }
}

// Trata o clique esquerdo: botões da barra lateral, puxar fios ou arrastar portas
void JanelaApp::onMousePressed(sf::Vector2f ponto) {
    if (barra.contains(ponto)) {
        if (modo == Modo::LigandoFio) {
            modo = Modo::Parado;
            origemFio = nullptr;
        }
        handleToolAction(barra.handleClick(ponto));
        return;
    }

    if (barra.isDeleteMode()) {
        if (!deleteAt(ponto)) {
            msgStatus = "Modo EXCLUIR ativo: clique sobre um componente, pino ou fio para removê-lo.";
        }
        return;
    }

    if (modo == Modo::LigandoFio) {
        for (size_t i = placa.getComponentCount(); i-- > 0;) {
            Componente* comp = placa.getComponent(i);
            const int pino = Desenhista::hitInputPin(*comp, ponto);
            if (pino >= 0) {
                tryConnect(*origemFio, *comp, static_cast<size_t>(pino));
                modo = Modo::Parado;
                origemFio = nullptr;
                return;
            }
        }
        cancelWiring("Ligação cancelada (clique fora de um pino de entrada).");
        return;
    }

    for (size_t i = placa.getComponentCount(); i-- > 0;) {
        Componente* comp = placa.getComponent(i);
        if (Desenhista::hitOutputPin(*comp, ponto)) {
            modo = Modo::LigandoFio;
            origemFio = comp;
            msgStatus = "Fio a partir de " + descreveComp(*comp) +
                        ": clique em um pino de ENTRADA (ESC ou botão direito cancela).";
            return;
        }
        if (Desenhista::hitInputPin(*comp, ponto) >= 0) {
            msgStatus = "Para ligar, clique primeiro no pino de SAÍDA (lado direito). Botão direito remove a conexão.";
            return;
        }
    }

    if (Componente* comp = componentAt(ponto)) {
        modo = Modo::Arrastando;
        alvoArraste = comp;
        offsetArraste = ponto - sf::Vector2f(comp->getX(), comp->getY());
        posClique = ponto;
        moveuArraste = false;
    }
}

// Clique direito serve tanto pra cancelar um fio quanto pra apagar coisas rápido
void JanelaApp::onRightMousePressed(sf::Vector2f ponto) {
    if (modo == Modo::LigandoFio) {
        cancelWiring("Ligação cancelada.");
        return;
    }
    if (barra.contains(ponto)) return;

    if (!deleteAt(ponto)) {
        msgStatus = "Botão direito: clique sobre uma porta/chave/LED para excluí-la ou sobre um fio/pino para desconectar.";
    }
}

// Tenta apagar o que estiver debaixo do mouse (primeiro pinos, depois componente, depois fio)
bool JanelaApp::deleteAt(sf::Vector2f ponto) {
    for (size_t i = placa.getComponentCount(); i-- > 0;) {
        Componente* comp = placa.getComponent(i);
        const int pinoIn = Desenhista::hitInputPin(*comp, ponto);
        if (pinoIn >= 0) {
            if (placa.disconnect(comp, static_cast<size_t>(pinoIn))) {
                refreshSimulation();
                msgStatus = "Conexão removida da entrada " + std::to_string(pinoIn) +
                            " de " + descreveComp(*comp) + ".";
                return true;
            }
            return false;
        }
        if (Desenhista::hitOutputPin(*comp, ponto)) {
            const size_t removidos = placa.disconnectOutput(comp->getId());
            if (removidos > 0) {
                refreshSimulation();
                msgStatus = std::to_string(removidos) + " conexão(ões) removida(s) da saída de " +
                            descreveComp(*comp) + ".";
                return true;
            }
            return false;
        }
    }

    if (Componente* comp = componentAt(ponto)) {
        const std::string etiqueta = descreveComp(*comp);
        if (alvoArraste == comp) {
            alvoArraste = nullptr;
            modo = Modo::Parado;
        }
        if (origemFio == comp) {
            origemFio = nullptr;
            modo = Modo::Parado;
        }
        if (placa.removeComponent(comp)) {
            refreshSimulation();
            msgStatus = "Componente " + etiqueta + " e suas conexões foram excluídos.";
            return true;
        }
    }

    const int fioIdx = Desenhista::hitWire(placa, ponto);
    if (fioIdx >= 0) {
        if (placa.removeWireAt(static_cast<size_t>(fioIdx))) {
            refreshSimulation();
            msgStatus = "Conexão (fio) removida.";
            return true;
        }
    }

    return false;
}

// Segue o movimento do mouse e arrasta o componente se passou do limite mínimo
void JanelaApp::onMouseMoved(sf::Vector2f ponto) {
    posMouse = ponto;
    if (modo != Modo::Arrastando || alvoArraste == nullptr) return;

    const sf::Vector2f delta = ponto - posClique;
    if (!moveuArraste && std::sqrt(delta.x * delta.x + delta.y * delta.y) > DRAG_THRESHOLD) {
        moveuArraste = true;
    }
    if (moveuArraste) {
        sf::Vector2f destino = ponto - offsetArraste;
        if (destino.x < BarraLateral::WIDTH + 10.0f) destino.x = BarraLateral::WIDTH + 10.0f;
        if (destino.y < 5.0f) destino.y = 5.0f;
        alvoArraste->setPosition(destino.x, destino.y);
    }
}

// Soltou o mouse: se nem mexeu e era uma chave, inverte o valor dela
void JanelaApp::onMouseReleased() {
    if (modo != Modo::Arrastando) return;

    if (!moveuArraste && alvoArraste != nullptr) {
        if (auto* ch = dynamic_cast<Chave*>(alvoArraste)) {
            ch->toggle();
            refreshSimulation();
            msgStatus = descreveComp(*ch) + " = " +
                        (ch->getOutputPin()->getState() ? "1" : "0");
        }
    } else if (alvoArraste != nullptr) {
        alvoArraste->setPosition(std::make_pair(alvoArraste->getX(), alvoArraste->getY()));
        msgStatus = descreveComp(*alvoArraste) + " movido.";
    }
    modo = Modo::Parado;
    alvoArraste = nullptr;
}

// Sai do modo de criação de fio e mostra a mensagem passada
void JanelaApp::cancelWiring(const std::string& motivo) {
    modo = Modo::Parado;
    origemFio = nullptr;
    msgStatus = motivo;
}

// Procura de trás pra frente qual componente contém o ponto clicado
Componente* JanelaApp::componentAt(sf::Vector2f ponto) const {
    for (size_t i = placa.getComponentCount(); i-- > 0;) {
        Componente* comp = placa.getComponent(i);
        if (Desenhista::bodyRect(*comp).contains(ponto)) return comp;
    }
    return nullptr;
}

// Varre a tela em grade procurando um lugar vazio pra não jogar um componente em cima do outro
sf::Vector2f JanelaApp::findFreeSpot() const {
    const float folga = 20.0f;
    const float celW = Desenhista::BODY_WIDTH + 50.0f;
    const float celH = Desenhista::BODY_HEIGHT + 36.0f;
    const float inicioX = BarraLateral::WIDTH + 40.0f;
    const float inicioY = 50.0f;
    const sf::Vector2f area = janela.getView().getSize();
    const int cols = std::max(1, static_cast<int>((area.x - inicioX) / celW));
    const int rows = std::max(1, static_cast<int>((area.y - inicioY - STATUS_BAR_HEIGHT) / celH));

    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            const sf::Vector2f origem(inicioX + c * celW, inicioY + r * celH);
            const sf::FloatRect caixaFolga(origem - sf::Vector2f(folga, folga),
                                           {Desenhista::BODY_WIDTH + 2.0f * folga,
                                            Desenhista::BODY_HEIGHT + 2.0f * folga});
            bool ocupado = false;
            for (size_t i = 0; i < placa.getComponentCount() && !ocupado; ++i) {
                ocupado = Desenhista::bodyRect(*placa.getComponent(i)).findIntersection(caixaFolga).has_value();
            }
            if (!ocupado) return origem;
        }
    }
    return {inicioX, inicioY};
}

// Template pra criar portas de 2 ou 3 entradas sem repetir código no switch
template <typename TipoPorta>
void JanelaApp::spawnMultiInputGate() {
    const sf::Vector2f pos = findFreeSpot();
    TipoPorta* porta = placa.createComponent<TipoPorta>(placa.nextFreeId(), barra.getSelectedInputs(),
                                                        pos.x, pos.y);
    refreshSimulation();
    msgStatus = descreveComp(*porta) + " criado.";
}

// Faz o que o botão da barra lateral pediu (adicionar porta, switch, LED, limpar, etc.)
void JanelaApp::handleToolAction(ToolAction acao) {
    const sf::Vector2f pos = findFreeSpot();
    const int id = placa.nextFreeId();
    Componente* criado = nullptr;

    switch (acao) {
        case ToolAction::AddAnd:  spawnMultiInputGate<PortaAnd>();  return;
        case ToolAction::AddNand: spawnMultiInputGate<PortaNand>(); return;
        case ToolAction::AddOr:   spawnMultiInputGate<PortaOr>();   return;
        case ToolAction::AddNor:  spawnMultiInputGate<PortaNor>();  return;
        case ToolAction::AddXor:  spawnMultiInputGate<PortaXor>();  return;
        case ToolAction::AddNot:
            criado = placa.createComponent<PortaNot>(id, pos.x, pos.y);
            break;
        case ToolAction::AddSwitch:
            criado = placa.createComponent<Chave>(id, "SW_" + std::to_string(id), false,
                                                  pos.x, pos.y);
            break;
        case ToolAction::AddLed:
            criado = placa.createComponent<Led>(id, "LED_" + std::to_string(id),
                                                pos.x, pos.y);
            break;
        case ToolAction::SetInputs2:
        case ToolAction::SetInputs3:
            msgStatus = "Novas portas multi-entrada terão " +
                        std::to_string(barra.getSelectedInputs()) + " entradas.";
            return;
        case ToolAction::ToggleDeleteMode:
            msgStatus = barra.isDeleteMode()
                ? "Modo EXCLUIR ativado: clique sobre qualquer porta, chave, LED ou fio para removê-lo."
                : "Modo EXCLUIR desativado.";
            return;
        case ToolAction::ClearAll:
            modo = Modo::Parado;
            origemFio = nullptr;
            alvoArraste = nullptr;
            barra.setDeleteMode(false);
            placa.clear();
            msgStatus = "Placa limpa (todos os componentes e conexões foram removidos).";
            return;
        case ToolAction::None:
            return;
    }

    if (criado) {
        refreshSimulation();
        msgStatus = descreveComp(*criado) + " criado.";
    }
}

// Valida e conecta dois componentes na placa, atualizando a mensagem lá embaixo
void JanelaApp::tryConnect(Componente& origem, Componente& destino, size_t pinoIdx) {
    if (&origem == &destino) {
        msgStatus = "Um componente não pode ser ligado a si mesmo.";
        return;
    }
    if (placa.isInputConnected(destino.getId(), pinoIdx)) {
        msgStatus = "Pino de entrada " + std::to_string(pinoIdx) + " de " +
                    descreveComp(destino) + " já está ocupado.";
        return;
    }
    if (placa.connect(&origem, &destino, pinoIdx)) {
        refreshSimulation();
        msgStatus = "Conectado " + descreveComp(origem) + " -> " + descreveComp(destino) +
                    " (pino " + std::to_string(pinoIdx) + ").";
    } else {
        msgStatus = "Ligação inválida.";
    }
}

// Roda um passo completo da simulação da placa
void JanelaApp::refreshSimulation() {
    placa.stepSimulation();
}

// Desenha tudo na janela: grade de fundo, circuito, fio fantasma e interface
void JanelaApp::render() {
    janela.clear(COR_FUNDO);

    const sf::Vector2f area = janela.getView().getSize();
    const float espacamento = 25.0f;
    sf::RectangleShape linha;
    linha.setFillColor(COR_GRADE);
    for (float x = BarraLateral::WIDTH; x < area.x; x += espacamento) {
        linha.setSize({1.0f, area.y});
        linha.setPosition({x, 0.0f});
        janela.draw(linha);
    }
    for (float y = 0.0f; y < area.y; y += espacamento) {
        linha.setSize({area.x, 1.0f});
        linha.setPosition({BarraLateral::WIDTH, y});
        janela.draw(linha);
    }

    desenhista.drawBoard(janela, placa, posMouse);

    if (modo == Modo::LigandoFio && origemFio != nullptr) {
        desenhista.drawWire(janela, Desenhista::outputPinPos(*origemFio), posMouse,
                            COR_FIO_FANTASMA);
    }

    barra.draw(janela, desenhista, posMouse, area.y);
    drawOverlay();
    janela.display();
}

// Desenha o texto de ajuda lá em cima e a barra de status no rodapé
void JanelaApp::drawOverlay() {
    const sf::Vector2f area = janela.getView().getSize();

    desenhista.drawText(janela,
                        "Clique na chave: alterna  |  Saída -> Entrada: liga  |  Arraste: move  |  Botão direito ou EXCLUIR: remove",
                        {BarraLateral::WIDTH + 15.0f, 12.0f}, 13, COR_DICA);

    sf::RectangleShape barraRodape({area.x, STATUS_BAR_HEIGHT});
    barraRodape.setPosition({0.0f, area.y - STATUS_BAR_HEIGHT});
    barraRodape.setFillColor(COR_BARRA_STATUS);
    janela.draw(barraRodape);

    const std::string contadores = "Componentes: " + std::to_string(placa.getComponentCount()) +
                                   "   Fios: " + std::to_string(placa.getWireCount()) + "   |   ";
    desenhista.drawText(janela, contadores + msgStatus, {12.0f, area.y - STATUS_BAR_HEIGHT + 6.0f},
                        14, COR_TEXTO_STATUS);
}

#if !defined(MULTI_FILE_BUILD) && !defined(LOGIC_SIM_UNITY_INCLUDED)
#include "main_gui.cpp"
#endif

#endif
