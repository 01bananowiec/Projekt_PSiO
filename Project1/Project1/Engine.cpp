#include <iostream>
#include <stdlib.h>
#include <chrono>
#include <time.h>
#include <list>

#include <SFML/Graphics.hpp>
#include "Game.h"

using namespace sf;
using namespace std;

/*
****************************************Transform struct************************************************
*/


void GameTransform::SetTansform(Vector2f pos, Vector2f siz) {
    position = pos;
    size = siz;
}
bool GameTransform::IsRectColidingTo(GameTransform rectColiding) {
    return (position.x + size.x >= rectColiding.position.x &&              // r1 right edge past r2 left
        position.x <= rectColiding.position.x + rectColiding.size.x &&     // r1 left edge past r2 right
        position.y + size.y >= rectColiding.position.y &&                  // r1 top edge past r2 bottom
        position.y <= rectColiding.position.y + rectColiding.size.y);      // r1 bottom edge past r2 top    
}
Corner GameTransform::IsTouchCorner() {
    return (Corner)((position.x + size.x >= GameObj::windW) * Right |
        (position.x <= 0) * Left |
        (position.y + size.y >= GameObj::windH) * Down |
        (position.y <= 0) * Up);
}

bool GameTransform::OutGameAarea() {
    return ((position.x >= GameObj::windW) ||
        (position.x + size.x <= 0) ||
        (position.y >= GameObj::windH) ||
        (position.y + size.y <= 0));
}

/*
****************************************GameObj Class************************************************
*/
list<GameObj*>* GameObj::obj = new list<GameObj*>();
list<GameObj*>* GameObj::toDelete = new list<GameObj*>();
int GameObj::lastFrameUpdate = 0;
int GameObj::windH = 0;
int GameObj::windW = 0;

bool GameObj::IsKeyPressed(Keyboard::Key keyCode) {
    return Keyboard::isKeyPressed(keyCode);
}
float GameObj::getElapsedTick() {
    int t = clock() - lastFrameUpdate;
    return t;
}

GameObj* GameObj::SpawnGameObj(GameObj* ob, int tag) {
    ob->objTag = tag;
    ob->Start();
    obj->push_back(ob);
    return ob;
}
void GameObj::DeleteGameObj(GameObj* ob) {
    if (ob->markedToDelete)return;
    ob->markedToDelete = true;
    toDelete->push_back(ob);
}
void GameObj::GameUpadateInvoke() {
    for (GameObj* instance : *obj) instance->GameUpdate();
    lastFrameUpdate = clock();
}
void GameObj::DrawObjectInvoke(RenderWindow& window) {
    for (GameObj* instance : *obj) instance->DrawObject(window);
}
void GameObj::DeleteGmInvoke() {
    for (GameObj* instance : *toDelete) {
        instance->DelObj();
        obj->remove(instance);
        delete instance;
    }
    toDelete->clear();
}
void GameObj::CleareScene() {
    for (GameObj* instance : *obj) {
        DeleteGameObj(instance);
    }
    DeleteGmInvoke();
}
GameObj* GameObj::IsColiding(GameObj* gm) {
    for (GameObj* instance : *obj) {
        if (instance == gm || (!instance->transform.colingObj)) continue;
        if (instance->transform.IsRectColidingTo(gm->transform)) return instance;
    }
    return NULL;
}
void GameObj::SyncGameArea(Event& event) {
    windH = event.size.height;
    windW = event.size.width;
}
void GameObj::InitRandom() {
    srand(time(NULL));
}


int GameObj::GetRng(int min, int max) {
    return min + rand() % ((max + 1) - min);
}
int GameObj::GetRngNN(int min, int max) {
    int wnk = min + rand() % ((max + 1) - min);
    if (wnk == 0) wnk = max;
    return wnk;
}