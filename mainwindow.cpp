#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "datetime.h"
#include "camera.h"
#include <QDebug>
#include <QDir>
#include <QCoreApplication>
#include <QFileDialog>
#include <QTime>
#include <iostream>



#define cout qDebug() << "[" <<__FILE__ <<":"<<__FUNCTION__<<":"<<__LINE__ <<"]"

quint64 getDiskFreeSpace(QString driver)
{
    LPCWSTR lpcwstrDriver=(LPCWSTR)driver.utf16();

    ULARGE_INTEGER liFreeBytesAvailable, liTotalBytes, liTotalFreeBytes;

    if( !GetDiskFreeSpaceEx( lpcwstrDriver, &liFreeBytesAvailable, &liTotalBytes, &liTotalFreeBytes) )
    {
      qDebug() << "ERROR: Call to GetDiskFreeSpaceEx() failed.";
      return 0;
    }
    return (quint64) liTotalFreeBytes.QuadPart/1024/1024/1024;
}

void removefilesindir(const QString& path)
{
    QDir dir(path);
    QFileInfoList info_list = dir.entryInfoList(QDir::Files | QDir::Hidden | QDir::NoDotAndDotDot | QDir::NoSymLinks | QDir::AllDirs);
    foreach(QFileInfo file_info, info_list)
    {
        if (file_info.isDir())
        {
            removefilesindir(file_info.absoluteFilePath());
        }
        else if (file_info.isFile())
        {
            QFile file(file_info.absoluteFilePath());
            qDebug() << "remove file  : " << file_info.absoluteFilePath();
            file.remove();
        }
    }
    QDir temp_dir;
    temp_dir.rmdir(path) ;
    qDebug() << "remove empty dir : " << path;
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    login = new Login();
    choose = new Choose();
    drivesetting = new DriveSeting();
    mainpage = new MainPage();

    this->camera = HKCamera::getInterface(0);

    screen_check = mainpage->ScreenCheck_();
    screen_result = mainpage->ScreenResult_();
    camera_label_ = screen_check->camera_label();

    this->screen_check->setDriversetting(this->drivesetting);

    this->time = new DateTime(this->ui->stackedWidget);
    this->time->setGeometry(860,10,200,50);

    this->setCentralWidget(this->ui->stackedWidget);

    this->ui->stackedWidget->removeWidget(this->ui->page);
    this->ui->stackedWidget->removeWidget(this->ui->page_2);

    this->ui->stackedWidget->addWidget(login);
    this->ui->stackedWidget->addWidget(choose);
    this->ui->stackedWidget->addWidget(drivesetting);
    this->ui->stackedWidget->addWidget(mainpage);

    this->initDB();
    this->CheckDB();
    this->sigcon();
    this->initdir();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::sigcon()
{
    this->connect(this->login,&Login::tell_window_step_page,this,&MainWindow::accept_change_page);
    this->connect(this->choose,&Choose::tell_window_step_page,this,&MainWindow::accept_change_page);
    this->connect(this->screen_check,&ScreenCheck::tell_window_step_page, this,&MainWindow::accept_change_page);
    this->connect(this->drivesetting,&DriveSeting::tell_window_step_page, this,&MainWindow::accept_change_page);

    this->connect(this->login,&Login::tell_window_men_login,this,&MainWindow::accept_men_login);
    this->connect(this->choose,&Choose::tell_window_check_self,this,&MainWindow::check_self);

    this->connect(this->screen_check,&ScreenCheck::tell_window_start_check, this,&MainWindow::accept_camera_start_check);
    this->connect(this->screen_check,&ScreenCheck::ask_serial_setting, this->drivesetting,&DriveSeting::accept_return_serial_setting);

    this->connect(this->drivesetting,&DriveSeting::tell_screencheck_setting,this->screen_check,&ScreenCheck::accept_serial_setting);
    this->connect(this->screen_check,&ScreenCheck::tell_result_update_data, this->screen_result,&ScreenResult::accept_update_data);

}

void MainWindow::initdir()
{
    if (getDiskFreeSpace("d:/")< 1)
        removefilesindir("d:/qt_photo/");
    QDir photo("./photo");
    QDir picture("./picture");
    QDir database("./database");

    QDir photo_path = QDir(photo.absolutePath());
    QDir picture_path = QDir(picture.absolutePath());
    QDir database_path = QDir(database.absolutePath());
    QDir photoS("D:/qt_photo/");
    QDir image("D:/qt_photo/"+QDate::currentDate().toString("yyyyMMdd"));
    QDir image_path(image.absolutePath());

    QDir good_image_path(QDir("D:/qt_photo/"+QDate::currentDate().toString("yyyyMMdd")+"/GOOD").absolutePath());
    QDir ok_image_path(QDir("D:/qt_photo/"+QDate::currentDate().toString("yyyyMMdd")+"/OK").absolutePath());
    QDir ng_image_path(QDir("D:/qt_photo/"+QDate::currentDate().toString("yyyyMMdd")+"/NG").absolutePath());

    QDir deal_image_path(QDir("D:/qt_photo/"+QDate::currentDate().toString("yyyyMMdd")+"/DEAL").absolutePath());

    if(!photo_path.exists()){
        if (!photo_path.mkdir("."))   qDebug() << "make dir photo failed";
    }
    if(!picture_path.exists()){
        if (!picture_path.mkdir("."))   qDebug() << "make dir picture failed";
    }
    if(!database_path.exists()){
        if (!database_path.mkdir("."))   qDebug() << "make dir database failed";
    }
    if(!photoS.exists()){
        if (!photoS.mkdir("."))   qDebug() << "make dir image failed";
    }
    if(!image_path.exists()){
        if (!image_path.mkdir("."))   qDebug() << "make dir image failed";
    }
    if(!ok_image_path.exists()){
        if (!ok_image_path.mkdir("."))   qDebug() << "make OK dir image failed";
    }
    if(!good_image_path.exists()){
        if (!good_image_path.mkdir("."))   qDebug() << "make GOOD dir image failed";
    }
    if(!ng_image_path.exists()){
        if (!ng_image_path.mkdir("."))   qDebug() << "make NG dir image failed";
    }
    if(!deal_image_path.exists()){
        if (!deal_image_path.mkdir("."))   qDebug() << "make DEAL dir image failed";
    }
}
void MainWindow::initDB()
{
    int checked = 0;
    QSqlDatabase *db = DB::getInterface();
    if (!db->open())
        return;
    QSqlQuery query(*db);
    QString temps = QString("SELECT count(*) FROM sqlite_master WHERE type=\"%1\" AND name=\"%2\";").arg("table").arg("checked_result");
    if (!query.exec(temps)){
        qDebug() << "failed";
        return;
    }
    while(query.next()) {
        QSqlRecord rec = query.record();
        checked = rec.value(0).toInt();
    }
    if (!checked){
        QString temp = "CREATE TABLE checked_result ( "
                        "id integer NOT NULL,"
                        "product_id integer,"
                        "check_result TEXT NOT NULL,"
                        "wrap_result TEXT,"
                        "scratch text,"
                        "white_point text,"
                        "black_point text,"
                        "time TEXT,"
                        "men_id TEXT NOT NULL,"
                        "men_name TEXT,"
                        "comments TEXT,"
                        "PRIMARY KEY (id));";
        if (!query.exec(temp)) qDebug() << "create table failed";
    }
    temps = QString("SELECT count(*) FROM sqlite_master WHERE type=\"%1\" AND name=\"%2\";").arg("table").arg("config_table");
    if (!query.exec(temps)){
        qDebug() << "failed";
        return;
    }
    while(query.next()) {
        QSqlRecord rec = query.record();
        checked = rec.value(0).toInt();
    }
    if (!checked){
        QString temp = "CREATE TABLE config_table ("
                        "id integer NOT NULL,"
                        "admin TEXT,"
                        "type TEXT NOT NULL,"
                        "data TEXT,"
                        "time TEXT,"
                        "PRIMARY KEY (id));";
        if (!query.exec(temp)) qDebug() << "create table failed";
        temp = QString::fromLocal8Bit("INSERT INTO config_table(id,admin,type) VALUES (\"%1\",\"%2\",\"%3\")").arg(1).arg("admin").arg("serial_config");
        if (!query.exec(temp)){ qDebug() << "exec failed1";} else qDebug() << "exec successed";
        temp = QString::fromLocal8Bit("INSERT INTO config_table(id,admin,type) VALUES (\"%1\",\"%2\",\"%3\")").arg(2).arg("admin").arg("camera1_config");
        if (!query.exec(temp)){ qDebug() << "exec failed2";} else qDebug() << "exec successed";
        temp = QString::fromLocal8Bit("INSERT INTO config_table(id,admin,type) VALUES (\"%1\",\"%2\",\"%3\")").arg(3).arg("admin").arg("camera2_config");
        if (!query.exec(temp)){ qDebug() << "exec failed3";} else qDebug() << "exec successed";
        temp = QString::fromLocal8Bit("INSERT INTO config_table(id,admin,type) VALUES (\"%1\",\"%2\",\"%3\")").arg(4).arg("admin").arg("algorithmic_parameters");
        if (!query.exec(temp)){ qDebug() << "exec failed4";} else qDebug() << "exec successed";
    }


    temps = QString("SELECT count(*) FROM sqlite_master WHERE type=\"%1\" AND name=\"%2\";").arg("table").arg("men");
    if (!query.exec(temps)){
        qDebug() << "failed";
        return;
    }

    while(query.next()) {
        QSqlRecord rec = query.record();
        checked = rec.value(0).toInt();
    }
    if (!checked){
        QString temp = "CREATE TABLE men (id INTEGER NOT NULL,"
                       "number text NOT NULL,"
                       "name text NOT NULL,"
                       "picture text,"
                       "isengineer text NOT NULL,"
                       "gender text,"
                       "effective INTEGER NOT NULL,"
                       "PRIMARY KEY (id));";
        if (!query.exec(temp)) qDebug() << "create table failed"; else qDebug() << "create men successed";
        temp = QString("INSERT INTO men (number,name,picture,isengineer,gender,effective) VALUES (\"%1\",\"%2\",\"%3\",\"%4\",\"%5\",\"%6\")")
                   .arg("admin").arg("admin").arg("admin").arg("Root").arg("men").arg(1);
        if (!query.exec(temp)){ qDebug() << "exec failed2";} else qDebug() << "exec successed";
    }
    db->close();
}
void MainWindow::CheckDB()
{
    QSqlDatabase *db = DB::getInterface();
    if (!db->open())
        return;
    QSqlQuery query(*db);
    QDateTime start = QDateTime::fromTime_t(QDateTime::currentDateTime().toTime_t() - 604800);
    QString temps = QString("DELETE FROM checked_result WHERE time < \"%1\";").arg(start.toString("yyyy-MM-dd hh:mm:ss"));
    if (!query.exec(temps)){
        qDebug() << "clear data failed";
        return;
    }
    db->close();
}

void MainWindow::accept_men_login(const Meninfo &info)
{
    this->choose->setMen(info);
    this->screen_check->setMen(info);
    this->screen_result->setMen(info);
}

void MainWindow::accept_change_page(const int &page)
{
    if (page == 0)
    {
        Meninfo info;
        this->login->clear();
        this->choose->setMen(info);
        this->ui->stackedWidget->setCurrentIndex(0);
    }
    else if(page < 4)
        this->ui->stackedWidget->setCurrentIndex(page);
    else if (page >= 4)
    {
        this->ui->stackedWidget->setCurrentIndex(3);
        this->mainpage->set_current_page(page - 3);
    }
}


void MainWindow::check_self()
{
    if (this->drivesetting->init()){
       this->camera->startCollect();
       this->camera->collectFrame(this->camera_label_->winId());
       this->ui->stackedWidget->setCurrentIndex(3);
       this->screen_check->start_check();
    }
    else
    {
        this->drivesetting->display_init();
    }
}

void MainWindow::accept_camera_start_check()
{
    if (this->drivesetting->init())
    {
        this->screen_check->start_check();
    }
    else
    {
        QMessageBox messageBox;
        messageBox.setWindowTitle("警告");
        messageBox.setIcon(QMessageBox::Warning);
        QPushButton button("确定");
        messageBox.setText("系统自检失败,\n请先返回选择页面完成系统自检!");
        messageBox.exec();
    }
}

void MainWindow::on_stackedWidget_currentChanged(int arg1)
{
    if (arg1 == 0)
    {
        this->time->changedstyle("white");
    }
    else
    {
        this->time->changedstyle("black");
    }
}
