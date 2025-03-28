#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <chrono>
#include <windows.h>
#include <conio.h>

enum class TetrominoType {
    I, O, T, S, Z, J, L
};

class Tetromino {
public:
    TetrominoType type;
    std::vector<std::vector<int>> shape;
    int color;

    Tetromino() : type(TetrominoType::I), color(0) {}

    Tetromino(TetrominoType t) : type(t) {
        initializeShape();
    }

private:
    void initializeShape() {
        switch (type) {
        case TetrominoType::I:
            shape = { {1,1,1,1} };
            color = 1;
            break;
        case TetrominoType::O:
            shape = { {1,1},{1,1} };
            color = 2;
            break;
        case TetrominoType::T:
            shape = { {0,1,0},{1,1,1} };
            color = 3;
            break;
        case TetrominoType::S:
            shape = { {0,1,1},{1,1,0} };
            color = 4;
            break;
        case TetrominoType::Z:
            shape = { {1,1,0},{0,1,1} };
            color = 5;
            break;
        case TetrominoType::J:
            shape = { {1,0,0},{1,1,1} };
            color = 6;
            break;
        case TetrominoType::L:
            shape = { {0,0,1},{1,1,1} };
            color = 7;
            break;
        }
    }
};

class TetrisGame {
private:
    static const int WIDTH = 10;
    static const int HEIGHT = 20;
    std::vector<std::vector<int>> grid;
    Tetromino currentPiece;
    int currentX, currentY;
    int score;
    int level;
    int dropTimer;

public:
    TetrisGame() :
        grid(HEIGHT, std::vector<int>(WIDTH, 0)),
        currentPiece(getRandomTetromino()),
        currentX(WIDTH / 2),
        currentY(0),
        score(0),
        level(1),
        dropTimer(0) {

        srand(static_cast<unsigned>(time(nullptr)));
    }

    void run() {
        while (!isGameOver()) {
            handleInput();
            update();
            render();
            Sleep(100); // Reduced sleep for more responsive controls
        }
        gameOver();
    }

private:
    void handleInput() {
        if (_kbhit()) {
            int key = _getch();
            switch (key) {
            case 75: // Left arrow
                moveLeft();
                break;
            case 77: // Right arrow
                moveRight();
                break;
            case 72: // Up arrow
                rotate();
                break;
            case 80: // Down arrow
                softDrop();
                break;
            case 32: // Spacebar
                hardDrop();
                break;
            case 27: // ESC key
                exit(0);
            }
        }
    }

    TetrominoType getRandomTetromino() {
        return static_cast<TetrominoType>(rand() % 7);
    }

    void moveLeft() {
        if (canMove(currentPiece, currentX - 1, currentY)) {
            currentX--;
        }
    }

    void moveRight() {
        if (canMove(currentPiece, currentX + 1, currentY)) {
            currentX++;
        }
    }

    void rotate() {
        Tetromino rotatedPiece = currentPiece;
        // Rotate 90 degrees
        std::vector<std::vector<int>> newShape(
            currentPiece.shape[0].size(),
            std::vector<int>(currentPiece.shape.size())
        );

        for (size_t i = 0; i < currentPiece.shape.size(); ++i) {
            for (size_t j = 0; j < currentPiece.shape[i].size(); ++j) {
                newShape[j][currentPiece.shape.size() - 1 - i] = currentPiece.shape[i][j];
            }
        }

        rotatedPiece.shape = newShape;

        if (canMove(rotatedPiece, currentX, currentY)) {
            currentPiece = rotatedPiece;
        }
    }

    void update() {
        dropTimer++;

        // Adjust drop speed based on level
        if (dropTimer >= (20 - level)) {
            if (!canMove(currentPiece, currentX, currentY + 1)) {
                lockPiece();
                clearLines();
                spawnNewPiece();
            }
            else {
                currentY++;
            }
            dropTimer = 0;
        }
    }

