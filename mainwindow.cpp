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
