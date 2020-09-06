#include "calendarwidget.h"
#include <QDebug>
#include <time.h>

DateIconButton::DateIconButton(QWidget *parent):
    QAbstractButton(parent)
{
    type = DayType::normal;
    prevType = DayType::normal;
    setTabletTracking(true);
}

void DateIconButton::setEnabled(bool value)
{
    QAbstractButton::setEnabled(value);
    if(value){
        type = DayType::normal;
    }else{
        type = DayType::notenable;
    }
    update();
}

void DateIconButton::setType(DateIconButton::DayType t,bool enable)
{
    type = t;
    QAbstractButton::setEnabled(enable);
    update();
}

DateIconButton::~DateIconButton()
{
    qDebug()<<"DateIconButton析构函数";
}

void DateIconButton::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e);
    QPainter painter(this);
    switch (type)
    {
    case normal:
    {
        drawText(&painter,Qt::black);
    }break;
    case weekend:
    {
        drawText(&painter,Qt::red);
    }break;
    case notworking:
    {
        QPen pen(Qt::black);
        pen.setWidth(3);
        painter.setPen(pen);
        //30 20
        painter.drawLine(width()/2 -15,height()/2-10,width()/2 + 15,height()/2 +10);
        painter.drawLine(width()/2 -15,height()/2 + 10,width()/2 + 15,height()/2 -10);
        drawText(&painter,Qt::gray);
    }break;
    case notenable:
    {
        drawText(&painter,Qt::gray);
    }break;
    case selected:
    {
        int whiteBorderW = 4;
        QPen pen(Qt::blue);
        pen.setWidth(whiteBorderW);
        painter.setPen(pen);
        QBrush brush(Qt::white);
        painter.setBrush(brush);
        painter.drawRect(0,0,width(),height());

        brush.setColor(Qt::blue);
        painter.setBrush(brush);
        pen.setWidth(0);
        painter.setPen(pen);
        painter.drawRect(2*whiteBorderW,2*whiteBorderW,width()-4*whiteBorderW,height()-4*whiteBorderW);
        drawText(&painter,Qt::white);
    }break;
    case hover:
    {
        int whiteBorderW = 4;
        QPen pen("#666666");
        pen.setWidth(whiteBorderW);
        painter.setPen(pen);
        QBrush brush(Qt::white);
        painter.setBrush(brush);
        painter.drawRect(0,0,width(),height());

        brush.setColor("#666666");
        painter.setBrush(brush);
        pen.setWidth(0);
        painter.setPen(pen);
        painter.drawRect(2*whiteBorderW,2*whiteBorderW,width()-4*whiteBorderW,height()-4*whiteBorderW);
        drawText(&painter,Qt::white);
    }break;
    default:
        break;
    }
}

void DateIconButton::enterEvent(QEvent *event)
{
    Q_UNUSED(event);
    prevType = type;
    type = hover;
}

void DateIconButton::leaveEvent(QEvent *event)
{
    Q_UNUSED(event);
    type = prevType;
}

void DateIconButton::drawText(QPainter *painter, QColor color)
{
    QPen pen(color);
    pen.setWidth(1);
    painter->setPen(pen);
    painter->drawText(0,0,width(),height(),Qt::AlignCenter,text());
}

CalendarWidget::CalendarWidget(QWidget *parent):
    QWidget(parent),
    stackedWidget(nullptr),
    dayWideget(nullptr),
    monthWidget(nullptr),
    yearWidget(nullptr)
{
    date = QDate::currentDate();
    year = date.year();
    month = date.month();
    day = date.day();
    openDays = false;
    minDate = QDate(1970,1,1);
    maxDate = QDate(2050,1,1);
    weekHeight = 40;
    titleHeight = 50;
    initStackedWidget();
    type = TitleType::DayType;
    stackedWidget->setCurrentIndex((int)TitleType::DayType);
    initTitle();
    initWeek();
    this->setMinimumSize(minWidth,minHeight);
}

CalendarWidget::~CalendarWidget()
{

}

void CalendarWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setPen(Qt::white);
    painter.setBrush(Qt::white);
    painter.drawRect(rect());
}

