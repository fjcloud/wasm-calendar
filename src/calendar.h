#ifndef CALENDAR_H
#define CALENDAR_H

namespace calendar {

class CalendarLogic {
public:
    static int getDaysInMonth(int month, int year);
    static int getFirstDayOfMonth(int month, int year);
    static void getWeekDates(int startDay, int month, int year, int dates[7]);
    static void advanceWeek(int& day, int& month, int& year, int direction);
    
    static const char* getMonthName(int month);
    static const char* getDayName(int day);

private:
    static const char* monthNames_[12];
    static const char* dayNames_[7];
};

} // namespace calendar

#endif // CALENDAR_H

