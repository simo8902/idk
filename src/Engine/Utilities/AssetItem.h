//
// Created by Simeon on 10/7/2024.
//

#ifndef ASSETITEM_H
#define ASSETITEM_H

#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>

enum class AssetType {
    Folder = 0,
    Shader = 1,
    PredefinedShader = 5,
    Material = 3,
    Mesh = 8,
    Unknown = -1
};

class AssetItem : public std::enable_shared_from_this<AssetItem> {
public:
    AssetItem(const std::string& name, AssetType type, const std::string& path, std::shared_ptr<AssetItem> parent = nullptr)
        : name(name), type(type), path(path), parent(parent), uuid(boost::uuids::random_generator()()) {
        if (uuid.is_nil())
        {
            uuid = boost::uuids::random_generator()();
            if (uuid.is_nil()) {
                std::cerr << "[AssetItem] Failed to generate a valid UUID for asset: " << name << std::endl;
            }
        }
        uuidStr = boost::uuids::to_string(uuid);
        if (uuidStr.empty()) {
            std::cerr << "[AssetItem] Generated UUID string is empty for asset: " << name << std::endl;
        }
        std::cout << "[AssetItem] Created: " << name << " UUID: " << uuidStr
                  << " Type: " << static_cast<int>(type) << std::endl;
    }

    virtual ~AssetItem(){}

    const std::string& getName() const { return name; }
    AssetType getType() const { return type; }
    const std::string& getPath() const { return path; }
    const boost::uuids::uuid& getUUID() const { return uuid; }
    const std::string& getUUIDStr() const { return uuidStr; }

    std::shared_ptr<AssetItem> getParent() const;
    const std::vector<std::shared_ptr<AssetItem>>& getChildren() const;
    void addChild(const std::shared_ptr<AssetItem>& child);
    std::string path;
    void clearChildren();

private:
    std::string name;
    AssetType type;
    std::weak_ptr<AssetItem> parent;
    boost::uuids::uuid uuid;
    std::string uuidStr;

    std::vector<std::shared_ptr<AssetItem>> children;
};

#endif //ASSETITEM_H
