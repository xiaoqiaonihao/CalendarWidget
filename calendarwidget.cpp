#include "calendarwidget.h"

#include "calendarwidget.h"

DateIconButton::DateIconButton(BtnType btnTy,QWidget *parent):
    QAbstractButton(parent)
{
    type = DayType::normal;
    prevType = DayType::normal;
    setTabletTracking(true);
    currentBtn = btnTy;
    if(currentBtn == BtnType::DayBtn){
        selectedSize = QSize(24,24);
    }
    else if(currentBtn == BtnType::MonthBtn){
        selectedSize = QSize(37,25);
    }else if(currentBtn == BtnType::YearBtn){
        selectedSize = QSize(54,25);
    }
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
}

void DateIconButton::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing,true);
    switch (type)
    {
    case normal:
    {
        drawText(&painter,QColor(__NORMALCOLOR));
    }break;
    case weekend:
    {
        drawText(&painter,QColor(__WEEKENCOLOR));
    }break;
    case notworking:
    {
        double halfWidth = double(selectedSize.width()/2.0) - 4;
        double halfHeight = double(selectedSize.height()/2.0) - 4;
        QPen pen(QColor(__NOTWORKINGBACKGROUND));
        pen.setWidth(3);
        painter.setPen(pen);
        //绘制 X
        painter.drawLine(width()/2.0 -halfWidth,height()/2.0-halfHeight,width()/2.0 + halfWidth,height()/2.0 +halfHeight);
        painter.drawLine(width()/2.0 -halfWidth,height()/2.0 + halfHeight,width()/2.0 + halfWidth,height()/2.0 -halfHeight);
        drawText(&painter,QColor(__NOTWORKINGCOLOR));
    }break;
    case notenable:
    {
        drawText(&painter,QColor(__NOTENABLECOLOR));
    }break;
    case selected:
    {
        QBrush brush(QColor(__SELECTEDBACKGROUND));
        painter.setPen(QColor(__SELECTEDBACKGROUND));
        painter.setBrush(brush);
        painter.drawRoundedRect(QRectF( width()/2.0 - (double)selectedSize.width()/2.0,height()/2.0-selectedSize.height()/2.0,selectedSize.width(),selectedSize.height()),4,4);
        drawText(&painter,QColor(__SELECTEDCOLOR));
    }break;
    case hover:
    {
        QColor _col(__HOVERBACKGROUND);
        QBrush brush(QColor(_col.red(),_col.green(),_col.blue(),80));
        painter.setPen(QColor(_col.red(),_col.green(),_col.blue(),80));
        painter.setBrush(brush);
        painter.drawRoundedRect(width()/2.0 - (double)selectedSize.width()/2.0,height()/2.0-selectedSize.height()/2.0,selectedSize.width(),selectedSize.height(),4,4);
        drawText(&painter,QColor(__HOVERCOLOR));
    }break;
    default:
        break;
    }
}

void DateIconButton::enterEvent(QEvent *event)
{
    Q_UNUSED(event);
    if(type == notenable || type == notworking)
        return;
    prevType = type;
    type = hover;
}

void DateIconButton::leaveEvent(QEvent *event)
{
    Q_UNUSED(event);
    if(type == notenable || type == notworking)
        return;
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
    minDate = QDate(1900,1,1);
    maxDate = QDate(2080,1,1);
    weekHeight = 40;
    titleHeight = 40;
    initStackedWidget();
    type = TitleType::DayType;
    stackedWidget->setCurrentIndex((int)TitleType::DayType);
    initTitle();
    initWeek();
    this->setMinimumSize(minWidth,minHeight);
    this->setMaximumSize(minWidth,minHeight);
}

CalendarWidget::~CalendarWidget()
{

}

void CalendarWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPen pen(__CALENDARBORDER);
    pen.setWidth(margin);
    QPainter painter(this);
    painter.setPen(pen);
    painter.setBrush(QColor(__CALENDARBACKGROUND));
    painter.drawRect(0,0,width() - 2*margin ,height() -margin);

    painter.setBrush(QColor(__CALENDARMARGIN));
    painter.setPen(QColor(__CALENDARMARGIN));
    painter.drawRect(0,titleHeight,width() - 2*margin ,titleSpace);
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

void CalendarWidget::showEvent(QShowEvent *event)
{
    Q_UNUSED(event);
    resizeDays();
}

