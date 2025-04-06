#include "ProjectExplorer.h"

#include "AssetManager.h"
#include "SelectionManager.h"
#include "imgui.h"
#include <thread>
#include "IconsFontAwesome6Brands.h"
#include "DragAndDropPayload.h"
#include <string>
#include <iostream>
#include <boost/uuid/uuid_io.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "MeshRenderer.h"
#include "SceneManager.h"
#include "stb_image.h"
#include "Light.h"
#include "Camera.h"

ProjectExplorer::ProjectExplorer(){
    std::thread([this] {
        rootFolder = AssetManager::getInstance().getRootFolder();
        if(rootFolder)
        {
            rootFolder->ScanDirectory((fs::path(SOURCE_DIR) / "ROOT").string());
        }
    }).detach();
}

ProjectExplorer::~ProjectExplorer() = default;

void ProjectExplorer::renderProjectExplorer() {
    ImGui::Begin("Project Explorer");

    auto sharedRootFolder = AssetManager::getInstance().getRootFolder();
    if (!sharedRootFolder) {
        ImGui::Text("No assets found!");
        ImGui::End();
        return;
    }

    float windowWidth   = ImGui::GetContentRegionAvail().x;
    float windowHeight  = ImGui::GetContentRegionAvail().y;
    float leftPanelWidth = 250.0f;

    ImGui::BeginChild("TreePanel", ImVec2(leftPanelWidth, windowHeight), true, ImGuiWindowFlags_HorizontalScrollbar);
    RenderFolderTree(sharedRootFolder);
    ImGui::EndChild();

    ImGui::SameLine();

    ImGui::BeginChild("ContentPanel", ImVec2(0, windowHeight), true, ImGuiWindowFlags_HorizontalScrollbar);
    {
        const auto selectedFolder = SelectionManager::getInstance().getSelectedFolder();
        const auto folderToRender = selectedFolder ? selectedFolder : sharedRootFolder;
        if (folderToRender) {
            RenderContentArea(folderToRender);
        } else {
            ImGui::Text("No folder selected.");
        }
    }
    ImGui::EndChild();

    ImGui::End();
}

void ProjectExplorer::RenderFolderTree(const std::shared_ptr<AssetItem>& folder) {
    if (!folder) return;
  //  std::cerr << "Rendering folder: " << folder->getName()
  //            << " (Virtual: " << folder->isVirtual() << ")" << std::endl;

    if (folder->getType() != AssetType::Folder) {
        return;
    }

    auto children = folder->getChildrenSafe();

    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow |
                               ImGuiTreeNodeFlags_SpanAvailWidth |
                               (folder->getChildren().empty() ? ImGuiTreeNodeFlags_Leaf : 0);

    bool nodeOpen = ImGui::TreeNodeEx(folder->getName().c_str(), flags, "%s %s",
        (folder->getType() == AssetType::Folder) ? ICON_FA_FOLDER : ICON_FA_JS,
        folder->getName().c_str());

    if (ImGui::IsItemClicked()) {
        SelectionManager::getInstance().selectFolder(folder);
    }

    if (nodeOpen) {
        if (!folder->isScanned && !folder->isVirtual()) {
            folder->ScanDirectory(folder->getPath());
        }

        /*
        if (!folder->isScanned) {
            if (folder->isVirtual()) {
                std::cerr << "Virtual folder: " << folder->getName() << std::endl;
                folder->PopulateVirtualChildren();
            } else {
             //   folder->ScanDirectory(folder->getPath());
            }
        }*/

        for (const auto& child : children) {
            if (child->getType() == AssetType::Folder ||
                child->getType() == AssetType::GameObject||
                child->getType() == AssetType::Entity) {
                    RenderFolderTree(child);
                }
        }
        ImGui::TreePop();
    }
}

void ProjectExplorer::RenderContentArea(const std::shared_ptr<AssetItem>& folder) {
    if (!folder) return;

    const float iconSize = 80.0f;
    const float padding = 10.0f;
    const int itemsPerRow = static_cast<int>(ImGui::GetContentRegionAvail().x / (iconSize + padding));

    ImGui::BeginChild("ContentArea");
    if (ImGui::BeginTable("##ContentTable", itemsPerRow)) {
        for (const auto& child : folder->getChildren()) {

            ImGui::TableNextColumn();
            if (child->getType() == AssetType::GameObject) {
               // std::cerr << "ContentArea GameObject: " << child->getGameObject()->getName() << std::endl;
               // RenderGameObject(child->getGameObject(), iconSize);
            }else if (child->getType() == AssetType::Entity) {
                if (!printLog) {
                    std::cerr << "ContentArea EntityObjs: " << child->getEntity()->getName() << std::endl;
                    printLog = true;
                }
                RenderGameObject(child->getEntity(), iconSize);
            }
            else {
                RenderAssetItem(child, iconSize);
            }
        }

        /*
        // Then render the GameObjects from the scene (only once, outside the folder structure)
        for (const auto& obj : SceneManager::getInstance().getAllObjects()) {
            ImGui::TableNextColumn();
            RenderGameObject(obj, iconSize);
        }
        for (const auto& weakVirtualAsset : AssetManager::getInstance().getVirtualAssets()) {
            if (auto virtualAsset = weakVirtualAsset.lock()) {
                ImGui::TableNextColumn();
                RenderAssetItem(virtualAsset, iconSize);
                itemCount++;
            }
        }*/

        ImGui::EndTable();
    }

    ImGui::EndChild();
}

