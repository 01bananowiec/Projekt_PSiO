#include <iostream>
#include <stdlib.h>
#include <chrono>
#include <time.h>
#include <list>

#include <SFML/Graphics.hpp>
#include "Game.h"

#define _WIN32_WINNT 0x0500
#include <windows.h>

using namespace sf;
using namespace std;


enum GameObjts
{
    Meteor = 1,
    Alien = 2,
    AlineBullets = 3,
    HealthBosus10 = 4,
    AmmoBosus1 = 6,
};

struct  GameSett
{
    int metorMaxSpeed;
    int metorDemage;
    int ticking;
    int lvl;
    bool spawnAlien;

    uint64_t score;

    sf::Font font;

    int menuMode = 0;
    void GameReset() {
        GameObj::CleareScene();
        metorMaxSpeed = 3;
        metorDemage = 3;
        ticking = 2;
        score = 0;
        lvl = 0;
        spawnAlien = false;
    }

    void InitGame() {
        metorMaxSpeed = 3;
        metorDemage = 3;
        ticking = 2;
        score = 0;
        lvl = 0;
        spawnAlien = false;

        if (!font.loadFromFile("DejaVuSans.ttf"))
        {
            cout << "Fail to load font: DejaVuSans.ttf" << endl;
        }
    }
} GameControl;


class Particle :public GameObj
{
public:
    RectangleShape shape;
    Vector2f velocity;
    Color col;

    Particle(float particleSize, Vector2f location, Color color) {
        transform.SetTansform(location, Vector2f(particleSize, particleSize));
        shape = RectangleShape(transform.size);
        uint8_t sh = (uint8_t)GetRng(0, 50);
        col = Color(color.r + sh, color.g + sh, color.r + sh);
        shape.setFillColor(col);
        velocity = Vector2f(((float)GetRngNN(-5, 5)) / 20, ((float)GetRngNN(-5, 5)) / 20);
        transform.colingObj = false;
    }

    void GameUpdate() {
        transform.position += velocity * getElapsedTick();
        if (transform.OutGameAarea()) {
            DeleteGameObj(this);
            return;
        }
        GameObj* colided = GameObj::IsColiding(this);
        if (colided != NULL) {
            GameObj::DeleteGameObj(this);
            return;
        }
        shape.setPosition(transform.position);
    }
    void DrawObject(RenderWindow& window) {
        window.draw(shape);
    }
};
void SpawnParicles(int particleCount, Vector2f location, Color col) {
    for (size_t i = 0; i < particleCount; i++)
    {
        GameObj::SpawnGameObj(new Particle(4, location, col));
    }
}

class Label :public GameObj
{
public:
    sf::Text text;
    string textString;


    Label(string message, Vector2f location, int size = 24, Color col = Color::White) {
        text.setFont(GameControl.font);
        textString = message;
        text.setString(textString);
        text.setPosition(location);
        text.setFillColor(col);
        text.setCharacterSize(size);
        transform.colingObj = false;
    }

    void DrawObject(RenderWindow& window) {
        window.draw(text);
    }
};


class Metor :public GameObj
{
public:
    RectangleShape shape;
    float fallingSpeed = 0.4f;
    Color col;


    Metor(Vector2f size = Vector2f(30, 30)) {
        transform.SetTansform(Vector2f(GetRng(1, windW), 1), size);
        shape = RectangleShape(transform.size);
        col = Color((uint8_t)GetRng(0, 255), (uint8_t)GetRng(0, 255), (uint8_t)GetRng(0, 255));
        shape.setFillColor(col);
        shape.setPosition(transform.position);
        fallingSpeed = GetRng(3, GameControl.metorMaxSpeed) / (float)10;
    }
    void GameUpdate() {
        transform.position.y += fallingSpeed * getElapsedTick();
        if (transform.OutGameAarea()) {
            DeleteGameObj(this);
            return;
        }
        shape.setPosition(transform.position);
    }
    void DrawObject(RenderWindow& window) {
        window.draw(shape);
    }
};
class Bar :public GameObj {
public:
    RectangleShape bar;
    RectangleShape barBg;

    int maxBarSize = 100;

