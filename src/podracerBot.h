//
// Created by Kevin on 1/08/2016.
//

#ifndef CODERSSTRIKEBACKC_PODRACERBOT_H_H
#define CODERSSTRIKEBACKC_PODRACERBOT_H_H


#include <iostream>
#include <string>
#include <algorithm>
#include <sstream>

#include "Navigation.h"

using namespace std;


class PodraceBot {
public:
    virtual ~PodraceBot() {};
    virtual PodOutput move(GameState& gameState) {};
};

class MinimalBot : PodraceBot {
    int _racer;
    Navigation nav;
public:
    MinimalBot(int racer) : _racer(racer), nav() {};
    PodOutput move(GameState& gameState) {
        Checkpoint ck = gameState.race.checkpoints[gameState.ourState().pods[_racer].nextCheckpoint];
        PodOutput move = nav.seek(gameState.ourState().pods[_racer], ck.pos, 100);
        return move;//PodOutput(100, ck.pos);
    }
};




#endif //CODERSSTRIKEBACKC_PODRACERBOT_H_H
