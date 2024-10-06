//
// Created by Simeon on 10/5/2024.
//

#include "Texture.h"
#include "stb_image.h"
#include <iostream>
#include <boost/uuid/uuid_io.hpp>

boost::uuids::random_generator Texture::uuidGenerator;

Texture::Texture(const std::string& filePath)
    : name(filePath), textureID(0)
{
    uuid = uuidGenerator();
    uuidStr = boost::uuids::to_string(uuid);

    std::cout << "[Texture] Texture created: " << name << " with UUID: " << uuidStr << std::endl;

    loadTextureFromFile(filePath);
}

Texture::~Texture() {
    glDeleteTextures(1, &textureID);
    std::cout << "[Texture] Texture deleted: " << name << " with UUID: " << uuidStr << std::endl;
}

void Texture::loadTextureFromFile(const std::string& filePath) {
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(filePath.c_str(), &width, &height, &nrChannels, 0);
    if (data) {
        GLenum format;
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;
        else {
            std::cerr << "[Texture] Unsupported number of channels: " << nrChannels << " in " << filePath << std::endl;
            stbi_image_free(data);
            return;
        }

        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(data);
        std::cout << "[Texture] Loaded texture: " << name << " with ID: " << textureID << std::endl;
    } else {
        std::cerr << "[Texture] Failed to load texture from: " << filePath << std::endl;
    }
}

void Texture::bind(GLenum textureUnit) const {
    glActiveTexture(textureUnit);
    glBindTexture(GL_TEXTURE_2D, textureID);
}