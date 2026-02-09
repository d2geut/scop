#include "program.h"
#include <iostream>

ProgramUPtr Program::Create(const std::vector<ShaderPtr>& shaders) {
    auto program = ProgramUPtr(new Program());
    if (!program->Link(shaders))
        return nullptr;
    return std::move(program);
}

Program::~Program() {
    if (m_program) {
        glDeleteProgram(m_program);
    }
}

bool Program::Link(const std::vector<ShaderPtr>& shaders) {
    // program 후 shader 붙이기
    m_program = glCreateProgram();
    for (auto& shader: shaders)
        glAttachShader(m_program, shader->Get());
    glLinkProgram(m_program);

    // program 링크 상태 확인
    int success = 0;
    glGetProgramiv(m_program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[1024];
        glGetProgramInfoLog(m_program, 1024, nullptr, infoLog);
        std::cerr << "failed to link program: " << infoLog << std::endl;
        return false;
    }
    return true;
}