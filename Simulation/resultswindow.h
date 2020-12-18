#ifndef RESULTSWINDOW_H
#define RESULTSWINDOW_H

#include <QDialog>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"

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
