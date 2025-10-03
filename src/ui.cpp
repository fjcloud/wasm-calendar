#include "ui.h"
#include "imgui.h"
#include "storage.h"
#include <ctime>
#include <cstring>
#include <cstdio>

namespace calendar {

CalendarState::CalendarState() 
    : currentMonth(0), currentYear(0), selectedDay(0),
      showAddEvent(false), weeklyView(false), weekStartDay(1),
      eventHour(9), eventMinute(0) {
    eventInput[0] = '\0';
    initCurrentDate();
}

void CalendarState::initCurrentDate() {
    time_t now = time(nullptr);
    tm* timeinfo = localtime(&now);
    currentMonth = timeinfo->tm_mon;
    currentYear = timeinfo->tm_year + 1900;
    selectedDay = timeinfo->tm_mday;
}

CalendarUI::CalendarUI(CalendarState& state, EventManager& eventManager)
    : state_(state), eventManager_(eventManager) {}

void CalendarUI::render() {
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(800, 600), ImGuiCond_Always);
    ImGui::Begin("CALENDAR", nullptr, 
                 ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);

    renderViewToggle();
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    renderNavigation();
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    renderActionButtons();
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    if (state_.weeklyView) {
        renderWeeklyView();
    } else {
        renderMonthlyView();
    }

    ImGui::End();
}

void CalendarUI::renderViewToggle() {
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 8));
    if (ImGui::Button(state_.weeklyView ? "[MONTH VIEW]" : "[WEEK VIEW]")) {
        state_.weeklyView = !state_.weeklyView;
        if (state_.weeklyView && state_.selectedDay > 0) {
            state_.weekStartDay = state_.selectedDay;
        } else if (state_.weeklyView) {
            time_t now = time(nullptr);
            tm* timeinfo = localtime(&now);
            state_.weekStartDay = timeinfo->tm_mday;
        }
    }
    ImGui::PopStyleVar();
}

void CalendarUI::renderNavigation() {
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 10));
    
    if (ImGui::Button("< PREV")) {
        if (state_.weeklyView) {
            CalendarLogic::advanceWeek(state_.weekStartDay, state_.currentMonth, state_.currentYear, -1);
        } else {
            state_.currentMonth--;
            if (state_.currentMonth < 0) {
                state_.currentMonth = 11;
                state_.currentYear--;
            }
        }
    }
    ImGui::SameLine();
    
    char title[64];
    if (state_.weeklyView) {
        int weekDates[7];
        CalendarLogic::getWeekDates(state_.weekStartDay, state_.currentMonth, state_.currentYear, weekDates);
        int lastValid = state_.weekStartDay;
        for (int i = 6; i >= 0; i--) {
            if (weekDates[i] > 0) {
                lastValid = weekDates[i];
                break;
            }
        }
        snprintf(title, sizeof(title), "Week: %s %d-%d, %d", 
                CalendarLogic::getMonthName(state_.currentMonth), 
                state_.weekStartDay, lastValid, state_.currentYear);
    } else {
        snprintf(title, sizeof(title), "%s %d", 
                CalendarLogic::getMonthName(state_.currentMonth), state_.currentYear);
    }
    ImGui::SetCursorPosX((800 - ImGui::CalcTextSize(title).x) / 2);
    ImGui::Text("%s", title);
    
    ImGui::SameLine();
    ImGui::SetCursorPosX(680);
    if (ImGui::Button("NEXT >")) {
        if (state_.weeklyView) {
            CalendarLogic::advanceWeek(state_.weekStartDay, state_.currentMonth, state_.currentYear, 1);
        } else {
            state_.currentMonth++;
            if (state_.currentMonth > 11) {
                state_.currentMonth = 0;
                state_.currentYear++;
            }
        }
    }
    ImGui::PopStyleVar();
}

void CalendarUI::renderActionButtons() {
    if (ImGui::Button("TODAY")) {
        state_.initCurrentDate();
        if (state_.weeklyView) {
            state_.weekStartDay = state_.selectedDay;
        }
    }
    ImGui::SameLine();
    if (state_.selectedDay > 0 && ImGui::Button("ADD EVENT")) {
        state_.showAddEvent = !state_.showAddEvent;
    }
}

