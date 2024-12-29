#include <easyx.h>
#include <conio.h>
#include <vector>
#include <ctime>
#include <string>
#include <windows.h>
#include <algorithm>

#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable:4996)

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int GRID_SIZE = 40;
const int PLAYER_SIZE = 30;
const int BOMB_SIZE = 20;
const int ENEMY_SIZE = 30;
const int POWERUP_SIZE = 20;
const int FPS = 60;
const int ENEMY_COUNT = 5;

struct Position {
    int x, y;
};

struct Bomb {
    Position pos;
    int timer;
};

struct Enemy {
    Position pos;
    int moveTimer;
    int direction;
};

struct PowerUp {
    Position pos;
    int type; // 0: stronger bombs, 1: time bombs, 2: speed boost
};

std::vector<std::vector<int>> grid;
Position player;
std::vector<Bomb> bombs;
std::vector<Enemy> enemies;
std::vector<PowerUp> powerUps;
bool hasKey = false;
int score = 0;
int gameTime = 60 * FPS; // 1 minute
int bombStrength = 1;
int playerSpeed = 1;
bool gameover = false;

void InitializeGrid() {
    grid = std::vector<std::vector<int>>(SCREEN_WIDTH / GRID_SIZE, std::vector<int>(SCREEN_HEIGHT / GRID_SIZE, 0));
    for (int i = 0; i < grid.size(); i++) {
        for (int j = 0; j < grid[0].size(); j++) {
            if (i % 2 == 1 && j % 2 == 1) {
                grid[i][j] = 1; // Indestructible wall
            }
            else if (rand() % 3 == 0) {
                grid[i][j] = 2; // Destructible wall
            }
        }
    }

    // Clear the starting area for the player
    int clearSize = 3; // Size of the clear area
    for (int i = 0; i < clearSize; i++) {
        for (int j = 0; j < clearSize; j++) {
            grid[i][j] = 0;
        }
    }

    // Set player's starting position
    player = { 1, 1 };

    // Place the key in a random destructible wall
    while (true) {
        int x = rand() % grid.size();
        int y = rand() % grid[0].size();
        if (grid[x][y] == 2) {
            grid[x][y] = 3; // 3 represents the key
            break;
        }
    }
}

void DrawGrid() {
    for (int i = 0; i < grid.size(); i++) {
        for (int j = 0; j < grid[0].size(); j++) {
            if (grid[i][j] == 1) {
                setfillcolor(DARKGRAY);
                solidrectangle(i * GRID_SIZE, j * GRID_SIZE, (i + 1) * GRID_SIZE, (j + 1) * GRID_SIZE);
            }
            else if (grid[i][j] == 2) {
                setfillcolor(BROWN);
                solidrectangle(i * GRID_SIZE, j * GRID_SIZE, (i + 1) * GRID_SIZE, (j + 1) * GRID_SIZE);
            }
            else if (grid[i][j] == 3) {
                setfillcolor(YELLOW);
                solidrectangle(i * GRID_SIZE, j * GRID_SIZE, (i + 1) * GRID_SIZE, (j + 1) * GRID_SIZE);
            }
        }
    }
}



void DrawPlayer() {
    // Body
    setfillcolor(BLUE);
    solidrectangle(player.x * GRID_SIZE + GRID_SIZE / 4, player.y * GRID_SIZE + GRID_SIZE / 4,
        player.x * GRID_SIZE + 3 * GRID_SIZE / 4, player.y * GRID_SIZE + 3 * GRID_SIZE / 4);

    // Head
    solidcircle(player.x * GRID_SIZE + GRID_SIZE / 2, player.y * GRID_SIZE + GRID_SIZE / 4, GRID_SIZE / 6);

    // Eyes
    setfillcolor(WHITE);
    solidcircle(player.x * GRID_SIZE + 5 * GRID_SIZE / 12, player.y * GRID_SIZE + GRID_SIZE / 5, 2);
    solidcircle(player.x * GRID_SIZE + 7 * GRID_SIZE / 12, player.y * GRID_SIZE + GRID_SIZE / 5, 2);

    // Mouth
    setlinecolor(WHITE);
    arc(player.x * GRID_SIZE + 5 * GRID_SIZE / 12, player.y * GRID_SIZE + GRID_SIZE / 5,
        player.x * GRID_SIZE + 7 * GRID_SIZE / 12, player.y * GRID_SIZE + 7 * GRID_SIZE / 20, 3.14, 2 * 3.14);
}

int bombLimit = 30;
int bombsAvailable = bombLimit;

