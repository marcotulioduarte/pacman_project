#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "../include/ghost.hpp"
#include <vector>
#include <iostream>
#include <cmath>
#include <chrono>

// ==========================================
// STRUCT BOLINHA (original do pacman.cpp)
// ==========================================
struct Bolinha
{
	sf::CircleShape formato;
	bool ativa = true;
	std::chrono::steady_clock::time_point tempoColeta;
};

int main()
{
	int pontuacao = -10;

	// Configurações do Grid
	const int TAMANHO_BLOCO = 40;
	const float HUD_HEIGHT = 60.f;
	const int COLUNAS = 20;
	const int LINHAS = 15;

	const float LARGURA_TELA = static_cast<float>(COLUNAS * TAMANHO_BLOCO);
	const float ALTURA_TELA = static_cast<float>(LINHAS * TAMANHO_BLOCO + HUD_HEIGHT);
	sf::RenderWindow window(
		sf::VideoMode({static_cast<unsigned int>(LARGURA_TELA),
					   static_cast<unsigned int>(ALTURA_TELA)}),
		"Pacman com Colisao, Teleporte e Fantasmas");
	window.setFramerateLimit(60);

	// --- Texturas ---

	sf::Music musica;
	if (!musica.openFromFile("assets/audio/waka_waka.ogg"))
	{
		std::cerr << "Erro ao carregar musica waka waka\n";
		return -1;
	}

	musica.setLooping(true);
	musica.setVolume(50.f); // opcional
	musica.play();

	sf::Texture texturaParede;

	sf::Font fonte;
	if (!fonte.openFromFile("assets/fonts/PressStart2P-Regular.ttf"))
	{
		std::cerr << "Erro: Nao foi possivel carregar fonte\n";
		return -1;
	}

	sf::Text textoPontuacao(fonte);
	textoPontuacao.setCharacterSize(20);
	textoPontuacao.setFillColor(sf::Color::White);
	textoPontuacao.setPosition({10.f, 15.f});

	if (!texturaParede.loadFromFile("assets/sprites/parede.png"))
	{
		std::cerr << "Erro: Nao foi possivel carregar 'parede.png'\n";
		return -1;
	}

	sf::Texture texturaPacman;
	if (!texturaPacman.loadFromFile("assets/sprites/pacman.png"))
	{
		std::cerr << "Erro: Nao foi possivel carregar 'pacman.png'\n";
		return -1;
	}

	sf::Sprite spritePacman(texturaPacman);

	sf::Vector2u tamanhoTextura = texturaPacman.getSize();
	float scaleX = (static_cast<float>(TAMANHO_BLOCO) - 2.0f) / tamanhoTextura.x;
	float scaleY = (static_cast<float>(TAMANHO_BLOCO) - 2.0f) / tamanhoTextura.y;
	spritePacman.setScale({scaleX, scaleY});

	// Mapa (1=parede, 0=corredor)
	std::vector<std::vector<int>> mapa = {
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1},
		{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
		{1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 1},
		{1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1},
		{1, 0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1},
		{1, 0, 0, 1, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1},
		{1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1},
		{1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1},
		{1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1},
		{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1},
		{1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1},
		{1, 0, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1},
		{1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1},
		{0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1}};

	// --- Bolinhas ---
	std::vector<Bolinha> listaBolinhas;
	const float RAIO_BOLINHA = 5.0f;

	for (int i = 0; i < LINHAS; ++i)
	{
		for (int j = 0; j < COLUNAS; ++j)
		{
			if (mapa[i][j] == 0)
			{
				Bolinha b;
				b.formato.setRadius(RAIO_BOLINHA);
				b.formato.setFillColor(sf::Color(255, 184, 151));
				float posX = (j * TAMANHO_BLOCO) + (TAMANHO_BLOCO / 2.0f) - RAIO_BOLINHA;
				float posY = (i * TAMANHO_BLOCO + HUD_HEIGHT) + (TAMANHO_BLOCO / 2.0f) - RAIO_BOLINHA;
				b.formato.setPosition({posX, posY});
				listaBolinhas.push_back(b);
			}
		}
	}

	sf::RectangleShape bloco({static_cast<float>(TAMANHO_BLOCO),
							  static_cast<float>(TAMANHO_BLOCO)});
	bloco.setOutlineThickness(-1.0f);
	bloco.setOutlineColor(sf::Color(20, 20, 20));

	sf::RectangleShape hudBar;
	hudBar.setSize(sf::Vector2f(LARGURA_TELA, HUD_HEIGHT));
	hudBar.setPosition(sf::Vector2f(0.f, 0.f));
	hudBar.setFillColor(sf::Color::Black);
	// --- Fantasmas ---
	FantasmaBlinky fantasma1(9, 7, mapa, LINHAS, COLUNAS);
	FantasmaPinky fantasma2(10, 7, mapa, LINHAS, COLUNAS);
	FantasmaInky fantasma3(8, 7, mapa, LINHAS, COLUNAS);
	FantasmaClyde fantasma4(9, 9, mapa, LINHAS, COLUNAS);

	// ==========================================
	// POSIÇÃO DO PAC-MAN EM CÉLULAS (como boniexclyde.cpp)
	// ==========================================
	const int PAC_X_INICIAL = 0;
	const int PAC_Y_INICIAL = 13;
	int posx = PAC_X_INICIAL;
	int posy = PAC_Y_INICIAL;
	spritePacman.setPosition({posx * TAMANHO_BLOCO,
							  posy * TAMANHO_BLOCO + HUD_HEIGHT});

	// Direção persistente (como boniexclyde.cpp): tecla define direção até bater na parede
	bool cima = false;
	bool baixo = false;
	bool esq = false;
	bool dir = false;

	// Direção em vetor (para a IA dos fantasmas)
	int pacDirX = 0, pacDirY = 0;

	// Clocks independentes para Pac-Man e fantasmas (como boniexclyde.cpp)
	sf::Clock clockPacman;
	sf::Clock clockFantasmas;

	// ==========================================
	// LOOP DO JOGO
	// ==========================================
	while (window.isOpen())
	{

		// 1. EVENTOS E ENTRADAS
		while (const std::optional event = window.pollEvent())
		{
			if (event->is<sf::Event::Closed>())
			{
				window.close();
			}
			else if (const auto *keyPressed = event->getIf<sf::Event::KeyPressed>())
			{
				if (keyPressed->scancode == sf::Keyboard::Scancode::Escape)
					window.close();
				// Cada tecla ativa sua direção e desativa as demais (como boniexclyde.cpp)
				else if (keyPressed->scancode == sf::Keyboard::Scancode::Left)
				{
					esq = true;
					dir = cima = baixo = false;
					pacDirX = -1;
					pacDirY = 0;
				}
				else if (keyPressed->scancode == sf::Keyboard::Scancode::Right)
				{
					dir = true;
					esq = cima = baixo = false;
					pacDirX = 1;
					pacDirY = 0;
				}
				else if (keyPressed->scancode == sf::Keyboard::Scancode::Up)
				{
					cima = true;
					esq = dir = baixo = false;
					pacDirX = 0;
					pacDirY = -1;
				}
				else if (keyPressed->scancode == sf::Keyboard::Scancode::Down)
				{
					baixo = true;
					esq = dir = cima = false;
					pacDirX = 0;
					pacDirY = 1;
				}
			}
		}

		// movimento do pacman
		if (clockPacman.getElapsedTime() > sf::seconds(0.2f))
		{
			clockPacman.restart();

			// Aplica wrap e retorna o par (coluna, linha) da célula destino
			auto proxCelula = [&](int cx, int cy) -> std::pair<int, int>
			{
				if (cx < 0)
					cx = COLUNAS - 1;
				if (cx >= COLUNAS)
					cx = 0;
				if (cy < 0)
					cy = LINHAS - 1;
				if (cy >= LINHAS)
					cy = 0;
				return {cx, cy};
			};

			if (cima)
			{
				auto [nx, ny] = proxCelula(posx, posy - 1);
				if (mapa[ny][nx] != 1)
				{
					posx = nx;
					posy = ny;
				}
				else
					cima = false;
			}
			if (baixo)
			{
				auto [nx, ny] = proxCelula(posx, posy + 1);
				if (mapa[ny][nx] != 1)
				{
					posx = nx;
					posy = ny;
				}
				else
					baixo = false;
			}
			if (esq)
			{
				auto [nx, ny] = proxCelula(posx - 1, posy);
				if (mapa[ny][nx] != 1)
				{
					posx = nx;
					posy = ny;
				}
				else
					esq = false;
			}
			if (dir)
			{
				auto [nx, ny] = proxCelula(posx + 1, posy);
				if (mapa[ny][nx] != 1)
				{
					posx = nx;
					posy = ny;
				}
				else
					dir = false;
			}

			spritePacman.setPosition({posx * TAMANHO_BLOCO,
									  posy * TAMANHO_BLOCO + HUD_HEIGHT});
		}

		// coleta bolinha
		// coleta bolinha
		for (auto &b : listaBolinhas)
		{
			if (!b.ativa)
				continue;

			int bolinhaX = static_cast<int>((b.formato.getPosition().x + RAIO_BOLINHA) / TAMANHO_BLOCO);
			int bolinhaY = static_cast<int>((b.formato.getPosition().y - HUD_HEIGHT + RAIO_BOLINHA) / TAMANHO_BLOCO);

			if (bolinhaX == posx && bolinhaY == posy)
			{
				b.ativa = false;
				pontuacao += 10;
			}
		}

		// Verifica se todas as bolinhas foram comidas
		bool venceu = true;

		for (const auto &b : listaBolinhas)
		{
			if (b.ativa)
			{
				venceu = false;
				break;
			}
		}

		if (venceu)
		{
			std::cout << "VOCE VENCEU!\n";
			window.close(); // fecha o jogo
		}

		// movimento dos fantasmas

		if (clockFantasmas.getElapsedTime() > sf::seconds(0.3f))
		{
			clockFantasmas.restart();

			int blinkyX = fantasma1.posx;
			int blinkyY = fantasma1.posy;

			fantasma1.mover(posx, posy, pacDirX, pacDirY, blinkyX, blinkyY);
			fantasma2.mover(posx, posy, pacDirX, pacDirY, blinkyX, blinkyY);
			fantasma3.mover(posx, posy, pacDirX, pacDirY, blinkyX, blinkyY);
			fantasma4.mover(posx, posy, pacDirX, pacDirY, blinkyX, blinkyY);
		}

		// colisão pacman x fantasmas

		if ((posx == fantasma1.posx && posy == fantasma1.posy) ||
			(posx == fantasma2.posx && posy == fantasma2.posy) ||
			(posx == fantasma3.posx && posy == fantasma3.posy) ||
			(posx == fantasma4.posx && posy == fantasma4.posy))
		{
			std::cout << "GAME OVER! Um fantasma te pegou.\n";

			// Reseta Pac-Man
			posx = PAC_X_INICIAL;
			posy = PAC_Y_INICIAL;
			spritePacman.setPosition({posx * TAMANHO_BLOCO,
									  posy * TAMANHO_BLOCO + HUD_HEIGHT});
			cima = baixo = esq = dir = false;
			pacDirX = 0;
			pacDirY = 0;

			pontuacao = 0;
			textoPontuacao.setString("Score: 0");

			for (auto &b : listaBolinhas)
			{
				b.ativa = true;
			}

			// Reseta fantasmas
			fantasma1.posx = 9;
			fantasma1.posy = 7;
			fantasma1.dirx = -1;
			fantasma1.diry = 0;
			fantasma2.posx = 10;
			fantasma2.posy = 7;
			fantasma2.dirx = -1;
			fantasma2.diry = 0;
			fantasma3.posx = 8;
			fantasma3.posy = 7;
			fantasma3.dirx = -1;
			fantasma3.diry = 0;
			fantasma4.posx = 9;
			fantasma4.posy = 9;
			fantasma4.dirx = -1;
			fantasma4.diry = 0;
		}

		// ==========================================
		// RENDERIZAÇÃO
		// ==========================================
		window.clear();

		// Labirinto
		for (int i = 0; i < LINHAS; ++i)
		{
			for (int j = 0; j < COLUNAS; ++j)
			{
				bloco.setPosition({static_cast<float>(j * TAMANHO_BLOCO),
								   static_cast<float>(i * TAMANHO_BLOCO) + HUD_HEIGHT});
				if (mapa[i][j] == 1)
				{
					bloco.setTexture(&texturaParede);
					bloco.setFillColor(sf::Color::White);
				}
				else
				{
					bloco.setTexture(nullptr);
					bloco.setFillColor(sf::Color::Black);
				}
				window.draw(bloco);
			}
		}

		// Bolinhas ativas
		for (const auto &b : listaBolinhas)
		{
			if (b.ativa)
				window.draw(b.formato);
		}

		// Fantasmas (abaixo do Pac-Man)

		fantasma1.desenhar(window, TAMANHO_BLOCO, HUD_HEIGHT);
		fantasma2.desenhar(window, TAMANHO_BLOCO, HUD_HEIGHT);
		fantasma3.desenhar(window, TAMANHO_BLOCO, HUD_HEIGHT);
		fantasma4.desenhar(window, TAMANHO_BLOCO, HUD_HEIGHT);

		// Pac-Man por cima de tudo
		window.draw(spritePacman);

		// HUD
		window.draw(hudBar);

		// Atualiza texto do score
		textoPontuacao.setString("Score: " + std::to_string(pontuacao));
		window.draw(textoPontuacao);

		window.display();
	}

	return 0;
}