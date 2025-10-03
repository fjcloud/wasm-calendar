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
    int hour;    // 0-23, -1 for no time set
    int minute;  // 0-59
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

