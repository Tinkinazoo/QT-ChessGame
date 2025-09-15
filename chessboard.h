#ifndef CHESSBOARD_H
#define CHESSBOARD_H

#include <QGraphicsItem>
#include <QObject>
#include <QVector>

enum PieceType {
    NO_PIECE,
    PAWN,
    KNIGHT,
    BISHOP,
    ROOK,
    QUEEN,
    KING
};

enum PieceColor {
    WHITE,
    BLACK,
    NO_COLOR
};

enum GameState {
    IN_PROGRESS,
    WHITE_WIN,
    BLACK_WIN,
    STALEMATE,
    DRAW
};

struct ChessPiece {
    PieceType type;
    PieceColor color;
    bool hasMoved;

    ChessPiece() : type(NO_PIECE), color(NO_COLOR), hasMoved(false) {}
    ChessPiece(PieceType t, PieceColor c) : type(t), color(c), hasMoved(false) {}
};

// Класс только для данных, без QObject
class ChessBoardData {
public:
    ChessPiece board[8][8];
    PieceColor currentPlayer;
    GameState gameState;

    ChessBoardData();
    void reset();
    void copyFrom(const ChessBoardData &other);
};

class ChessBoard : public QObject, public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)

public:
    ChessBoard(QObject *parent = nullptr);
    ~ChessBoard();

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    void resetBoard();
    bool makeMove(int fromRow, int fromCol, int toRow, int toCol);
    bool isValidMove(int fromRow, int fromCol, int toRow, int toCol) const;
    QVector<QPair<int, int>> getValidMoves(int row, int col) const;
    bool isInCheck(PieceColor color) const;
    bool isCheckmate(PieceColor color) const;
    bool isStalemate(PieceColor color) const;

    PieceColor getCurrentPlayer() const { return data.currentPlayer; }
    GameState getGameState() const { return data.gameState; }
    ChessPiece getPiece(int row, int col) const { return data.board[row][col]; }

    // Для ИИ
    ChessBoardData getBoardData() const { return data; }
    void setBoardData(const ChessBoardData &newData);
    void makeMoveForAI(int fromRow, int fromCol, int toRow, int toCol);

signals:
    void gameStateChanged();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

private:
    ChessBoardData data;
    int selectedRow, selectedCol;
    bool pieceSelected;

    void drawPiece(QPainter *painter, PieceType type, PieceColor color, const QRectF &rect);
    bool isValidPawnMove(int fromRow, int fromCol, int toRow, int toCol) const;
    bool isValidKnightMove(int fromRow, int fromCol, int toRow, int toCol) const;
    bool isValidBishopMove(int fromRow, int fromCol, int toRow, int toCol) const;
    bool isValidRookMove(int fromRow, int fromCol, int toRow, int toCol) const;
    bool isValidQueenMove(int fromRow, int fromCol, int toRow, int toCol) const;
    bool isValidKingMove(int fromRow, int fromCol, int toRow, int toCol) const;
    bool isPathClear(int fromRow, int fromCol, int toRow, int toCol) const;
    void switchPlayer();
    void updateGameState();
};

#endif // CHESSBOARD_H
