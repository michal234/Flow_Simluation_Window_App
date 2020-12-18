#include "resultswindow.h"
#include "ui_resultswindow.h"

ResultsWindow::ResultsWindow(QWidget *parent, MainWindow* mw) :
    QDialog(parent),
    ui(new Ui::ResultsWindow)
{
    ui->setupUi(this);
    this->mw = mw;

    ui->textEdit_info->setReadOnly(true);
    ui->label_pic->clear();
    ui->label_scale->clear();

    if(!this->mw->userInterface.SimulationIsCalculated())
    {
        ui->textEdit_info->append(">>You must run calculation before you can show results");
        ui->comboBox_variable->setDisabled(true);
        ui->pushButton_showContour->setDisabled(true);
    }
    else
    {
        ui->textEdit_info->append(">>Results were loaded correctly");
        ui->comboBox_variable->setEnabled(true);
        ui->pushButton_showContour->setEnabled(true);
    }
}

ResultsWindow::~ResultsWindow()
{
    delete ui;
}

void ResultsWindow::on_pushButton_showContour_clicked()
{
    QString qVariable = ui->comboBox_variable->currentText();
    int variable = 0;
    if( qVariable == "velocity" )
        variable = 1;
    if( qVariable == "presure" )
        variable = 2;

    int w = ui->label_pic->width();
    int h = ui->label_pic->height();

    int ws = ui->label_scale->width();
    int hs = ui->label_scale->height();

    switch(variable)
    {
    case 1:
        ui->label_pic->setPixmap(QPixmap::fromImage(this->mw->userInterface.ShowResults()).scaled(w, h, Qt::KeepAspectRatio));
        ui->label_scale->setPixmap(QPixmap::fromImage(this->mw->userInterface.ShowScale()).scaled(ws, hs, Qt::KeepAspectRatio));
        double min = this->mw->userInterface.GetMinValue(variable);
        double max = this->mw->userInterface.GetMaxValue(variable);
        QString qMin = QString::number(min, 'g', 2);
        QString qMax = QString::number(max, 'g', 2);
        ui->label_min->setText(qMin);
        ui->label_max->setText(qMax);
        break;

    }
}
