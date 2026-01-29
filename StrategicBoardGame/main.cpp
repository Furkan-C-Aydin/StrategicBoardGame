#include <iostream>
#include <string>
#include <cctype>
#include <limits>
#include "Board.h"
#include "Game.h"

using std::cout;
using std::cin;
using std::endl;

bool parseSquare(const std::string &input, int &row, int &col) {
    if (input.size() != 2)
        return false;

    char r = std::tolower(static_cast<unsigned char>(input[0]));
    char c = input[1];

    if (r < 'a' || r > 'g')
        return false;
    if (c < '1' || c > '7')
        return false;

    row = r - 'a';
    col = c - '1';
    return true;
}

std::string squareToString(int row, int col) {
    char r = static_cast<char>('a' + row);
    char c = static_cast<char>('1' + col);
    return std::string() + r + c;
}

int selectAIDepth() {
    while (true) {
        cout << "Select AI Difficulty Level:\n";
        cout << "1 - Easy   (depth = 1)\n";
        cout << "2 - Medium (depth = 3)\n";
        cout << "3 - Hard   (depth = 4)\n";
        cout << "Your choice: ";

        int choice;
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }

        switch (choice) {
            case 1: return 1;
            case 2: return 3;
            case 3: return 4;
            default:
                cout << "Invalid choice. Please try again.\n";
        }
    }
}

int main() {
    Board board;
    Game game;

    Board::Cell currentPlayer = Board::Cell::P1;

    int AI_DEPTH = selectAIDepth();
    cout << "\nSelected AI depth: " << AI_DEPTH << "\n\n";

    cout << "Strategic Board Game - CLI Version\n";
    cout << "P1 = B (AI),  P2 = R (Human)\n\n";

    while (true) {
        board.print();
        cout << endl;

        if (!board.hasAnyMove(currentPlayer)) {
            cout << "Player ";
            if (currentPlayer == Board::Cell::P1) cout << "P1 (B - AI)";
            else cout << "P2 (R - Human)";
            cout << " has no legal moves. Game Over!\n";
            cout << "Winner: ";
            if (currentPlayer == Board::Cell::P1) cout << "P2 (Human)\n";
            else cout << "P1 (AI)\n";
            break;
        }

        if (currentPlayer == Board::Cell::P1) {
            cout << "[AI] Player 1 (B) is thinking...\n";

            Game::Move best = game.findBestMove(board, Board::Cell::P1, AI_DEPTH);

            if (best.moveRow == -1) {
                cout << "[AI] No valid move found.\n";
                break;
            }

            cout << "[AI] Moves to " << squareToString(best.moveRow, best.moveCol)
                 << " and removes " << squareToString(best.removeRow, best.removeCol) << ".\n";

            board = game.applyMove(board, best, Board::Cell::P1);

        } else {
            cout << "Player 2 (R - Human), it's your turn.\n";

            int targetRow, targetCol;
            while (true) {
                cout << "Enter your move target (e.g. b3): ";
                std::string moveStr;
                cin >> moveStr;

                if (parseSquare(moveStr, targetRow, targetCol) &&
                    board.movePlayer(currentPlayer, targetRow, targetCol)) {
                    break;
                } else {
                    cout << "Invalid move! Please try again.\n";
                }
            }

            board.print();
            cout << endl;

            int remRow, remCol;
            while (true) {
                cout << "Enter a square to remove (e.g. c4): ";
                std::string remStr;
                cin >> remStr;

                if (parseSquare(remStr, remRow, remCol) &&
                    board.removeCellAt(remRow, remCol)) {
                    break;
                } else {
                    cout << "Invalid removal! Try again.\n";
                }
            }
        }

        currentPlayer = (currentPlayer == Board::Cell::P1)
                        ? Board::Cell::P2
                        : Board::Cell::P1;

        cout << endl;
    }

    cout << "Game finished. Press ENTER to exit...\n";
    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    cin.get();
    return 0;
}
