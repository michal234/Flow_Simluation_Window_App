#ifndef SOLVERWINDOW_H
#define SOLVERWINDOW_H

#include <QDialog>

namespace Ui {
class SolverWindow;
}

class SolverWindow : public QDialog
{
    Q_OBJECT

public:
    explicit SolverWindow(QWidget *parent = nullptr);
    ~SolverWindow();

private:
    Ui::SolverWindow *ui;
};

#endif // SOLVERWINDOW_H
