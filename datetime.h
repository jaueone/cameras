#ifndef DATATIME_H
#define DATATIME_H

#include <QWidget>
#include <QFrame>
#include <QLabel>
#include <QTimer>
#include <QDateTime>
#include <QString>
#include <QHBoxLayout>
#include <QDate>
#include <QTime>
#include <QDebug>

class DateTime:public QFrame
{
public:
    DateTime(QWidget *parent=nullptr):QFrame(parent){
        timer = new QTimer(this);
        datetime = new QLabel(this);
        layout = new QHBoxLayout(this);
        layout->addWidget(datetime);
        this->datetime->setStyleSheet("QLabel{color:white;font-family:Microsoft YaHei; font-size:14px;}");
        this->connect(this->timer,&QTimer::timeout,this,&DateTime::ontimeout_);
        this->ontimeout_();
        this->timer->start(60000);

    }
    void changedstyle(const QString &color){
        if (color == "white"){
            this->datetime->setStyleSheet("QLabel{color:rgb(255,255,255);font-family:Microsoft YaHei; font-size:14px;}");
            return;
        }
        else if (color == "black") {
            this->datetime->setStyleSheet("QLabel{color:black;font-family:Microsoft YaHei; font-size:14px;}");
            return;
        }
    }
public slots:
    void ontimeout_(){
        QDateTime datatime_c = QDateTime::currentDateTime();
        QString datetime_s = datatime_c.toString("yyyy-MM-dd  hh:mm");
        this->datetime->setText(datetime_s);
    }

private:
    QLabel *datetime;
    QTimer *timer;
    QHBoxLayout *layout;

};

#endif // DATATIME_H
