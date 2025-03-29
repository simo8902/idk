//
// Created by Simeon on 10/7/2024.
//

#ifndef ASSETITEM_H
#define ASSETITEM_H

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <filesystem>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "SceneManager.h"
#include <mutex>

#include "Entity.h"

namespace fs = std::filesystem;

enum class AssetType {
    Folder = 0,
    Shader=1,
    PredefinedShader = 2,
    RuntimeShader = 3,
    Material = 4,
    Mesh = 5,
    File = 6,
    GameObject = 7,
    Entity = 8,
    Unknown = -1
};

class AssetItem : public std::enable_shared_from_this<AssetItem>
{
public:
    AssetItem(const std::string& name, const AssetType type, const std::string& path, bool isVirtual = false)
        : isVirtual_(isVirtual), name_(name), type_(type), path_(path),
            uuid_(boost::uuids::random_generator()()) {

        std::cerr << "[AssetItem.h] " << "Name::" << name
                  << " Type::" << static_cast<int>(type)
                  << " Virtual::" << (isVirtual ? "Yes" : "No") << std::endl;

    }
    virtual ~AssetItem() {}

    bool isVirtual() const { return isVirtual_; }

    const std::string& getUUIDStr() const {
        if (uuidStr_.empty()) {
            uuidStr_ = boost::uuids::to_string(uuid_);
        }
        return uuidStr_;
    }

    std::shared_ptr<AssetItem> getChildByName(const std::string& name) {
        std::lock_guard<std::mutex> lock(mutex_);
        std::cerr << "[AssetItem.h] Looking for child: " << name << " in " << getName() << std::endl;
        for (const auto& child : children) {
            std::cerr << "[AssetItem.h] Checking child: " << child->getName() << std::endl;
            if (!child) {
                std::cerr << "[AssetItem.h] WARNING: Null child in " << getName() << std::endl;
                continue;
            }

            if (child->getName() == name) {
                std::cout << "[AssetItem.h] Found child: " << child->getName() << std::endl;
                return child;
            }
        }

        /*
        if (name == "Scene") {
            std::cerr << "Creating new Scene folder" << std::endl;
            auto sceneFolder = std::make_shared<AssetItem>("Scene", AssetType::Folder, "", true);
            children.push_back(sceneFolder);
            return sceneFolder;
        }*/

        std::cerr << "C[AssetItem.h] hild not found: " << name << std::endl;
        return nullptr;
    }

    void PopulateVirtualChildren() {
       // if (!isVirtual_ || isScanned) return;

        if (!isVirtual_) return;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            if (isScanned) return; // Already scanned?
            isScanned = true;
            children.clear();
        }