QWidget *CalendarWidget::_getYearWidet()
{
    if(yearWidget != nullptr)
        return yearWidget;

    yearWidget = new QWidget( this);
    yearWidget->setStyleSheet(__YEARWIDGETSTYLE);
    for(int i = 0; i < 12; i++)
    {
        DateIconButton *btn = new DateIconButton(DateIconButton::YearBtn,yearWidget);
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
    monthWidget->setStyleSheet(__MONTHWIDGETSTYLE);
    for(int i = 0; i < 12; i++)
    {
        DateIconButton *btn = new DateIconButton(DateIconButton::MonthBtn,monthWidget);
        connect(btn,SIGNAL(clicked()),this,SLOT(monthClick()));
        btn->setText(QString::asprintf("%d月",i+1));
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
    dayWideget->setStyleSheet(__DAYWIDGETSTYLE);
    for(int i = 0; i < WeekCount; i++)
    {
        for(int j = 0; j < RowCount; j++)
        {
            DateIconButton *btn = new DateIconButton(DateIconButton::DayBtn,dayWideget);
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
    int i=0;
    foreach(QString week, weeks)
    {
        QLabel *label = new QLabel(week,dayWideget);
        label->setStyleSheet(__WEEKWIDGETSTYLE);
        if(i == 5 || i== 6){
            label->setStyleSheet(QString::asprintf("color:%s",__WEEKENCOLOR));
        }
        i++;
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
    int btnWidth = 120;
    int btnHeight = 22;
    prevBtn->setGeometry(20, 15, 10,10);
    nextBtn->setGeometry(width()-20-21,15,10,10);
    titleBtn->setGeometry(width()/2 - btnWidth/2, titleHeight/2 - btnHeight/2,btnWidth,btnHeight);
}

void CalendarWidget::resizeWeek()
{
    double oneWidth = width()/WeekCount;
    QLabel *label = nullptr;
    for(int i = 0;i < weekList.size(); i++)
    {
        label = weekList.at(i);
        label->setGeometry(i*oneWidth , 0, oneWidth , titleHeight);
        label->show();
    }
}

void CalendarWidget::resizeStackedWidget()
{
    int beginY = titleHeight;
    stackedWidget->setGeometry(margin,beginY+titleSpace,width()-3*margin,height()-beginY-margin);
}

void CalendarWidget::resizeDays()
{
    QDate tDate = QDate(year,month,1);
    int week = tDate.dayOfWeek();
    tDate = tDate.addDays(1 - week);
    double oneHeight = (double)(dayWideget->height() - weekHeight)/RowCount;
    double oneWidth = (double)dayWideget->width() /(double)WeekCount;
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

            tDate = tDate.addDays(1);
            btn->setGeometry(i * oneWidth ,weekHeight + j * oneHeight ,oneWidth, oneHeight);
        }
    }
    setTitleBtnText();
}

void CalendarWidget::resizeMonth()
{
    int monthRow = 4;
    int monthCol = 3;
    double oneHeight = stackedWidget->height()/monthRow;
    double oneWidth = stackedWidget->width()/monthCol;

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
            if(month == (i+1)){
                btn->setType(DateIconButton::selected);
            }else
                btn->setType(DateIconButton::normal);
        }
        monthList.at(i)->setGeometry((i%monthCol)*oneWidth,(i/monthCol)*oneHeight,oneWidth,oneHeight);
        monthList.at(i)->show();
    }
}

void CalendarWidget::resizeYear()
{
    int yearRow = 4;
    int yearCol = 3;
    double oneHeight = stackedWidget->height()/yearRow;
    double oneWidth = stackedWidget->width()/yearCol;

    int beginYear = year - YearBeginCount;
    if(beginYear < minDate.year())
        beginYear = minDate.year();

    for(int i = 0; i < yearList.size(); i++)
    {
        yearList.at(i)->setGeometry((i%yearCol)*oneWidth,(i/yearCol)*oneHeight,oneWidth,oneHeight);
        yearList.at(i)->setText(QString::asprintf("%d年",i+beginYear));
        if(i + beginYear > maxDate.year())
        {
            yearList.at(i)->hide();
        }
        else
        {
            yearList.at(i)->show();
            yearList.at(i)->setEnabled(true);
            if(year == i+beginYear)
                yearList.at(i)->setType(DateIconButton::selected);
            else
                yearList.at(i)->setType(DateIconButton::normal);
        }

    }
}

void CalendarWidget::setTitleBtnText()
{
    switch (type)
    {
    case TitleType::DayType:
    {
        titleBtn->setText(QString::asprintf("%d年%d月",year,month,day));
    }break;
    case TitleType::MonthType:
    {
        titleBtn->setText(QString::asprintf("%d年",year,month));
    }break;
    case TitleType::YearType:
    {
        int minYear = qMax<int>(year - YearBeginCount,minDate.year());
        int maxYear = qMin<int> (year + 11 - YearBeginCount,maxDate.year());
        if(minYear == minDate.year()){
            maxYear = qMin<int>(year + 11,maxDate.year());
        }

        titleBtn->setText(QString::asprintf("%d年 - %d年",minYear,maxYear));
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
    QString text = btn->text();
    month = text.left(text.length()-1).toInt();
    resizeDays();
    type = TitleType::DayType;
    stackedWidget->setCurrentIndex(int(type));
    setTitleBtnText();
}

void CalendarWidget::yearClick()
{
    DateIconButton *btn = static_cast<DateIconButton *>(sender());
    QString text = btn->text();
    year = text.left(text.length()-1).toInt();
    resizeMonth();
    type = TitleType::MonthType;
    stackedWidget->setCurrentIndex(int(type));
    setTitleBtnText();
}

void CalendarWidget::daysClick()
{
    DateIconButton *btn = static_cast<DateIconButton *>(sender());
    day = btn->text().toInt();
    date = QDate(year,month,day);
    setTitleBtnText();
    emit clicked(date);
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
        pen.setColor(Qt::white);
        pen.setWidth(2);
        painter.setPen(pen);
        QPoint mid(width(),height()/2);
        painter.drawLine(QPoint(0,0),mid);
        painter.drawLine(QPoint(0,height()),mid);
    }break;
    case DrawLeftIcon:
    {
        pen.setColor(Qt::white);
        pen.setWidth(2);
        painter.setPen(pen);
        QPoint mid(0,height()/2);
        painter.drawLine(QPoint(width(),0),mid);
        painter.drawLine(QPoint(width(),height()),mid);
    }break;
    case DrawText:
    {
        pen.setColor(Qt::white);
        painter.setPen(pen);
        painter.drawText(0,0,width(),height(),Qt::AlignCenter,text());
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
}
