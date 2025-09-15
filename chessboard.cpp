#include "chessboard.h"
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QDebug>

// Реализация ChessBoardData
ChessBoardData::ChessBoardData() : currentPlayer(WHITE), gameState(IN_PROGRESS)
{
    reset();
}

void ChessBoardData::reset()
{
    // Очищаем доску
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            board[row][col] = ChessPiece();
        }
    }

    // Расставляем пешки
    for (int col = 0; col < 8; ++col) {
        board[1][col] = ChessPiece(PAWN, BLACK);
        board[6][col] = ChessPiece(PAWN, WHITE);
    }

    // Расставляем фигуры
    board[0][0] = ChessPiece(ROOK, BLACK);
    board[0][1] = ChessPiece(KNIGHT, BLACK);
    board[0][2] = ChessPiece(BISHOP, BLACK);
    board[0][3] = ChessPiece(QUEEN, BLACK);
    board[0][4] = ChessPiece(KING, BLACK);
    board[0][5] = ChessPiece(BISHOP, BLACK);
    board[0][6] = ChessPiece(KNIGHT, BLACK);
    board[0][7] = ChessPiece(ROOK, BLACK);

    board[7][0] = ChessPiece(ROOK, WHITE);
    board[7][1] = ChessPiece(KNIGHT, WHITE);
    board[7][2] = ChessPiece(BISHOP, WHITE);
    board[7][3] = ChessPiece(QUEEN, WHITE);
    board[7][4] = ChessPiece(KING, WHITE);
    board[7][5] = ChessPiece(BISHOP, WHITE);
    board[7][6] = ChessPiece(KNIGHT, WHITE);
    board[7][7] = ChessPiece(ROOK, WHITE);

    currentPlayer = WHITE;
    gameState = IN_PROGRESS;
}

void ChessBoardData::copyFrom(const ChessBoardData &other)
{
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            board[i][j] = other.board[i][j];
        }
    }
    currentPlayer = other.currentPlayer;
    gameState = other.gameState;
}

// Реализация ChessBoard
ChessBoard::ChessBoard(QObject *parent)
    : QObject(parent), selectedRow(-1), selectedCol(-1), pieceSelected(false)
{
    setFlag(QGraphicsItem::ItemIsFocusable);
    resetBoard();
}

ChessBoard::~ChessBoard()
{
}

QRectF ChessBoard::boundingRect() const
{
    return QRectF(0, 0, 520, 520);
}

void ChessBoard::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    // Рисуем доску
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            QRectF rect(col * 65, row * 65, 65, 65);

            // Чередуем цвета клеток
            if ((row + col) % 2 == 0) {
                painter->fillRect(rect, QColor(240, 217, 181)); // Светлая клетка
            } else {
                painter->fillRect(rect, QColor(181, 136, 99)); // Темная клетка
            }

            // Подсвечиваем выбранную клетку
            if (pieceSelected && selectedRow == row && selectedCol == col) {
                painter->fillRect(rect, QColor(255, 255, 0, 100));
            }

            // Рисуем фигуру
            if (data.board[row][col].type != NO_PIECE) {
                drawPiece(painter, data.board[row][col].type, data.board[row][col].color, rect);
            }
        }
    }
}

void ChessBoard::drawPiece(QPainter *painter, PieceType type, PieceColor color, const QRectF &rect)
{
    QString pieceChar;
    QColor pieceColor = (color == WHITE) ? Qt::white : Qt::black;

    switch (type) {
    case PAWN:   pieceChar = "♙"; break;
    case KNIGHT: pieceChar = "♘"; break;
    case BISHOP: pieceChar = "♗"; break;
    case ROOK:   pieceChar = "♖"; break;
    case QUEEN:  pieceChar = "♕"; break;
    case KING:   pieceChar = "♔"; break;
    default:     return;
    }

    painter->setFont(QFont("Arial", 40));
    painter->setPen(pieceColor);
    painter->drawText(rect, Qt::AlignCenter, pieceChar);
}

void ChessBoard::resetBoard()
{
    data.reset();
    pieceSelected = false;
    update();
    emit gameStateChanged();
}

void ChessBoard::setBoardData(const ChessBoardData &newData)
{
    data.copyFrom(newData);
    update();
    emit gameStateChanged();
}

bool ChessBoard::makeMove(int fromRow, int fromCol, int toRow, int toCol)
{
    if (!isValidMove(fromRow, fromCol, toRow, toCol)) {
        return false;
    }

    // Выполняем ход
    data.board[toRow][toCol] = data.board[fromRow][fromCol];
    data.board[fromRow][fromCol] = ChessPiece();
    data.board[toRow][toCol].hasMoved = true;

    // Превращение пешки
    if (data.board[toRow][toCol].type == PAWN && (toRow == 0 || toRow == 7)) {
        data.board[toRow][toCol].type = QUEEN;
    }

    switchPlayer();
    updateGameState();
    update();
    emit gameStateChanged();

    return true;
}

