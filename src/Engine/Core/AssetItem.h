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

namespace fs = std::filesystem;

enum class AssetType {
    Folder = 0,
    Shader=1,
    PredefinedShader = 2,
    RuntimeShader = 3,
    Material = 4,
    Mesh = 5,
    File = 6,
    Unknown = -1
};
class AssetItem : public std::enable_shared_from_this<AssetItem> {
public:
    AssetItem(const std::string& name, AssetType type, const std::string& path)
        : name_(name), type_(type), path_(path), uuid_(boost::uuids::random_generator()()) {}

    const std::string& getUUIDStr() const {
        if (uuidStr_.empty()) {
            uuidStr_ = boost::uuids::to_string(uuid_);
        }
        return uuidStr_;
    }

    ~AssetItem() {}

    void ScanDirectory(const std::string &directoryPath)
    {
        if (isScanned) return;
        isScanned = true;

        // Clear any existing children.
        children.clear();

        std::cout << "[ScanDirectory] Scanning directory: " << directoryPath << std::endl;

        // Check if the directory exists and is indeed a directory.
        if (!fs::exists(directoryPath) || !fs::is_directory(directoryPath))
        {
            std::cout << "[ScanDirectory] ERROR: '" << directoryPath << "' does not exist or is not a directory." << std::endl;
            return;
        }

        // Iterate over each entry in the directory.
        for (const auto &entry : fs::directory_iterator(directoryPath))
        {
            std::string name = entry.path().filename().string();
            std::string fullPath = entry.path().string();

            if (entry.is_directory())
            {
                std::cout << "[ScanDirectory] Found directory: " << fullPath << std::endl;

                // Create a new AssetItem for the folder.
                auto childFolder = std::make_shared<AssetItem>(name, AssetType::Folder, fullPath);

                // Recursively scan the subdirectory.
                childFolder->ScanDirectory(fullPath);

                // Add the scanned folder to the children.
                children.push_back(childFolder);
                std::cout << "[ScanDirectory] Added folder: " << fullPath << std::endl;
            }
            else if (entry.is_regular_file())
            {
                std::cout << "[ScanDirectory] Found file: " << fullPath << std::endl;

                // Determine the asset type based on file extension if needed.
                // Here we simply use AssetType::File for all files.
                AssetType type = AssetType::File;  // Modify this logic for shaders, materials, etc.

                auto childFile = std::make_shared<AssetItem>(name, type, fullPath);
                children.push_back(childFile);
                std::cout << "[ScanDirectory] Added file: " << fullPath << std::endl;
            }
            else
            {
                std::cout << "[ScanDirectory] Skipped non-regular entry: " << fullPath << std::endl;
            }
        }
    }

    void printID() const {
        std::cout << "Name: " << name_ << " ID: " << getUUIDStr() << std::endl;
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

    const std::string& getName() const { return name_; }
    AssetType getType() const { return type_; }
    const std::string& getPath() const { return path_; }

    void addChild(const std::shared_ptr<AssetItem>& child) {
        if (children.size() < MAX_CHILDREN) {
            children.push_back(child);
            child->parent_ = shared_from_this();
        }
    }

    const std::vector<std::shared_ptr<AssetItem>>& getChildren() const { return children; }
    std::shared_ptr<AssetItem> getParent() const { return parent_.lock(); }

    void clearChildren() {
        for (auto& child : children) {
            child->clearChildren();
            child.reset();
        }
        children.clear();
    }

private:
    std::string name_;
    AssetType type_;
    std::string path_;
    boost::uuids::uuid uuid_;
    mutable std::string uuidStr_;
    bool isScanned = false;

    std::vector<std::shared_ptr<AssetItem>> children; // Ensure consistent naming
    std::weak_ptr<AssetItem> parent_;

    static const int MAX_CHILDREN = 1024;
};

#endif //ASSETITEM_H