void ProjectExplorer::RenderAssetItem(const std::shared_ptr<AssetItem>& asset, float iconSize) {
    ImGui::PushID(asset->getUUIDStr().c_str());

    const char* icon = "[File]";
    if (asset->getType() == AssetType::Folder) icon = ICON_FA_FOLDER;
    else if (asset->getType() == AssetType::Shader) icon = "[Code]";
    else if (asset->getType() == AssetType::Mesh) icon = "[Cube]";

    if (asset->isVirtual()) {
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(100, 255, 100, 255));
    }

    ImGui::Button((std::string(icon) + "##" + asset->getName()).c_str(), ImVec2(iconSize, iconSize));

    if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {
        SelectionManager::getInstance().selectFolder(asset);
    }

    if (ImGui::BeginDragDropSource()) {
        ImGui::SetDragDropPayload("ASSET_ITEM", &asset, sizeof(asset));
        ImGui::Text("%s %s", icon, asset->getName().c_str());
        ImGui::EndDragDropSource();
    }

    ImGui::TextWrapped("%s", asset->getName().c_str());

    if (asset->isVirtual()) {
        ImGui::PopStyleColor();
    }

    ImGui::PopID();
}

void ProjectExplorer::RenderGameObject(const std::shared_ptr<Entity>& entity, float iconSize) {
    if (!entity) return;

    ImGui::PushID(entity->getName().c_str());

    const char* icon = "[Object]";

    if (entity->hasComponent<IDK::Graphics::Light>()) icon = "[Light]";
    if (entity->hasComponent<IDK::Graphics::Camera>()) icon = "[Camera]";
   // if (entity->hasComponent<IDK::Graphics::MeshRenderer>()) icon = "[Mesh]";

    if (ImGui::Button((std::string(icon) + "##" + entity->getName()).c_str(), ImVec2(iconSize, iconSize))) {
        SelectionManager::getInstance().select(entity);
    }

    if (ImGui::IsItemClicked()) {
        SelectionManager::getInstance().select(entity);
    }

    ImGui::TextWrapped("%s", entity->getName().c_str());

    ImGui::PopID();
}

