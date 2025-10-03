#include "ui.h"
#include "imgui.h"
#include "storage.h"
#include <cstdio>

namespace calendar {

void CalendarUI::renderDayView() {
    renderTimeGrid(state_.selectedDay, 1);
    
    if (state_.showAddEvent && state_.selectedDay > 0) {
        renderAddEventDialog();
    }
}

void CalendarUI::renderWeekView() {
    // Ensure we start on Monday
    int mondayDay, mondayMonth, mondayYear;
    CalendarLogic::getMondayOfWeek(state_.weekStartDay, state_.currentMonth, state_.currentYear,
                                   mondayDay, mondayMonth, mondayYear);
    
    // Update state to Monday
    state_.weekStartDay = mondayDay;
    state_.currentMonth = mondayMonth;
    state_.currentYear = mondayYear;
    
    renderTimeGrid(mondayDay, 7);
    
    if (state_.showAddEvent && state_.selectedDay > 0) {
        renderAddEventDialog();
    }
}

void CalendarUI::renderMonthView() {
    // Day headers
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 4));
    for (int i = 0; i < 7; i++) {
        ImGui::Button(CalendarLogic::getDayName(i), ImVec2(106, 30));
        if (i < 6) ImGui::SameLine();
    }
    ImGui::PopStyleVar();
    ImGui::Spacing();

    // Calendar grid
    int firstDay = CalendarLogic::getFirstDayOfMonth(state_.currentMonth, state_.currentYear);
    int daysInMonth = CalendarLogic::getDaysInMonth(state_.currentMonth, state_.currentYear);
    int day = 1;

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12, 12));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 4));
    
    for (int week = 0; week < 6 && day <= daysInMonth; week++) {
        for (int dow = 0; dow < 7; dow++) {
            if (week == 0 && dow < firstDay) {
                char emptyId[32];
                snprintf(emptyId, sizeof(emptyId), "empty_%d_%d", week, dow);
                ImGui::InvisibleButton(emptyId, ImVec2(106, 50));
            } else if (day <= daysInMonth) {
                char buttonLabel[32];
                auto dayEvents = eventManager_.getEventsForDate(day, state_.currentMonth, state_.currentYear);
                
                if (dayEvents.size() > 0) {
                    snprintf(buttonLabel, sizeof(buttonLabel), "%d*", day);
                } else {
                    snprintf(buttonLabel, sizeof(buttonLabel), "%d", day);
                }
                
                bool isSelected = (day == state_.selectedDay);
                
                if (isSelected) {
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.0f, 0.8f, 0.0f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0f, 0.6f, 0.0f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
                }
                
                if (ImGui::Button(buttonLabel, ImVec2(106, 50))) {
                    state_.selectedDay = day;
                    state_.showAddEvent = false;
                }
                
                if (isSelected) {
                    ImGui::PopStyleColor(4);
                }
                
                day++;
            } else {
                char emptyId[32];
                snprintf(emptyId, sizeof(emptyId), "empty_end_%d_%d", week, dow);
                ImGui::InvisibleButton(emptyId, ImVec2(106, 50));
            }
            
            if (dow < 6) ImGui::SameLine();
        }
    }
    ImGui::PopStyleVar(2);

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // Show selected date and events
    if (state_.selectedDay > 0) {
        ImGui::Text("SELECTED: %s %d, %d", 
                   CalendarLogic::getMonthName(state_.currentMonth), 
                   state_.selectedDay, state_.currentYear);
        
        auto dayEvents = eventManager_.getEventsForDate(state_.selectedDay, state_.currentMonth, state_.currentYear);
        if (dayEvents.size() > 0) {
            ImGui::Spacing();
            ImGui::Text("EVENTS:");
            for (size_t i = 0; i < dayEvents.size(); i++) {
                std::string timeStr = eventManager_.formatEventTime(dayEvents[i]);
                if (!timeStr.empty()) {
                    ImGui::BulletText("[%s] %s", timeStr.c_str(), dayEvents[i]->text.c_str());
                } else {
                    ImGui::BulletText("%s", dayEvents[i]->text.c_str());
                }
                ImGui::SameLine();
                ImGui::PushID((int)(1000 + i));
                if (ImGui::SmallButton("[DEL]")) {
                    eventManager_.removeEvent(dayEvents[i]);
                    StorageManager::saveEventsToStorage(eventManager_.getAllEvents());
                }
                ImGui::PopID();
            }
        }
        
        if (state_.showAddEvent) {
            renderAddEventDialog();
        }
    }
}

} // namespace calendar

