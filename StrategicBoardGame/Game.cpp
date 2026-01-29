#include "Game.h"
#include <limits>
#include <cmath>

namespace {
    const int INF = 1000000;

    int countStepMoves(const Board &board, Board::Cell player) {
        Board::Position pos = board.getPlayerPosition(player);
        Board::Cell opponent = (player == Board::Cell::P1) ? Board::Cell::P2 : Board::Cell::P1;

        int cnt = 0;
        for (int dr = -1; dr <= 1; ++dr) {
            for (int dc = -1; dc <= 1; ++dc) {
                if (dr == 0 && dc == 0) continue;

                int nr = pos.row + dr;
                int nc = pos.col + dc;

                if (nr < 0 || nr >= Board::SIZE || nc < 0 || nc >= Board::SIZE)
                    continue;

                auto cell = board.getCell(nr, nc);
                if (cell != Board::Cell::Blocked && cell != opponent)
                    cnt++;
            }
        }
        return cnt;
    }

    int removalQuality(const Board &afterMoveBoard, int removeRow, int removeCol, Board::Cell aiPlayer) {
        Board::Cell opponent = (aiPlayer == Board::Cell::P1) ? Board::Cell::P2 : Board::Cell::P1;

        Board::Position aiPos = afterMoveBoard.getPlayerPosition(aiPlayer);
        Board::Position opPos = afterMoveBoard.getPlayerPosition(opponent);

        auto isAdjacent = [](const Board::Position &p, int r, int c) {
            int dr = std::abs(r - p.row);
            int dc = std::abs(c - p.col);
            return (dr <= 1 && dc <= 1 && !(dr == 0 && dc == 0));
        };

        int score = 0;

        if (isAdjacent(opPos, removeRow, removeCol)) score += 10;
        if (isAdjacent(aiPos, removeRow, removeCol)) score -= 4;

        int center = Board::SIZE / 2;
        score -= (std::abs(removeRow - center) + std::abs(removeCol - center)) / 3;

        return score;
    }
    std::vector<Board::Position> collectRemovalCandidates(const Board &board,
                                                      Board::Cell currentPlayer,
                                                      int K = 12) {
        Board::Cell opponent = (currentPlayer == Board::Cell::P1) ? Board::Cell::P2 : Board::Cell::P1;

        Board::Position curPos = board.getPlayerPosition(currentPlayer);
        Board::Position opPos  = board.getPlayerPosition(opponent);

        bool used[Board::SIZE][Board::SIZE] = {false};
        std::vector<Board::Position> cand;

        auto tryAdd = [&](int r, int c) {
            if (r < 0 || r >= Board::SIZE || c < 0 || c >= Board::SIZE) return;
            if (used[r][c]) return;
            if (board.getCell(r, c) != Board::Cell::Empty) return;
            used[r][c] = true;
            cand.push_back({r, c});
        };

        auto addNeighbors = [&](const Board::Position &p) {
            for (int dr = -1; dr <= 1; ++dr) {
                for (int dc = -1; dc <= 1; ++dc) {
                    if (dr == 0 && dc == 0) continue;
                    tryAdd(p.row + dr, p.col + dc);
                    if ((int)cand.size() >= K) return;
                }
            }
        };

        addNeighbors(opPos);
        if ((int)cand.size() >= K) return cand;

        addNeighbors(curPos);
        if ((int)cand.size() >= K) return cand;

        int center = Board::SIZE / 2;
        for (int dist = 0; dist <= Board::SIZE; ++dist) {
            for (int r = 0; r < Board::SIZE; ++r) {
                for (int c = 0; c < Board::SIZE; ++c) {
                    if (std::abs(r - center) + std::abs(c - center) == dist) {
                        tryAdd(r, c);
                        if ((int)cand.size() >= K) return cand;
                    }
                }
            }
        }

        return cand;
    }

}