void CalendarWidget::setDays(QList<QString> list)
{
    if(list.size() == 0)
        return;
    QString sMax = list.first();
    if(!sMax.isEmpty()){
        QStringList vList = sMax.split("-");
        if(vList.size() >=3)
            maxDate.setDate(vList.at(0).toInt(),vList.at(1).toInt(),vList.at(2).toInt());
    }

    QString sMin = list.last();
    while(sMin.isEmpty()){
        list.pop_back();
        sMin = list.last();
    }
    QStringList vList = sMin.split("-");
    if(vList.size() >=3)
        minDate.setDate(vList.at(0).toInt(),vList.at(1).toInt(),vList.at(2).toInt());

    openDays = true;
    days.clear();
    foreach(QString key, list)
    {
        days.insert(key);
    }
}

void CalendarWidget::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    resizeTitle();
    resizeWeek();
    resizeStackedWidget();
    resizeDays();
    resizeMonth();
    resizeYear();
}

void CalendarWidget::wheelEvent(QWheelEvent *event)
{
    if(event->delta()>0){
        prevClick();
    }else{
        nextClick();
    }
}

QWidget *CalendarWidget::_getYearWidet()
{
    if(yearWidget != nullptr)
        return yearWidget;

    yearWidget = new QWidget( this);
    for(int i = 0; i < 12; i++)
    {
        DateIconButton *btn = new DateIconButton(yearWidget);
        connect(btn,SIGNAL(clicked()),this,SLOT(yearClick()));
        btn->setText(QString::number(i + year - YearBeginCount));
        btn->hide();
        yearList.append(btn);
    }
    return yearWidget;
}

QWidget *CalendarWidget::_getMonthWidget()
{
    if(monthWidget != nullptr)
        return monthWidget;

    monthWidget = new QWidget( this);
    for(int i = 0; i < 12; i++)
    {
        DateIconButton *btn = new DateIconButton(monthWidget);
        connect(btn,SIGNAL(clicked()),this,SLOT(monthClick()));
        btn->setText(QString::number(i+1));
        btn->hide();
        monthList.append(btn);
    }
    return monthWidget;
}

QWidget *CalendarWidget::_getDayWidget()
{
    if(dayWideget != nullptr)
        return dayWideget;

    dayWideget = new QWidget(this);
    for(int i = 0; i < WeekCount; i++)
    {
        for(int j = 0; j < RowCount; j++)
        {
            DateIconButton *btn = new DateIconButton(dayWideget);
            connect(btn,SIGNAL(clicked()),this,SLOT(daysClick()));
            btn->hide();
            calendarList.append(btn);
        }
    }
    return dayWideget;
}

void CalendarWidget::initTitle()
{
    titleBtn = new TitleButton(this);
    connect(titleBtn,SIGNAL(clicked()),this,SLOT(titleClick()));
    titleBtn->setType(TitleButton::DrawText);
    setTitleBtnText();

    nextBtn = new TitleButton(this);
    connect(nextBtn,SIGNAL(clicked()),this,SLOT(nextClick()));
    nextBtn->setType(TitleButton::DrawRightIcon);

    prevBtn = new TitleButton(this);
    connect(prevBtn,SIGNAL(clicked()),this,SLOT(prevClick()));
    prevBtn->setType(TitleButton::DrawLeftIcon);
}

void CalendarWidget::initWeek()
{
    QStringList weeks;
    weeks<< "一"<<"二"<<"三"<<"四"<<"五"<<"六"<<"日";
    foreach(QString week, weeks)
    {
        QLabel *label = new QLabel(week,dayWideget);
        label->setAlignment(Qt::AlignCenter);
        label->hide();
        weekList.append(label);
    }

}

void CalendarWidget::initStackedWidget()
{
    if(stackedWidget != nullptr)
        return;

    stackedWidget = new QStackedWidget( this);
    dayWideget = _getDayWidget();
    monthWidget = _getMonthWidget();
    yearWidget = _getYearWidet();
    stackedWidget->addWidget(dayWideget);
    stackedWidget->addWidget(monthWidget);
    stackedWidget->addWidget(yearWidget);
}

void CalendarWidget::resizeTitle()
{
    int btnWidth = 166;
    int btnHeight = 21;
    prevBtn->setGeometry(20, 10, 21,21);
    nextBtn->setGeometry(width()-20-21,10,21,21);
    titleBtn->setGeometry(width()/2 - btnWidth/2, 10,btnWidth,btnHeight);
}

