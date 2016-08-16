#ifndef CODERSSTRIKEBACK_GAMESTATE_H
#define CODERSSTRIKEBACK_GAMESTATE_H


#define _USE_MATH_DEFINES
#include <cmath>
#include <vector>
#include <string>
#include <sstream>

#include "Vector.h"

using namespace std;


static const int MAX_THRUST = 200;
static const int MAX_ANGLE_DEG = 15;
static constexpr float MAX_ANGLE = M_PI * MAX_ANGLE_DEG / 180;
static constexpr float DRAG = 0.85f;
static const int POD_COUNT = 2;
static const int PLAYER_COUNT = 2;
static const int CHECKPOINT_RADIUS = 600;
static const int POD_RADIUS = 400;
static const int POD_RADIUS_SQ = 400 * 400;
static const int WANDER_TIMEOUT = 100;
static const int SHIELD_COOLDOWN = 3;
static const int OUR_PLAYER_ID = 0;
static const int BOOST_ACC = 650;


struct Checkpoint {
    Vector pos;
    int order;

    Checkpoint(int x, int y, int order) :
            pos(x, y), order(order) {}
};

struct Race {
    int laps;
    vector<Checkpoint> checkpoints;
    float maxCheckpointDist = 0;

    Race() {}

    Race(int laps, vector<Checkpoint> checkpoints) :
            laps(laps), checkpoints(checkpoints) {
        for(int i = 0; i < checkpoints.size()-1; i++) {
            maxCheckpointDist = max(maxCheckpointDist, (checkpoints[i].pos - checkpoints[i+1].pos).getLength());
        }
    }

    int totalCPCount() {
        return laps * checkpoints.size();
    }

    int followingCheckpoint(int cp) {
        return (cp + 1) % checkpoints.size();
    }
};

struct PodState {
    Vector pos;
    Vector vel;
    // In radians
    float angle;
    bool shieldEnabled = false;
    int nextCheckpoint = 0;
    int passedCheckpoints = 0;
    int turnsSinceCP = 0;
    int turnsSinceShield = 0;
    bool boostAvailable = true;

    PodState() {}

    PodState(int x, int y, int vx, int vy, float angle, int nextCheckpoint) :
            pos(x, y), vel(vx, vy), angle(angle), nextCheckpoint(nextCheckpoint) { }

    PodState(Vector pos, Vector vel, float angle, int nextCheckpoint) :
            pos(pos), vel(vel), angle(angle), nextCheckpoint(nextCheckpoint) {}

    // Many tests and simulations don't care about the checkpoints.
    PodState(Vector pos, Vector vel, float angle) : pos(pos), vel(vel), angle(angle) {}

    bool operator ==(const PodState& other) const {
        return (pos == other.pos && vel == other.vel && angle == other.angle && nextCheckpoint == other.nextCheckpoint);
    }

    bool operator !=(const PodState& other) const {
        return !(*this == other);
    }
};

struct PlayerState {
    vector<PodState> pods;
    vector<PodState> lastPods;
    int leadPodID = 0;
    PlayerState( vector<PodState> pods) : pods(pods) {}

    PlayerState(vector<PodState> pods, vector<PodState> lastPods)
            : pods(pods), lastPods(lastPods) {}

    PodState& leadPod() {
        return pods[leadPodID];
    }

    PodState& laggingPod() {
        return pods[(leadPodID + 1) % 2];
    }
};

struct GameState {
    Race race;
    vector<PlayerState> playerStates;
    int turn = 0;

    GameState() {};

    GameState(Race& race, vector<PlayerState>& playerStates, int turn) :
            race(race), playerStates(playerStates), turn(turn) {}

    PlayerState& ourState() {
        return playerStates[OUR_PLAYER_ID];
    }

    PlayerState& enemyState() {
        return playerStates[(OUR_PLAYER_ID + 1) % 2];
    }
};


struct PodOutputAbs {
    float thrust;
    Vector target;
    static const int BOOST = -1;
    static const int SHIELD = -2;

    PodOutputAbs() {}

    PodOutputAbs(float thrust, Vector direction) :
            thrust(thrust), target(direction) {}

    string toString() {
        stringstream out;
        string thrustStr;
        if(thrust == BOOST) {
            thrustStr = "BOOST";
        } else if(thrust == SHIELD){
            thrustStr = "SHIELD";
        } else {
            thrustStr = to_string((int)thrust);
        }
        out << (int) target.x << " " << (int) target.y << " " << thrustStr;
        return out.str();
    }

    void enableShield() {
        thrust = SHIELD;
    }

    void enableBoost() {
        thrust = BOOST;
    }
};


class PodOutputSim {
public:
    int thrust;
    float angle;
    bool shieldEnabled;
    bool boostEnabled;

    PodOutputSim() {}

    PodOutputSim(int thrust, float angle, bool shieldEnabled, bool boostEnabled) :
            thrust(thrust), angle(angle), shieldEnabled(shieldEnabled), boostEnabled(boostEnabled) {}

    static PodOutputSim fromAbsolute(const PodState &pod, const PodOutputAbs &abs);

    PodOutputAbs absolute(const PodState &pod);
};

struct PairOutput {
    PodOutputSim o1;
    PodOutputSim o2;

    PairOutput() {}
    PairOutput(PodOutputSim o1, PodOutputSim o2) : o1(o1), o2(o2) {}
};

class State {
    void update(const PodOutputAbs& pod1, int id);
    GameState previous;
    GameState current;
public:
    Race race;
    int turn = 0;

    State() {}
    State(Race race) : race(race) {}
    void preTurnUpdate(vector<PlayerState> input);
    void postTurnUpdate(PodOutputAbs pod1, PodOutputAbs pod2);
    GameState& game() {return current;}
};

#endif //CODERSSTRIKEBACK_GAMESTATE_H
