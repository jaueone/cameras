#include "timeselect.h"
#include "ui_timeselect.h"
#include <QDebug>

timeSelect::timeSelect(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::timeSelect)
{
    ui->setupUi(this);

}

timeSelect::~timeSelect()
{
    delete ui;
}

void timeSelect::on_pushButton_released()
{
    this->close();
}

void timeSelect::on_pushButton_2_released()
{
    this->start = this->ui->timeEdit->time();
    this->end = this->ui->timeEdit_2->time();
    emit tell_window_time_quantum(start,end);
    this->close();
}

void timeSelect::on_pushButton_3_released()
{
    QTime time(0,0,0,0);
    this->ui->timeEdit->setTime(time);
    this->ui->timeEdit_2->setTime(QTime::currentTime());
}