    Bar(Vector2f loc, Color barColor, Color bgColor = Color::Yellow) {
        bar = RectangleShape(transform.size);
        barBg = RectangleShape(transform.size);
        bar.setFillColor(barColor);
        barBg.setFillColor(bgColor);
        transform.position = loc;

        barBg.setPosition(transform.position);
        bar.setPosition(transform.position);
        transform.colingObj = false;

    }
    void DrawObject(RenderWindow& window) {
        barBg.setSize(Vector2f(maxBarSize, transform.size.y));
        barBg.setPosition(transform.position);

        bar.setPosition(transform.position);
        bar.setSize(transform.size);


        window.draw(barBg);
        window.draw(bar);
    }
};

class AlienUint :public GameObj
{
public:
    RectangleShape shape;
    float moveSpeed = 0.4f;
    float fallingSpeed = 0.1f;

    int life = 3;
    int tickCnt = 0;

    AlienUint(Texture* txt) {
        transform.SetTansform(Vector2f(GetRng(1, windW), 1), Vector2f(60, 60));
        shape = RectangleShape(transform.size);

        shape.setFillColor(Color::White);
        shape.setPosition(transform.position);
        shape.setTexture(txt);

        moveSpeed = GetRng(3, 10) / (float)20;
        fallingSpeed = GetRng(0, 4) / (float)40;
    }
    void GameUpdate() {
        tickCnt += getElapsedTick();
        if (tickCnt > GetRng(300, 2000)) {
            Metor* met = new Metor(Vector2f(2, 10));
            met->fallingSpeed = 0.9f;
            met->transform.position = transform.position + Vector2f(transform.size.x / 2 - 1.25, transform.size.y);
            SpawnGameObj(met, AlineBullets);
            tickCnt = 0;
        }
        transform.position.x += moveSpeed * getElapsedTick();
        transform.position.y += fallingSpeed * getElapsedTick();
        if (transform.IsTouchCorner() & (Right))if (moveSpeed > 0)moveSpeed *= -1;
        if (transform.IsTouchCorner() & (Left))if (moveSpeed < 0)moveSpeed *= -1;


        if (transform.OutGameAarea()) {
            DeleteGameObj(this);
            return;
        }
        shape.setPosition(transform.position);
    }
    void DrawObject(RenderWindow& window) {
        window.draw(shape);
    }
};


class Bullet :public GameObj
{
public:
    RectangleShape shape;
    Vector2f velocity;
    Color col;

    Bullet(Vector2f location, Vector2f size = Vector2f(2.5, 20)) {
        transform.SetTansform(location, size);
        shape = RectangleShape(transform.size);

        col = Color(200, 150, 0);
        shape.setFillColor(col);
        velocity = Vector2f(0, -0.9f);
        transform.colingObj = false;
    }

    void GameUpdate() {
        transform.position += velocity * getElapsedTick();

        if (transform.OutGameAarea()) {
            DeleteGameObj(this);
            return;
        }
        GameObj* colided = GameObj::IsColiding(this);
        if (colided != NULL) {
            if (colided->objTag == Meteor) {
                DeleteGameObj(colided);
                colided->transform.colingObj = false;
                DeleteGameObj(this);
                SpawnParicles(30, colided->transform.position, Color(100, 00, 0));
                GameControl.score += (GameControl.lvl + 1) * 2;
            }
            if (colided->objTag == Alien) {
                AlienUint* au = (AlienUint*)colided;
                if (au->life) {
                    au->life--;
                }
                else {
                    DeleteGameObj(colided);
                    colided->transform.colingObj = false;
                    GameControl.score += (GameControl.lvl + 1) * 40;
                    SpawnParicles(60, colided->transform.position, Color(190, 100, 0));
                }
                DeleteGameObj(this);
            }


            return;
        }
        shape.setPosition(transform.position);
    }
    void DrawObject(RenderWindow& window) {
        window.draw(shape);
    }
};

class Player :public GameObj
{
public:
    Bar* playerLifeGUI, * playerBonusGUI;
    Label* scoere, * mode;

