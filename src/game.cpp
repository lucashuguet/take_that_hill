#include "game.h"
#include "utils.h"
#include <string>
#include <iostream>
#include <ranges>
#include <fstream>
#include <sstream>

Platoon::Platoon(int n) : number(n), deployed(false), state(Fresh) {};

PlatoonData::PlatoonData(int n) : q(-1), r(-1), platoon(Platoon(n)) {};

void Game::display() {
    std::string hexgrid;
    hexgrid.reserve(GRID_ROWS * (GRID_COLS + 1));
    for (int i = 0; i < GRID_ROWS -1; i++)
        hexgrid += std::string(GRID_COLS, ' ') + "\n";

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
    std::cout << border << "\n" << std::format("Turn {} | Score {}\n", turns, turns + hits) << hexgrid << "\n\n";
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
    platoons[n].platoon.state = Spent;

    board[hex_index(q, r)][n] = &platoons[n].platoon;
}

int Game::step() {
    int final_score = turns + hits;
    if (final_score > 15) return final_score;

    PlatoonData& p_eny = platoons[0];

    // Movement
    std::cout << "--------MOVEMENT-------\n";
    std::vector<int> undeployed, fresh;
    for (int i = 1; i <= PLATOONS_PER_HEX; i++) {
        auto& p = platoons[i].platoon;
        if (!p.deployed)           undeployed.push_back(i);
        else if (p.state == Fresh) fresh.push_back(i);
    }

    while (!undeployed.empty()) {
        if (auto i = ask_choice("deploy a platoon?", undeployed)) {
            // iota generate a vector<int> range from 1 to GRID_Q
            int q = ask_choice("which column?", std::views::iota(1, GRID_Q + 1) | std::ranges::to<std::vector>()).value_or((GRID_Q +1) /2) -1;
            int r = q % 2 == 0 ? 3 : 4;
            set(q, r, *i);
            std::erase(undeployed, *i);
        } else break;
    }

    while (!fresh.empty()) {
        if (auto i = ask_choice("move a platoon?", fresh)) {
            auto [dx, dy] = ask_move(platoons[*i].q % 2);
            int q = platoons[*i].q + dx;
            int r = platoons[*i].r + dy;
            set(q, r, *i);

            // win condition
            if (q == p_eny.q && r == p_eny.r) {
                return turns + hits;
            }

            std::erase(fresh, *i);
        } else break;
    }

    // Firing
    std::cout << "---------FIRING--------\n";
    while (!fresh.empty()) {
        if (auto i = ask_choice("open fire?", fresh)) {
            PlatoonData& p_data = platoons[*i];
            int dice = roll_dice();

            double d = distance(p_data.q, p_data.r, p_eny.q, p_eny.r);

            if ((double)dice > d) {
                std::cout << std::format("  rolled a {} > {}", dice, d) << "\n";
                std::cout << "  enemy hit, suppressed for this turn\n";
                p_eny.platoon.state = Spent;
            } else {
                std::cout << std::format("  rolled a {} !> {}", dice, d) << "\n";
                std::cout << "  you missed\n";
            }

            p_data.platoon.state = Spent;
            std::erase(fresh, *i);
        } else break;
    }

    // Rally
    std::cout << "---------RALLY---------\n";
    PlatoonData& p_a1 = platoons[1];
    for (int i = 1; i <= PLATOONS_PER_HEX; i++) {
        PlatoonData& p = platoons[i];
        int d = distance(p.q, p.r, p_a1.q, p_a1.r);
        int dice = roll_dice();
        if (d == 0 || dice > d) {
            p.platoon.state = Fresh;
            std::cout << std::format("  A{} has recovered ({} > {})", i, dice, d) << "\n";
        } else
            std::cout << std::format("  A{} remain spent ({} <= {})", i, dice, d) << "\n";
    }

    // Enemy Action
    std::cout << "------ENEMY ACTION-----\n";
    if (p_eny.platoon.state == Fresh) {
        std::vector<std::pair<int, int>> occupied_hexes;
        for (int i = 1; i <= PLATOONS_PER_HEX; i++) {
            if (platoons[i].platoon.deployed) {
                bool exists = false;
                for (auto& h : occupied_hexes) {
                    if (h.first == platoons[i].q && h.second == platoons[i].r) {
                        exists = true;
                        break;
                    }
                }
                if (!exists) occupied_hexes.push_back({platoons[i].q, platoons[i].r});
            }
        }

        if (occupied_hexes.empty()) {
            std::cout << "  no targets available.\n";
        } else {
            int primary_idx = -1;
            double min_dist_pri = 1e9;
            for (int i = 0; i < occupied_hexes.size(); i++) {
                double d = distance(p_eny.q, p_eny.r, occupied_hexes[i].first, occupied_hexes[i].second);
                if (d < min_dist_pri) {
                    min_dist_pri = d;
                    primary_idx = i;
                }
            }
            auto primary = occupied_hexes[primary_idx];
            std::cout << std::format("  enemy targets primary hex ({}, {}) at dist {:.1f}\n", primary.first, primary.second, min_dist_pri);

            std::optional<std::pair<int, int>> secondary;
            if (occupied_hexes.size() > 1) {
                double min_dist_sec = 1e9;
                std::vector<int> candidates;
                for (int i = 0; i < occupied_hexes.size(); i++) {
                    if (i == primary_idx) continue;
                    double d = distance(primary.first, primary.second, occupied_hexes[i].first, occupied_hexes[i].second);
                    if (d < min_dist_sec) {
                        min_dist_sec = d;
                        candidates = {i};
                    } else if (d == min_dist_sec) {
                        candidates.push_back(i);
                    }
                }

                int max_platoons = -1;
                std::vector<int> best_candidates;
                for (int idx : candidates) {
                    int count = 0;
                    for (int p = 1; p <= PLATOONS_PER_HEX; p++) {
                        if (platoons[p].platoon.deployed && platoons[p].q == occupied_hexes[idx].first && platoons[p].r == occupied_hexes[idx].second) {
                            count++;
                        }
                    }
                    if (count > max_platoons) {
                        max_platoons = count;
                        best_candidates = {idx};
                    } else if (count == max_platoons) {
                        best_candidates.push_back(idx);
                    }
                }
                secondary = occupied_hexes[best_candidates[roll_dice(best_candidates.size()) - 1]];
                double d_eny_sec = distance(p_eny.q, p_eny.r, secondary->first, secondary->second);
                std::cout << std::format("  enemy targets secondary hex ({}, {}) at dist {:.1f}\n", secondary->first, secondary->second, d_eny_sec);
            }

            resolve_combat(primary.first, primary.second);
            if (secondary) {
                resolve_combat(secondary->first, secondary->second);
            }
        }
    }

    p_eny.platoon.state = Fresh;

    turns += 1;
    return 0;
}

