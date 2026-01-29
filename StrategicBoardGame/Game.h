#ifndef STRATEGICBOARDGAME_GAME_H
#define STRATEGICBOARDGAME_GAME_H

#include "Board.h"
#include <vector>

class Game {
public:
    struct Move {
        int moveRow;
        int moveCol;
        int removeRow;
        int removeCol;
    };

    std::vector<Move> generateMoves(const Board &board, Board::Cell player) const;

    Board applyMove(const Board &board, const Move &m, Board::Cell player) const;

    int evaluateBoard(const Board &board, Board::Cell aiPlayer) const;

    Move findBestMove(const Board &board, Board::Cell aiPlayer, int depth) const;

private:
    int minimax(Board board,
                int depth,
                bool maximizingPlayer,
                Board::Cell aiPlayer,
                int alpha,
                int beta) const;
};

#endif
