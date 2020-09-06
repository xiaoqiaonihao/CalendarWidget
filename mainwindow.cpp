#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QTextStream>
#include <time.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    showCalendar = false;
    calendar = new CalendarWidget(this);
    ui->verticalLayout->addWidget(calendar);
    QFile file("date.txt");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream out(&file);
    QString list = out.readAll();
    date = list.split("\n");
    file.close();
    qDebug()<<date.size();
    calendar->setDays(date);
    calendar->setMinimumWidth(500);
    qDebug()<< "文件大小"<< list.size();


//    calendar->setMaxDate(QDate::currentDate());
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
    if(showCalendar){
        showCalendar = false;
        calendar->hide();
    }else{
        showCalendar = true;
        calendar->show();
    }
}
