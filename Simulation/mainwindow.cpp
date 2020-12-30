#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "binarymapwindow.h"
#include "solverwindow.h"
#include "resultswindow.h"
#include <QDir>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->userInterface = UserInterface();

    int w = ui->label_agh->width();
    int h = ui->label_agh->height();

    int w2 = ui->label_ggios->width();
    int h2 = ui->label_ggios->height();

    QDir dir;
    QString qPathDir = dir.currentPath();
    QString qPathAgh = qPathDir + "/../Simulation/agh.png";
    QString qPathGgios = qPathDir + "/../Simulation/ggios.png";
    QPixmap agh(qPathAgh);
    QPixmap ggios(qPathGgios);

    ui->label_agh->setPixmap(agh.scaled(w, h, Qt::KeepAspectRatio));
    ui->label_ggios->setPixmap(ggios.scaled(w2, h2, Qt::KeepAspectRatio));
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
    SolverWindow solverWindow(nullptr, this);
    solverWindow.setModal(true);
    solverWindow.exec();
}

void MainWindow::on_actionResults_triggered()
{
    ResultsWindow resultsWindow(nullptr, this);
    resultsWindow.setModal(true);
    resultsWindow.exec();
}

void MainWindow::on_pushButton_binaryMap_clicked()
{
    this->on_actionMap_of_the_porous_media_triggered();
}

void MainWindow::on_pushButton_solver_clicked()
{
    this->on_actionSimulate_triggered();
}

void MainWindow::on_pushButton_results_clicked()
{
    this->on_actionResults_triggered();
}
