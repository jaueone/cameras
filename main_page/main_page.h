#ifndef MAIN_PAGE_H
#define MAIN_PAGE_H

#include "screencheck.h"
#include "screenresult.h"
#include "login.h"
#include <QWidget>
#include <QPushButton>
#include <QSerialPort>

namespace Ui {
class MainPage;
}

class MainPage : public QWidget
{
    Q_OBJECT

public:
    explicit MainPage(QWidget *parent = nullptr);
    ~MainPage();

    void set_current_page(const int &page);
    ScreenCheck *ScreenCheck_(){return this->screencheck;}
    ScreenResult *ScreenResult_(){return this->screenresult;}
public slots:
    void accept_button_clicked(const bool &checked);

private:
    Ui::MainPage *ui;
    ScreenCheck *screencheck;
    ScreenResult *screenresult;
};

#endif // MAIN_PAGE_H
