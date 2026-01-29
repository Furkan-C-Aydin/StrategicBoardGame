#ifndef STRATEGICBOARDGAME_BOARD_H
#define STRATEGICBOARDGAME_BOARD_H

#include <array>

class Board {
public:
    static constexpr int SIZE = 7; //compile-time constant

    enum class Cell {
        Empty,
        Blocked,
        P1,
        P2
    };

    struct Position {
        int row;
        int col;
    };

    Board();

    void print() const;

    bool movePlayer(Cell player, int newRow, int newCol);

    bool removeCellAt(int row, int col);

    bool hasAnyMove(Cell player) const;

    Position getPlayerPosition(Cell player) const;

    Cell getCell(int row, int col) const { return grid[row][col]; }

private:
    std::array<std::array<Cell, SIZE>, SIZE> grid{};

    Position p1Pos{};
    Position p2Pos{};

    bool isInside(int row, int col) const;
};

#endif
