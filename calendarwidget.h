#ifndef CALENDARWIDGET_H
#define CALENDARWIDGET_H

#include <QString>
#include <QWidget>
#include <QDate>
#include <QPushButton>
#include <QAbstractButton>
#include <QStackedWidget>
#include <QSet>
#include <QObject>
#include <QLabel>
#include <QFont>
#include <QPainter>
#include <QMouseEvent>

class DateIconButton : public QAbstractButton
{
    Q_OBJECT
public:
    enum DayType{
        normal,
        weekend,
        notworking,
        notenable,
        selected,
        hover,
    };
    explicit DateIconButton(QWidget *parent = nullptr);
    void setEnabled(bool value);
    void setType(DayType t,bool enable = true);
    ~DateIconButton();

private:
    DayType type;
    DayType prevType;

protected:
    void paintEvent(QPaintEvent *e);
    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);
    void drawText(QPainter *painter,QColor color);
};

class TitleButton : public QAbstractButton
{
    Q_OBJECT
public:
    enum DrawType{
        DrawText,
        DrawRightIcon,
        DrawLeftIcon,
    };
    explicit TitleButton(QWidget *parent = nullptr);
    void mouseMoveEvent(QMouseEvent *e);
    void paintEvent(QPaintEvent *e);
    void setType(DrawType ty);
    ~TitleButton();

private:
    DrawType type;
};


class CalendarWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CalendarWidget(QWidget *parent = nullptr);
    ~CalendarWidget();
    QDate getDate(){return date;}
    void paintEvent(QPaintEvent *event);
    void setMinDate(QDate date){minDate = date; }
    void setMaxDate(QDate date){maxDate = date; }
    void setDays(QList<QString> list);

protected:
    void resizeEvent(QResizeEvent *event);
    void wheelEvent(QWheelEvent * event);

private:
    enum TitleType{
        DayType,
        MonthType,
        YearType
    };

    QSet<QString> days;
    QDate date;
    QDate maxDate;
    QDate minDate;
    const int WeekCount = 7;
    const int RowCount = 6;
    const int minWidth = 150;
    const int minHeight = 150;
    QList <DateIconButton *> calendarList;
    QList <DateIconButton *> monthList;
    QList <DateIconButton *> yearList;
    QList <QLabel *> weekList;
    TitleType type;
    TitleButton *titleBtn;
    TitleButton *nextBtn;
    TitleButton *prevBtn;
    int titleHeight;
    int weekHeight;
    int year;
    int month;
    int day;
    bool openDays;
    const int YearBeginCount = 5;

    QStackedWidget *stackedWidget;
    QWidget *dayWideget;
    QWidget *monthWidget;
    QWidget *yearWidget;

    QWidget *_getYearWidet();
    QWidget *_getMonthWidget();
    QWidget *_getDayWidget();

    void initTitle();
    void initWeek();
    void initStackedWidget();
    void resizeTitle();
    void resizeWeek();
    void resizeStackedWidget();
    void resizeDays();
    void resizeMonth();
    void resizeYear();
    int addYear(int value);
    int addMonth(int value);
    void setTitleBtnText();

private slots:
    void titleClick();
    void prevClick();
    void nextClick();
    void monthClick();
    void yearClick();
    void daysClick();

signals:
    void clicked(QDate currentDate);

};

#endif // MYCALENDARWIDGET_H