void ProjectExplorer::RenderAssetItemAsIcon(const std::shared_ptr<AssetItem>& item, const float & iconSize) {
    if (!item) return;

    const char* icon = GetAssetIcon(item->getType());
    // Ensure icon is not empty
    if (!icon || strlen(icon) == 0) {
        icon = "[Unknown]";
    }

    ImGui::BeginGroup();


    /*
    std::string prefix;
    if (item->getType() == AssetType::Material) {
        
        auto material = std::dynamic_pointer_cast<Material>(item);
        if (material) {
            prefix = material->isPredefinedMaterial() ? "[Predefined] " : "[File-based] ";
        }
    }

   // const std::string uniqueID = boost::uuids::to_string(item->getUUID());
    const std::string buttonLabel = prefix + icon + "##" + uniqueID;

    const float buttonSizeMultiplier = 1.25f;
    const ImVec2 buttonSize(iconSize * buttonSizeMultiplier, iconSize * buttonSizeMultiplier);

    bool isSelected = SelectionManager::getInstance().isItemSelected(item);


    if (ImGui::Button(buttonLabel.c_str(), buttonSize)) {
        clickedInsideSelectable = true;
        SelectionManager::getInstance().toggleSelectItem(item);

        switch (item->getType()) {
            case AssetType::Folder:
                SelectionManager::getInstance().selectFolder(item);
            break;
            case AssetType::PredefinedShader:
                SelectionManager::getInstance().selectShader(std::static_pointer_cast<Shader>(item));
            break;
            
            case AssetType::Material:
                SelectionManager::getInstance().selectMaterial(std::static_pointer_cast<Material>(item));
            break;
            default:
                break;
        }
    }

    if (isSelected) {
        ImVec2 itemMin = ImGui::GetItemRectMin();
        ImVec2 itemMax = ImGui::GetItemRectMax();
        ImGui::GetWindowDrawList()->AddRectFilled(itemMin, itemMax, ImGui::GetColorU32(ImGuiCol_FrameBgHovered));
    }

    // Handle hover effects
    if (ImGui::IsItemHovered()) {
        ImVec2 itemMin = ImGui::GetItemRectMin();
        ImVec2 itemMax = ImGui::GetItemRectMax();
        ImGui::GetWindowDrawList()->AddRect(itemMin, itemMax, ImGui::GetColorU32(ImGuiCol_ButtonHovered), 4.0f);
    }

    if (item->getType() == AssetType::Material) {
        
        auto material = std::dynamic_pointer_cast<Material>(item);
        if (material) {
            std::string materialUniqueID = material->getUUIDStr();

            // Validate materialUniqueID
            if (materialUniqueID.empty()) {
                std::cerr << "[RenderAssetItemAsIcon] Warning: materialUniqueID is empty for material: " << material->getName() << std::endl;
                materialUniqueID = "invalid_material_uuid";
            }

            
            // Display material status
            if (material->isPredefinedMaterial()) {
                ImGui::TextColored(ImVec4(0.9f, 0.7f, 0.0f, 1.0f), "Predefined Material");
            } else {
                ImGui::Text("File-based Material");
            }
        }


        // Drag and drop handling for materials
        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
            const std::string uuidStr = boost::uuids::to_string(item->getUUID());
            ImGui::SetDragDropPayload(PAYLOAD_MATERIAL, uuidStr.c_str(), sizeof(char) * (uuidStr.size() + 1));
            ImGui::Text("Dragging %s", item->getName().c_str());
            ImGui::EndDragDropSource();
        }
    
    }else if (item->getType() == AssetType::Shader) {
        // Drag and drop handling for shaders
        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
            const std::string uuidStr = boost::uuids::to_string(item->getUUID());
            ImGui::SetDragDropPayload(PAYLOAD_SHADER, uuidStr.c_str(), sizeof(char) * (uuidStr.size() + 1));
            ImGui::Text("Dragging %s", item->getName().c_str());
            ImGui::EndDragDropSource();
        }
    }

    float textWidth = ImGui::CalcTextSize(item->getName().c_str()).x;
    float maxTextWidth = iconSize * 1.5f;
    if (textWidth > maxTextWidth) {
        ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + maxTextWidth);
        ImGui::TextWrapped("%s", item->getName().c_str());
        ImGui::PopTextWrapPos();
    } else {
        float textOffset = (iconSize - textWidth) / 2.0f;
        if (textOffset > 0) {
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + textOffset);
        }
        ImGui::Text("%s", item->getName().c_str());
    }
    */
    ImGui::EndGroup();
}
std::vector<std::string> ProjectExplorer::SplitTextIntoLines(const std::string& text, float maxWidth, const ImFont* font) {
    std::vector<std::string> lines;
    std::istringstream iss(text);
    std::string word, currentLine;

    const ImFont* usedFont = font ? font : ImGui::GetFont();

    while (iss >> word) {
        std::string testLine = currentLine.empty() ? word : currentLine + " " + word;
        float lineWidth = usedFont->CalcTextSizeA(usedFont->FontSize, FLT_MAX, 0.0f, testLine.c_str()).x;

        if (lineWidth > maxWidth && !currentLine.empty()) {
            lines.push_back(currentLine);
            currentLine = word;
        } else {
            currentLine = testLine;
        }
    }
    if (!currentLine.empty()) {
        lines.push_back(currentLine);
    }
    return lines;
}



void ProjectExplorer::HandleFolderPopups(const std::shared_ptr<AssetItem>& folder) {
    if (ImGui::BeginPopupModal("Create New Folder", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        static char newFolderName[256] = "";
        ImGui::InputText("Folder Name", newFolderName, IM_ARRAYSIZE(newFolderName));
        if (ImGui::Button("Create")) {

           // AssetManager::getInstance().createFolder(folder, newFolderName);
            newFolderName[0] = '\0';
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();

        if (ImGui::Button("Cancel")) {
            newFolderName[0] = '\0';
            ImGui::CloseCurrentPopup();}

        ImGui::EndPopup();
    }

    // TODO: renameFolderPopupOpen and deleteFolderPopupOpen
}

const char* ProjectExplorer::GetAssetIcon(const AssetType & type) {
    switch (type) {
        case AssetType::RuntimeShader:
            return !shaderIcon.empty() ? shaderIcon.c_str() : "[Shader]";
        case AssetType::PredefinedShader:
            return !shaderIcon.empty() ? shaderIcon.c_str() : "[Shader]";
        case AssetType::Material:
            return !materialIcon.empty() ? materialIcon.c_str() : "[Material]";
        case AssetType::Folder:
            return !folderIcon.empty() ? folderIcon.c_str() : "[Folder]";
        default:
            return "[Asset]";
    }
}


GLuint ProjectExplorer::loadTexture(const std::string& filePath) {
    int width, height, channels;
    unsigned char* data = stbi_load(filePath.c_str(), &width, &height, &channels, 4);
    if (!data) {
        std::cerr << "Failed to load texture: " << filePath << std::endl;
        return 0;
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return textureID;
}
void ProjectExplorer::initializeTextures() {
    shaderIconTexture = loadTexture(SOURCE_DIR "/src/data/gui/shaderIcon.png");
    if (shaderIconTexture == 0) {
        std::cerr << "Failed to load shader icon texture!" << std::endl;
    }
}