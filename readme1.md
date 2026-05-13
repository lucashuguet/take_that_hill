# Take That Hill
Huguet & Laurence

## Enum
On définit les états de la section par des enums

```cpp
  enum State {
    Fresh,
    Spent
  }
```

## Structures
Le jeu, les sections amies et ennemies sont des structures:

```cpp
  struct Platoon {
    int number; // numéro de la section (1, 2, 3). 0 pour ennemi
    bool deployed;
    State state;
  };

  struct Game {
    int turns; // nombres de tours
    Platoon* platoons;
    Platoon** board; // tableau en 2D si il y a plusieurs sections sur une case
  };
```

## Fonctions
Il faut:

 - `Game::display()` qui affiche l'état actuel du jeu
 avec le compteur de tours et de l'état fresh/spent du joueur
 - `Game::step()` pour passer au tour suivant, gérant la logique déplacement, tir, rassemblement, ennemi (ces étapes pouvant à leur tour être des fonctions). Vérifie les conditions de victoire.
 - `rollDice()` gérant l'aléatoire
 - `interact()` pour intéragir avec le joueur tout en prévenant toute erreur

## Structure du projet

```
src
 |- main.cpp // contient la boucle principale du jeu
 |- game.cpp // définit les différentes classes et leurs méthodes
 |- utils.cpp // définit quelques fonctions utiles. ie rollDice()
lib
 |- game.h // fichier d'entête de game.cpp
 |- utils.h // fichier d'entête de utils.cpp
readme1.md
```
