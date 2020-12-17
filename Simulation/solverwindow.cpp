#include "solverwindow.h"
#include "ui_solverwindow.h"

SolverWindow::SolverWindow(QWidget *parent, MainWindow* mw) :
    QDialog(parent),
    ui(new Ui::SolverWindow)
{
    ui->setupUi(this);
    this->mw = mw;
    ui->info_from_solver->setReadOnly(true);
    if( this->mw->userInterface.BinaryMapIsEmpty() )
    {
        ui->info_from_solver->setText(">>Binary map is empty. Insert image before launching the solver");
        //disable all features in window
        ui->label_pic->clear();
        ui->comboBox_direction->setDisabled(true);
        ui->textEdit_inlet->setDisabled(true);
        ui->textEdit_iterations->setDisabled(true);
        ui->pushButton_simulation->setDisabled(true);
    }
    else
    {
        int w = ui->label_pic->width();
        int h = ui->label_pic->height();
        ui->label_pic->setPixmap(this->mw->userInterface.GetImage().scaled(w, h, Qt::KeepAspectRatio));

        ui->info_from_solver->setText(">>Grid of automata cells initialization in progress...");
        this->mw->userInterface.SetSolver();
        ui->info_from_solver->append(">>Grid of automata cells initialization completed");

        //enable all features
        ui->comboBox_direction->setEnabled(true);
        ui->textEdit_inlet->setEnabled(true);
        ui->textEdit_iterations->setEnabled(true);
    }
}

SolverWindow::~SolverWindow()
{
    delete ui;
}

void SolverWindow::on_pushButton_simulation_clicked()
{
    QString qDirection = ui->comboBox_direction->currentText();
    QString qVelocity = ui->textEdit_inlet->toPlainText();
    QString qIterations = ui->textEdit_iterations->toPlainText();

    if( qVelocity.isEmpty() || qIterations.isEmpty() )
    {
        ui->info_from_solver->append(">>Enter inlet velocity and maximum number of iterations before you start the simulation");
        return;
    }

    double v0 = qVelocity.toDouble();
    int iter = qIterations.toInt();
    int direction = 0;
    if( qDirection == "Left" )      direction = 1;
    if( qDirection == "Right" )     direction = 2;
    if( qDirection == "Top" )       direction = 3;
    if( qDirection == "Bottom" )    direction = 4;

    ui->info_from_solver->append(">>Setting boundary conditions in progress...");
    this->mw->userInterface.SetBoundaryConditions(v0, direction);
    ui->info_from_solver->append(">>Setting boundary conditions completed");

    this->mw->userInterface.SetMaxIter(iter);

    ui->info_from_solver->append(">>Simulation in progress...");
    this->mw->userInterface.StartSimulation();
    ui->info_from_solver->append(">>Simulation completed");
}
