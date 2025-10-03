#ifndef UI_H
#define UI_H

#include "event.h"
#include "calendar.h"

namespace calendar {

enum ViewMode {
    VIEW_MONTH,
    VIEW_WEEK,
    VIEW_DAY
};

struct CalendarState {
    int currentMonth;
    int currentYear;
    int selectedDay;
    bool showAddEvent;
    ViewMode viewMode;
    int weekStartDay;
    int eventHourStart;
    int eventMinuteStart;
    int eventHourEnd;
    int eventMinuteEnd;
    bool eventIsAllDay;
    char eventInput[256];
    
    // Drag and drop state
    bool isDragging;
    Event* draggedEvent;
    int dragOffsetMinutes;  // Offset from event start to where user grabbed
    
    CalendarState();
    void initCurrentDate();
};

class CalendarUI {
public:
    CalendarUI(CalendarState& state, EventManager& eventManager);
    
    void render();
    void setupTerminalStyle();

private:
    void renderViewSelector();
    void renderNavigation();
    void renderActionButtons();
    void renderDayView();
    void renderWeekView();
    void renderMonthView();
    void renderAddEventDialog();
    
    void renderTimeGrid(int startDay, int numDays);
    void renderEventBlock(Event* event, float x, float y, float width, float height);
    void renderAllDayEvents(const std::vector<Event*>& events, float x, float y, float width);
    float getEventYPosition(int hour, int minute);
    float getEventHeight(int startHour, int startMinute, int endHour, int endMinute);
    
    CalendarState& state_;
    EventManager& eventManager_;
};

} // namespace calendar

#endif // UI_H

