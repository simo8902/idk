//
// Created by SIMEON on 2/25/2025.
//

#ifndef IRENDERFORWARD_H
#define IRENDERFORWARD_H

#include "glad/glad.h"

class IRenderForward {
public:
    virtual ~IRenderForward() = default;
    virtual void render() = 0;
    virtual void resizeFramebuffer(int width, int height) = 0;
    virtual GLuint getFramebuffer() const = 0;
    virtual GLuint getTexture() const = 0;
    virtual int getFBOWidth() const = 0;
    virtual int getFBOHeight() const = 0;
    virtual void updateFramebuffer(int viewportWidth, int viewportHeight) = 0;
};

#endif //IRENDERFORWARD_H
