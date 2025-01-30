//
// Created by Simeon on 9/22/2024.
//

#ifndef LIGHTMANAGER_H
#define LIGHTMANAGER_H

#include <memory>
#include <vector>

#include "DirectionalLight.h"

class LightManager {
public:
    LightManager() = default;

    void addDirectionalLight(const std::shared_ptr<DirectionalLight>& light) {
        if (!light || !light->isInitialized()) {
            throw std::invalid_argument("Attempted to add an uninitialized DirectionalLight.");
        }
        directionalLights.push_back(light);
    }

    const std::vector<std::shared_ptr<DirectionalLight>>& getDirectionalLights() const {
        return directionalLights;
    }

    std::string getName() const {
        return name;
    }

private:
    std::vector<std::shared_ptr<DirectionalLight>> directionalLights;
    std::string name;

};


#endif
