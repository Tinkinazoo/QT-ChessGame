#include "chessai.h"
#include <QDebug>
#include <algorithm>
#include <limits>

ChessAI::ChessAI(ChessBoard *board, QObject *parent)
    : QObject(parent), chessBoard(board)
{
}

ChessMove ChessAI::findBestMove(int depth)
{
    ChessBoardData currentData = chessBoard->getBoardData();
    QVector<ChessMove> allMoves = getAllPossibleMoves(&currentData, BLACK);

    if (allMoves.isEmpty()) {
        return ChessMove();
    }

    int bestScore = std::numeric_limits<int>::min();
    ChessMove bestMove;

    for (ChessMove &move : allMoves) {
        ChessBoardData newData = currentData;
        ChessBoard tempBoard;
        tempBoard.setBoardData(newData);
        tempBoard.makeMoveForAI(move.fromRow, move.fromCol, move.toRow, move.toCol);

        int score = minimax(tempBoard.getBoardData(), depth - 1,
                          std::numeric_limits<int>::min(),
                          std::numeric_limits<int>::max(), false);

        move.score = score;

        if (score > bestScore) {
            bestScore = score;
            bestMove = move;
        }
    }

    return bestMove;
}

int ChessAI::minimax(ChessBoardData boardData, int depth, int alpha, int beta, bool maximizingPlayer)
{
    ChessBoard tempBoard;
    tempBoard.setBoardData(boardData);

    if (depth == 0 || tempBoard.getGameState() != IN_PROGRESS) {
        return evaluateBoard(tempBoard.getBoardData());
    }

    PieceColor currentColor = maximizingPlayer ? BLACK : WHITE;
    QVector<ChessMove> moves = getAllPossibleMoves(&boardData, currentColor);

    if (maximizingPlayer) {
        int maxEval = std::numeric_limits<int>::min();
        for (const ChessMove &move : moves) {
            ChessBoardData newData = boardData;
            ChessBoard newBoard;
            newBoard.setBoardData(newData);
            newBoard.makeMoveForAI(move.fromRow, move.fromCol, move.toRow, move.toCol);

            int eval = minimax(newBoard.getBoardData(), depth - 1, alpha, beta, false);
            maxEval = std::max(maxEval, eval);
            alpha = std::max(alpha, eval);

            if (beta <= alpha) {
                break;
            }
        }
        return maxEval;
    } else {
        int minEval = std::numeric_limits<int>::max();
        for (const ChessMove &move : moves) {
            ChessBoardData newData = boardData;
            ChessBoard newBoard;
            newBoard.setBoardData(newData);
            newBoard.makeMoveForAI(move.fromRow, move.fromCol, move.toRow, move.toCol);

            int eval = minimax(newBoard.getBoardData(), depth - 1, alpha, beta, true);
            minEval = std::min(minEval, eval);
            beta = std::min(beta, eval);

            if (beta <= alpha) {
                break;
            }
        }
        return minEval;
    }
}

int ChessAI::evaluateBoard(const ChessBoardData &boardData)
{
    int score = 0;

    // Веса фигур
    const int pawnValue = 100;
    const int knightValue = 320;
    const int bishopValue = 330;
    const int rookValue = 500;
    const int queenValue = 900;
    const int kingValue = 20000;

    // Подсчет материала
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            ChessPiece piece = boardData.board[row][col];
            int value = 0;

            switch (piece.type) {
            case PAWN:   value = pawnValue; break;
            case KNIGHT: value = knightValue; break;
            case BISHOP: value = bishopValue; break;
            case ROOK:   value = rookValue; break;
            case QUEEN:  value = queenValue; break;
            case KING:   value = kingValue; break;
            default:     value = 0;
            }

            if (piece.color == WHITE) {
                score -= value;
            } else if (piece.color == BLACK) {
                score += value;
            }
        }
    }

    // Бонусы за позицию
    int centerBonus = 0;
    for (int row = 3; row <= 4; ++row) {
        for (int col = 3; col <= 4; ++col) {
            ChessPiece piece = boardData.board[row][col];
            if (piece.type == PAWN) {
                if (piece.color == BLACK) centerBonus += 10;
                else centerBonus -= 10;
            }
        }
    }
    score += centerBonus;

    // Проверяем состояние игры
    ChessBoard tempBoard;
    tempBoard.setBoardData(boardData);

    if (tempBoard.getGameState() == BLACK_WIN) {
        score += 10000;
    } else if (tempBoard.getGameState() == WHITE_WIN) {
        score -= 10000;
    }

    return score;
}

QVector<ChessMove> ChessAI::getAllPossibleMoves(const ChessBoardData *boardData, PieceColor color)
{
    QVector<ChessMove> moves;

    ChessBoard tempBoard;
    tempBoard.setBoardData(*boardData);

    for (int fromRow = 0; fromRow < 8; ++fromRow) {
        for (int fromCol = 0; fromCol < 8; ++fromCol) {
            ChessPiece piece = boardData->board[fromRow][fromCol];
            if (piece.color == color) {
                QVector<QPair<int, int>> validMoves = tempBoard.getValidMoves(fromRow, fromCol);
                for (const auto &move : validMoves) {
                    moves.append(ChessMove(fromRow, fromCol, move.first, move.second));
                }
            }
        }
    }

    return moves;
}
