#include "ui.h"
#include "imgui.h"
#include "../core/storage.h"
#include <cstring>
#include <cstdio>

namespace calendar {

void CalendarUI::renderAddEventDialog() {
    // Show as popup when triggered by right-click
    if (ImGui::BeginPopup("AddEventPopup")) {
        ImGui::Text("NEW EVENT: %s %d", 
                   CalendarLogic::getMonthName(state_.currentMonth), state_.selectedDay);
        ImGui::Separator();
        
        ImGui::Checkbox("All-day", &state_.eventIsAllDay);
        
        if (!state_.eventIsAllDay) {
            ImGui::Text("START:");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(50);
            ImGui::InputInt("##hourStart", &state_.eventHourStart, 0, 0);
            if (state_.eventHourStart < 0) state_.eventHourStart = 0;
            if (state_.eventHourStart > 23) state_.eventHourStart = 23;
            ImGui::SameLine();
            ImGui::Text(":");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(50);
            ImGui::InputInt("##minuteStart", &state_.eventMinuteStart, 0, 0);
            if (state_.eventMinuteStart < 0) state_.eventMinuteStart = 0;
            if (state_.eventMinuteStart > 59) state_.eventMinuteStart = 59;
            
            ImGui::Text("END:");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(50);
            ImGui::InputInt("##hourEnd", &state_.eventHourEnd, 0, 0);
            if (state_.eventHourEnd < 0) state_.eventHourEnd = 0;
            if (state_.eventHourEnd > 23) state_.eventHourEnd = 23;
            ImGui::SameLine();
            ImGui::Text(":");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(50);
            ImGui::InputInt("##minuteEnd", &state_.eventMinuteEnd, 0, 0);
            if (state_.eventMinuteEnd < 0) state_.eventMinuteEnd = 0;
            if (state_.eventMinuteEnd > 59) state_.eventMinuteEnd = 59;
        }
        
        ImGui::Text("DESCRIPTION:");
        ImGui::SetNextItemWidth(300);
        ImGui::InputText("##event", state_.eventInput, sizeof(state_.eventInput));
        
        ImGui::Spacing();
        
        if (ImGui::Button("SAVE", ImVec2(140, 0)) && strlen(state_.eventInput) > 0) {
            Event evt;
            evt.text = state_.eventInput;
            evt.day = state_.selectedDay;
            evt.month = state_.currentMonth;
            evt.year = state_.currentYear;
            evt.isAllDay = state_.eventIsAllDay;
            
            if (state_.eventIsAllDay) {
                evt.hourStart = -1;
                evt.minuteStart = 0;
                evt.hourEnd = -1;
                evt.minuteEnd = 0;
            } else {
                evt.hourStart = state_.eventHourStart;
                evt.minuteStart = state_.eventMinuteStart;
                evt.hourEnd = state_.eventHourEnd;
                evt.minuteEnd = state_.eventMinuteEnd;
            }
            
            eventManager_.addEvent(evt);
            StorageManager::saveEventsToStorage(eventManager_.getAllEvents());
            state_.eventInput[0] = '\0';
            state_.showAddEvent = false;
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("CANCEL", ImVec2(140, 0))) {
            state_.showAddEvent = false;
            state_.eventInput[0] = '\0';
            ImGui::CloseCurrentPopup();
        }
        
        ImGui::EndPopup();
    }
    
    // Also show inline for Month view (when using ADD EVENT button)
    if (state_.showAddEvent && !ImGui::IsPopupOpen("AddEventPopup") && state_.viewMode == VIEW_MONTH) {
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        ImGui::Text("NEW EVENT FOR %s %d:", 
                   CalendarLogic::getMonthName(state_.currentMonth), state_.selectedDay);
        
        ImGui::Checkbox("All-day event", &state_.eventIsAllDay);
        
        if (!state_.eventIsAllDay) {
            ImGui::Text("START:");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(50);
            ImGui::InputInt("##hourStart", &state_.eventHourStart, 0, 0);
            if (state_.eventHourStart < 0) state_.eventHourStart = 0;
            if (state_.eventHourStart > 23) state_.eventHourStart = 23;
            ImGui::SameLine();
            ImGui::Text(":");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(50);
            ImGui::InputInt("##minuteStart", &state_.eventMinuteStart, 0, 0);
            if (state_.eventMinuteStart < 0) state_.eventMinuteStart = 0;
            if (state_.eventMinuteStart > 59) state_.eventMinuteStart = 59;
            
            ImGui::SameLine();
            ImGui::Text("  END:");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(50);
            ImGui::InputInt("##hourEnd", &state_.eventHourEnd, 0, 0);
            if (state_.eventHourEnd < 0) state_.eventHourEnd = 0;
            if (state_.eventHourEnd > 23) state_.eventHourEnd = 23;
            ImGui::SameLine();
            ImGui::Text(":");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(50);
            ImGui::InputInt("##minuteEnd", &state_.eventMinuteEnd, 0, 0);
            if (state_.eventMinuteEnd < 0) state_.eventMinuteEnd = 0;
            if (state_.eventMinuteEnd > 59) state_.eventMinuteEnd = 59;
        }
        
        ImGui::Text("DESCRIPTION:");
        ImGui::SetNextItemWidth(600);
        ImGui::InputText("##event", state_.eventInput, sizeof(state_.eventInput));
        
        if (ImGui::Button("SAVE") && strlen(state_.eventInput) > 0) {
            Event evt;
            evt.text = state_.eventInput;
            evt.day = state_.selectedDay;
            evt.month = state_.currentMonth;
            evt.year = state_.currentYear;
            evt.isAllDay = state_.eventIsAllDay;
            
            if (state_.eventIsAllDay) {
                evt.hourStart = -1;
                evt.minuteStart = 0;
                evt.hourEnd = -1;
                evt.minuteEnd = 0;
            } else {
                evt.hourStart = state_.eventHourStart;
                evt.minuteStart = state_.eventMinuteStart;
                evt.hourEnd = state_.eventHourEnd;
                evt.minuteEnd = state_.eventMinuteEnd;
            }
            
            eventManager_.addEvent(evt);
            StorageManager::saveEventsToStorage(eventManager_.getAllEvents());
            state_.eventInput[0] = '\0';
            state_.showAddEvent = false;
        }
        ImGui::SameLine();
        if (ImGui::Button("CANCEL")) {
            state_.showAddEvent = false;
            state_.eventInput[0] = '\0';
        }
    }
}

} // namespace calendar

