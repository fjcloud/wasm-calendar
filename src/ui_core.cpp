#include "ui.h"
#include "imgui.h"
#include <ctime>

namespace calendar {

CalendarState::CalendarState() 
    : currentMonth(0), currentYear(0), selectedDay(0),
      showAddEvent(false), viewMode(VIEW_MONTH), weekStartDay(1),
      eventHourStart(9), eventMinuteStart(0),
      eventHourEnd(10), eventMinuteEnd(0), eventIsAllDay(false),
      isDragging(false), draggedEvent(nullptr), dragOffsetMinutes(0) {
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
    // Use actual display size for responsive layout
    ImGuiIO& io = ImGui::GetIO();
    ImVec2 displaySize = io.DisplaySize;
    
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetNextWindowSize(displaySize, ImGuiCond_Always);
    ImGui::Begin("CALENDAR", nullptr, 
                 ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse |
                 ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

    renderViewSelector();
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

    switch (state_.viewMode) {
        case VIEW_DAY:
            renderDayView();
            break;
        case VIEW_WEEK:
            renderWeekView();
            break;
        case VIEW_MONTH:
            renderMonthView();
            break;
    }

    ImGui::End();
}

void CalendarUI::renderViewSelector() {
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 8));
    
    if (ImGui::Button("DAY")) {
        state_.viewMode = VIEW_DAY;
    }
    ImGui::SameLine();
    if (ImGui::Button("WEEK")) {
        state_.viewMode = VIEW_WEEK;
        if (state_.selectedDay > 0) {
            // Always start week on Monday
            CalendarLogic::getMondayOfWeek(state_.selectedDay, state_.currentMonth, state_.currentYear,
                                          state_.weekStartDay, state_.currentMonth, state_.currentYear);
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("MONTH")) {
        state_.viewMode = VIEW_MONTH;
    }
    
    ImGui::PopStyleVar();
}

void CalendarUI::renderNavigation() {
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 10));
    
    if (ImGui::Button("< PREV")) {
        if (state_.viewMode == VIEW_DAY) {
            state_.selectedDay--;
            if (state_.selectedDay < 1) {
                state_.currentMonth--;
                if (state_.currentMonth < 0) {
                    state_.currentMonth = 11;
                    state_.currentYear--;
                }
                state_.selectedDay = CalendarLogic::getDaysInMonth(state_.currentMonth, state_.currentYear);
            }
        } else if (state_.viewMode == VIEW_WEEK) {
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
    if (state_.viewMode == VIEW_DAY) {
        snprintf(title, sizeof(title), "%s %d, %d", 
                CalendarLogic::getMonthName(state_.currentMonth), 
                state_.selectedDay, state_.currentYear);
    } else if (state_.viewMode == VIEW_WEEK) {
        int weekDates[7];
        CalendarLogic::getWeekDates(state_.weekStartDay, state_.currentMonth, state_.currentYear, weekDates);
        int lastValid = state_.weekStartDay;
        for (int i = 6; i >= 0; i--) {
            if (weekDates[i] > 0) {
                lastValid = weekDates[i];
                break;
            }
        }
        snprintf(title, sizeof(title), "%s %d-%d, %d", 
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
        if (state_.viewMode == VIEW_DAY) {
            state_.selectedDay++;
            int daysInMonth = CalendarLogic::getDaysInMonth(state_.currentMonth, state_.currentYear);
            if (state_.selectedDay > daysInMonth) {
                state_.currentMonth++;
                if (state_.currentMonth > 11) {
                    state_.currentMonth = 0;
                    state_.currentYear++;
                }
                state_.selectedDay = 1;
            }
        } else if (state_.viewMode == VIEW_WEEK) {
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
        if (state_.viewMode == VIEW_WEEK) {
            // Always start week on Monday
            CalendarLogic::getMondayOfWeek(state_.selectedDay, state_.currentMonth, state_.currentYear,
                                          state_.weekStartDay, state_.currentMonth, state_.currentYear);
        }
    }
    ImGui::SameLine();
    if (state_.selectedDay > 0 && ImGui::Button("ADD EVENT")) {
        state_.showAddEvent = !state_.showAddEvent;
    }
}

void CalendarUI::setupTerminalStyle() {
    ImGuiStyle& style = ImGui::GetStyle();
    
    // Soft phosphor green - timeless terminal aesthetic
    ImVec4 green = ImVec4(0.4f, 0.95f, 0.5f, 1.0f);
    ImVec4 greenDim = ImVec4(0.3f, 0.75f, 0.4f, 1.0f);
    ImVec4 greenDark = ImVec4(0.15f, 0.4f, 0.2f, 1.0f);
    ImVec4 blackBg = ImVec4(0.02f, 0.05f, 0.03f, 0.98f);
    
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

