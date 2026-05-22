#include "utils.h"
#include <algorithm>
#include <iostream>
#include <optional>

bool is_valid(int q, int r) {
    return q >= 0 && q < GRID_Q && r >= 0 && r < GRID_R;
}

bool is_valid_slot(int n) {
    return n >= 0 && n < PLATOONS_PER_HEX +1;
}

int hex_index(int q, int r) { return r * GRID_Q + q; }
int hex_q(int i) { return i % GRID_Q; }
int hex_r(int i) { return i / GRID_Q; }

// each hex is 7 chars wide (they overlap by 2)
int stamp_col(int q) { return q * 7; }

// even columns shifted down
int stamp_row(int q, int r) { return r * 4 + (q % 2 == 0 ? 2 : 0); }

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

std::string ask(std::string t) {
    std::string res;
    std::cout << t << " > ";
    std::getline(std::cin, res);

    return res;
}

std::optional<int> ask_choice(const std::string& prompt, const std::vector<int>& choices) {
    while (true) {
        auto res = ask(std::format("{} [{}]", prompt, join(choices, ',')));
        if (res.empty()) return std::nullopt;
        int tmp;
        auto [ptr, ec] = std::from_chars(res.data(), res.data() + res.size(), tmp);
        if (ec == std::errc{} && ptr == res.data() + res.size() && std::ranges::contains(choices, tmp))
            return tmp;
        std::cout << "invalid input, try again\n";
    }
}

bool is_valid_dir(char c) {
    return c == 'r' || c == 'l' || c == 'u' || c == 'd';
}

bool has(const std::string& s, char c) {
    return s.find(c) != std::string::npos;
}

std::pair<int,int> ask_move(int parity) {
    static const int evenq_dir[2][6][2] = {
        { {+1,+1},{+1, 0},{ 0,-1},{-1, 0},{-1,+1},{ 0,+1} },
        { {+1, 0},{+1,-1},{ 0,-1},{-1,-1},{-1, 0},{ 0,+1} },
    };

    while (true) {
        auto res = ask("where do we move? [l]eft/[r]ight/[u]p/[d]own");
        if (res.empty() || !std::ranges::all_of(res, is_valid_dir)) {
            std::cout << "invalid input, try again\n";
            continue;
        }
        if ((has(res, 'r') && has(res, 'l')) || (has(res, 'u') && has(res, 'd'))) {
            std::cout << "conflicting directions, try again\n";
            continue;
        }

        // map input to one of the 6 hex direction indices
        // u=2, d=5, l=3(even)/4(odd), r=1(even)/0(odd)
        bool u = has(res,'u'), d = has(res,'d'),
             l = has(res,'l'), r = has(res,'r');

        int dir = -1;
        if      ( u && !l && !r) dir = 2;
        else if ( d && !l && !r) dir = 5;
        else if ( u &&  l)       dir = 3;
        else if ( u &&  r)       dir = 1;
        else if ( d &&  l)       dir = 4;
        else if ( d &&  r)       dir = 0;

        if (dir == -1) continue;
        return { evenq_dir[parity][dir][0], evenq_dir[parity][dir][1] };
    }
}

std::string join(std::vector<int> v, char sep) {
    std::string out;
    for (int i = 0; i < v.size(); i++) {
        out += std::to_string(v[i]) + sep;
    }

    return out.substr(0, out.size() -1); // remove the last sep
}
