#include "game.h"
#include "utils.h"

#include <iostream>

Platoon::Platoon(int n) : number(n), deployed(false), state(Fresh) {};

PlatoonData::PlatoonData(int n) : q(-1), r(-1), platoon(Platoon(n)) {};

void Game::display() {
    std::string hexgrid;
    hexgrid.reserve(GRID_ROWS * (GRID_COLS + 1));
    for (int i = 0; i < GRID_ROWS -1; i++)
        hexgrid += std::string(GRID_COLS, ' ') + '\n';

    int k = 0;
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
    if (!is_valid(q, r) or !is_valid_slot(n)) return;

    if (platoons[n].q >= 0 && platoons[n].r >= 0) {
        board[hex_index(platoons[n].q, platoons[n].r)][0] = nullptr;
    }

    platoons[n].q = q;
    platoons[n].r = r;

    board[hex_index(q, r)][0] = &platoons[n].platoon;
}

Game::Game() : turns(0) {
    platoons = new PlatoonData[PLATOONS_PER_HEX +1]; // 3 friendly + 1 enemy
    for (int i = 0; i < PLATOONS_PER_HEX +1; i++)
        platoons[i] = PlatoonData(i);

    board = new Platoon**[BOARD_SIZE];
    for (int i = 0; i < BOARD_SIZE; i++)
        board[i] = new Platoon*[PLATOONS_PER_HEX]();

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
    std::string l[] = {"   ", "   ", "   "};
    for (int j = 0; j < PLATOONS_PER_HEX; j++) {
        if (board[i][j] == nullptr) continue;
        else if (board[i][j]->number == 0) l[1] = "ENI";
        else l[j] = "A-" + std::to_string(board[i][j]->number);
    }

    return {
        "  _____  ",
        " / " + l[0] + " \\ ",
        "/  " + l[1] + "  \\",
        "\\  " + l[2] + "  /",
        " \\_____/ "
    };
}