        /*
        if (name_ == "Scene") {
            for (const auto& obj : SceneManager::getInstance().getRootObjects()) {
                auto child = std::make_shared<AssetItem>(obj->getName(), AssetType::GameObject, "", true);
                child->setGameObject(obj);
                addChild(child); // addChild() should lock internally.
                std::cerr << "Added GameObject: " << obj->getName() << " (Virtual)" << std::endl;
            }
        }*/
        if (name_ == "ROOT") {
            for (const auto& obj : SceneManager::getInstance().getRootObjects()) {
                auto entity = std::make_shared<Entity>(obj);
                auto child = std::make_shared<AssetItem>(obj->getName(), AssetType::Entity, "", true);
                child->setEntityObject(entity);
                addChild(child);
                std::cerr << "Added Entity: " << obj->getName() << " directly to ROOT (Virtual)" << std::endl;
            }
        } else
        if (type_ == AssetType::GameObject) {
            if (auto obj = gameObject_.lock()) {
                for (const auto& childObj : obj->getChildren()) {
                    auto child = std::make_shared<AssetItem>(childObj->getName(), AssetType::GameObject, "", true);
                    child->setGameObject(childObj);
                    addChild(child);
                    std::cerr << "[AssetItem.h] Added child GameObject: " << childObj->getName() << " (Virtual)" << std::endl;
                }
            }
        }else if (type_ == AssetType::Entity) {
            if (const auto &obj = entities.lock()) {
                for (const auto& childObj : obj->getChildren()) {
                    auto child = std::make_shared<AssetItem>(childObj->getName(), AssetType::Entity, "", true);
                    child->setEntityObject(childObj);
                    addChild(child);
                    std::cerr << "[AssetItem.h] Added child EntityObject: " << childObj->getName() << " (Virtual)" << std::endl;
                }
            }
        }
    }


    void ScanDirectory(const std::string &directoryPath)
    {
        std::lock_guard<std::mutex> lock(mutex_);

        if (isScanned || isVirtual_) return; // Skip virtual items.
        isScanned = true;

        // Instead of clearing everything, remove only filesystem-related children:
        auto it = std::remove_if(children.begin(), children.end(),
            [](const std::shared_ptr<AssetItem>& child) {
                return !child->isVirtual();
            });
        children.erase(it, children.end());

        if (!fs::exists(directoryPath) || !fs::is_directory(directoryPath)) {
            std::cerr << "[AssetItem.h] Directory does not exist or is not a directory: " << directoryPath << std::endl;
            return;
        }

        for (const auto& entry : fs::directory_iterator(directoryPath)) {
            std::string name = entry.path().filename().string();
            std::string fullPath = entry.path().string();

            if (entry.is_directory()) {
                auto childFolder = std::make_shared<AssetItem>(name, AssetType::Folder, fullPath);
                // safe to call ScanDirectory recursively; each call will lock its own instance's mutex.
                childFolder->ScanDirectory(fullPath);
                children.push_back(childFolder);
                std::cout << "[AssetItem.h] Added folder: " << fullPath << std::endl;
            } else if (entry.is_regular_file()) {
                auto type = AssetType::File;
                std::string ext = entry.path().extension().string();
                if (ext == ".glsl" || ext == ".shader") type = AssetType::Shader;
                else if (ext == ".material") type = AssetType::Material;
                else if (ext == ".obj" || ext == ".fbx") type = AssetType::Mesh;

                auto childFile = std::make_shared<AssetItem>(name, type, fullPath);
                children.push_back(childFile);
                std::cout << "[AssetItem.h] Added file: " << fullPath << std::endl;
            }
        }
    }

    void printID() const {
        std::cout << "[AssetItem.h] Name: " << name_ << " ID: " << getUUIDStr() << std::endl;
    }

    void debugPrint() const {
        std::shared_ptr<AssetItem> parentPtr = parent_.lock();
        std::cout << "[AssetItem] Name: " << name_
                  << " UUID: " << getUUIDStr()
                  << " Type: " << static_cast<int>(type_)
                  << " Path: " << path_
                  << " Parent: " << (parentPtr ? parentPtr->getName() : "None")
                  << std::endl;
    }

    virtual const std::string& getName() const {
        if (isVirtual_ && type_ == AssetType::GameObject) {
            if (const auto& obj = gameObject_.lock()) {
                return obj->getName();
            }
        } else if (isVirtual_ && type_ == AssetType::Entity) {
            if (const auto& ent = entities.lock()) {
                return ent->getName();
            }
        }
        return name_;
    }

    AssetType getType() const { return type_; }
    const std::string& getPath() const { return path_; }

    // Thread-safe addChild
    void addChild(const std::shared_ptr<AssetItem>& child) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (children.size() < MAX_CHILDREN) {
            std::cerr << "[AssetItem.h] Child added: " << this->getName()
                << " to " << this->getName()
                << " (Parent Virtual::" << (this->isVirtual() ? "Yes" : "No")
                << ", Child Virtual::" << (child->isVirtual() ? "Yes" : "No") << ")"
                << std::endl;

            children.push_back(child);
            child->parent_ = shared_from_this();
        }
    }

    // Thread-safe getChildren
    std::vector<std::shared_ptr<AssetItem>> getChildrenSafe() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return children; // returning a copy so the caller can iterate safely
    }
    const std::vector<std::shared_ptr<AssetItem>>& getChildren() const { return children; }
    std::shared_ptr<AssetItem> getParent() const { return parent_.lock(); }

    void setGameObject(const std::shared_ptr<GameObject>& obj) {
        gameObject_ = obj;
    }

    void setEntityObject(const std::shared_ptr<Entity>& obj) {
        entities = obj;
    }

    std::shared_ptr<GameObject> getGameObject() const {
        return gameObject_.lock();
    }
    std::shared_ptr<Entity> getEntity() const {
        return entities.lock();
    }

    void clearChildren() {
        for (auto& child : children) {
            child->clearChildren();
            child.reset();
        }
        children.clear();
    }
    bool isScanned = false;

private:
    bool isVirtual_ = false;
    std::weak_ptr<GameObject> gameObject_;
    std::weak_ptr<Entity> entities;
    mutable std::mutex mutex_;

    std::string name_;
    AssetType type_;
    std::string path_;
    boost::uuids::uuid uuid_;
    mutable std::string uuidStr_;

    std::vector<std::shared_ptr<AssetItem>> children;
    std::weak_ptr<AssetItem> parent_;

    static constexpr int MAX_CHILDREN = 1024;
};

#endif //ASSETITEM_H
