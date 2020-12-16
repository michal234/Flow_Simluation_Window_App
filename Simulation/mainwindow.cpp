#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "binarymapwindow.h"
#include "solverwindow.h"
#include "resultswindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->userInterface = UserInterface();
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_actionMap_of_the_porous_media_triggered()
{
    BinaryMapWindow binaryMapWindow(nullptr, this);
    binaryMapWindow.setModal(true);
    binaryMapWindow.exec();
}

void MainWindow::on_actionSimulate_triggered()
{
    SolverWindow solverWindow;
    solverWindow.setModal(true);
    solverWindow.exec();
}

void MainWindow::on_actionResults_triggered()
{
    ResultsWindow resultsWindow;
    resultsWindow.setModal(true);
    resultsWindow.exec();
}
