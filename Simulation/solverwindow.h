#ifndef SOLVERWINDOW_H
#define SOLVERWINDOW_H

#include <QDialog>
#include "mainwindow.h"

namespace Ui {
class SolverWindow;
}

class SolverWindow : public QDialog
{
    Q_OBJECT

public:
    explicit SolverWindow(QWidget *parent = nullptr, MainWindow* mw = nullptr);
    ~SolverWindow();

private slots:
    void on_pushButton_simulation_clicked();

private:
    Ui::SolverWindow *ui;
    MainWindow* mw;
};

#endif // SOLVERWINDOW_H
