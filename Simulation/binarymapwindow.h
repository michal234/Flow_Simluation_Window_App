#ifndef BINARYMAPWINDOW_H
#define BINARYMAPWINDOW_H

#include <QDialog>

namespace Ui {
class BinaryMapWindow;
}

class BinaryMapWindow : public QDialog
{
    Q_OBJECT

public:
    explicit BinaryMapWindow(QWidget *parent = nullptr);
    ~BinaryMapWindow();

private:
    Ui::BinaryMapWindow *ui;
};

#endif // BINARYMAPWINDOW_H
