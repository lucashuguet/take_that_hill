#include "utils.h"

bool is_valid(int q, int r) {
    return q >= 0 && q < GRID_Q && r >= 0 && r < GRID_R;
}

bool is_valid_slot(int n) {
    return n >= 0 && n < PLATOONS_PER_HEX;
}

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