void ChessBoard::makeMoveForAI(int fromRow, int fromCol, int toRow, int toCol)
{
    data.board[toRow][toCol] = data.board[fromRow][fromCol];
    data.board[fromRow][fromCol] = ChessPiece();
    data.board[toRow][toCol].hasMoved = true;

    if (data.board[toRow][toCol].type == PAWN && (toRow == 0 || toRow == 7)) {
        data.board[toRow][toCol].type = QUEEN;
    }

    switchPlayer();
    updateGameState();
    update();
    emit gameStateChanged();
}

bool ChessBoard::isValidMove(int fromRow, int fromCol, int toRow, int toCol) const
{
    // Проверяем базовые условия
    if (fromRow < 0 || fromRow > 7 || fromCol < 0 || fromCol > 7 ||
        toRow < 0 || toRow > 7 || toCol < 0 || toCol > 7) {
        return false;
    }

    const ChessPiece &fromPiece = data.board[fromRow][fromCol];
    const ChessPiece &toPiece = data.board[toRow][toCol];

    // Нельзя ходить пустой клеткой
    if (fromPiece.type == NO_PIECE) {
        return false;
    }

    // Нельзя ходить не своим цветом
    if (fromPiece.color != data.currentPlayer) {
        return false;
    }

    // Нельзя бить свои фигуры
    if (toPiece.type != NO_PIECE && toPiece.color == fromPiece.color) {
        return false;
    }

    // Проверяем правила движения для каждой фигуры
    switch (fromPiece.type) {
    case PAWN:   return isValidPawnMove(fromRow, fromCol, toRow, toCol);
    case KNIGHT: return isValidKnightMove(fromRow, fromCol, toRow, toCol);
    case BISHOP: return isValidBishopMove(fromRow, fromCol, toRow, toCol);
    case ROOK:   return isValidRookMove(fromRow, fromCol, toRow, toCol);
    case QUEEN:  return isValidQueenMove(fromRow, fromCol, toRow, toCol);
    case KING:   return isValidKingMove(fromRow, fromCol, toRow, toCol);
    default:     return false;
    }
}

// Реализации функций проверки ходов для каждой фигуры
bool ChessBoard::isValidPawnMove(int fromRow, int fromCol, int toRow, int toCol) const
{
    const ChessPiece &fromPiece = data.board[fromRow][fromCol];
    const ChessPiece &toPiece = data.board[toRow][toCol];
    int direction = (fromPiece.color == WHITE) ? -1 : 1;

    // Обычный ход вперед
    if (fromCol == toCol && toPiece.type == NO_PIECE) {
        if (toRow == fromRow + direction) {
            return true;
        }
        // Двойной ход из начальной позиции
        if (!fromPiece.hasMoved && toRow == fromRow + 2 * direction &&
            data.board[fromRow + direction][fromCol].type == NO_PIECE) {
            return true;
        }
    }

    // Взятие
    if (abs(fromCol - toCol) == 1 && toRow == fromRow + direction &&
        toPiece.type != NO_PIECE && toPiece.color != fromPiece.color) {
        return true;
    }

    return false;
}

bool ChessBoard::isValidKnightMove(int fromRow, int fromCol, int toRow, int toCol) const
{
    int rowDiff = abs(fromRow - toRow);
    int colDiff = abs(fromCol - toCol);
    return (rowDiff == 2 && colDiff == 1) || (rowDiff == 1 && colDiff == 2);
}

bool ChessBoard::isValidBishopMove(int fromRow, int fromCol, int toRow, int toCol) const
{
    if (abs(fromRow - toRow) != abs(fromCol - toCol)) {
        return false;
    }
    return isPathClear(fromRow, fromCol, toRow, toCol);
}

bool ChessBoard::isValidRookMove(int fromRow, int fromCol, int toRow, int toCol) const
{
    if (fromRow != toRow && fromCol != toCol) {
        return false;
    }
    return isPathClear(fromRow, fromCol, toRow, toCol);
}

bool ChessBoard::isValidQueenMove(int fromRow, int fromCol, int toRow, int toCol) const
{
    return isValidBishopMove(fromRow, fromCol, toRow, toCol) ||
           isValidRookMove(fromRow, fromCol, toRow, toCol);
}

bool ChessBoard::isValidKingMove(int fromRow, int fromCol, int toRow, int toCol) const
{
    int rowDiff = abs(fromRow - toRow);
    int colDiff = abs(fromCol - toCol);
    return rowDiff <= 1 && colDiff <= 1;
}