void DrawBombs() {
    for (const auto& bomb : bombs) {
        // Draw the main body of the bomb (dark gray circle)
        setfillcolor(RGB(50, 50, 50));
        solidcircle(bomb.pos.x * GRID_SIZE + GRID_SIZE / 2, bomb.pos.y * GRID_SIZE + GRID_SIZE / 2, BOMB_SIZE / 2);

        // Draw the fuse (brown rectangle)
        setfillcolor(RGB(139, 69, 19));
        solidrectangle(
            bomb.pos.x * GRID_SIZE + GRID_SIZE / 2 - 2,
            bomb.pos.y * GRID_SIZE + GRID_SIZE / 2 - BOMB_SIZE / 2 - 5,
            bomb.pos.x * GRID_SIZE + GRID_SIZE / 2 + 2,
            bomb.pos.y * GRID_SIZE + GRID_SIZE / 2 - BOMB_SIZE / 2
        );

        // Draw the spark (yellow circle)
        setfillcolor(YELLOW);
        solidcircle(
            bomb.pos.x * GRID_SIZE + GRID_SIZE / 2,
            bomb.pos.y * GRID_SIZE + GRID_SIZE / 2 - BOMB_SIZE / 2 - 7,
            3
        );
    }
}

void DrawEnemies() {
    for (const auto& enemy : enemies) {
        setfillcolor(RED);
        solidcircle(enemy.pos.x * GRID_SIZE + GRID_SIZE / 2, enemy.pos.y * GRID_SIZE + GRID_SIZE / 2, ENEMY_SIZE / 2);
    }
}

void DrawPowerUps() {
    for (const auto& powerUp : powerUps) {
        switch (powerUp.type) {
        case 0: setfillcolor(YELLOW); break; // Stronger bombs
        case 1: setfillcolor(MAGENTA); break; // Time bombs
        case 2: setfillcolor(GREEN); break; // Speed boost
        }
        solidcircle(powerUp.pos.x * GRID_SIZE + GRID_SIZE / 2, powerUp.pos.y * GRID_SIZE + GRID_SIZE / 2, POWERUP_SIZE / 2);
    }
}

void DrawUI() {
    settextcolor(WHITE);
    settextstyle(20, 0, _T("Arial"));

    TCHAR s[100];  // Increase buffer size to 100
    swprintf_s(s, 100, _T("Score: %d  Time: %d  Bombs: %d/%d  Strength: %d  Speed: %d"),
        score, gameTime / FPS, bombsAvailable, bombLimit, bombStrength, playerSpeed);

    outtextxy(10, 10, s);

    if (hasKey) {
        outtextxy(10, 40, _T("You have the key!"));
    }
}


void PlaceBomb() {
    if (grid[player.x][player.y] == 0 && bombsAvailable > 0) {
        bombs.push_back({ player, 2 * FPS }); // 2 second timer
        bombsAvailable--;
    }
}


void ExplodeBomb(const Bomb& bomb) {
    std::vector<Position> directions = { {0, -1}, {1, 0}, {0, 1}, {-1, 0} };
    for (const auto& dir : directions) {
        for (int i = 1; i <= bombStrength; i++) {
            int x = bomb.pos.x + dir.x * i;
            int y = bomb.pos.y + dir.y * i;
            if (x < 0 || x >= grid.size() || y < 0 || y >= grid[0].size() || grid[x][y] == 1) {
                break;
            }
            if (grid[x][y] == 2 || grid[x][y] == 3) {
                if (grid[x][y] == 3) {
                    hasKey = true;
                }
                grid[x][y] = 0;
                score += 10;
                if (rand() % 10 == 0) { // 10% chance to spawn a power-up
                    powerUps.push_back({ {x, y}, rand() % 3 });
                }
                break;
            }
            // Check for player collision
            if (player.x == x && player.y == y) {
                gameover = true;
            }
            // Check for enemy collision
            for (auto it = enemies.begin(); it != enemies.end();) {
                if (it->pos.x == x && it->pos.y == y) {
                    it = enemies.erase(it);
                    score += 50;
                }
                else {
                    ++it;
                }
            }
        }
    }
}

void UpdateBombs() {
    for (auto it = bombs.begin(); it != bombs.end();) {
        it->timer--;
        if (it->timer <= 0) {
            ExplodeBomb(*it);
            it = bombs.erase(it);
        }
        else {
            ++it;
        }
    }
}

void MoveEnemies() {
    std::vector<Position> directions = { {0, -1}, {1, 0}, {0, 1}, {-1, 0} };
    for (auto& enemy : enemies) {
        enemy.moveTimer--;
        if (enemy.moveTimer <= 0) {
            int newX = enemy.pos.x + directions[enemy.direction].x;
            int newY = enemy.pos.y + directions[enemy.direction].y;
            if (newX >= 0 && newX < grid.size() && newY >= 0 && newY < grid[0].size() && grid[newX][newY] == 0) {
                enemy.pos = { newX, newY };
            }
            else {
                // Change direction if blocked
                enemy.direction = rand() % 4;
            }
            enemy.moveTimer = FPS / 2; // Move twice per second
        }
    }
}