    string modesName[3] = { "Easy","Medium","Hard" };
    Texture texture;
    RectangleShape shape;
    int playerLife = 100;
    int shootLock = 1;
    int powerUp1 = 0;
    void Start() {

        transform.SetTansform(Vector2f(windW / 2 - 25, windH - 60), Vector2f(50, 50));
        shape = RectangleShape(transform.size);
        transform.colingObj = false;
        playerLifeGUI = new Bar(Vector2f(36, 5), Color::Red, Color(10, 10, 10));
        playerBonusGUI = new Bar(Vector2f(36, 30), Color(0, 90, 120), Color(10, 10, 10));


        playerLifeGUI->transform.size.y = 15;
        playerBonusGUI->transform.size.y = 15;

        SpawnGameObj(playerLifeGUI);
        SpawnGameObj(playerBonusGUI);
        SpawnGameObj(playerBonusGUI);

        SpawnGameObj(new Label("LIFE", Vector2f(0, 3), 15));
        SpawnGameObj(new Label("AMO", Vector2f(0, 29), 15));

        scoere = (Label*)SpawnGameObj(new Label("", Vector2f(windW - 80, 3), 15));
        mode = (Label*)SpawnGameObj(new Label("", Vector2f(windW - 80, 29), 15));


        if (!texture.loadFromFile("player.png"))
        {
            cout << "fail to open file" << endl;
        }
        else shape.setTexture(&texture);

    }

    void GameUpdate() {

        playerLifeGUI->transform.size.x = playerLife;

        playerBonusGUI->transform.size.x = powerUp1;
        playerBonusGUI->bar.setFillColor(Color(0, 90, 120));
        scoere->text.setString(to_string(GameControl.score));
        mode->text.setString(modesName[GameControl.lvl]);
        GameControl.score += getElapsedTick() * (GameControl.lvl + 1);

        switch (GameControl.lvl)
        {
        case 0:
            if (GameControl.score > 50000) {
                GameControl.ticking = 3;
                GameControl.lvl++;
                GameControl.metorMaxSpeed = 4;
            }
            break;
        case 1:
            if (GameControl.score > 200000) {
                GameControl.ticking = 4;
                GameControl.spawnAlien = true;
                GameControl.metorMaxSpeed = 5;
                GameControl.lvl++;
            }
            break;

        }

        if (IsKeyPressed(Keyboard::Key::Right)) {
            if (!(transform.IsTouchCorner() & Right)) {
                transform.position.x += 0.5f * getElapsedTick();
            }
        }
        if (IsKeyPressed(Keyboard::Key::Left)) {
            if (!(transform.IsTouchCorner() & Left)) {
                transform.position.x -= 0.5f * getElapsedTick();
            }
        }
        GameObj* colided = GameObj::IsColiding(this);
        if (colided != NULL) {
            colided->transform.colingObj = false;
            if (colided->objTag == Meteor) {
                Metor* col = (Metor*)colided;
                SpawnParicles(40, colided->transform.position + (Vector2f((colided->transform.size.x / 2), colided->transform.size.y / 2)), col->col);
                playerLife -= GameControl.metorDemage;
            }
            else if (colided->objTag == AlineBullets) {
                Metor* col = (Metor*)colided;
                SpawnParicles(40, colided->transform.position + (Vector2f((colided->transform.size.x / 2), colided->transform.size.y / 2)), col->col);
                playerLife -= GameControl.metorDemage * 2;
            }
            else if (colided->objTag == Alien) {
                SpawnParicles(40, colided->transform.position + (Vector2f((colided->transform.size.x / 2), colided->transform.size.y / 2)), Color::Red);
                playerLife -= GameControl.metorDemage * 3;
            }
            else if (colided->objTag == HealthBosus10) {
                playerLife += 10;
                if (playerLife > 100) playerLife = 100;
            }
            else if (colided->objTag == AmmoBosus1) {
                powerUp1 += 10;
                if (powerUp1 > 100) powerUp1 = 100;
            }

            DeleteGameObj(colided);

        }
        if (IsKeyPressed(Keyboard::Key::Space)) {
            Vector2f loc = transform.position + Vector2f(transform.size.x / 2 - 1.25, 0);
            if (shootLock) {
                if (powerUp1) {
                    SpawnGameObj(new Bullet(loc, Vector2f(5, 5)));
                    Bullet* b1 = new Bullet(loc, Vector2f(5, 5)), * b2 = new Bullet(loc, Vector2f(5, 5));

                    SpawnGameObj(b1);
                    SpawnGameObj(b2);
                    b1->velocity.x = 0.3f;
                    b2->velocity.x = -0.3f;
                    powerUp1--;
                }
                else {
                    SpawnGameObj(new Bullet(loc));
                }
                shootLock--;
            }
        }
        else shootLock = 1;
        shape.setPosition(transform.position);
    }
    void DrawObject(RenderWindow& window) {
        window.draw(shape);
    }
};
class FallingBonus :public GameObj
{
public:
    RectangleShape shape;
    float fallingSpeed = 0.4f;
    FallingBonus(float speed = 0.4f, Texture* text = NULL) {
        transform.SetTansform(Vector2f(GetRng(1, windW), 1), Vector2f(30, 30));
        shape = RectangleShape(transform.size);
        shape.setFillColor(Color::White);
        shape.setPosition(transform.position);
        if (text)shape.setTexture(text);

        fallingSpeed = speed;
    }
    void GameUpdate() {
        transform.position.y += fallingSpeed * getElapsedTick();
        if (transform.OutGameAarea()) {
            DeleteGameObj(this);
            return;
        }
        shape.setPosition(transform.position);
    }
    void DrawObject(RenderWindow& window) {
        window.draw(shape);
    }
};



