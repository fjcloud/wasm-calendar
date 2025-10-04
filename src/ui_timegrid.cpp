#include "ui.h"
#include "imgui.h"
#include "storage.h"
#include <cstdio>
#include <vector>

namespace calendar {

void CalendarUI::renderTimeGrid(int startDay, int numDays) {
    ImGuiStyle& style = ImGui::GetStyle();
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    
    ImVec2 canvas_size = ImGui::GetContentRegionAvail();
    
    const int startHour = 7;   // Start at 7:00 AM
    const int endHour = 24;    // End at midnight (23:59)
    int totalHours = endHour - startHour;
    
    // Calculate responsive dimensions based on available space
    float timeColumnWidth = canvas_size.x * 0.05f;  // 5% for time labels
    float remainingWidth = canvas_size.x - timeColumnWidth;
    float columnWidth = remainingWidth / numDays;
    float headerHeight = canvas_size.y * 0.04f;     // 4% for header
    
    // Calculate hourHeight to fill all available space
    float availableHeight = canvas_size.y - headerHeight - 10.0f; // Small margin
    float hourHeight = availableHeight / totalHours;
    
    // Calculate day info first
    int dayInfos[7][3]; // [day][month][year]
    for (int i = 0; i < numDays; i++) {
        int day = startDay + i;
        int month = state_.currentMonth;
        int year = state_.currentYear;
        
        int daysInMonth = CalendarLogic::getDaysInMonth(month, year);
        while (day > daysInMonth) {
            day -= daysInMonth;
            month++;
            if (month > 11) {
                month = 0;
                year++;
            }
            daysInMonth = CalendarLogic::getDaysInMonth(month, year);
        }
        dayInfos[i][0] = day;
        dayInfos[i][1] = month;
        dayInfos[i][2] = year;
    }
    
    // Draw day headers OUTSIDE the scroll area - aligned with grid
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 8));
    
    ImGui::Dummy(ImVec2(timeColumnWidth, headerHeight));
    ImGui::SameLine();
    
    for (int i = 0; i < numDays; i++) {
        int day = dayInfos[i][0];
        int month = dayInfos[i][1];
        int firstDay = CalendarLogic::getFirstDayOfMonth(month, state_.currentYear);
        int dayOfWeek = (firstDay + day - 1) % 7;
        
        char header[32];
        if (numDays == 1) {
            snprintf(header, sizeof(header), "%s %d", 
                    CalendarLogic::getDayName(dayOfWeek), day);
        } else {
            snprintf(header, sizeof(header), "%s\n%d", 
                    CalendarLogic::getDayName(dayOfWeek), day);
        }
        
        bool isToday = (day == state_.selectedDay && month == state_.currentMonth);
        if (isToday) {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.7f, 0.3f, 1.0f));
        }
        
        if (ImGui::Button(header, ImVec2(columnWidth, headerHeight))) {
            state_.selectedDay = day;
            state_.currentMonth = month;
            state_.currentYear = dayInfos[i][2];
            state_.showAddEvent = false;
        }
        
        if (isToday) {
            ImGui::PopStyleColor();
        }
        
        if (i < numDays - 1) ImGui::SameLine();
    }
    ImGui::PopStyleVar(2);
    
    ImGui::Spacing();
    
    // Draw the time grid directly (no scrolling)
    ImVec2 grid_start = ImGui::GetCursorScreenPos();
    for (int i = 0; i < totalHours; i++) {
        int hour = startHour + i;
        ImVec2 time_pos(grid_start.x, grid_start.y + i * hourHeight);
        
        // Time label
        char timeLabel[8];
        snprintf(timeLabel, sizeof(timeLabel), "%02d:00", hour);
        draw_list->AddText(time_pos, ImGui::ColorConvertFloat4ToU32(style.Colors[ImGuiCol_Text]), timeLabel);
        
        // Horizontal line
        ImVec2 line_start(grid_start.x + timeColumnWidth, time_pos.y);
        ImVec2 line_end(grid_start.x + timeColumnWidth + (columnWidth * numDays), time_pos.y);
        draw_list->AddLine(line_start, line_end, 
                          ImGui::ColorConvertFloat4ToU32(ImVec4(0.2f, 0.5f, 0.3f, 0.5f)));
    }
    
    // Draw vertical column separators
    for (int i = 0; i <= numDays; i++) {
        float x = grid_start.x + timeColumnWidth + (i * columnWidth);
        draw_list->AddLine(
            ImVec2(x, grid_start.y),
            ImVec2(x, grid_start.y + totalHours * hourHeight),
            ImGui::ColorConvertFloat4ToU32(ImVec4(0.2f, 0.5f, 0.3f, 0.5f))
        );
    }
    
    // Handle drag and drop
    ImVec2 mouse_pos = ImGui::GetMousePos();
    
    // If we're currently dragging
    if (state_.isDragging && state_.draggedEvent) {
        // Show dragging visual feedback
        ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
        
        // If mouse released, drop the event
        if (!ImGui::IsMouseDown(0)) {
            // Calculate drop position
            float gridLeft = grid_start.x + timeColumnWidth;
            float gridRight = grid_start.x + timeColumnWidth + (columnWidth * numDays);
            float gridTop = grid_start.y;
            float gridBottom = grid_start.y + (totalHours * hourHeight);
            
            if (mouse_pos.x >= gridLeft && mouse_pos.x < gridRight &&
                mouse_pos.y >= gridTop && mouse_pos.y < gridBottom) {
                
                // Calculate new time and day
                int dropDayOffset = (int)((mouse_pos.x - gridLeft) / columnWidth);
                int dropMinutesFromTop = (int)(((mouse_pos.y - gridTop) / hourHeight) * 60.0f);
                
                // Adjust for drag offset
                dropMinutesFromTop -= state_.dragOffsetMinutes;
                if (dropMinutesFromTop < 0) dropMinutesFromTop = 0;
                
                int dropHour = startHour + (dropMinutesFromTop / 60);
                int dropMinute = dropMinutesFromTop % 60;
                
                if (dropDayOffset >= 0 && dropDayOffset < numDays &&
                    dropHour >= startHour && dropHour < endHour) {
                    
                    // Calculate duration
                    int durationMinutes = 60; // Default 1 hour
                    if (state_.draggedEvent->hourEnd != -1) {
                        int startMinutes = state_.draggedEvent->hourStart * 60 + state_.draggedEvent->minuteStart;
                        int endMinutes = state_.draggedEvent->hourEnd * 60 + state_.draggedEvent->minuteEnd;
                        durationMinutes = endMinutes - startMinutes;
                    }
                    
                    // Update event
                    state_.draggedEvent->day = dayInfos[dropDayOffset][0];
                    state_.draggedEvent->month = dayInfos[dropDayOffset][1];
                    state_.draggedEvent->year = dayInfos[dropDayOffset][2];
                    state_.draggedEvent->hourStart = dropHour;
                    state_.draggedEvent->minuteStart = dropMinute;
                    state_.draggedEvent->hourEnd = dropHour + (durationMinutes / 60);
                    state_.draggedEvent->minuteEnd = (dropMinute + durationMinutes) % 60;
                    
                    // Handle hour overflow
                    if (state_.draggedEvent->minuteEnd >= 60) {
                        state_.draggedEvent->hourEnd++;
                        state_.draggedEvent->minuteEnd -= 60;
                    }
                    if (state_.draggedEvent->hourEnd >= 24) {
                        state_.draggedEvent->hourEnd = 23;
                        state_.draggedEvent->minuteEnd = 59;
                    }
                    
                    // Save to storage
                    StorageManager::saveEventsToStorage(eventManager_.getAllEvents());
                }
            }
            
            // End dragging
            state_.isDragging = false;
            state_.draggedEvent = nullptr;
        }
    }
    
    // Check for RIGHT-CLICKS on the time grid to create events
    bool rightClicked = ImGui::IsMouseClicked(1); // Right mouse button
    
    if (rightClicked && !state_.isDragging) {
        // Check if click is within grid bounds
        float gridLeft = grid_start.x + timeColumnWidth;
        float gridRight = grid_start.x + timeColumnWidth + (columnWidth * numDays);
        float gridTop = grid_start.y;
        float gridBottom = grid_start.y + (totalHours * hourHeight);
        
        if (mouse_pos.x >= gridLeft && mouse_pos.x < gridRight &&
            mouse_pos.y >= gridTop && mouse_pos.y < gridBottom) {
            
            // Calculate which day and hour was clicked
            int clickedDayOffset = (int)((mouse_pos.x - gridLeft) / columnWidth);
            int clickedHourIndex = (int)((mouse_pos.y - gridTop) / hourHeight);
            int clickedHour = startHour + clickedHourIndex;
            
            if (clickedDayOffset >= 0 && clickedDayOffset < numDays &&
                clickedHour >= startHour && clickedHour < endHour) {
                
                // Set the clicked day and time
                state_.selectedDay = dayInfos[clickedDayOffset][0];
                state_.currentMonth = dayInfos[clickedDayOffset][1];
                state_.currentYear = dayInfos[clickedDayOffset][2];
                state_.eventHourStart = clickedHour;
                state_.eventMinuteStart = 0;
                state_.eventHourEnd = (clickedHour + 1) % 24;
                state_.eventMinuteEnd = 0;
                state_.eventIsAllDay = false;
                state_.showAddEvent = true;
                
                // Open popup at mouse position
                ImGui::OpenPopup("AddEventPopup");
            }
        }
    }
    
    // Render events and collect data
    for (int dayOffset = 0; dayOffset < numDays; dayOffset++) {
        // Use pre-calculated day info
        int day = dayInfos[dayOffset][0];
        int month = dayInfos[dayOffset][1];
        int year = dayInfos[dayOffset][2];
        
        // Get events for this day
        auto dayEvents = eventManager_.getEventsForDate(day, month, year);
        
        // Render all-day events at the top
        std::vector<Event*> allDayEvents;
        std::vector<Event*> timedEvents;
        for (auto evt : dayEvents) {
            if (evt->isAllDay) {
                allDayEvents.push_back(evt);
            } else {
                timedEvents.push_back(evt);
            }
        }
        
        // Render timed events in the grid (only if within visible hours)
        for (auto evt : timedEvents) {
            if (evt->hourStart >= startHour && evt->hourStart < endHour) {
                // Skip rendering if this is the event being dragged (we'll draw it separately)
                bool isBeingDragged = (state_.isDragging && state_.draggedEvent == evt);
                
                // Calculate Y position relative to start hour
                float eventY = ((evt->hourStart - startHour) * hourHeight) + (evt->minuteStart * hourHeight / 60.0f);
                float eventHeight;
                if (evt->hourEnd != -1) {
                    float endY = ((evt->hourEnd - startHour) * hourHeight) + (evt->minuteEnd * hourHeight / 60.0f);
                    eventHeight = endY - eventY;
                } else {
                    eventHeight = hourHeight; // Default 1 hour
                }
                float eventX = grid_start.x + timeColumnWidth + (dayOffset * columnWidth);
                
                // Draw event block
                ImVec2 block_min(eventX + 2, grid_start.y + eventY);
                ImVec2 block_max(eventX + columnWidth - 2, grid_start.y + eventY + eventHeight - 2);
                
                // If being dragged, draw at mouse position with transparency
                if (isBeingDragged) {
                    float dragY = mouse_pos.y - (state_.dragOffsetMinutes * hourHeight / 60.0f);
                    block_min.y = dragY;
                    block_max.y = dragY + eventHeight - 2;
                    
                    // Semi-transparent while dragging
                    draw_list->AddRectFilled(block_min, block_max,
                        ImGui::ColorConvertFloat4ToU32(ImVec4(0.2f, 0.6f, 0.3f, 0.5f)));
                    draw_list->AddRect(block_min, block_max,
                        ImGui::ColorConvertFloat4ToU32(ImVec4(0.4f, 0.95f, 0.5f, 0.8f)), 0.0f, 0, 2.0f);
                } else {
                    // Normal rendering
                    draw_list->AddRectFilled(block_min, block_max,
                        ImGui::ColorConvertFloat4ToU32(ImVec4(0.2f, 0.6f, 0.3f, 0.7f)));
                    draw_list->AddRect(block_min, block_max,
                        ImGui::ColorConvertFloat4ToU32(ImVec4(0.4f, 0.95f, 0.5f, 1.0f)));
                }
                
                // Event text
                std::string timeStr = eventManager_.formatEventTime(evt);
                char eventLabel[256];
                snprintf(eventLabel, sizeof(eventLabel), "%s\n%s", 
                        timeStr.c_str(), evt->text.c_str());
                
                ImVec2 text_pos(block_min.x + 4, block_min.y + 2);
                draw_list->AddText(text_pos, 
                    ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f, 0.0f, 0.0f, 1.0f)), 
                    eventLabel);
                
                // Detect mouse down on event to start dragging
                if (!state_.isDragging && !isBeingDragged) {
                    if (mouse_pos.x >= block_min.x && mouse_pos.x <= block_max.x &&
                        mouse_pos.y >= block_min.y && mouse_pos.y <= block_max.y) {
                        
                        if (ImGui::IsMouseClicked(0)) { // Left click
                            state_.isDragging = true;
                            state_.draggedEvent = evt;
                            // Calculate offset from event start to where user clicked
                            int eventStartMinutes = (evt->hourStart - startHour) * 60 + evt->minuteStart;
                            int clickMinutes = (int)(((mouse_pos.y - grid_start.y) / hourHeight) * 60.0f);
                            state_.dragOffsetMinutes = clickMinutes - eventStartMinutes;
                        }
                    }
                }
            }
        }
        
        // Draw all-day events banner at the top if any
        if (!allDayEvents.empty()) {
            for (size_t i = 0; i < allDayEvents.size(); i++) {
                float eventX = grid_start.x + timeColumnWidth + (dayOffset * columnWidth);
                ImVec2 block_min(eventX + 2, grid_start.y - 30 + (i * 25));
                ImVec2 block_max(eventX + columnWidth - 2, grid_start.y - 30 + (i * 25) + 23);
                
                draw_list->AddRectFilled(block_min, block_max,
                    ImGui::ColorConvertFloat4ToU32(ImVec4(0.3f, 0.7f, 0.4f, 0.6f)));
                draw_list->AddRect(block_min, block_max,
                    ImGui::ColorConvertFloat4ToU32(ImVec4(0.4f, 0.9f, 0.5f, 0.8f)));
                
                draw_list->AddText(ImVec2(block_min.x + 4, block_min.y + 4),
                    ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f, 0.0f, 0.0f, 1.0f)),
                    allDayEvents[i]->text.c_str());
            }
        }
    }
    
    // Reserve space for the full grid
    ImGui::Dummy(ImVec2(timeColumnWidth + (columnWidth * numDays), totalHours * hourHeight));
}

} // namespace calendar

