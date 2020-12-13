#include "binarymapwindow.h"
#include "ui_binarymapwindow.h"

BinaryMapWindow::BinaryMapWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BinaryMapWindow)
{
    ui->setupUi(this);
}

BinaryMapWindow::~BinaryMapWindow()
{
    delete ui;
}
