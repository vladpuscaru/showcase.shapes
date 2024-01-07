//
// Created by Vlad Puscaru on 08.12.2023.
//

#ifndef COMP4300_ASSIGNMENT2_CONFIGSPECIFICATION_H
#define COMP4300_ASSIGNMENT2_CONFIGSPECIFICATION_H

#include <iostream>
#include <fstream>
#include <vector>

struct PlayerConfig {
    int shapeRadius;
    float speed;
    int fillRed, fillGreen, fillBlue;
    int outRed, outGreen, outBlue;
    int outThickness;
    int vertices;
};

struct BulletConfig {
    int shapeRadius;
    float speed;
    int fillRed, fillGreen, fillBlue;
    int outRed, outGreen, outBlue;
    int outThickness;
    int vertices;
    int lifespan;
};

struct EnemyConfig {
    int shapeRadius;
    float minSpeed, maxSpeed;
    int minVertices, maxVertices;
    int lifespan; // for small enemies
    int spawnInterval;
};

struct GameConfig {
    bool isSUserInputActive = true;
    bool isSLifespanActive = true;
    bool isSCollisionActive = true;
    bool isSMovementActive = true;
    bool isSSpawnerActive = true;
};

struct Config {
    PlayerConfig playerConfig;
    BulletConfig bulletConfig;
    EnemyConfig enemyConfig;
    GameConfig gameConfig;
    int selectedUITab;
};

#endif //COMP4300_ASSIGNMENT2_CONFIGSPECIFICATION_H
