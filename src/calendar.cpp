#include "calendar.h"
#include <ctime>

namespace calendar {

const char* CalendarLogic::monthNames_[12] = {
    "January", "February", "March", "April", "May", "June",
    "July", "August", "September", "October", "November", "December"
};

const char* CalendarLogic::dayNames_[7] = {
    "SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"
};

int CalendarLogic::getDaysInMonth(int month, int year) {
    int days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (month == 1 && ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0))) {
        return 29; // Leap year
    }
    return days[month];
}

int CalendarLogic::getFirstDayOfMonth(int month, int year) {
    tm timeinfo = {};
    timeinfo.tm_year = year - 1900;
    timeinfo.tm_mon = month;
    timeinfo.tm_mday = 1;
    mktime(&timeinfo);
    return timeinfo.tm_wday;
}

void CalendarLogic::getWeekDates(int startDay, int month, int year, int dates[7]) {
    int daysInMonth = getDaysInMonth(month, year);
    for (int i = 0; i < 7; i++) {
        dates[i] = startDay + i;
        if (dates[i] > daysInMonth) {
            dates[i] = -1; // Invalid date
        }
    }
}

void CalendarLogic::advanceWeek(int& day, int& month, int& year, int direction) {
    day += (7 * direction);
    int daysInCurrent = getDaysInMonth(month, year);
    
    while (day > daysInCurrent) {
        day -= daysInCurrent;
        month++;
        if (month > 11) {
            month = 0;
            year++;
        }
        daysInCurrent = getDaysInMonth(month, year);
    }
    
    while (day < 1) {
        month--;
        if (month < 0) {
            month = 11;
            year--;
        }
        daysInCurrent = getDaysInMonth(month, year);
        day += daysInCurrent;
    }
}

const char* CalendarLogic::getMonthName(int month) {
    if (month >= 0 && month < 12) {
        return monthNames_[month];
    }
    return "";
}

const char* CalendarLogic::getDayName(int day) {
    if (day >= 0 && day < 7) {
        return dayNames_[day];
    }
    return "";
}

} // namespace calendar

