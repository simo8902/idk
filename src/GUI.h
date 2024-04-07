//
// Created by Simeon-PC on 3/27/2024.
//

#ifndef GUI_H
#define GUI_H

#include "Shader.h"

class GUI {
public:
    GUI(const char* vertexShaderCode, const char* fragmentShaderCode);

private:
    Shader shader;
};


#endif //GUI_H
