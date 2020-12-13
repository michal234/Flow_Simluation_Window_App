#include "solverwindow.h"
#include "ui_solverwindow.h"

SolverWindow::SolverWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SolverWindow)
{
    ui->setupUi(this);
}

SolverWindow::~SolverWindow()
{
    delete ui;
}
