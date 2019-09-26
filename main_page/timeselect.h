#ifndef TIMESELECT_H
#define TIMESELECT_H

#include <QWidget>
#include <QTime>

namespace Ui {
class timeSelect;
}

class timeSelect : public QWidget
{
    Q_OBJECT

public:
    explicit timeSelect(QWidget *parent = nullptr);
    ~timeSelect();

private slots:
    void on_pushButton_2_released();

    void on_pushButton_released();

    void on_pushButton_3_released();

signals:
    void tell_window_time_quantum(const QTime &start,const QTime &end);

private:
    Ui::timeSelect *ui;
    QTime start;
    QTime end;
};

#endif // TIMESELECT_H