void CalendarUI::renderWeeklyView() {
    int weekDates[7];
    CalendarLogic::getWeekDates(state_.weekStartDay, state_.currentMonth, state_.currentYear, weekDates);
    int firstDay = CalendarLogic::getFirstDayOfMonth(state_.currentMonth, state_.currentYear);
    
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 8));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 8));
    
    for (int i = 0; i < 7; i++) {
        if (weekDates[i] <= 0) continue;
        
        int dayOfWeek = (firstDay + weekDates[i] - 1) % 7;
        
        char header[32];
        snprintf(header, sizeof(header), "%s %d", 
                CalendarLogic::getDayName(dayOfWeek), weekDates[i]);
        
        bool isSelected = (weekDates[i] == state_.selectedDay);
        if (isSelected) {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
        }
        
        if (ImGui::Button(header, ImVec2(760, 35))) {
            state_.selectedDay = weekDates[i];
            state_.showAddEvent = false;
        }
        
        if (isSelected) {
            ImGui::PopStyleColor(2);
        }
        
        auto dayEvents = eventManager_.getEventsForDate(weekDates[i], state_.currentMonth, state_.currentYear);
        if (dayEvents.size() > 0) {
            ImGui::Indent(20);
            for (size_t j = 0; j < dayEvents.size(); j++) {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.7f, 0.0f, 1.0f));
                std::string timeStr = EventManager::formatEventTime(dayEvents[j]);
                if (!timeStr.empty()) {
                    ImGui::BulletText("[%s] %s", timeStr.c_str(), dayEvents[j]->text.c_str());
                } else {
                    ImGui::BulletText("%s", dayEvents[j]->text.c_str());
                }
                ImGui::PopStyleColor();
                
                if (isSelected) {
                    ImGui::SameLine();
                    ImGui::PushID((int)(2000 + i * 100 + j));
                    if (ImGui::SmallButton("[DEL]")) {
                        eventManager_.removeEvent(dayEvents[j]);
                        StorageManager::saveEventsToStorage(eventManager_.getAllEvents());
                    }
                    ImGui::PopID();
                }
            }
            ImGui::Unindent(20);
        } else if (isSelected) {
            ImGui::Indent(20);
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.5f, 0.0f, 1.0f));
            ImGui::Text("No events");
            ImGui::PopStyleColor();
            ImGui::Unindent(20);
        }
        
        if (i < 6 && weekDates[i+1] > 0) {
            ImGui::Spacing();
        }
    }
    
    ImGui::PopStyleVar(2);
    
    if (state_.showAddEvent && state_.selectedDay > 0) {
        renderWeeklyAddEventDialog();
    }
}

void CalendarUI::renderMonthlyView() {
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
                std::string timeStr = EventManager::formatEventTime(dayEvents[i]);
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
            renderMonthlyAddEventDialog();
        }
    }
}