std::vector<Game::Move> Game::generateMoves(const Board &board, Board::Cell player) const {
    std::vector<Move> moves;

    Board::Position pos = board.getPlayerPosition(player);
    Board::Cell opponent = (player == Board::Cell::P1) ? Board::Cell::P2 : Board::Cell::P1;


    const int K = 12;
    std::vector<Board::Position> removalCands = collectRemovalCandidates(board, player, K);


    for (int dr = -1; dr <= 1; ++dr) {
        for (int dc = -1; dc <= 1; ++dc) {
            if (dr == 0 && dc == 0) continue;

            int nr = pos.row + dr;
            int nc = pos.col + dc;

            if (nr < 0 || nr >= Board::SIZE || nc < 0 || nc >= Board::SIZE)
                continue;

            Board::Cell cell = board.getCell(nr, nc);
            if (cell == Board::Cell::Blocked || cell == opponent)
                continue;

            // For each valid movement, try only a filtered list of removal squares
            for (const auto &rc : removalCands) {
                // Cannot remove the square where the player just moved
                if (rc.row == nr && rc.col == nc) continue;

                // Allow removing the player's old position (it becomes Empty after move)
                bool rcIsOldPos = (rc.row == pos.row && rc.col == pos.col);

                if (!rcIsOldPos && board.getCell(rc.row, rc.col) != Board::Cell::Empty)
                    continue;


                moves.push_back(Move{nr, nc, rc.row, rc.col});
            }
        }
    }

    return moves;
}


Board Game::applyMove(const Board &board, const Move &m, Board::Cell player) const {
    Board newBoard = board;
    newBoard.movePlayer(player, m.moveRow, m.moveCol);
    newBoard.removeCellAt(m.removeRow, m.removeCol);
    return newBoard;
}

int Game::evaluateBoard(const Board &board, Board::Cell aiPlayer) const {
    Board::Cell opponent = (aiPlayer == Board::Cell::P1)
                           ? Board::Cell::P2
                           : Board::Cell::P1;

    int mySteps  = countStepMoves(board, aiPlayer);
    int oppSteps = countStepMoves(board, opponent);

    return 20 * (mySteps - oppSteps);
}

int Game::minimax(Board board,
                  int depth,
                  bool maximizingPlayer,
                  Board::Cell aiPlayer,
                  int alpha,
                  int beta) const {
    Board::Cell opponent = (aiPlayer == Board::Cell::P1)
                           ? Board::Cell::P2
                           : Board::Cell::P1;

    Board::Cell current = maximizingPlayer ? aiPlayer : opponent;

    if (!board.hasAnyMove(current)) {
        if (current == aiPlayer)
            return -INF;
        else
            return +INF;
    }

    if (depth == 0) {
        return evaluateBoard(board, aiPlayer);
    }

    std::vector<Move> moves = generateMoves(board, current);

    if (maximizingPlayer) {
        int bestVal = -INF;
        for (const Move &m : moves) {
            Board nextBoard = applyMove(board, m, current);
            int val = minimax(nextBoard, depth - 1, false, aiPlayer, alpha, beta);

            if (val > bestVal) bestVal = val;
            if (val > alpha)   alpha = val;
            if (beta <= alpha) break;
        }
        return bestVal;
    } else {
        int bestVal = +INF;
        for (const Move &m : moves) {
            Board nextBoard = applyMove(board, m, current);
            int val = minimax(nextBoard, depth - 1, true, aiPlayer, alpha, beta);

            if (val < bestVal) bestVal = val;
            if (val < beta)    beta = val;
            if (beta <= alpha) break;
        }
        return bestVal;
    }
}

Game::Move Game::findBestMove(const Board &board, Board::Cell aiPlayer, int depth) const {
    std::vector<Move> moves = generateMoves(board, aiPlayer);

    if (moves.empty()) {
        return Move{-1, -1, -1, -1};
    }

    int bestVal = -INF;
    Move bestMove = moves[0];

    int alpha = -INF;
    int beta  = +INF;

    for (const Move &m : moves) {
        Board nextBoard = applyMove(board, m, aiPlayer);
        int val = minimax(nextBoard, depth - 1, false, aiPlayer, alpha, beta);

        if (val > bestVal) {
            bestVal = val;
            bestMove = m;
        } else if (val == bestVal) {
            Board candBoard = nextBoard;
            Board bestBoard = applyMove(board, bestMove, aiPlayer);

            int q1 = removalQuality(candBoard, m.removeRow, m.removeCol, aiPlayer);
            int q2 = removalQuality(bestBoard, bestMove.removeRow, bestMove.removeCol, aiPlayer);

            if (q1 > q2) {
                bestMove = m;
            }
        }

        if (val > alpha) alpha = val;
        if (beta <= alpha) break;
    }

    return bestMove;
}
