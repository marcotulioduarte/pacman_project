#ifndef GHOST_HPP
#define GHOST_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <cmath>
#include <string>

class Fantasma {
public:
    int posx, posy;
    int dirx, diry;
    sf::Texture textura;
    sf::Sprite sprite;

    const std::vector<std::vector<int>>& mapa;
    int linhas, colunas;

    Fantasma(int startX, int startY, const std::string& imagePath,
             const std::vector<std::vector<int>>& mapaRef, int l, int c);

    virtual sf::Vector2i calcularPontoAlvo(int pacX, int pacY,
                                           int pacDirX, int pacDirY,
                                           int blinkyX, int blinkyY) = 0;

    bool celulaPassavel(int cx, int cy) const;

    int wrapX(int cx) const;

    int wrapY(int cy) const;

    void mover(int pacX, int pacY, int pacDirX, int pacDirY,
               int blinkyX, int blinkyY);

    void desenhar(sf::RenderWindow &window, int tamanhoBloco, float hudHeight);

    virtual ~Fantasma() = default;
};

class FantasmaBlinky : public Fantasma {
public:
    FantasmaBlinky(int x, int y,
                   const std::vector<std::vector<int>>& m,
                   int l, int c);

    sf::Vector2i calcularPontoAlvo(int pacX, int pacY,
                                   int pacDirX, int pacDirY,
                                   int blinkyX, int blinkyY) override;
};

class FantasmaPinky : public Fantasma {
public:
    FantasmaPinky(int x, int y,
                  const std::vector<std::vector<int>>& m,
                  int l, int c);

    sf::Vector2i calcularPontoAlvo(int pacX, int pacY,
                                   int pacDirX, int pacDirY,
                                   int blinkyX, int blinkyY) override;
};

class FantasmaInky : public Fantasma {
public:
    FantasmaInky(int x, int y,
                 const std::vector<std::vector<int>>& m,
                 int l, int c);

    sf::Vector2i calcularPontoAlvo(int pacX, int pacY,
                                   int pacDirX, int pacDirY,
                                   int blinkyX, int blinkyY) override;
};

class FantasmaClyde : public Fantasma {
public:
    FantasmaClyde(int x, int y,
                  const std::vector<std::vector<int>>& m,
                  int l, int c);

    sf::Vector2i calcularPontoAlvo(int pacX, int pacY,
                                   int pacDirX, int pacDirY,
                                   int blinkyX, int blinkyY) override;
};

#endif