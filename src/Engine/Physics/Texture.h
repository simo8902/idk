//
// Created by Simeon on 10/5/2024.
//

#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>
#include <boost/uuid/random_generator.hpp>

#include "glad/glad.h"

class Texture {
public:
    Texture(const std::string& filePath);
    ~Texture();

    std::string getName() const { return name; }
    std::string getUUID() const { return uuidStr; }
    GLuint getID() const { return textureID; }

    void bind(GLenum textureUnit = GL_TEXTURE0) const;
    boost::uuids::uuid uuid;

private:
    std::string name;
    std::string uuidStr;
    GLuint textureID;

    static boost::uuids::random_generator uuidGenerator;

    void loadTextureFromFile(const std::string& filePath);
};


#endif //TEXTURE_H