void CalendarUI::renderWeeklyAddEventDialog() {
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    ImGui::Text("NEW EVENT FOR %s %d:", 
               CalendarLogic::getMonthName(state_.currentMonth), state_.selectedDay);
    
    ImGui::Text("TIME:");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(60);
    ImGui::InputInt("##hour", &state_.eventHour, 0, 0);
    if (state_.eventHour < 0) state_.eventHour = 0;
    if (state_.eventHour > 23) state_.eventHour = 23;
    ImGui::SameLine();
    ImGui::Text(":");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(60);
    ImGui::InputInt("##minute", &state_.eventMinute, 0, 0);
    if (state_.eventMinute < 0) state_.eventMinute = 0;
    if (state_.eventMinute > 59) state_.eventMinute = 59;
    ImGui::SameLine();
    ImGui::Text("(24hr format)");
    
    ImGui::Text("DESCRIPTION:");
    ImGui::SetNextItemWidth(600);
    ImGui::InputText("##event", state_.eventInput, sizeof(state_.eventInput));
    
    if (ImGui::Button("SAVE") && strlen(state_.eventInput) > 0) {
        Event evt;
        evt.text = state_.eventInput;
        evt.day = state_.selectedDay;
        evt.month = state_.currentMonth;
        evt.year = state_.currentYear;
        evt.hour = state_.eventHour;
        evt.minute = state_.eventMinute;
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

void CalendarUI::renderMonthlyAddEventDialog() {
    ImGui::Spacing();
    ImGui::Text("NEW EVENT:");
    
    ImGui::Text("TIME:");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(50);
    ImGui::InputInt("##hour", &state_.eventHour, 0, 0);
    if (state_.eventHour < 0) state_.eventHour = 0;
    if (state_.eventHour > 23) state_.eventHour = 23;
    ImGui::SameLine();
    ImGui::Text(":");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(50);
    ImGui::InputInt("##minute", &state_.eventMinute, 0, 0);
    if (state_.eventMinute < 0) state_.eventMinute = 0;
    if (state_.eventMinute > 59) state_.eventMinute = 59;
    
    ImGui::SetNextItemWidth(400);
    ImGui::InputText("##event", state_.eventInput, sizeof(state_.eventInput));
    ImGui::SameLine();
    if (ImGui::Button("SAVE") && strlen(state_.eventInput) > 0) {
        Event evt;
        evt.text = state_.eventInput;
        evt.day = state_.selectedDay;
        evt.month = state_.currentMonth;
        evt.year = state_.currentYear;
        evt.hour = state_.eventHour;
        evt.minute = state_.eventMinute;
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

void CalendarUI::setupTerminalStyle() {
    ImGuiStyle& style = ImGui::GetStyle();
    
    // Soft phosphor green - timeless terminal aesthetic
    ImVec4 green = ImVec4(0.4f, 0.95f, 0.5f, 1.0f);      // Softer, more pleasant green
    ImVec4 greenDim = ImVec4(0.3f, 0.75f, 0.4f, 1.0f);   // Muted for less contrast
    ImVec4 greenDark = ImVec4(0.15f, 0.4f, 0.2f, 1.0f);  // Darker tones
    ImVec4 blackBg = ImVec4(0.02f, 0.05f, 0.03f, 0.98f); // Very dark with slight green tint
    
    ImVec4* colors = style.Colors;
    colors[ImGuiCol_Text]                   = green;
    colors[ImGuiCol_TextDisabled]           = greenDark;
    colors[ImGuiCol_WindowBg]               = blackBg;
    colors[ImGuiCol_ChildBg]                = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    colors[ImGuiCol_PopupBg]                = blackBg;
    colors[ImGuiCol_Border]                 = greenDim;
    colors[ImGuiCol_BorderShadow]           = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    colors[ImGuiCol_FrameBg]                = ImVec4(0.0f, 0.15f, 0.0f, 0.54f);
    colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.0f, 0.25f, 0.0f, 0.40f);
    colors[ImGuiCol_FrameBgActive]          = ImVec4(0.0f, 0.35f, 0.0f, 0.67f);
    colors[ImGuiCol_TitleBg]                = ImVec4(0.0f, 0.2f, 0.0f, 1.0f);
    colors[ImGuiCol_TitleBgActive]          = ImVec4(0.0f, 0.3f, 0.0f, 1.0f);
    colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.0f, 0.1f, 0.0f, 0.51f);
    colors[ImGuiCol_MenuBarBg]              = ImVec4(0.0f, 0.2f, 0.0f, 1.0f);
    colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.0f, 0.05f, 0.0f, 0.53f);
    colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.0f, 0.5f, 0.0f, 1.0f);
    colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.0f, 0.6f, 0.0f, 1.0f);
    colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.0f, 0.7f, 0.0f, 1.0f);
    colors[ImGuiCol_CheckMark]              = green;
    colors[ImGuiCol_SliderGrab]             = greenDim;
    colors[ImGuiCol_SliderGrabActive]       = green;
    colors[ImGuiCol_Button]                 = ImVec4(0.0f, 0.2f, 0.0f, 0.80f);
    colors[ImGuiCol_ButtonHovered]          = ImVec4(0.0f, 0.3f, 0.0f, 1.0f);
    colors[ImGuiCol_ButtonActive]           = ImVec4(0.0f, 0.4f, 0.0f, 1.0f);
    colors[ImGuiCol_Header]                 = ImVec4(0.0f, 0.3f, 0.0f, 0.80f);
    colors[ImGuiCol_HeaderHovered]          = ImVec4(0.0f, 0.4f, 0.0f, 0.80f);
    colors[ImGuiCol_HeaderActive]           = ImVec4(0.0f, 0.5f, 0.0f, 1.0f);
    colors[ImGuiCol_Separator]              = greenDim;
    colors[ImGuiCol_SeparatorHovered]       = green;
    colors[ImGuiCol_SeparatorActive]        = green;
    colors[ImGuiCol_ResizeGrip]             = ImVec4(0.0f, 0.3f, 0.0f, 0.20f);
    colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.0f, 0.5f, 0.0f, 0.67f);
    colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.0f, 0.7f, 0.0f, 0.95f);
    colors[ImGuiCol_Tab]                    = ImVec4(0.0f, 0.2f, 0.0f, 0.86f);
    colors[ImGuiCol_TabHovered]             = ImVec4(0.0f, 0.4f, 0.0f, 0.80f);
    colors[ImGuiCol_TabActive]              = ImVec4(0.0f, 0.3f, 0.0f, 1.0f);
    colors[ImGuiCol_TabUnfocused]           = ImVec4(0.0f, 0.15f, 0.0f, 0.97f);
    colors[ImGuiCol_TabUnfocusedActive]     = ImVec4(0.0f, 0.2f, 0.0f, 1.0f);
    
    // Style settings
    style.WindowRounding = 0.0f;
    style.FrameRounding = 0.0f;
    style.FrameBorderSize = 1.0f;
    style.WindowBorderSize = 1.0f;
    style.ScrollbarRounding = 0.0f;
    style.GrabRounding = 0.0f;
}

} // namespace calendar

