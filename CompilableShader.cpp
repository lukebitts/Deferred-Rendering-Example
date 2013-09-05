#define GL_GLEXT_PROTOTYPES
#include "CompilableShader.hpp"

#ifdef __APPLE__
	#include <OpenGL/glext.h>
#else
	#include <GL/glext.h>
#endif

#include "GLErrors.hpp"

#ifdef DEBUG

#include <memory>
#include <iostream>

#endif

CompilableShader::CompilableShader(const char* source, GLenum type) : _id(glCreateShader(type))
{
    glShaderSource(_id, 1, &source, nullptr);
    glCompileShader(_id);

    #ifdef DEBUG

    GLint result = GL_FALSE;
    GLint info_log_length;

    glGetShaderiv(_id, GL_COMPILE_STATUS, &result);
    glGetShaderiv(_id, GL_INFO_LOG_LENGTH, &info_log_length);

    std::unique_ptr<char> shader_error(new char[info_log_length]);
    glGetShaderInfoLog(_id, info_log_length, NULL, shader_error.get());
    if(result == GL_FALSE)
        std::cout<<shader_error.get()<<std::endl;

    #endif
}

CompilableShader::CompilableShader(CompilableShader&& other) : _id(other._id)
{
    other._id = 0;
}

CompilableShader& CompilableShader::operator=(CompilableShader&& other)
{
    _id = other._id;
    other._id = 0;

    return *this;
}

CompilableShader::~CompilableShader()
{
    glDeleteShader(_id);
}


VertexShader::VertexShader(const char* source) : CompilableShader(source, GL_VERTEX_SHADER)
{

}

FragmentShader::FragmentShader(const char* source) : CompilableShader(source, GL_FRAGMENT_SHADER)
{

}
