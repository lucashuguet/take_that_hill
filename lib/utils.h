#pragma once
#include <optional>
#include <string>
#include <vector>

const int PLATOONS_PER_HEX = 3;

const int GRID_Q = 3;
const int GRID_COLS = GRID_Q * 7 + 2;

const int GRID_R = 5;
const int GRID_ROWS = GRID_R * 4 + 3;

const int BOARD_SIZE = GRID_Q * GRID_R;

bool is_valid(int q, int r);
bool is_valid_slot(int n);

int hex_index(int q, int r);
int hex_q(int i);
int hex_r(int i);

int stamp_col(int q);
int stamp_row(int q, int r);
void stamp(std::string& grid, const std::vector<std::string>& lines, int row, int col);

std::string ask(std::string t);
std::optional<int> ask_choice(const std::string&, const std::vector<int>& choices);
bool is_valid_dir(char c);
bool has(const std::string& s, char c);
std::pair<int, int> ask_move(int parity);

std::string join(std::vector<int> v, char sep);
