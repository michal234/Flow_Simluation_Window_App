#include "binarymapwindow.h"
#include "ui_binarymapwindow.h"
#include <QBitmap>

BinaryMapWindow::BinaryMapWindow(QWidget *parent, MainWindow* mw) :
    QDialog(parent),
    ui(new Ui::BinaryMapWindow)
{
    ui->setupUi(this);
    this->mw = mw;
}

BinaryMapWindow::~BinaryMapWindow()
{
    delete ui;
}

void BinaryMapWindow::on_pushButton_clicked()
{
    QString qPath = ui->textEdit->toPlainText();
    const string mPath = qPath.toUtf8().constData();

    QBitmap map(qPath);
    if( map.isNull() )
    {
        ui->label_info->setText("Cannot open selected picture. Check if the path is correct");
        ui->label_pic->clear();
    }
    else
    {
        ui->label_info->setText("Binary map was insert correctly");
        mw->userInterface.SetBinaryMap(qPath, mPath);
        int w = ui->label_pic->width();
        int h = ui->label_pic->height();
        ui->label_pic->setPixmap(map.scaled(w, h, Qt::KeepAspectRatio));
    }
}
