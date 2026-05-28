# Take That Hill
Huguet & Laurence

## Enum
The states of a platoon are defined by an enum:

```cpp
enum State {
  Fresh,
  Spent
}
```

## Structures
The game and the platoons are represented by the following structures:

```cpp
struct Platoon {
  int number; // Platoon number (1, 2, 3). 0 for enemy
  bool deployed;
  State state;
};

struct PlatoonData {
  int q; // Column coordinate
  int r; // Row coordinate
  Platoon platoon;
};

struct Game {
  int turns; // Number of turns elapsed
  int hits; // Number of times a friendly platoon was hit
  PlatoonData* platoons;
  Platoon*** board; // 2D array for platoons in each hex
};
```

## Logic and Functions
- `Game::display()`: Displays the current game state, including the turn counter, the current score (turns + hits), and the board. Spent platoons are displayed in braces (e.g., `[A-1]`).
- `Game::step()`: Manages the game turn sequence:
    - **Movement**: Deploying and moving `Fresh` platoons.
    - **Win Condition**: Checks if a friendly platoon has reached the enemy hex (1, 0).
    - **Firing**: Friendly platoons can attack the enemy.
    - **Rally**: Friendly platoons attempt to recover from `Spent` to `Fresh`.
    - **Enemy Action**: The enemy targets the closest friendly hex (primary) and a secondary hex based on proximity to the primary and platoon density.
- `Game::resolve_combat()`: Internal logic to roll dice and determine if a platoon in a target hex is hit (roll >= distance to hill).
- `roll_dice()`: Generates a random integer (1-6).
- `ask_choice()`: Manages validated user input.

### Victory and Defeat
The game uses a score calculated as: `score = turns + hits`.
- **Victory**: Friendly platoon reaches (1, 0) with `Score <= 10`.
- **Draw**: Friendly platoon reaches (1, 0) with `11 <= Score <= 15`.
- **Defeat**: `Score >= 16` or the turn limit (15) is reached without victory.

## Project Structure

```
src
 |- main.cpp // Game loop and win/loss screens
 |- game.cpp // Game logic and combat resolution
 |- utils.cpp // Utility functions (dice, input, distance)
lib
 |- game.h // Game and Platoon definitions
 |- utils.h // Utility function declarations
```

## Utilisation de l'IA
L'IA a été assez peu utilisé sauf pour quelques fonction complexes comme `stamp` où l'IA (Claude) fut un réel avantage pour ne pas avoir à passer une éterniter à trouver et implémenter un algorithme, dans le fond peu intéressant.
Ayant une idée très précise de ce que l'ont cherchait à atteindre avec l'IA, ses résultats étaient très satisfaisant.
L'IA a aussi permis d'expliquer quelques concepts de c++.
