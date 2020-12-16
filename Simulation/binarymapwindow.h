#ifndef BINARYMAPWINDOW_H
#define BINARYMAPWINDOW_H

#include <QDialog>
#include "mainwindow.h"

namespace Ui {
class BinaryMapWindow;
}

class BinaryMapWindow : public QDialog
{
    Q_OBJECT

public:
    explicit BinaryMapWindow(QWidget *parent = nullptr, MainWindow* mw = nullptr);
    ~BinaryMapWindow();

private slots:
    void on_pushButton_clicked();

private:
    Ui::BinaryMapWindow *ui;
    MainWindow* mw;
};

#endif // BINARYMAPWINDOW_H
