#ifndef LOGIN_H
#define LOGIN_H

#include <QFrame>
#include <QSqlDatabase>
#include <QSqlError>
#include <QPixmap>

namespace Ui {
class Login;
}

class Meninfo
{
public:
    Meninfo() {}
    QString number = "";
    QString name = "";
    QString picture = "";
    QString isengineer = "";
};

class Login : public QFrame
{
    Q_OBJECT

public:
    explicit Login(QWidget *parent = nullptr);
    ~Login();
    Meninfo search_meninfo(const QString &number);
    void clear();

private slots:
    void on_pushButton_released();
    void on_lineEdit_textChanged(const QString &arg1);


    void on_pushButton_2_released();

signals:
    void tell_window_step_page(int page);
    void tell_window_men_login(Meninfo info);
private:
    Ui::Login *ui;
};

#endif // LOGIN_H