void CalendarWidget::resizeWeek()
{
    double oneWidth = width()/WeekCount;
    QLabel *label = nullptr;
    for(int i = 0;i < weekList.size(); i++)
    {
        label = weekList.at(i);
        label->setGeometry(i*oneWidth , 0, oneWidth , 30);
        label->show();
    }
}

void CalendarWidget::resizeStackedWidget()
{
    int beginY = titleHeight;
    stackedWidget->setGeometry(0,beginY,width(),height()-beginY);
}

void CalendarWidget::resizeDays()
{
    QDate tDate = QDate(year,month,1);
    int week = tDate.dayOfWeek();
    tDate = tDate.addDays(1 - week);
    double oneHeight = (double)(dayWideget->height() - weekHeight)/RowCount;
    double oneWidth = (double)dayWideget->width() /(double)WeekCount;
    QString year_mstr = "";
    DateIconButton *btn = nullptr;
    for(int j = 0; j < RowCount; j++)
    {
        for(int i = 0; i < WeekCount; i++)
        {
            int index = (j * WeekCount) + i;
            if(index > calendarList.size())
                return;

            btn = calendarList.at(index);
            if(tDate > maxDate || tDate < minDate){
                btn->hide();
            }
            else{
                btn->show();
            }

            btn->setText(QString::number(tDate.day()));
            if(tDate.month() != month)
            {
                btn->setEnabled(false);
            }
            else
            {
                btn->setEnabled(true);
                if(i==5 || i== 6){
                    btn->setType(DateIconButton::weekend);
                }
            }
            if(date == tDate){
                btn->setType(DateIconButton::selected);
            }

            year_mstr = QString::asprintf("%d-%02d-",tDate.year(),tDate.month());
            bool result = days.contains(year_mstr + QString::asprintf("%02d",tDate.day()));
            if(!result && openDays)
                btn->setType(DateIconButton::notworking,false);


            tDate = tDate.addDays(1);
            btn->setGeometry(i * oneWidth,weekHeight + j * oneHeight ,oneWidth, oneHeight);
        }
    }
    setTitleBtnText();
}

void CalendarWidget::resizeMonth()
{
    double oneHeight = stackedWidget->height()/3;
    double oneWidth = stackedWidget->width()/4;

    DateIconButton *btn = nullptr;
    for(int i = 0;i < monthList.size(); i++)
    {
        btn = monthList.at(i);
        if(year == minDate.year() && (i+1) < minDate.month()){
            btn->setEnabled(false);
        }
        else if(year == maxDate.year() && (i+1) > maxDate.month()){
            btn->setEnabled(false);
        }
        else{
            btn->setEnabled(true);
        }
        monthList.at(i)->setGeometry((i%4)*oneWidth,(i/4)*oneHeight,oneWidth,oneHeight);
        monthList.at(i)->show();
    }
}

void CalendarWidget::resizeYear()
{
    double oneHeight = stackedWidget->height()/3;
    double oneWidth = stackedWidget->width()/4;

    int beginYear = year - YearBeginCount;
    if(beginYear < minDate.year())
        beginYear = minDate.year();

    for(int i = 0; i < yearList.size(); i++)
    {
        yearList.at(i)->setGeometry((i%4)*oneWidth,(i/4)*oneHeight,oneWidth,oneHeight);
        yearList.at(i)->setText(QString::number(i + beginYear));
        if(i + beginYear > maxDate.year())
        {
            yearList.at(i)->hide();
        }
        else
        {
            yearList.at(i)->show();
            yearList.at(i)->setEnabled(true);
        }

    }
}

void CalendarWidget::setTitleBtnText()
{
    switch (type)
    {
    case TitleType::DayType:
    {
        titleBtn->setText(QString::asprintf("%d年%d月%d日",year,month,day));
    }break;
    case TitleType::MonthType:
    {
        titleBtn->setText(QString::asprintf("%d年%d月",year,month));
    }break;
    case TitleType::YearType:
    {
        int minYear = qMax<int>(year - YearBeginCount,minDate.year());
        int maxYear = qMin<int> (year + 11 - YearBeginCount,maxDate.year());
        titleBtn->setText(QString::asprintf("%d -- %d",minYear,maxYear));
    }break;
    default:
        break;
    }
}

