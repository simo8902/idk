//
// Created by Simeon on 2/8/2025.
//

#include "libData.h"
#include "imgui.h"

#include <iostream>

ImGuiContext* g_ImGuiContext = nullptr;


void imguieffects() {
    if (!g_ImGuiContext) {
        std::cerr << "ImGui context is null in imguieffects!" << std::endl;
        return;
    }
    ImGui::SetCurrentContext(g_ImGuiContext);

    ImGuiStyle &style = ImGui::GetStyle();

    // Padding and spacing
    style.WindowPadding = ImVec2(2.0f, 0.0f);
    style.FramePadding = ImVec2(10.0f, 5.0f);
    style.ItemSpacing = ImVec2(8.0f, 3.0f);
    style.ItemInnerSpacing = ImVec2(0.0f, 0.0f);
    style.TouchExtraPadding = ImVec2(1.0f, 0.0f);

    // Borders
    style.WindowBorderSize = 1.0f;
    style.FrameBorderSize = 0.0f;
    style.TabBorderSize = 0.0f;
    style.ChildBorderSize = 0.0f;

    // Indentation
    style.IndentSpacing = 0.0f;

    // Tab bar appearance
    style.TabBarOverlineSize = 2.0f;

    // Rounding
    style.WindowRounding = 0.0f;
    style.TabRounding = 4.0f;
    style.FrameRounding = 0.0f;

    constexpr auto RED = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
    constexpr auto BLACK = ImVec4(0.12f, 0.12f, 0.12f, 1.0f);
    constexpr auto DARK_GREY = ImVec4(0.18f, 0.18f, 0.18f, 1.0f);
    constexpr auto TEXT = ImVec4(0.8f, 0.8f, 0.8f, 1.0f);
    constexpr auto FRAME = ImVec4(0.10f, 0.10f, 0.10f, 1.0f);
    constexpr auto TOOLBAR_BG = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);
    // BG and frame
    style.Colors[ImGuiCol_WindowBg] = TOOLBAR_BG;

    style.Colors[ImGuiCol_FrameBg] = BLACK;
    style.Colors[ImGuiCol_FrameBgHovered] = RED;
    style.Colors[ImGuiCol_FrameBgActive] = RED;

    // Title and border
    style.Colors[ImGuiCol_TitleBg] = BLACK;
    style.Colors[ImGuiCol_TitleBgCollapsed] = BLACK;
    style.Colors[ImGuiCol_TitleBgActive] = BLACK;
    style.Colors[ImGuiCol_Border] = FRAME;

    // Toolbar and menubar
    style.Colors[ImGuiCol_MenuBarBg] = TOOLBAR_BG;
    style.Colors[ImGuiCol_Tab] = BLACK;
    style.Colors[ImGuiCol_TabHovered] = RED;
    style.Colors[ImGuiCol_TabSelected] = BLACK;
    style.Colors[ImGuiCol_TabDimmedSelected] = DARK_GREY;
    style.Colors[ImGuiCol_TabDimmedSelectedOverline] = BLACK;
    style.Colors[ImGuiCol_TabSelectedOverline] = BLACK;

    // Buttons and headers
    style.Colors[ImGuiCol_Button] = BLACK;
    style.Colors[ImGuiCol_ButtonHovered] = RED;
    style.Colors[ImGuiCol_ButtonActive] = RED;
    style.Colors[ImGuiCol_Header] = BLACK;
    style.Colors[ImGuiCol_HeaderHovered] = BLACK;
    style.Colors[ImGuiCol_HeaderActive] = BLACK;

    // Resize grips
    style.Colors[ImGuiCol_ResizeGrip] = FRAME;
    style.Colors[ImGuiCol_ResizeGripActive] = FRAME;
    style.Colors[ImGuiCol_ResizeGripHovered] = FRAME;

    // Text and others
    style.Colors[ImGuiCol_Text] = TEXT;
    style.Colors[ImGuiCol_TextDisabled] = RED;
    style.Colors[ImGuiCol_BorderShadow] = BLACK;

    style.WindowMenuButtonPosition = ImGuiDir_None;

    // style.Colors[ImGuiCol_Button] = style.Colors[ImGuiCol_WindowBg];
}

void hierarchyeffects() {
    ImGuiStyle& style = ImGui::GetStyle();

    style.WindowPadding = ImVec2(5.0f, 5.0f);
    style.FramePadding = ImVec2(8.0f, 6.0f);
    style.CellPadding = ImVec2(6.0f, 6.0f);
    style.ItemSpacing = ImVec2(8.0f, 4.0f);
    style.ItemInnerSpacing = ImVec2(6.0f, 6.0f);
    style.IndentSpacing = 25.0f;
    style.Colors[ImGuiCol_WindowBg]         = ImVec4(0.13f, 0.13f, 0.13f, 1.0f);
    style.Colors[ImGuiCol_ChildBg]          = ImVec4(0.15f, 1.1f, 1.15f, 0.0f);
    style.Colors[ImGuiCol_FrameBg]          = ImVec4(0.07f, 0.07f, 0.07f, 1.0f);
    style.Colors[ImGuiCol_Border]           = ImVec4(0.0f, 0.40f, 0.40f, 0.0f);
    style.Colors[ImGuiCol_TableBorderLight] = ImVec4(0.10f, 0.1f, 0.12f, 1.0f);
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.62f, 0.2f, 0.05f, 1.0f);
    style.Colors[ImGuiCol_TabDimmedSelected] = ImVec4(0.14f, 0.06f, 0.2f, 1.0f);
}