#pragma once
#include <iostream>
#include <stdlib.h>
#include <chrono>
#include <time.h>
#include <list>

#include <SFML/Graphics.hpp>
typedef enum Corner
{
    Up = 1,
    Down = 2,
    Left = 4,
    Right = 8,
}Corner;

struct  GameTransform
{
    sf::Vector2f position;
    float    rotation;
    sf::Vector2f size;
    bool     colingObj = true;

    void SetTansform(sf::Vector2f pos, sf::Vector2f siz);
    bool IsRectColidingTo(GameTransform rectColiding);
    Corner IsTouchCorner();

    bool OutGameAarea();
};

class GameObj {
private:
    bool markedToDelete = false; // to prevent multiple add to deleting list

public:
    static std::list<GameObj*>* obj;
    static std::list<GameObj*>* toDelete;
    static int lastFrameUpdate;


    static int windH;
    static int windW;

    GameTransform transform;
    int objTag; // tag to indetify type of object

    virtual void Start() {};
    virtual void GameUpdate() {};
    virtual void DrawObject(sf::RenderWindow& window) {};
    virtual void DelObj() { /*cout << "df" << endl;*/ };


    bool IsKeyPressed(sf::Keyboard::Key keyCode);
    static float getElapsedTick();

    static GameObj* SpawnGameObj(GameObj* ob, int tag = 0);
    static void DeleteGameObj(GameObj* ob);
    static void GameUpadateInvoke();
    static void DrawObjectInvoke(sf::RenderWindow& window);
    static void DeleteGmInvoke();
    static void CleareScene();
    static GameObj* IsColiding(GameObj* gm);
    static void SyncGameArea(sf::Event& event);
    static void InitRandom();


    static int GetRng(int min, int max);
    static int GetRngNN(int min, int max);
    ~GameObj() {
        DelObj();
    }
};
#pragma once
