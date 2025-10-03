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
        if (a->hour == -1 && b->hour == -1) return false;
        if (a->hour == -1) return false;
        if (b->hour == -1) return true;
        if (a->hour != b->hour) return a->hour < b->hour;
        return a->minute < b->minute;
    });
}

std::string EventManager::formatEventTime(const Event* event) {
    if (event->hour == -1) {
        return "";
    }
    char buffer[16];
    snprintf(buffer, sizeof(buffer), "%02d:%02d", event->hour, event->minute);
    return std::string(buffer);
}

} // namespace calendar

