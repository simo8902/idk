//
// Created by Simeon on 9/22/2024.
//

#ifndef CAMERAMANAGER_H
#define CAMERAMANAGER_H
#include <memory>
#include <vector>

#include "Camera.h"

class CameraManager {
public:
    void addCamera(std::shared_ptr<Camera> camera) {
        cameras.push_back(camera);
    }

    const std::vector<std::shared_ptr<Camera>>& getCameras() const {
        return cameras;
    }

private:
    std::vector<std::shared_ptr<Camera>> cameras;
};

#endif //NAV2SFM Core_CAMERA_MANAGER_H
