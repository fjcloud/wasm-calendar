#include "event.h"
#include <algorithm>
#include <cstdio>

namespace calendar {

void EventManager::addEvent(const Event& event) {
    events_.push_back(event);
}

void EventManager::removeEvent(Event* event) {
    for (auto it = events_.begin(); it != events_.end(); ++it) {
        if (&(*it) == event) {
            events_.erase(it);
            break;
        }
    }
}

std::vector<Event*> EventManager::getEventsForDate(int day, int month, int year) {
    std::vector<Event*> result;
    for (auto& evt : events_) {
        if (evt.day == day && evt.month == month && evt.year == year) {
            result.push_back(&evt);
        }
    }
    sortEventsByTime(result);
    return result;
}

void EventManager::sortEventsByTime(std::vector<Event*>& events) {
    std::sort(events.begin(), events.end(), [](Event* a, Event* b) {
        // All-day events first
        if (a->isAllDay && !b->isAllDay) return true;
        if (!a->isAllDay && b->isAllDay) return false;
        if (a->isAllDay && b->isAllDay) return false;
        
        // Then by start time
        if (a->hourStart != b->hourStart) return a->hourStart < b->hourStart;
        return a->minuteStart < b->minuteStart;
    });
}

std::string EventManager::formatEventTime(const Event* event) {
    if (event->isAllDay || event->hourStart == -1) {
        return "All day";
    }
    char buffer[32];
    if (event->hourEnd != -1) {
        snprintf(buffer, sizeof(buffer), "%02d:%02d - %02d:%02d", 
                event->hourStart, event->minuteStart,
                event->hourEnd, event->minuteEnd);
    } else {
        snprintf(buffer, sizeof(buffer), "%02d:%02d", 
                event->hourStart, event->minuteStart);
    }
    return std::string(buffer);
}

} // namespace calendar

