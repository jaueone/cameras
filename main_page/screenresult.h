#ifndef SCREENRESULT_H
#define SCREENRESULT_H

#include "login.h"
#include "main_page/timeselect.h"
#include <QWidget>
#include <QtCharts/QPieSlice>
#include <QtCharts/QPieSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QBarSeries>
#include <QtCharts/QtCharts>
#include <QtCharts/QChartView>
#include <QTimer>
#include <QTime>
#include <QDate>
#include <QStandardItemModel>
#include <QSerialPort>

namespace Ui {
class ScreenResult;
}

class ScreenResult : public QWidget
{
    Q_OBJECT

public:
    explicit ScreenResult(QWidget *parent = nullptr);
    ~ScreenResult();

    void setMen(const Meninfo &info);
    void paint_pie();
    void paint_bar();
    void update_data();
    QMap<QString,int> select_result();
    void setSerial(QSerialPort *serial){this->serial = serial;}

    void accept_update_data();


signals:
    void tell_window_step_page(int page);

protected:
    bool eventFilter(QObject *watched, QEvent *event);

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void accept_time_quantum(const QTime &start,const QTime &end);

    void on_calendarWidget_clicked(const QDate &date);

    void on_pushButton_4_clicked();

    void on_listView_clicked(const QModelIndex &index);

private:
    Ui::ScreenResult *ui;
    QChartView *chart_pie_view;
    QChartView *chart_bar_view;
    QChart *chart_pie;
    QChart *chart_bar;
    QPieSeries *series_pie;
    QBarSeries *series_bar;

    QDate check_date;
    QTime start_time;
    QTime end_time;
    QString check_name;

    Meninfo men;
    QStandardItemModel* men_model;
    QTimer *timer;
    timeSelect *time_select_form_;
    QSerialPort *serial;
};

#endif // SCREENRESULT_H
