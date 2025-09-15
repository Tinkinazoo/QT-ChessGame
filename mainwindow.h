#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QPushButton>
#include <QLabel>
#include "chessboard.h"
#include "chessai.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void newGame();
    void aiMove();
    void updateStatus();

private:
    QGraphicsScene *scene;
    QGraphicsView *view;
    ChessBoard *chessBoard;
    ChessAI *chessAI;
    QPushButton *newGameButton;
    QPushButton *aiMoveButton;
    QLabel *statusLabel;
};

#endif // MAINWINDOW_H
