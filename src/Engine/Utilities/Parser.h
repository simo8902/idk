//
// Created by Simeon on 10/2/2024.
//

#ifndef PARSER_H
#define PARSER_H

#include "glslang/Public/ShaderLang.h"

class Parser {
    public:
    // Initialize glslang
    void initGlslang() {
        glslang::InitializeProcess();
    }

    // Finalize glslang
    void finalizeGlslang() {
        glslang::FinalizeProcess();
    }

    // Convert HLSL to GLSL using glslang
    std::string convertHLSLtoGLSL(const std::string& hlslCode, EShLanguage stage) {
        // Set up shader and program
        glslang::TShader shader(stage);
        const char* shaderStrings[1];
        shaderStrings[0] = hlslCode.c_str();
        shader.setStrings(shaderStrings, 1);

        // Set the environment to HLSL
        shader.setEnvInput(glslang::EShSourceHlsl, stage, glslang::EShClientOpenGL, 100);

        // Parse the shader
        TBuiltInResource resources = glslang::DefaultTBuiltInResource;
        EShMessages messages = EShMsgDefault;
        if (!shader.parse(&resources, 110, false, messages)) {
            std::cerr << "GLSLang shader parsing failed: " << shader.getInfoLog() << std::endl;
            return "";
        }

        // Link the program
        glslang::TProgram program;
        program.addShader(&shader);
        if (!program.link(messages)) {
            std::cerr << "GLSLang program linking failed: " << program.getInfoLog() << std::endl;
            return "";
        }

        // Convert to GLSL
        std::string glslCode;
        glslang::GlslangToGlsl(program.getIntermediate(stage), glslCode);

        return glslCode;
    }

};

#endif //PARSER_H
