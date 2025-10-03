#include "storage.h"
#include <emscripten.h>
#include <emscripten/html5.h>
#include <string>

namespace calendar {

void StorageManager::saveEventsToStorage(const std::vector<Event>& events) {
    std::string json = serializeToJSON(events);
    
    EM_ASM({
        localStorage.setItem('calendar_events', UTF8ToString($0));
    }, json.c_str());
}

void StorageManager::loadEventsFromStorage(std::vector<Event>& events) {
    char* stored = (char*)EM_ASM_PTR({
        const data = localStorage.getItem('calendar_events');
        if (!data) return null;
        const len = lengthBytesUTF8(data) + 1;
        const ptr = _malloc(len);
        stringToUTF8(data, ptr, len);
        return ptr;
    });
    
    if (stored) {
        std::string json = stored;
        free(stored);
        parseFromJSON(json, events);
    }
}

std::string StorageManager::serializeToJSON(const std::vector<Event>& events) {
    std::string json = "[";
    for (size_t i = 0; i < events.size(); i++) {
        if (i > 0) json += ",";
        json += "{\"day\":" + std::to_string(events[i].day);
        json += ",\"month\":" + std::to_string(events[i].month);
        json += ",\"year\":" + std::to_string(events[i].year);
        json += ",\"hour\":" + std::to_string(events[i].hour);
        json += ",\"minute\":" + std::to_string(events[i].minute);
        json += ",\"text\":\"" + events[i].text + "\"}";
    }
    json += "]";
    return json;
}

void StorageManager::parseFromJSON(const std::string& json, std::vector<Event>& events) {
    events.clear();
    
    // Simple JSON parsing
    size_t pos = 0;
    while ((pos = json.find("\"day\":", pos)) != std::string::npos) {
        Event evt;
        evt.hour = -1;  // Default: no time
        evt.minute = 0;
        
        pos += 6;
        evt.day = std::stoi(json.substr(pos, json.find(",", pos) - pos));
        
        pos = json.find("\"month\":", pos) + 8;
        evt.month = std::stoi(json.substr(pos, json.find(",", pos) - pos));
        
        pos = json.find("\"year\":", pos) + 7;
        evt.year = std::stoi(json.substr(pos, json.find(",", pos) - pos));
        
        // Try to read hour
        size_t hourPos = json.find("\"hour\":", pos);
        if (hourPos != std::string::npos && hourPos < json.find("\"text\":", pos)) {
            evt.hour = std::stoi(json.substr(hourPos + 7, json.find(",", hourPos) - (hourPos + 7)));
        }
        
        // Try to read minute
        size_t minutePos = json.find("\"minute\":", pos);
        if (minutePos != std::string::npos && minutePos < json.find("\"text\":", pos)) {
            evt.minute = std::stoi(json.substr(minutePos + 9, json.find(",", minutePos) - (minutePos + 9)));
        }
        
        pos = json.find("\"text\":\"", pos) + 8;
        size_t end = json.find("\"}", pos);
        evt.text = json.substr(pos, end - pos);
        
        events.push_back(evt);
        pos = end;
    }
}

} // namespace calendar