void CheckCollisions() {
    // Player-Enemy collision
    for (const auto& enemy : enemies) {
        if (player.x == enemy.pos.x && player.y == enemy.pos.y) {
            gameover = true;
            return;
        }
    }
    // Player-PowerUp collision
    for (auto it = powerUps.begin(); it != powerUps.end();) {
        if (player.x == it->pos.x && player.y == it->pos.y) {
            switch (it->type) {
            case 0: bombStrength++; break;
            case 1: gameTime += 20 * FPS; break; // Add 20 seconds
            case 2: playerSpeed = (playerSpeed < 3) ? playerSpeed + 1 : 3; break; // Max speed of 3
            }
            score += 25;
            it = powerUps.erase(it);
        }
        else {
            ++it;
        }
    }
}



bool successAchieved = false; // Flag to check if the success condition is met



void GameLoop() {
    int inputCooldown = 0;
    while (!gameover && gameTime > 0 && !hasKey) {
        // Handle input
        if (_kbhit() && inputCooldown <= 0) {
            int key = _getch();
            int newX = player.x, newY = player.y;
            if (key == 224) { // Arrow key prefix
                key = _getch(); // Get the actual arrow key
                switch (key) {
                case 72: newY -= playerSpeed; break; // Up arrow
                case 80: newY += playerSpeed; break; // Down arrow
                case 75: newX -= playerSpeed; break; // Left arrow
                case 77: newX += playerSpeed; break; // Right arrow
                }
            }
            else if (key == 32) { // Space key
                PlaceBomb();
            }
            if (newX >= 0 && newX < grid.size() && newY >= 0 && newY < grid[0].size() && grid[newX][newY] == 0) {
                player = { newX, newY };
            }
            inputCooldown = FPS / 10; // 100ms cooldown
        }
        if (inputCooldown > 0) inputCooldown--;

        // Update game state
        UpdateBombs();
        MoveEnemies();
        CheckCollisions();
        gameTime--;

        // Check if score reaches 200 for success message
        if (score >= 200 && !successAchieved) {
            successAchieved = true; // Set the flag to prevent multiple prints
            cleardevice(); // Clear the screen
            settextstyle(60, 0, _T("Comic Sans MS"));
            settextcolor(GREEN);
            outtextxy(SCREEN_WIDTH / 2 - 200, SCREEN_HEIGHT / 2 - 120, _T("Success! You scored 200 points!"));
            FlushBatchDraw();
            Sleep(2000); // Show message for 2 seconds
        }

        // Draw everything
        cleardevice();
        DrawGrid();
        DrawPlayer();
        DrawBombs();
        DrawEnemies();
        DrawPowerUps();
        DrawUI();
        FlushBatchDraw();

        Sleep(1000 / FPS); // 60 FPS
    }
}





int main() {
    initgraph(SCREEN_WIDTH, SCREEN_HEIGHT);
    srand(static_cast<unsigned>(time(0)));

    do {
        InitializeGrid();
        player = { 1, 1 };
        enemies.clear();
        for (int i = 0; i < ENEMY_COUNT; i++) {
            int x, y;
            do {
                x = rand() % (SCREEN_WIDTH / GRID_SIZE);
                y = rand() % (SCREEN_HEIGHT / GRID_SIZE);
            } while (grid[x][y] != 0 || (x == 1 && y == 1)); // Ensure enemies don't spawn on walls or the player
            enemies.push_back({ {x, y}, FPS / 2, rand() % 4 });
        }
        bombs.clear();
        powerUps.clear();
        hasKey = false;
        score = 0;
        gameTime = 60 * FPS; // Reset to 1 minute
        bombStrength = 1;
        playerSpeed = 1;
        gameover = false;

        BeginBatchDraw();
        GameLoop();
        EndBatchDraw();

        // Game over screen
        cleardevice();
        settextstyle(60, 0, _T("Comic Sans MS"));
        settextcolor(CYAN);
        if (gameover) {
            outtextxy(SCREEN_WIDTH / 2 - 200, SCREEN_HEIGHT / 2 - 120, _T("Game Over!"));
        }
        else if (hasKey) {
            outtextxy(SCREEN_WIDTH / 2 - 200, SCREEN_HEIGHT / 2 - 120, _T("Congratulations!"));
        }
        else {
            outtextxy(SCREEN_WIDTH / 2 - 200, SCREEN_HEIGHT / 2 - 120, _T("Time's Up!"));
        }

        settextstyle(40, 0, _T("Arial"));
        settextcolor(YELLOW);
        TCHAR scoreText[50];
        swprintf_s(scoreText, 50, _T("Final Score: %d"), score);
        outtextxy(SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT / 2 - 30, scoreText);

        settextcolor(MAGENTA);
        outtextxy(SCREEN_WIDTH / 2 - 200, SCREEN_HEIGHT / 2 + 60, _T("Press Enter to restart"));
        settextcolor(GREEN);
        outtextxy(SCREEN_WIDTH / 2 - 200, SCREEN_HEIGHT / 2 + 120, _T("Press Esc to exit"));

        FlushBatchDraw();

        // Wait for Enter or Esc
        int key;
        do {
            key = _getch();
        } while (key != 13 && key != 27);

        if (key == 27) break; // Exit if Esc is pressed

    } while (true); // Loop until Esc is pressed

    closegraph();
    return 0;
}