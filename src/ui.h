#ifndef UI_H
#define UI_H

#include "event.h"
#include "calendar.h"

namespace calendar {

struct CalendarState {
    int currentMonth;
    int currentYear;
    int selectedDay;
    bool showAddEvent;
    bool weeklyView;
    int weekStartDay;
    int eventHour;
    int eventMinute;
    char eventInput[256];
    
    CalendarState();
    void initCurrentDate();
};

class CalendarUI {
public:
    CalendarUI(CalendarState& state, EventManager& eventManager);
    
    void render();
    void setupTerminalStyle();

private:
    void renderViewToggle();
    void renderNavigation();
    void renderActionButtons();
    void renderWeeklyView();
    void renderMonthlyView();
    void renderAddEventDialog();
    
    void renderWeeklyAddEventDialog();
    void renderMonthlyAddEventDialog();
    
    CalendarState& state_;
    EventManager& eventManager_;
};

} // namespace calendar

#endif // UI_H

