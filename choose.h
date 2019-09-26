#ifndef CHOOSE_H
#define CHOOSE_H

#include "login.h"
#include <QWidget>

namespace Ui {
class Choose;
}

class Choose : public QWidget
{
    Q_OBJECT

public:
    explicit Choose(QWidget *parent = nullptr);
    ~Choose();

    void setMen(const Meninfo &info);

private slots:
    void on_pushButton_2_released();

    void on_pushButton_released();

    void on_pushButton_4_released();

    void on_pushButton_3_released();

    void on_pushButton_5_released();

    void on_pushButton_6_released();

signals:
    void tell_window_step_page(int page);
    void tell_window_check_self();
    void tell_window_get_picture();
private:
    Ui::Choose *ui;
};

#endif // CHOOSE_H