bool ChessBoard::isPathClear(int fromRow, int fromCol, int toRow, int toCol) const
{
    int rowStep = (toRow > fromRow) ? 1 : (toRow < fromRow) ? -1 : 0;
    int colStep = (toCol > fromCol) ? 1 : (toCol < fromCol) ? -1 : 0;

    int currentRow = fromRow + rowStep;
    int currentCol = fromCol + colStep;

    while (currentRow != toRow || currentCol != toCol) {
        if (data.board[currentRow][currentCol].type != NO_PIECE) {
            return false;
        }
        currentRow += rowStep;
        currentCol += colStep;
    }

    return true;
}

QVector<QPair<int, int>> ChessBoard::getValidMoves(int row, int col) const
{
    QVector<QPair<int, int>> moves;
    for (int toRow = 0; toRow < 8; ++toRow) {
        for (int toCol = 0; toCol < 8; ++toCol) {
            if (isValidMove(row, col, toRow, toCol)) {
                moves.append(qMakePair(toRow, toCol));
            }
        }
    }
    return moves;
}

bool ChessBoard::isInCheck(PieceColor color) const
{
    // Находим короля
    int kingRow = -1, kingCol = -1;
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            if (data.board[row][col].type == KING && data.board[row][col].color == color) {
                kingRow = row;
                kingCol = col;
                break;
            }
        }
    }

    if (kingRow == -1) return true; // Король съеден

    // Проверяем, может ли любая фигура противника атаковать короля
    PieceColor opponentColor = (color == WHITE) ? BLACK : WHITE;
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            if (data.board[row][col].color == opponentColor) {
                // Используем текущую доску для проверки
                if (isValidMove(row, col, kingRow, kingCol)) {
                    return true;
                }
            }
        }
    }

    return false;
}

bool ChessBoard::isCheckmate(PieceColor color) const
{
    if (!isInCheck(color)) {
        return false;
    }

    // Проверяем, есть ли хоть один ход, который убирает шах
    for (int fromRow = 0; fromRow < 8; ++fromRow) {
        for (int fromCol = 0; fromCol < 8; ++fromCol) {
            if (data.board[fromRow][fromCol].color == color) {
                QVector<QPair<int, int>> moves = getValidMoves(fromRow, fromCol);
                for (const auto &move : moves) {
                    // Создаем временную копию доски
                    ChessBoardData tempData = data;
                    ChessBoard tempBoard;
                    tempBoard.setBoardData(tempData);

                    tempBoard.makeMoveForAI(fromRow, fromCol, move.first, move.second);
                    if (!tempBoard.isInCheck(color)) {
                        return false;
                    }
                }
            }
        }
    }

    return true;
}

bool ChessBoard::isStalemate(PieceColor color) const
{
    if (isInCheck(color)) {
        return false;
    }

    // Проверяем, есть ли хоть один допустимый ход
    for (int fromRow = 0; fromRow < 8; ++fromRow) {
        for (int fromCol = 0; fromCol < 8; ++fromCol) {
            if (data.board[fromRow][fromCol].color == color) {
                QVector<QPair<int, int>> moves = getValidMoves(fromRow, fromCol);
                if (!moves.isEmpty()) {
                    return false;
                }
            }
        }
    }

    return true;
}

void ChessBoard::switchPlayer()
{
    data.currentPlayer = (data.currentPlayer == WHITE) ? BLACK : WHITE;
}

void ChessBoard::updateGameState()
{
    if (isCheckmate(WHITE)) {
        data.gameState = BLACK_WIN;
    } else if (isCheckmate(BLACK)) {
        data.gameState = WHITE_WIN;
    } else if (isStalemate(data.currentPlayer)) {
        data.gameState = STALEMATE;
    } else {
        data.gameState = IN_PROGRESS;
    }
}

void ChessBoard::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (data.gameState != IN_PROGRESS) {
        return;
    }

    int col = event->pos().x() / 65;
    int row = event->pos().y() / 65;

    if (row < 0 || row > 7 || col < 0 || col > 7) {
        return;
    }

    if (!pieceSelected) {
        // Выбираем фигуру
        if (data.board[row][col].type != NO_PIECE && data.board[row][col].color == data.currentPlayer) {
            selectedRow = row;
            selectedCol = col;
            pieceSelected = true;
            update();
        }
    } else {
        // Пытаемся сделать ход
        if (makeMove(selectedRow, selectedCol, row, col)) {
            pieceSelected = false;
        } else {
            // Если ход не удался, снимаем выделение или выбираем другую фигуру
            if (data.board[row][col].type != NO_PIECE && data.board[row][col].color == data.currentPlayer) {
                selectedRow = row;
                selectedCol = col;
            } else {
                pieceSelected = false;
            }
        }
        update();
    }
}

void ChessBoard::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event);
}