    void softDrop() {
        if (canMove(currentPiece, currentX, currentY + 1)) {
            currentY++;
        }
        else {
            lockPiece();
            clearLines();
            spawnNewPiece();
        }
    }

    void hardDrop() {
        while (canMove(currentPiece, currentX, currentY + 1)) {
            currentY++;
        }
        lockPiece();
        clearLines();
        spawnNewPiece();
    }

    bool canMove(const Tetromino& piece, int newX, int newY) {
        for (size_t y = 0; y < piece.shape.size(); ++y) {
            for (size_t x = 0; x < piece.shape[y].size(); ++x) {
                if (piece.shape[y][x]) {
                    int gridX = newX + x;
                    int gridY = newY + y;

                    // Check bounds
                    if (gridX < 0 || gridX >= WIDTH ||
                        gridY >= HEIGHT) {
                        return false;
                    }

                    // Check collision with existing blocks
                    if (gridY >= 0 && grid[gridY][gridX]) {
                        return false;
                    }
                }
            }
        }
        return true;
    }

    void lockPiece() {
        for (size_t y = 0; y < currentPiece.shape.size(); ++y) {
            for (size_t x = 0; x < currentPiece.shape[y].size(); ++x) {
                if (currentPiece.shape[y][x]) {
                    grid[currentY + y][currentX + x] = currentPiece.color;
                }
            }
        }
    }

    void clearLines() {
        int linesCleared = 0;
        for (int y = HEIGHT - 1; y >= 0; --y) {
            bool fullLine = true;
            for (int x = 0; x < WIDTH; ++x) {
                if (!grid[y][x]) {
                    fullLine = false;
                    break;
                }
            }

            if (fullLine) {
                // Remove the line
                grid.erase(grid.begin() + y);
                // Add a new empty line at the top
                grid.insert(grid.begin(), std::vector<int>(WIDTH, 0));
                linesCleared++;
                y++; // Check the same row again
            }
        }

        // Update score and level
        updateScore(linesCleared);
    }

    void updateScore(int linesCleared) {
        switch (linesCleared) {
        case 1: score += 100; break;
        case 2: score += 300; break;
        case 3: score += 500; break;
        case 4: score += 800; break;
        }

        // Increase level every 10 lines
        level = 1 + (score / 1000);
    }

    void spawnNewPiece() {
        currentPiece = Tetromino(getRandomTetromino());
        currentX = WIDTH / 2;
        currentY = 0;

        // Check if game is over
        if (!canMove(currentPiece, currentX, currentY)) {
            gameOver();
        }
    }

    bool isGameOver() {
        // Check if any block reached the top
        for (int x = 0; x < WIDTH; ++x) {
            if (grid[0][x]) {
                return true;
            }
        }
        return false;
    }

    void gameOver() {
        system("cls");
        std::cout << "Game Over!\n";
        std::cout << "Final Score: " << score << std::endl;
        exit(0);
    }

    void render() {
        // Clear console
        system("cls");

        // Render game border
        std::cout << "TETRIS GAME\n";
        std::cout << "Score: " << score << " | Level: " << level << std::endl;
        std::cout << "----------------------\n";

        // Render grid
        for (int y = 0; y < HEIGHT; ++y) {
            std::cout << "|";
            for (int x = 0; x < WIDTH; ++x) {
                bool pieceHere = false;

                // Check if current piece is at this position
                for (size_t py = 0; py < currentPiece.shape.size(); ++py) {
                    for (size_t px = 0; px < currentPiece.shape[py].size(); ++px) {
                        if (currentPiece.shape[py][px] &&
                            y == currentY + py &&
                            x == currentX + px) {
                            pieceHere = true;
                            break;
                        }
                    }
                    if (pieceHere) break;
                }

                if (pieceHere) {
                    std::cout << "[]";
                }
                else if (grid[y][x]) {
                    std::cout << "[]";
                }
                else {
                    std::cout << "  ";
                }
            }
            std::cout << "|\n";
        }
        std::cout << "----------------------\n";
    }
};

int main() {
    TetrisGame game;
    game.run();
    return 0;
}
