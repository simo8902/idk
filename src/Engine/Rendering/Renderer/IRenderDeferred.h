//
// Created by SIMEON on 2/25/2025.
//

#ifndef IRENDERDEFERRED_H
#define IRENDERDEFERRED_H

#include "glad/glad.h"

class IRenderDeferred {
public:
    virtual ~IRenderDeferred() = default;
    virtual void render() = 0;
    virtual void resizeFramebuffer(int width, int height) = 0;
    virtual GLuint getFramebuffer() const = 0;
    virtual GLuint getTexture() const = 0;
    virtual int getFBOWidth() const = 0;
    virtual int getFBOHeight() const = 0;
    virtual void updateViewportFramebuffer(const int & viewportWidth,const int & viewportHeight) = 0;

    virtual GLuint getGPosition() = 0;
    virtual GLuint getGNormal() = 0;
    virtual GLuint getGAlbedoSpec() = 0;
};


#endif //IRENDERDEFERRED_H
