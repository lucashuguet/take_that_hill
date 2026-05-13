#pragma once
#include "utils.h"
#include <string>
#include <vector>

enum State { Fresh, Spent };

struct Platoon {
    int number;
    bool deployed;
    State state;

    Platoon(int n = 0);
};

struct PlatoonData {
    int q;
    int r;
    Platoon platoon;

    PlatoonData(int n = 0);
};

class Game {
 public:
    void display();

    Platoon** get(int q, int r);
    void set(int q, int r, int n);

    Game();
    ~Game();

 private:
    int turns;
    PlatoonData* platoons;
    Platoon*** board;

    std::vector<std::string> render_hex(int i);
};
