//
// Created by Simeon on 10/8/2024.
//
#include "AssetItem.h"
#include <iostream>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>



std::shared_ptr<AssetItem> AssetItem::getParent() const { return parent.lock(); }

const std::vector<std::shared_ptr<AssetItem>>& AssetItem::getChildren() const { return children; }

void AssetItem::addChild(const std::shared_ptr<AssetItem>& child) {
    child->parent = shared_from_this();
    children.push_back(child);
}
void AssetItem::clearChildren() {
    children.clear();
}