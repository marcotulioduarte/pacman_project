#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "../include/ghost.hpp"
#include <vector>
#include <iostream>
#include <cmath>
#include <chrono>


struct Bolinha
{
	sf::Sprite sprite;
	bool ativa = true;
	std::chrono::steady_clock::time_point tempoColeta;
	Bolinha(const sf::Texture &textura) : sprite(textura) {}
};

int main()
{
	// coloquei -10 porque ele começa comendo a primeira bolinha;
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
		"Pacman - World Cup");
	window.setFramerateLimit(60);

	//texturas

	sf::Music musica;
	if (!musica.openFromFile("assets/audio/waka_waka.ogg"))
	{
		std::cerr << "Erro ao carregar musica waka waka\n";
		return -1;
	}

	musica.setLooping(true);
	musica.setVolume(50.f); 
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

	sf::Text textoGameOver(fonte);
	textoGameOver.setCharacterSize(40);
	textoGameOver.setFillColor(sf::Color(255, 215, 0));
	textoGameOver.setStyle(sf::Text::Bold);
	textoGameOver.setString("GAME OVER");

	sf::FloatRect bounds = textoGameOver.getLocalBounds();
	textoGameOver.setOrigin({bounds.position.x + bounds.size.x / 2.f,
							 bounds.position.y + bounds.size.y / 2.f});
	textoGameOver.setPosition({LARGURA_TELA / 2.f,
							   ALTURA_TELA / 2.f});

	if (!texturaParede.loadFromFile("assets/sprites/parede.png"))
	{
		std::cerr << "Erro: Nao foi possivel carregar 'parede.png'\n";
		return -1;
	}

	// sprite do pavman;
	sf::Texture texturaPacmanUp;
	sf::Texture texturaPacmanDown;
	sf::Texture texturaPacmanLeft;
	sf::Texture texturaPacmanRight;

	if (!texturaPacmanUp.loadFromFile("assets/sprites/pacman_cima.png") ||
		!texturaPacmanDown.loadFromFile("assets/sprites/pacman_baixo.png") ||
		!texturaPacmanLeft.loadFromFile("assets/sprites/pacman_esquerda.png") ||
		!texturaPacmanRight.loadFromFile("assets/sprites/pacman_direita.png"))
	{
		std::cerr << "Erro ao carregar sprites do Pacman\n";
		return -1;
	}
	sf::Texture texturaSemente;
	if (!texturaSemente.loadFromFile("assets/sprites/pilulas.png"))
	{
		std::cerr << "Erro: Nao foi possivel carregar 'pilulas.png'\n";
		return -1;
	}

	sf::Sprite spritePacman(texturaPacmanRight);

	sf::Vector2u tamanhoTextura = texturaPacmanRight.getSize();
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

	// bolinhas
	std::vector<Bolinha> listaBolinhas;

	const float TAMANHO_DESEJADO = 60.0f;
	float RAIO_BOLINHA = TAMANHO_DESEJADO / 2.0f;

	sf::Vector2u tamSemente = texturaSemente.getSize();

	float scaleSementeX = TAMANHO_DESEJADO / tamSemente.x;
	float scaleSementeY = TAMANHO_DESEJADO / tamSemente.y;

	for (int i = 0; i < LINHAS; ++i)
	{
		for (int j = 0; j < COLUNAS; ++j)
		{
			if (mapa[i][j] == 0)
			{
				Bolinha b(texturaSemente);

				b.sprite.setScale({scaleSementeX, scaleSementeY});

				float posX = (j * TAMANHO_BLOCO) + (TAMANHO_BLOCO / 2.0f) - RAIO_BOLINHA;
				float posY = (i * TAMANHO_BLOCO + HUD_HEIGHT) + (TAMANHO_BLOCO / 2.0f) - RAIO_BOLINHA;

				b.sprite.setPosition({posX, posY});
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
	// fantasmas
	FantasmaBlinky fantasma1(9, 7, mapa, LINHAS, COLUNAS);
	FantasmaPinky fantasma2(10, 7, mapa, LINHAS, COLUNAS);
	FantasmaInky fantasma3(8, 7, mapa, LINHAS, COLUNAS);
	FantasmaClyde fantasma4(9, 9, mapa, LINHAS, COLUNAS);

	// posição do pacman
	const int PAC_X_INICIAL = 0;
	const int PAC_Y_INICIAL = 13;
	int posx = PAC_X_INICIAL;
	int posy = PAC_Y_INICIAL;
	spritePacman.setPosition({static_cast<float>(posx * TAMANHO_BLOCO),
							  static_cast<float>(posy * TAMANHO_BLOCO + HUD_HEIGHT)});

	//tecla define direção até bater na parede
	bool cima = false;
	bool baixo = false;
	bool esq = false;
	bool dir = false;

	// Direção em vetor 
	int pacDirX = 0, pacDirY = 0;

	auto atualizarSpritePacman = [&]()
	{
		if (pacDirX == 1)
			spritePacman.setTexture(texturaPacmanRight);
		else if (pacDirX == -1)
			spritePacman.setTexture(texturaPacmanLeft);
		else if (pacDirY == -1)
			spritePacman.setTexture(texturaPacmanUp);
		else if (pacDirY == 1)
			spritePacman.setTexture(texturaPacmanDown);
	};

	// Clocks independentes para pacman e fantasmas 
	sf::Clock clockPacman;
	sf::Clock clockFantasmas;
	sf::Clock relogio;
	
	auto suavizarMovimento = [&](auto &objetoGrafico, int gridX, int gridY, float tempoTimer, float deltaT)
	{
		float alvoX = static_cast<float>(gridX * TAMANHO_BLOCO);
		float alvoY = static_cast<float>(gridY * TAMANHO_BLOCO + HUD_HEIGHT);

		float atualX = objetoGrafico.getPosition().x;
		float atualY = objetoGrafico.getPosition().y;

		float velocidadeSuave = TAMANHO_BLOCO / tempoTimer;

		// Move na direção do alvo
		if (atualX < alvoX)
			atualX += velocidadeSuave * deltaT;
		else if (atualX > alvoX)
			atualX -= velocidadeSuave * deltaT;

		if (atualY < alvoY)
			atualY += velocidadeSuave * deltaT;
		else if (atualY > alvoY)
			atualY -= velocidadeSuave * deltaT;

		// Regra do teletransporte do túnel
		if (std::abs(alvoX - atualX) > TAMANHO_BLOCO * 1.5f ||
			std::abs(alvoY - atualY) > TAMANHO_BLOCO * 1.5f)
		{
			atualX = alvoX;
			atualY = alvoY;
		}
		else
		{
			
			if (std::abs(alvoX - atualX) < (velocidadeSuave * deltaT))
				atualX = alvoX;
			if (std::abs(alvoY - atualY) < (velocidadeSuave * deltaT))
				atualY = alvoY;
		}

		objetoGrafico.setPosition({atualX, atualY});
	};

	bool gameOver = false;
	sf::Clock gameOverClock;
	
	//loop do jogo
	while (window.isOpen())
	{
		float dt = relogio.restart().asSeconds();
		if (gameOver)
		{
			while (const std::optional event = window.pollEvent())
			{
				if (event->is<sf::Event::Closed>())
					window.close();
			}
			window.clear(sf::Color::Black);

			window.draw(textoGameOver);

			window.display();

			if (gameOverClock.getElapsedTime().asSeconds() >= 3.f)
			{
				gameOver = false;

				posx = PAC_X_INICIAL;
				posy = PAC_Y_INICIAL;

				spritePacman.setPosition({static_cast<float>(posx * TAMANHO_BLOCO),
										  static_cast<float>(posy * TAMANHO_BLOCO + HUD_HEIGHT)});

										  
				cima = baixo = esq = dir = false;
				pacDirX = 0;
				pacDirY = 0;

				pontuacao = -10;
				textoPontuacao.setString("Score: 0");

				for (auto &b : listaBolinhas)
					b.ativa = true;

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

				clockPacman.restart();
				clockFantasmas.restart();
			}

			continue;
		}
		
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

				// Cada tecla ativa sua direção e desativa as demais 
				else if (keyPressed->scancode == sf::Keyboard::Scancode::Left)
				{
					esq = true;
					dir = cima = baixo = false;
					pacDirX = -1;
					pacDirY = 0;
					atualizarSpritePacman();
				}
				else if (keyPressed->scancode == sf::Keyboard::Scancode::Right)
				{
					dir = true;
					esq = cima = baixo = false;
					pacDirX = 1;
					pacDirY = 0;
					atualizarSpritePacman();
				}
				else if (keyPressed->scancode == sf::Keyboard::Scancode::Up)
				{
					cima = true;
					esq = dir = baixo = false;
					pacDirX = 0;
					pacDirY = -1;
					atualizarSpritePacman();
				}
				else if (keyPressed->scancode == sf::Keyboard::Scancode::Down)
				{
					baixo = true;
					esq = dir = cima = false;
					pacDirX = 0;
					pacDirY = 1;
					atualizarSpritePacman();
				}
			}
		}

		// movimento do pacman
		if (clockPacman.getElapsedTime() > sf::seconds(0.2f))
		{
			clockPacman.restart();
		
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
		}

		// coleta bolinha
		for (auto &b : listaBolinhas)
		{
			if (!b.ativa)
				continue;
			int bolinhaX = static_cast<int>((b.sprite.getPosition().x + RAIO_BOLINHA) / TAMANHO_BLOCO);
			int bolinhaY = static_cast<int>((b.sprite.getPosition().y - HUD_HEIGHT + RAIO_BOLINHA) / TAMANHO_BLOCO);
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
			window.close();
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

		// colisão entre pacman e fantasmas
		if ((posx == fantasma1.posx && posy == fantasma1.posy) ||
			(posx == fantasma2.posx && posy == fantasma2.posy) ||
			(posx == fantasma3.posx && posy == fantasma3.posy) ||
			(posx == fantasma4.posx && posy == fantasma4.posy))
		{
			gameOver = true;
			gameOverClock.restart();
		}

		
		suavizarMovimento(spritePacman, posx, posy, 0.2f, dt);
		suavizarMovimento(fantasma1.sprite, fantasma1.posx, fantasma1.posy, 0.3f, dt);
		suavizarMovimento(fantasma2.sprite, fantasma2.posx, fantasma2.posy, 0.3f, dt);
		suavizarMovimento(fantasma3.sprite, fantasma3.posx, fantasma3.posy, 0.3f, dt);
		suavizarMovimento(fantasma4.sprite, fantasma4.posx, fantasma4.posy, 0.3f, dt);

		window.clear();

		
		for (int i = 0; i < LINHAS; ++i)
		{
			for (int j = 0; j < COLUNAS; ++j)
			{
				bloco.setPosition({static_cast<float>(j * TAMANHO_BLOCO),
								   static_cast<float>(i * TAMANHO_BLOCO) + HUD_HEIGHT});

				// mapa com textura de gramado
				bloco.setTexture(&texturaParede);

				if (mapa[i][j] == 1)
				{
					
					bloco.setFillColor(sf::Color::White);
				}
				else
				{
					// gramado escuro no corredor
					bloco.setFillColor(sf::Color(110, 110, 110));
				}
				window.draw(bloco);
			}
		}

		// ativa as pilulas
		for (const auto &b : listaBolinhas)
		{
			if (b.ativa)
				window.draw(b.sprite);
		}

		// Fantasmas
		fantasma1.desenhar(window, TAMANHO_BLOCO, HUD_HEIGHT);
		fantasma2.desenhar(window, TAMANHO_BLOCO, HUD_HEIGHT);
		fantasma3.desenhar(window, TAMANHO_BLOCO, HUD_HEIGHT);
		fantasma4.desenhar(window, TAMANHO_BLOCO, HUD_HEIGHT);

		
		window.draw(spritePacman);

		// texto de pontução
		window.draw(hudBar);
		textoPontuacao.setString("Score: " + std::to_string(pontuacao));
		window.draw(textoPontuacao);

		window.display();
	}

	return 0;
}