Game::Game() : turns(1), hits(0) {
    platoons = new PlatoonData[PLATOONS_PER_HEX +1]; // 3 friendly + 1 enemy
    for (int i = 0; i < PLATOONS_PER_HEX +1; i++)
        platoons[i] = PlatoonData(i);

    board = new Platoon**[BOARD_SIZE];
    for (int i = 0; i < BOARD_SIZE; i++)
        board[i] = new Platoon*[PLATOONS_PER_HEX +1]();

    // place the ennemy at the top of the hill
    set(1, 0, 0);
    platoons[0].platoon.state = Fresh;
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
            present.push_back(" ENI ");
        else {
            char a = ' ', b = ' ';
            if (board[i][j]->state == Spent)
                a = '[', b = ']';
            present.push_back(std::format("{}A-{}{}", a, board[i][j]->number, b));
        }
    }


    std::string l[] = {"     ", "     ", "     "};
    int count = present.size();
    if (count == 1) l[1] = present[0];
    else if (count == 2) { l[1] = present[0]; l[2] = present[1]; }
    else if (count >= 3) { l[0] = present[0]; l[1] = present[1]; l[2] = present[2]; }

    return {
        "  _____  ",
        " /" + l[0] + "\\ ",
        "/ " + l[1] + " \\",
        "\\ " + l[2] + " /",
        " \\_____/ "
    };
}

void Game::resolve_combat(int q, int r) {
    for (int p = 1; p <= PLATOONS_PER_HEX; p++) {
        if (platoons[p].platoon.deployed && platoons[p].q == q && platoons[p].r == r) {
            int roll = roll_dice();
            double d_hill = distance(platoons[p].q, platoons[p].r, 1, 0);
            if (roll >= d_hill) {
                platoons[p].platoon.state = Spent;
                hits++;
                std::cout << std::format("    A{} hit! ({} >= {:.1f})\n", p, roll, d_hill);
            } else {
                std::cout << std::format("    A{} survived ({} < {:.1f})\n", p, roll, d_hill);
            }
        }
    }
}

bool Game::load(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "error: could not open file " << filename << "\n";
        return false;
    }

    std::string line;
    try {
        while (std::getline(file, line)) {
            if (line.empty()) continue;
            // remove comments if they exist
            size_t comment_pos = line.find('#');
            if (comment_pos != std::string::npos) {
                line = line.substr(0, comment_pos);
            }

            // trim trailing spaces
            while (!line.empty() && std::isspace(line.back())) line.pop_back();
            if (line.empty()) continue;

            size_t colon_pos = line.find(':');
            if (colon_pos == std::string::npos) continue;

            std::string key = line.substr(0, colon_pos);
            std::string value = line.substr(colon_pos + 1);

            if (key == "turns") {
                turns = std::stoi(value);
            } else if (key == "hits") {
                hits = std::stoi(value);
            } else if (key.size() == 2 && key[0] == 'A') {
                int n = key[1] - '0';
                if (n < 1 || n > PLATOONS_PER_HEX) continue;

                std::stringstream ss(value);
                std::string part;
                std::vector<std::string> parts;
                while (std::getline(ss, part, ',')) {
                    parts.push_back(part);
                }

                if (parts.size() < 3) continue;

                int q = std::stoi(parts[0]);
                int r = std::stoi(parts[1]);
                char state_char = parts[2][0];

                if (!is_valid(q, r)) continue;

                // clear existing position on board
                for (int i = 0; i < BOARD_SIZE; i++) {
                    for (int j = 0; j < PLATOONS_PER_HEX + 1; j++) {
                        if (board[i][j] && board[i][j]->number == n) {
                            board[i][j] = nullptr;
                        }
                    }
                }

                platoons[n].q = q;
                platoons[n].r = r;
                platoons[n].platoon.deployed = true;
                platoons[n].platoon.state = (state_char == 'F') ? Fresh : Spent;
                board[hex_index(q, r)][n] = &platoons[n].platoon;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "error parsing state file: " << e.what() << "\n";
        return false;
    }

    return true;
}
