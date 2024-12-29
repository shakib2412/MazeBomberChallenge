# Maze Bomber Challenge

## Overview
Maze Bomber Challenge is an action-packed grid-based game where players navigate through a dynamically generated maze, strategically placing bombs to clear paths and destroy enemies. The goal is to collect a key hidden behind destructible walls to unlock the next level, all while avoiding or eliminating patrolling enemies. This game combines strategic planning with quick reaction skills to provide a challenging and engaging experience.

---

## Features
- **Dynamic Maze Generation:** Each game level starts with a freshly generated grid of destructible and indestructible walls.
- **Power-Ups:** Enhance your abilities with power-ups that increase bomb strength, add extra time, or boost your speed.
- **Enemy AI:** Face AI-controlled enemies that patrol the maze and challenge your progress.
- **Timed Gameplay:** Complete your objectives within the time limit to maximize your score.
- **Scoring System:** Earn points by destroying walls, defeating enemies, and collecting the key.

---

## Technical Features
- **Language:** C++ with the EasyX graphics library for rendering.
- **Graphics:** Utilizes EasyX library functions to draw custom shapes and objects including players, enemies, bombs, and power-ups.
- **Game Mechanics:**
  - Bomb placement with countdown timers for explosions.
  - Collision detection between bombs, player, enemies, and power-ups.
  - Enemy movement logic that changes direction upon hitting obstacles.
- **Controls:**
  - **Arrow Keys:** Move the player.
  - **Space Bar:** Place bombs.

---

## Getting Started

### Prerequisites
- A C++ compiler that supports EasyX, such as MinGW or Visual Studio.
- EasyX library installed and configured in your IDE.

### Installation
1. Clone the repository:
git clone https://github.com/your-username/Maze-Bomber-Challenge.git
2. Open the project in your C++ IDE.
3. Build and run the `Source.cpp` file.

---

## Gameplay
Navigate through the maze using the arrow keys. Place bombs with the space bar to destroy walls and defeat enemies. Collect power-ups to enhance your abilities and find the key to complete the level. Be mindful of the timer and your proximity to bombs to avoid losing the game.

---

## Code Structure
- `InitializeGrid()`: Sets up the initial grid state.
- `DrawGrid()`, `DrawPlayer()`, `DrawBombs()`, `DrawEnemies()`, `DrawPowerUps()`, `DrawUI()`: Rendering functions for the game components.
- `PlaceBomb()`, `ExplodeBomb()`: Manage bomb logic.
- `UpdateBombs()`, `MoveEnemies()`, `CheckCollisions()`: Update the game state each frame.

---

## Contributing
Contributions are welcome. Please fork the project, create a new branch, and submit your pull requests for review.

---

## License
This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details.

---


## Acknowledgments
- Hat tip to anyone whose code was used
- Inspiration
- etc.

