//
// Created by Simeon on 11/14/2024.
//

#include "AssetItem.h"
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

std::shared_ptr<AssetItem> AssetItem::getParent() const {
    return parent.lock();
}

const std::vector<std::shared_ptr<AssetItem>>& AssetItem::getChildren() const {
    return children;
}

void AssetItem::addChild(const std::shared_ptr<AssetItem>& child) {
    if (children.size() < MAX_CHILDREN) {
        child->parent = shared_from_this();
        children.push_back(child);
    } else {
        throw std::runtime_error("Maximum number of children reached.");
    }
}

void AssetItem::clearChildren() {
    for (auto& child : children) {
        child->clearChildren();
        child.reset();
    }
    children.clear();
}