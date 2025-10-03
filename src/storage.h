#ifndef STORAGE_H
#define STORAGE_H

#include "event.h"
#include <vector>

namespace calendar {

class StorageManager {
public:
    static void saveEventsToStorage(const std::vector<Event>& events);
    static void loadEventsFromStorage(std::vector<Event>& events);

private:
    static std::string serializeToJSON(const std::vector<Event>& events);
    static void parseFromJSON(const std::string& json, std::vector<Event>& events);
};

} // namespace calendar

#endif // STORAGE_H

