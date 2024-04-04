//
// Created by Simeon-PC on 3/27/2024.
//

#ifndef ZGN_ENGINE_GUI_H
#define ZGN_ENGINE_GUI_H

#include "Shader.h"

class GUI {
public:
    GUI(const char* vertexShaderCode, const char* fragmentShaderCode);

private:
    Shader shader;
};


#endif //ZGN_ENGINE_GUI_H
