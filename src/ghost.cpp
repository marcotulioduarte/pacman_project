#include "../include/ghost.hpp"

Fantasma::Fantasma(int startX, int startY,
                   const std::string &imagePath,
                   const std::vector<std::vector<int>> &mapaRef,
                   int l, int c)
    : sprite(textura), mapa(mapaRef), linhas(l), colunas(c)
{
    posx = startX;
    posy = startY;
    dirx = -1;
    diry = 0;

    if (!textura.loadFromFile(imagePath))
    {
        std::cout << "Erro ao carregar " << imagePath << "\n";
    }
    sprite = sf::Sprite(textura);
}

bool Fantasma::celulaPassavel(int cx, int cy) const
{
    if (cx < 0)
        cx = colunas - 1;
    if (cx >= colunas)
        cx = 0;
    if (cy < 0)
        cy = linhas - 1;
    if (cy >= linhas)
        cy = 0;
    return mapa[cy][cx] != 1;
}

int Fantasma::wrapX(int cx) const
{
    if (cx < 0)
        return colunas - 1;
    if (cx >= colunas)
        return 0;
    return cx;
}

int Fantasma::wrapY(int cy) const
{
    if (cy < 0)
        return linhas - 1;
    if (cy >= linhas)
        return 0;
    return cy;
}

void Fantasma::mover(int pacX, int pacY, int pacDirX, int pacDirY,
                     int blinkyX, int blinkyY)
{
    sf::Vector2i alvo = calcularPontoAlvo(pacX, pacY, pacDirX, pacDirY,
                                          blinkyX, blinkyY);

    std::vector<sf::Vector2i> direcoes = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}};

    float menorDistancia = 999999.0f;
    sf::Vector2i melhorDirecao = {dirx, diry};
    bool moveu = false;

    for (auto &d : direcoes)
    {

        if (d.x == -dirx && d.y == -diry && (dirx != 0 || diry != 0))
            continue;

        int testaX = posx + d.x;
        int testaY = posy + d.y;

        if (celulaPassavel(testaX, testaY))
        {

            int wxT = wrapX(testaX);
            int wyT = wrapY(testaY);

            float dist = std::sqrt(
                std::pow(static_cast<float>(alvo.x - wxT), 2.0f) +
                std::pow(static_cast<float>(alvo.y - wyT), 2.0f));

            if (dist < menorDistancia)
            {
                menorDistancia = dist;
                melhorDirecao = d;
                moveu = true;
            }
        }
    }

    if (!moveu)
    {

        sf::Vector2i meia = {-dirx, -diry};

        if (celulaPassavel(posx + meia.x, posy + meia.y))
        {
            melhorDirecao = meia;
            moveu = true;
        }
    }

    if (moveu)
    {
        dirx = melhorDirecao.x;
        diry = melhorDirecao.y;
        posx = wrapX(posx + dirx);
        posy = wrapY(posy + diry);
    }
}

   void Fantasma::desenhar(sf::RenderWindow &window, int tamanhoBloco, float hudHeight)
{
    sf::Vector2u sz = textura.getSize();

    if (sz.x > 0 && sz.y > 0)
    {
        float sx = static_cast<float>(tamanhoBloco - 2) / sz.x;
        float sy = static_cast<float>(tamanhoBloco - 2) / sz.y;
        sprite.setScale({sx, sy});
    }

    sprite.setPosition({
        static_cast<float>(posx * tamanhoBloco),
        static_cast<float>(posy * tamanhoBloco + hudHeight)
    });

    window.draw(sprite);
}

// ==========================================
// BLINKY
// ==========================================

FantasmaBlinky::FantasmaBlinky(int x, int y,
                               const std::vector<std::vector<int>> &m,
                               int l, int c)
    : Fantasma(x, y, "assets/sprites/alemanha.png", m, l, c) {}

sf::Vector2i FantasmaBlinky::calcularPontoAlvo(int pacX, int pacY,
                                               int pacDirX, int pacDirY,
                                               int blinkyX, int blinkyY)
{
    return {pacX, pacY};
}

// ==========================================
// PINKY
// ==========================================

FantasmaPinky::FantasmaPinky(int x, int y,
                             const std::vector<std::vector<int>> &m,
                             int l, int c)
    : Fantasma(x, y, "assets/sprites/argentina.png", m, l, c) {}

sf::Vector2i FantasmaPinky::calcularPontoAlvo(int pacX, int pacY,
                                              int pacDirX, int pacDirY,
                                              int blinkyX, int blinkyY)
{
    return {pacX + (pacDirX * 4), pacY + (pacDirY * 4)};
}

// ==========================================
// INKY
// ==========================================

FantasmaInky::FantasmaInky(int x, int y,
                           const std::vector<std::vector<int>> &m,
                           int l, int c)
    : Fantasma(x, y, "assets/sprites/franca.png", m, l, c) {}

sf::Vector2i FantasmaInky::calcularPontoAlvo(int pacX, int pacY,
                                             int pacDirX, int pacDirY,
                                             int blinkyX, int blinkyY)
{
    int pivotX = pacX + (pacDirX * 2);
    int pivotY = pacY + (pacDirY * 2);

    return {pivotX + (pivotX - blinkyX),
            pivotY + (pivotY - blinkyY)};
}

// ==========================================
// CLYDE
// ==========================================

FantasmaClyde::FantasmaClyde(int x, int y,
                             const std::vector<std::vector<int>> &m,
                             int l, int c)
    : Fantasma(x, y, "assets/sprites/portugal.png", m, l, c) {}

sf::Vector2i FantasmaClyde::calcularPontoAlvo(int pacX, int pacY,
                                              int pacDirX, int pacDirY,
                                              int blinkyX, int blinkyY)
{
    float dist = std::sqrt(
        std::pow(static_cast<float>(pacX - posx), 2.0f) +
        std::pow(static_cast<float>(pacY - posy), 2.0f));

    if (dist > 8.0f)
        return {pacX, pacY};
    else
        return {1, 7};
}