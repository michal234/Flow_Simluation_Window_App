#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "userinterface.h"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    UserInterface userInterface;

private slots:
    void on_actionMap_of_the_porous_media_triggered();

    void on_actionSimulate_triggered();

    void on_actionResults_triggered();

    void on_pushButton_binaryMap_clicked();

    void on_pushButton_solver_clicked();

    void on_pushButton_results_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
