//
// Created by Simeon on 10/8/2024.
//
#include "AssetItem.h"
#include <iostream>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

AssetItem::AssetItem(const std::string& name, AssetType type, const std::string& path, std::weak_ptr<AssetItem> parent)
    : name(name), type(type), path(path), parent(parent), uuid(boost::uuids::random_generator()()) {
    if (uuid.is_nil()) {
        uuid = boost::uuids::random_generator()();
    }
    std::cout << "[AssetItem] Created: " << name << " UUID: " << boost::uuids::to_string(uuid)
              << " Type: " << static_cast<int>(type) << std::endl;
}

const std::string& AssetItem::getName() const { return name; }
const std::string& AssetItem::getPath() const { return path; }
AssetType AssetItem::getType() const { return type; }
const boost::uuids::uuid& AssetItem::getUUID() const { return uuid; }
std::shared_ptr<AssetItem> AssetItem::getParent() const { return parent.lock(); }

const std::vector<std::shared_ptr<AssetItem>>& AssetItem::getChildren() const { return children; }

void AssetItem::addChild(const std::shared_ptr<AssetItem>& child) {
    child->parent = shared_from_this();
    children.push_back(child);
}
void AssetItem::clearChildren() {
    children.clear();
}