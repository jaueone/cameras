#include "choose.h"
#include "my_control.h"
#include "ui_choose.h"
#include <QPixmap>
#include <QFileInfo>
#include <QProcess>

static bool compare (const QString& x, const QString& y)
{
    QStringList versionsX = x.split (".");
    QStringList versionsY = y.split (".");

    int count = qMin (versionsX.count(), versionsY.count());

    for (int i = 0; i < count; ++i) {
        int a = QString (versionsX.at (i)).toInt();
        int b = QString (versionsY.at (i)).toInt();

        if (a > b)
            return true;

        else if (b > a)
            return false;
    }

    return versionsY.count() < versionsX.count();
}

Choose::Choose(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Choose)
{
    ui->setupUi(this);
    this->ui->pushButton_5->hide();
    this->ui->label_5->setText(QString("%1").arg(Version_));
}

Choose::~Choose()
{
    delete ui;
}

void Choose::setMen(const Meninfo &info)
{
    this->ui->label_2->setText(info.name);
    QPixmap pixmap = Label::draw_ellipse(QString("./picture/%1").arg(info.picture));
    this->ui->label->setPixmap(pixmap);

    if (info.isengineer == "yes")
    {
        this->ui->pushButton_3->show();
    }
    else if (info.isengineer == "no") {
        this->ui->pushButton_3->hide();
    }
}

void Choose::on_pushButton_2_released()
{
    emit tell_window_check_self();
}

void Choose::on_pushButton_released()
{
    emit tell_window_step_page(4);
}

void Choose::on_pushButton_4_released()
{
    emit tell_window_step_page(0);
}

void Choose::on_pushButton_3_released()
{
    emit tell_window_step_page(2);
}

void Choose::on_pushButton_5_released()
{
    emit tell_window_get_picture();
}


void Choose::on_pushButton_6_released()
{
    QJsonObject obj = RemoteDB::CheckMacIPAndDownload();
    QString last_version,current_version;
    if (!obj.isEmpty())
        last_version = obj["latest_version"].toString();
    current_version = QString("%1").arg(Version_);
    qDebug() << last_version << current_version;
    QFileInfo file("./Update.exe");
    if (!file.exists()){
        QMessageBox messageBox;
        messageBox.setWindowTitle(QString("错误"));
        messageBox.setIcon(QMessageBox::Critical);
        messageBox.setText(QString("安装程序不存在，请联系管理员"));
        QPushButton button(QString("确定"));
        messageBox.addButton(&button, QMessageBox::YesRole);
        messageBox.exec();
        return;
    }
    if (compare(last_version,current_version)){
        QMessageBox messageBox;
        messageBox.setWindowTitle(QString("信息"));
        messageBox.setIcon(QMessageBox::Information);
        messageBox.setText(QString("有新的程序版本可以更新，选择是否更新"));
        QPushButton button(QString("更新"));
        QPushButton button1(QString("取消"));
        messageBox.addButton(&button, QMessageBox::YesRole);
        messageBox.addButton(&button1, QMessageBox::RejectRole);
        connect(&button,&QPushButton::clicked,[](){
            QProcess process;
            process.startDetached("./Update.exe");
            process.waitForFinished(-1);
            exit(0);
        });
        messageBox.exec();
    }
    else{
        QMessageBox messageBox;
        messageBox.setWindowTitle(QString("信息"));
        messageBox.setIcon(QMessageBox::Information);
        messageBox.setText(QString("没有新的可用程序"));
        QPushButton button(QString("确定"));
        messageBox.addButton(&button, QMessageBox::YesRole);
        messageBox.exec();
    }
}
