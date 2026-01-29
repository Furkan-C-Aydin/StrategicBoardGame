#include "Board.h"
#include <iostream>
#include <cmath>

Board::Board() {
    for (auto &row : grid) {
        for (auto &cell : row) {
            cell = Cell::Empty;
        }
    }

    int mid = SIZE / 2;
    p1Pos = {0, mid};
    p2Pos = {SIZE - 1, mid};

    grid[p1Pos.row][p1Pos.col] = Cell::P1;
    grid[p2Pos.row][p2Pos.col] = Cell::P2;
}

void Board::print() const {


    using std::cout;
    using std::endl;

    cout << "    1 2 3 4 5 6 7" << endl;
    cout << "   ----------------" << endl;

    for (int r = 0; r < SIZE; ++r) {
        char rowLabel = 'a' + r;
        cout << " " << rowLabel << " |";

        for (int c = 0; c < SIZE; ++c) {
            char ch;
            switch (grid[r][c]) {
                case Cell::Empty:   ch = '.'; break;
                case Cell::Blocked: ch = '#'; break;
                case Cell::P1:      ch = 'B'; break;
                case Cell::P2:      ch = 'R'; break;
            }
            cout << " " << ch;
        }
        cout << endl;
    }
}

bool Board::isInside(int row, int col) const {
    return row >= 0 && row < SIZE && col >= 0 && col < SIZE;
}

Board::Position Board::getPlayerPosition(Cell player) const {
    return (player == Cell::P1) ? p1Pos : p2Pos;
}

bool Board::movePlayer(Cell player, int newRow, int newCol) {
    if (!isInside(newRow, newCol)) {
        return false;
    }

    Position &pos = (player == Cell::P1) ? p1Pos : p2Pos;
    Cell other = (player == Cell::P1) ? Cell::P2 : Cell::P1;

    int dr = std::abs(newRow - pos.row);
    int dc = std::abs(newCol - pos.col);

    if ((dr == 0 && dc == 0) || dr > 1 || dc > 1) {
        return false;
    }

    if (grid[newRow][newCol] == Cell::Blocked || grid[newRow][newCol] == other) {
        return false;
    }

    grid[pos.row][pos.col] = Cell::Empty;
    grid[newRow][newCol] = player;
    pos = {newRow, newCol};

    return true;
}

bool Board::removeCellAt(int row, int col) {
    if (!isInside(row, col)) {
        return false;
    }

    if (grid[row][col] == Cell::Blocked ||
        grid[row][col] == Cell::P1 ||
        grid[row][col] == Cell::P2) {
        return false;
    }

    grid[row][col] = Cell::Blocked;
    return true;
}

bool Board::hasAnyMove(Cell player) const {
    Position pos = (player == Cell::P1) ? p1Pos : p2Pos;
    Cell other = (player == Cell::P1) ? Cell::P2 : Cell::P1;

    for (int dr = -1; dr <= 1; ++dr) {
        for (int dc = -1; dc <= 1; ++dc) {
            if (dr == 0 && dc == 0) continue;

            int nr = pos.row + dr;
            int nc = pos.col + dc;

            if (!isInside(nr, nc)) continue;
            if (grid[nr][nc] == Cell::Blocked || grid[nr][nc] == other) continue;

            return true;
        }
    }

    return false;
}
