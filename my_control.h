#ifndef MY_CONTROL_H
#define MY_CONTROL_H
#include <QLabel>
#include <QComboBox>
#include <QDebug>
#include <QPixmap>
#include <QPainterPath>
#include <QPainter>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QThread>
#include <QTime>
#include <QJsonObject>
#include <QMessageBox>
#include <QPushButton>
#include <QCoreApplication>
#include <QtNetwork/QNetworkInterface>

#define Version_ 2.0

static QSqlDatabase __database__;

class Label : public QLabel
{
    Q_OBJECT
public:
    Label(QWidget *parent=nullptr):QLabel(parent) {
        this->setStyleSheet("QLabel{color:white;font-family:Microsoft YaHei; font-size:25px; background-color:rgb(132,151,176);}");
        this->setMinimumHeight(30);
        this->setMaximumHeight(30);
    }
    virtual ~Label() {}
    static QPixmap draw_ellipse(const QString &url){
        QPixmap pixmapa(url);
        QPixmap pixmap(130,130);
        pixmap.fill(Qt::transparent);
        QPainter painter(&pixmap);
        painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
        QPainterPath path;
        path.addEllipse(0, 0, 130, 130);
        painter.setClipPath(path);
        painter.drawPixmap(0, 0, 130, 130, pixmapa);
        return pixmap;
    }
};

class DMessageBox
{
public:
    static void box(QMessageBox::Icon icon,QString info,QString content){
        QMessageBox messageBox;
        messageBox.setWindowTitle(info);
        messageBox.setIcon(icon);
        messageBox.setText(content);
        QPushButton button("确定");
        messageBox.addButton(&button, QMessageBox::YesRole);
        messageBox.exec();
    }

private:
    DMessageBox() {}
};

class ComboBox: public QComboBox
{
    Q_OBJECT
public:
    ComboBox(QWidget *parent = nullptr):QComboBox(parent) {
}
    void mousePressEvent(QMouseEvent *e){
        QComboBox::mousePressEvent(e);
        emit clicked();
    }
signals:
    void clicked();
};

class DB
{
public:
    static QSqlDatabase * getInterface();

protected:
    DB() {}
    DB(const DB& other){}
    ~DB(){}
};

static QString get_mac()
{
    QList<QNetworkInterface> NetList; //网卡链表
    int NetCount = 0; //网卡个数
    int Neti = 0;
    QNetworkInterface thisNet; //所要使用的网卡
    NetList = QNetworkInterface::allInterfaces();//获取所有网卡信息
    NetCount = NetList.count(); //统计网卡个数
    for(Neti = 0;Neti < NetCount; Neti++){ //遍历所有网卡
        if(NetList[Neti].isValid()){ //判断该网卡是否是合法
            thisNet = NetList[Neti]; //将该网卡置为当前网卡
            break;
        }
    }
    return ( thisNet.hardwareAddress() ); //获取该网卡的MAC
}

class RemoteDB
{
public:
    static bool mac_is_legal;
    static QJsonObject CheckMacIPAndDownload();

protected:
    RemoteDB() {}
    RemoteDB(const RemoteDB& other){}
    ~RemoteDB(){}
};

class Sleep
{
public:
    Sleep() {}
    static void sleep(unsigned int msec)
    {
        QTime dieTime = QTime::currentTime().addMSecs(msec);
        while( QTime::currentTime() < dieTime )
            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }
};


#endif
