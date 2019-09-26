#include "mainwindow.h"
#include "login.h"
#include <QApplication>
#include <QSqlDatabase>
#include <QSqlError>
#include <QTextCodec>


int main(int argc, char *argv[])
{
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GBK"));
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    RemoteDB::CheckMacIPAndDownload();

    return a.exec();
}
