#ifndef EVENT_H
#define EVENT_H

#include <string>
#include <vector>

namespace calendar {

struct Event {
    std::string text;
    int day;
    int month;
    int year;
    int hourStart;      // 0-23, -1 for all-day event
    int minuteStart;    // 0-59
    int hourEnd;        // 0-23, -1 for all-day event
    int minuteEnd;      // 0-59
    bool isAllDay;
    
    // Constructor for backward compatibility
    Event() : day(0), month(0), year(0), hourStart(-1), minuteStart(0), 
              hourEnd(-1), minuteEnd(0), isAllDay(false) {}
};

class EventManager {
public:
    EventManager() = default;
    
    void addEvent(const Event& event);
    void removeEvent(Event* event);
    std::vector<Event*> getEventsForDate(int day, int month, int year);
    std::vector<Event>& getAllEvents() { return events_; }
    const std::vector<Event>& getAllEvents() const { return events_; }
    void clear() { events_.clear(); }
    
    static std::string formatEventTime(const Event* event);

private:
    std::vector<Event> events_;
    void sortEventsByTime(std::vector<Event*>& events);
};

} // namespace calendar

#endif // EVENT_H

