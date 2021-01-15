#ifndef RESULTSWINDOW_H
#define RESULTSWINDOW_H

#include <QDialog>
#include <QPixmap>
#include <QDir>
#include "mainwindow.h"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"

namespace Ui {
class ResultsWindow;
}

class ResultsWindow : public QDialog
{
    Q_OBJECT

public:
    explicit ResultsWindow(QWidget *parent = nullptr, MainWindow* mw = nullptr);
    ~ResultsWindow();

private slots:
    void on_pushButton_showContour_clicked();


private:
    Ui::ResultsWindow *ui;
    MainWindow* mw;
};

#endif // RESULTSWINDOW_H