void GameMain(RenderWindow& window) {

    GameControl.GameReset();
    GameObj::InitRandom();
    Player* pl = (Player*)GameObj::SpawnGameObj(new Player());
    Texture metor, helathBonus, ammoBonus, ammoBonus2, alien;
    if (!metor.loadFromFile("metor.png")) {
        cout << "fail to open file: metor.png" << endl;
    }
    if (!helathBonus.loadFromFile("apteczka.jpg")) {
        cout << "fail to open file: apteczka.jpg" << endl;
    }
    if (!ammoBonus.loadFromFile("ammoBonus.png")) {
        cout << "fail to open file: ammoBonus.png" << endl;
    }
    if (!ammoBonus2.loadFromFile("ammoBonu2.png")) {
        cout << "fail to open file: ammoBonu2.png" << endl;
    }
    if (!alien.loadFromFile("alien.png")) {
        cout << "fail to open file: alien.png" << endl;
    }

    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                return;
            if (event.type == sf::Event::Resized)
            {
                sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
                window.setView(sf::View(visibleArea));
                GameObj::SyncGameArea(event);
            }

        }

        window.clear(Color(1, 30, 50));
        for (size_t i = 0; i < GameControl.ticking * GameObj::getElapsedTick(); i++)
        {
            if (GameObj::GetRng(0, 200) == 47) {
                Metor* met = new Metor();
                GameObj::SpawnGameObj(met, Meteor);
                met->shape.setTexture(&metor);
            }
            if (GameObj::GetRng(0, 20000) == 12034) {
                GameObj::SpawnGameObj(new FallingBonus(0.3f, &helathBonus), HealthBosus10);
            }
            if (GameObj::GetRng(0, 10000) == 5000) {
                GameObj::SpawnGameObj(new FallingBonus(0.3f, &ammoBonus), AmmoBosus1);
            }
            if (GameControl.spawnAlien)if (GameObj::GetRng(0, 10000) == 5000) {
                GameObj::SpawnGameObj(new AlienUint(&alien), Alien);
            }
        }
        if (pl->playerLife < 0) {
            GameControl.menuMode = 1;
            return;
        }
        GameObj::DrawObjectInvoke(window);
        GameObj::GameUpadateInvoke();
        GameObj::DeleteGmInvoke();
        window.display();
    }
}

int main()
{
    HWND hWnd = GetConsoleWindow();
    ShowWindow(hWnd, SW_HIDE);

    GameObj::windH = 500;
    GameObj::windW = 500;
    RenderWindow window(VideoMode(GameObj::windH, GameObj::windW), "Cake Invasion", Style::Titlebar | Style::Close);
    GameControl.InitGame();
    Vector2f center = Vector2f(GameObj::windW / 2, GameObj::windH / 2);
    Label* lab = new Label("Cake Invasion", center + Vector2f(-150, -100), 40, Color(100, 170, 0));
    Label* lab2 = new Label("press enter to play", center + Vector2f(-150, -50), 15, Color(130, 170, 0));
    Label* gameOver = new Label("Game over, your score:", center + Vector2f(-230, -100), 40, Color(100, 170, 0));
    Label* score = new Label("", center + Vector2f(-50, -30), 35, Color(130, 170, 0));
    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == Keyboard::Key::Enter) {
                    if (GameControl.menuMode == 0) {
                        GameMain(window);
                        score->text.setString(to_string(GameControl.score));
                        continue;
                    }
                    if (GameControl.menuMode == 1) {
                        GameControl.menuMode = 0;
                    }

                }
            }

        }
        window.clear();
        if (GameControl.menuMode == 0) {
            lab->DrawObject(window);
            lab2->DrawObject(window);
        }
        if (GameControl.menuMode == 1) {

            gameOver->DrawObject(window);
            score->DrawObject(window);
        }

        window.display();
    }

    return 0;
}