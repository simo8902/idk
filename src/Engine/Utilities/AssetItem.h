//
// Created by Simeon on 10/7/2024.
//

#ifndef ASSETITEM_H
#define ASSETITEM_H

#include <string>
#include <memory>
#include <vector>
#include <boost/uuid/uuid.hpp>

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
    AssetItem(const std::string& name, AssetType type, const std::string& path, std::weak_ptr<AssetItem> parent = std::weak_ptr<AssetItem>());
    const std::string& getName() const;
    const std::string& getPath() const;
    AssetType getType() const;
    const boost::uuids::uuid& getUUID() const;
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
    std::vector<std::shared_ptr<AssetItem>> children;
};

#endif //ASSETITEM_H
