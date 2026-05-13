#include <iostream>
#include <vector>

const int PLATOONS_PER_HEX = 3;
const int GRID_Q = 3;
const int GRID_COLS = GRID_Q * 7 + 2;
const int GRID_R = 5;
const int GRID_ROWS = GRID_R * 4 + 3;
const int BOARD_SIZE = GRID_Q * GRID_R;

int hex_index(int q, int r) { return r * GRID_Q + q; }
int hex_q(int i) { return i % GRID_Q; }
int hex_r(int i) { return i / GRID_Q; }

int stamp_col(int q) {
    return q * 7;  // each hex is 7 chars wide (they overlap by 2)
}

int stamp_row(int q, int r) {
    return r * 4 + (q % 2 == 0 ? 2 : 0);  // even columns shifted down
}

void stamp(std::string& grid, const std::vector<std::string>& lines, int row, int col) {
    for (int r = 0; r < lines.size(); r++) {
        for (int c = 0; c < lines[r].size(); c++) {
            int idx = (row + r) * (GRID_COLS + 1) + col + c;
            if (idx < grid.size() && lines[r][c] != ' ') {
                grid[idx] = lines[r][c];
            }
        }
    }
}

enum State { Fresh, Spent };

struct Platoon {
    int number;
    bool deployed;
    State state;

    Platoon(int n = 0) : number(n), deployed(false), state(Fresh) {};
};

struct PlatoonData {
    int q;
    int r;
    Platoon platoon;

    PlatoonData(int n = 0) : q(-1), r(-1), platoon(Platoon(n)) {};
};

struct Game {
    int turns;
    PlatoonData* platoons;
    Platoon*** board;

    std::vector<std::string> render_hex(int i) {
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

    void display() {
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

    Platoon** get(int q, int r) {
        int idx = hex_index(q, r);
        return board[idx];
    }

    void set(int q, int r, PlatoonData* p) {
        int idx = hex_index(q, r);
        p->q = q;
        p->r = r;

        board[idx][0] = &p->platoon;
    }

    Game() : turns(0) {
        platoons = new PlatoonData[PLATOONS_PER_HEX +1]; // 3 friendly + 1 ennemy
        for (int i = 0; i < PLATOONS_PER_HEX +1; i++)
            platoons[i] = PlatoonData(i);

        board = new Platoon**[BOARD_SIZE];
        for (int i = 0; i < BOARD_SIZE; i++)
            board[i] = new Platoon*[PLATOONS_PER_HEX]();

        // place the ennemy at the top of the hill
        set(1, 0, &platoons[0]);
    }

    ~Game() {
        delete[] platoons;

        for (int i = 0; i < BOARD_SIZE; i++) {
            delete[] board[i];
        }

        delete[] board;
    }
};

int main() {
    Game game;

    game.set(1, 4, &game.platoons[1]);
    game.display();

    return 0;
}
