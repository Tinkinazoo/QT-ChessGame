#ifndef CHESSAI_H
#define CHESSAI_H

#include "chessboard.h"
#include <QObject>
#include <QVector>

struct ChessMove {
    int fromRow;
    int fromCol;
    int toRow;
    int toCol;
    int score;

    ChessMove() : fromRow(-1), fromCol(-1), toRow(-1), toCol(-1), score(0) {}
    ChessMove(int fr, int fc, int tr, int tc, int s = 0)
        : fromRow(fr), fromCol(fc), toRow(tr), toCol(tc), score(s) {}
};

class ChessAI : public QObject
{
    Q_OBJECT

public:
    ChessAI(ChessBoard *board, QObject *parent = nullptr);
    ChessMove findBestMove(int depth);
    QVector<ChessMove> getAllPossibleMoves(const ChessBoardData *boardData, PieceColor color);
private:
    ChessBoard *chessBoard;

    int minimax(ChessBoardData boardData, int depth, int alpha, int beta, bool maximizingPlayer);
    int evaluateBoard(const ChessBoardData &boardData);

};

#endif // CHESSAI_H
