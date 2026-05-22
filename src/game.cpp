#include "game.h"
#include "utils.h"
#include <string>
#include <iostream>
#include <ranges>

Platoon::Platoon(int n) : number(n), deployed(false), state(Fresh) {};

PlatoonData::PlatoonData(int n) : q(-1), r(-1), platoon(Platoon(n)) {};

void Game::display() {
    std::string hexgrid;
    hexgrid.reserve(GRID_ROWS * (GRID_COLS + 1));
    for (int i = 0; i < GRID_ROWS -1; i++)
        hexgrid += std::string(GRID_COLS, ' ') + '\n';

    for (int i = 0; i < BOARD_SIZE; i++) {
        int q = hex_q(i);
        int r = hex_r(i);

        // // hide last row even hex
        if (r == GRID_R -1 && q % 2 == 0) {
            continue;
        }

        stamp(hexgrid, render_hex(i), stamp_row(q, r), stamp_col(q));
    }

    hexgrid.erase(hexgrid.find_last_not_of(" \n\r\t") +1); // trim

    std::string border = std::string(GRID_COLS, '-');
    std::cout << border << '\n' << hexgrid << '\n' << border << '\n';
}

Platoon** Game::get(int q, int r) {
    if (!is_valid(q, r)) return nullptr;
    return board[hex_index(q, r)];
}

void Game::set(int q, int r, int n) {
    if (!is_valid(q, r) or !is_valid_slot(n)) {
        std::cout << "invalid move\n";
        return;
    }

    if (platoons[n].platoon.deployed)
        board[hex_index(platoons[n].q, platoons[n].r)][n] = nullptr;
    else
        platoons[n].platoon.deployed = true;

    platoons[n].q = q;
    platoons[n].r = r;

    board[hex_index(q, r)][n] = &platoons[n].platoon;
}

void Game::step() {
    // Movement

    std::vector<int> undeployed, fresh;
    for (int i = 1; i <= PLATOONS_PER_HEX; i++) {
        auto& p = platoons[i].platoon;
        if (!p.deployed)           undeployed.push_back(i);
        else if (p.state == Fresh) fresh.push_back(i);
    }

    if (!undeployed.empty()) {
        if (auto i = ask_choice("deploy a platoon?", undeployed)) {
            // iota generate a vector<int> range from 1 to GRID_Q
            int q = ask_choice("which column?", std::views::iota(1, GRID_Q + 1) | std::ranges::to<std::vector>()).value_or((GRID_Q +1) /2) -1;
            int r = q % 2 == 0 ? 3 : 4;
            set(q, r, *i);
        }
    }

    if (!fresh.empty()) {
        if (auto i = ask_choice("move a platoon?", fresh)) {
            auto [dx, dy] = ask_move(platoons[*i].q % 2);
            int q = platoons[*i].q + dx;
            int r = platoons[*i].r + dy;
            set(q, r, *i);
        }
    }

    // Firing
    // Rally
    // Enemy Action
}

Game::Game() : turns(0) {
    platoons = new PlatoonData[PLATOONS_PER_HEX +1]; // 3 friendly + 1 enemy
    for (int i = 0; i < PLATOONS_PER_HEX +1; i++)
        platoons[i] = PlatoonData(i);

    board = new Platoon**[BOARD_SIZE];
    for (int i = 0; i < BOARD_SIZE; i++)
        board[i] = new Platoon*[PLATOONS_PER_HEX +1]();

    // place the ennemy at the top of the hill
    set(1, 0, 0);
}

Game::~Game() {
    delete[] platoons;

    for (int i = 0; i < BOARD_SIZE; i++) {
        delete[] board[i];
    }

    delete[] board;
}

std::vector<std::string> Game::render_hex(int i) {
    std::vector<std::string> present;
    for (int j = 0; j < PLATOONS_PER_HEX + 1; j++) {
        if (board[i][j] == nullptr) continue;
        if (board[i][j]->number == 0)
            present.push_back("ENI");
        else
            present.push_back("A-" + std::to_string(board[i][j]->number));
    }

    std::string l[] = {"   ", "   ", "   "};
    int count = present.size();
    if (count == 1) l[1] = present[0];
    else if (count == 2) { l[1] = present[0]; l[2] = present[1]; }
    else if (count >= 3) { l[0] = present[0]; l[1] = present[1]; l[2] = present[2]; }

    return {
        "  _____  ",
        " / " + l[0] + " \\ ",
        "/  " + l[1] + "  \\",
        "\\  " + l[2] + "  /",
        " \\_____/ "
    };
}
