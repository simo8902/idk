//
// Created by Simeon on 10/7/2024.
//

#ifndef ASSETITEM_H
#define ASSETITEM_H

#include <iostream>
#include <string>
#include <vector>
#include <memory>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

enum class AssetType {
    Folder = 0,
    Shader=1,
    PredefinedShader = 2,
    RuntimeShader = 3,
    Material = 4,
    Mesh = 5,
    Unknown = -1
};

class AssetItem : public std::enable_shared_from_this<AssetItem> {
public:
    AssetItem(const std::string& name, AssetType type, const std::string& path, std::shared_ptr<AssetItem> parent = nullptr)
        : path(path), name(name), parent(parent), type(type) {
        id = boost::uuids::random_generator()();
    }

    ~AssetItem() {}

    void printID() const {
        std::cout << "Name: " << name << " ID: " << id << std::endl;
    }

    void debugPrint() const {
        std::shared_ptr<AssetItem> parentPtr = parent.lock();
        std::cout << "[AssetItem] Name: " << name
                  << " UUID: " << uuidStr
                  << " Type: " << static_cast<int>(type)
                  << " Path: " << path
                  << " Parent: " << (parentPtr ? parentPtr->getName() : "None")
                  << std::endl;
    }


    const std::string& getName() const { return name; }
    AssetType getType() const { return type; }
    const std::string& getPath() const { return path; }
    const std::string& getUUIDStr() const { return uuidStr; }

    std::shared_ptr<AssetItem> getParent() const;
    const std::vector<std::shared_ptr<AssetItem>>& getChildren() const;

    void addChild(const std::shared_ptr<AssetItem>& child);
    void clearChildren();

    std::string path;
    boost::uuids::uuid id;
    std::string name;

private:
    std::weak_ptr<AssetItem> parent;
    AssetType type;
    std::string uuidStr;
    std::string vertexPath;
    std::string fragmentPath;
    std::vector<std::shared_ptr<AssetItem>> children;

    static const int MAX_CHILDREN = 1024;
};

#endif //ASSETITEM_H
