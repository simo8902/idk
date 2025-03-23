//
// Created by Simeon on 4/8/2024.
//

#include "HierarchyManager.h"
#include "../../Engine/Rendering/Renderer/Renderer.h"
#include "Scene.h"
#include "LightManager.h"
#include "SelectionManager.h"
#include "GameObject.h"
#include "Camera.h"
#include "Light.h"
#include "DirectionalLight.h"
#include <imgui.h>
#include <iostream>

const std::vector<std::shared_ptr<Entity>> HierarchyManager::m_emptyVector = {};

void HierarchyManager::initialize(Renderer* renderer, std::shared_ptr<Scene> scene) {
    m_renderer = renderer;
    m_scene = scene;
}

void HierarchyManager::renderHierarchyContent() {
    if (!m_scene || !m_renderer) return;

    size_t index = 0;
    for (const auto& entity : getEntities()) {
        renderEntityRow(entity, index++);
    }

    handleSelectionClear();
}

void HierarchyManager::renderEntityRow(const std::shared_ptr<Entity>& entity, size_t index) {
    if (!entity) return;

    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);

    const std::string label = entity->getName().empty()
        ? "Entity " + std::to_string(index)
        : entity->getName();

    const std::string selectableID = label + "##" + std::to_string(index);

    const bool isSelected = (m_selectedEntity == entity);

    if (ImGui::Selectable(selectableID.c_str(), isSelected, ImGuiSelectableFlags_SpanAllColumns)) {
        selectEntity(entity);
    }

    ImGui::TableSetColumnIndex(1);
    ImGui::Text("%s", entity->getType().c_str());

    ImGui::TableSetColumnIndex(2);
    ImGui::Text("%s", entity->isVisible() ? "Visible" : "Hidden");
}

void HierarchyManager::handleSelectionClear() {
    if (ImGui::IsWindowHovered() &&
        ImGui::IsMouseClicked(0) &&
        !ImGui::IsAnyItemHovered()) {
        clearSelection();
        }
}

void HierarchyManager::selectEntity(const std::shared_ptr<Entity>& entity) {
    if (!entity) return;

    m_selectedEntity = entity;
    SelectionManager::getInstance().select(entity);
    std::cout << "Selected Entity: " << entity->getName() << std::endl;
}

std::shared_ptr<Entity> HierarchyManager::getSelectedEntity() const {
    return m_selectedEntity;
}

void HierarchyManager::clearSelection() {
    m_selectedEntity.reset();
}

const std::vector<std::shared_ptr<Entity>>& HierarchyManager::getEntities() const {
    return m_scene ? m_scene->getComponents() : m_emptyVector;
}