int CalendarWidget::addYear(int value)
{
    int tYear = year;
    tYear += value;
    if(tYear > maxDate.year())
        tYear = maxDate.year();
    else if(tYear < minDate.year())
        tYear = minDate.year();
    return tYear;
}

int CalendarWidget::addMonth(int value)
{
    int tMonth = month + value;
    if(year == maxDate.year() && tMonth > maxDate.month())
        tMonth = maxDate.month();
    else if(year == minDate.year() && tMonth < minDate.month())
        tMonth = minDate.month();
    else{
        while (tMonth <= 0) {
            tMonth += 12;
            year = addYear(-1);
        }
        year = addYear(tMonth/13);
        tMonth %= 13;
        tMonth = tMonth == 0 ? 1 : tMonth;
    }
    return tMonth;
}

void CalendarWidget::titleClick()
{
    switch(type)
    {
    case DayType:
    {
        type = TitleType::MonthType;
        stackedWidget->setCurrentIndex(int(type));
    }break;
    case MonthType:
    {
        type = TitleType::YearType;
        stackedWidget->setCurrentIndex(int(type));
        resizeYear();
    }break;
    default:
        break;
    }
    setTitleBtnText();
}

void CalendarWidget::prevClick()
{
    switch (type)
    {
    case TitleType::DayType:
    {
        month = addMonth(-1);
        resizeDays();
    }break;
    case TitleType::MonthType:
    {
        year = addYear(0 - 1);
        resizeMonth();
    }break;
    case TitleType::YearType:
    {
        year = addYear(0 - 12 - YearBeginCount);
        resizeYear();
    }break;
    default:
        break;
    }
    setTitleBtnText();
}

void CalendarWidget::nextClick()
{
    switch(type)
    {
    case TitleType::DayType:
    {
        month = addMonth(1);
        resizeDays();
    }break;
    case TitleType::MonthType:
    {
        year = addYear(1);
        resizeMonth();
    }break;
    case TitleType::YearType:
    {
        year = addYear(12 + YearBeginCount);
        resizeYear();
    }break;
    default:
        break;
    }
    setTitleBtnText();
}

void CalendarWidget::monthClick()
{
    DateIconButton *btn = static_cast<DateIconButton *>(sender());
    month = btn->text().toInt();
    resizeDays();
    type = TitleType::DayType;
    stackedWidget->setCurrentIndex(int(type));
    setTitleBtnText();
}

void CalendarWidget::yearClick()
{
    DateIconButton *btn = static_cast<DateIconButton *>(sender());
    year = btn->text().toInt();
    resizeMonth();
    type = TitleType::MonthType;
    stackedWidget->setCurrentIndex(int(type));
    setTitleBtnText();
}

void CalendarWidget::daysClick()
{
    DateIconButton *btn = static_cast<DateIconButton *>(sender());
    day = btn->text().toInt();
    setTitleBtnText();
    emit clicked(QDate(year,month,day));
    qDebug()<<year<<"年"<<month<<"月"<<day<<"日";
}

TitleButton::TitleButton(QWidget *parent)
    : QAbstractButton(parent)
{
    type = DrawRightIcon;
    setMouseTracking(true);
}

void TitleButton::mouseMoveEvent(QMouseEvent *e)
{
    Q_UNUSED(e);
    setCursor(Qt::PointingHandCursor);
}

void TitleButton::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing ,true);
    QPen pen;
    QBrush brush;

    switch (type) {
    case DrawRightIcon:
    {
        pen.setColor(Qt::black);
        pen.setWidth(2);
        painter.setPen(pen);
        QPoint mid(width(),height()/2);
        painter.drawLine(QPoint(0,0),mid);
        painter.drawLine(QPoint(0,height()),mid);
    }break;
    case DrawLeftIcon:
    {
        pen.setColor(Qt::black);
        pen.setWidth(2);
        painter.setPen(pen);
        QPoint mid(0,height()/2);
        painter.drawLine(QPoint(width(),0),mid);
        painter.drawLine(QPoint(width(),height()),mid);
    }break;
    case DrawText:
    {
        painter.drawText(0,0,width(),height(),Qt::AlignVCenter,text());
    }break;
    default:
        break;
    }
}

void TitleButton::setType(TitleButton::DrawType ty)
{
    type = ty;
    update();
}

TitleButton::~TitleButton()
{
    qDebug()<<"TitleButton析构函数";
}
