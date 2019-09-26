#include "my_control.h"
#include <QSqlError>
QSqlDatabase *DB::getInterface(){
    if (QSqlDatabase::contains("qt_sql_default_connection"))
    {
        __database__ = QSqlDatabase::database("qt_sql_default_connection");
        return &__database__;
    }
    else {
        __database__ = QSqlDatabase::addDatabase("QSQLITE");
        __database__.setDatabaseName("./database/MyDataBase.db");
        __database__.setUserName("XingYeZhiXia");
        __database__.setPassword("123456");
        return &__database__;
    }
}



bool RemoteDB::mac_is_legal = false;

QJsonObject RemoteDB::CheckMacIPAndDownload(){
    QJsonObject obj;
    QString name;
    {
        name = "mysql";
        QString mac = get_mac();
        QSqlDatabase database = QSqlDatabase::addDatabase("QMYSQL",name);
        database.setHostName("nas.indooruwb.xyz");
        database.setPort(3307);
        database.setDatabaseName("zlkj");
        database.setUserName("root");
        database.setPassword("zjzl123456");
        if(!database.open()){
//            DMessageBox::box(QMessageBox::Critical,"登录错误","无法连接到远程服务器\n请联系烽火科技1");
//            qDebug() << 12;
            return obj;
        }
        QSqlQuery query(database);
        if(!query.exec(QString("SELECT * FROM device_table where mac_addr = \"%1\"").arg(mac))) {
//            DMessageBox::box(QMessageBox::Critical,"登录错误","无法连接到远程服务器\n请联系烽火科技2");
            return obj;
        }
        int i = 0;
        QString mac_addr,latest_version,changelog, remarks;
        int is_legal = 0, download_number = 0, mandatory = 0 ;
        while (query.next()) {
            i += 1;
            QSqlRecord rec = query.record();
            mac_addr = rec.value("mac_addr").toString();
            latest_version = rec.value("latest_version").toString();
            changelog = rec.value("changelog").toString();
            remarks = rec.value("remarks").toString();
            is_legal = rec.value("is_legal").toInt();
            download_number = rec.value("download_number").toInt();
            mandatory = rec.value("mandatory").toInt();
        }

        if (i == 0){
            if(!query.exec(QString("SELECT * FROM device_table where id = 1").arg(mac))) {
//                DMessageBox::box(QMessageBox::Critical,"登录错误","无法连接到远程服务器\n请联系烽火科技3");
                return obj;
            }

            QString mac_addr1,latest_version1,changelog1, remarks1;
            int is_legal1 = 0, download_number1 = 0, mandatory1 = 0 ;
            while (query.next()) {
                QSqlRecord rec = query.record();
                mac_addr1 = rec.value("mac_addr").toString();
                latest_version1 = rec.value("latest_version").toString();
                changelog1 = rec.value("changelog").toString();
                remarks1 = rec.value("remarks").toString();
                is_legal1 = rec.value("is_legal").toInt();
                download_number1 = rec.value("download_number").toInt();
                mandatory1 = rec.value("mandatory").toInt();
            }

            if (!query.exec(QString("INSERT INTO device_table(mac_addr, is_legal, latest_version, download_number, changelog, mandatory, create_time, remarks)"
                                    "VALUES (\"%1\", \"%2\", \"%3\", \"%4\", \"%5\", \"%6\", \"%7\", \"%8\");").arg(mac).arg(is_legal1).arg(latest_version1)
                                .arg(download_number1).arg(changelog1).arg(mandatory1).arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")).arg(remarks1))){
//                DMessageBox::box(QMessageBox::Critical,"登录错误","无法连接到远程服务器\n请联系烽火科技4");
                return obj;
            }
            else {
                download_number = download_number1;
                is_legal = is_legal1;
                changelog = changelog1;
                latest_version = latest_version1;
                mandatory = mandatory1;
                remarks = remarks1;
            }
        }
        if (!is_legal){
            DMessageBox::box(QMessageBox::Critical,"登录错误",QString("\350\275\257\344\273\266\346\234\252\346\263\250\345\206\214,%1\n"   //显示当前的mac地址 提示用户联系管理员提供mac地址进行激活
                                                                        "\350\201\224\347\263\273\347\256\241\347\220\206\345\221\230\346\217\220\344\276\233mac\345\234\260\345\235\200\350\277\233\350\241\214\346\277\200\346\264\273").arg(mac));
            mac_is_legal = false;
            return obj;
        }
        if (!query.exec(QString("SELECT * FROM Download_Url where id = \"%1\"").arg(download_number))){
            DMessageBox::box(QMessageBox::Critical,"登录错误","查询更新失败");
            return obj;
        }
        QString download_url;
        while (query.next()) {
            QSqlRecord rec = query.record();
            download_url = rec.value("URL").toString();
        }
        mac_is_legal = is_legal;
        obj["mac_addr"] = mac;
        obj["remarks"] = remarks;
        obj["mandatory"] = mandatory;
        obj["changelog"] = changelog;
        obj["latest_version"] = latest_version;
        obj["download_url"] = download_url;
        database.close();
    }
    QSqlDatabase::removeDatabase(name);
    return obj;
}
