#ifndef RESULTSWINDOW_H
#define RESULTSWINDOW_H

#include <QDialog>

namespace Ui {
class ResultsWindow;
}

class ResultsWindow : public QDialog
{
    Q_OBJECT

public:
    explicit ResultsWindow(QWidget *parent = nullptr);
    ~ResultsWindow();

private:
    Ui::ResultsWindow *ui;
};

#endif // RESULTSWINDOW_H
