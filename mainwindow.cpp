#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Шахматы с ИИ");
    resize(800, 600);

    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    // Создаем сцену и вид
    scene = new QGraphicsScene(this);
    view = new QGraphicsView(scene);
    view->setFixedSize(520, 520);
    mainLayout->addWidget(view);

    // Создаем шахматную доску
    chessBoard = new ChessBoard();
    scene->addItem(chessBoard);

    // Создаем ИИ
    chessAI = new ChessAI(chessBoard);

    // Панель управления
    QHBoxLayout *controlLayout = new QHBoxLayout();

    newGameButton = new QPushButton("Новая игра");
    aiMoveButton = new QPushButton("Ход ИИ");
    statusLabel = new QLabel("Ход белых");

    controlLayout->addWidget(newGameButton);
    controlLayout->addWidget(aiMoveButton);
    controlLayout->addWidget(statusLabel);
    controlLayout->addStretch();

    mainLayout->addLayout(controlLayout);

    // Подключаем сигналы
    connect(newGameButton, &QPushButton::clicked, this, &MainWindow::newGame);
    connect(aiMoveButton, &QPushButton::clicked, this, &MainWindow::aiMove);
    connect(chessBoard, &ChessBoard::gameStateChanged, this, &MainWindow::updateStatus);

    newGame();
}

MainWindow::~MainWindow()
{
}

void MainWindow::newGame()
{
    chessBoard->resetBoard();
    updateStatus();
}

void MainWindow::aiMove()
{
    if (chessBoard->getCurrentPlayer() == BLACK) {
        ChessMove move = chessAI->findBestMove(3); // Глубина поиска 3
        chessBoard->makeMove(move.fromRow, move.fromCol, move.toRow, move.toCol);
    }
}

void MainWindow::updateStatus()
{
    QString status;
    switch (chessBoard->getGameState()) {
    case IN_PROGRESS:
        status = chessBoard->getCurrentPlayer() == WHITE ? "Ход белых" : "Ход черных";
        break;
    case WHITE_WIN:
        status = "Белые выиграли!";
        break;
    case BLACK_WIN:
        status = "Черные выиграли!";
        break;
    case STALEMATE:
        status = "Пат!";
        break;
    case DRAW:
        status = "Ничья!";
        break;
    }
    statusLabel->setText(status);
}
