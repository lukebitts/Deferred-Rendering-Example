#define GL_GLEXT_PROTOTYPES
#include "Program.hpp"

#ifdef __APPLE__
	#include <OpenGL/glext.h>
#else
	#include <GL/glext.h>
#endif

#include "GLErrors.hpp"

#include "CompilableShader.hpp"

Program::Program(const VertexShader& vs, const FragmentShader& fs) : _id(glCreateProgram())
{
    glAttachShader(_id, vs.id());
    glAttachShader(_id, fs.id());
    glLinkProgram(_id);
}

Program::Program(const char* vs, const char* fs) : Program(VertexShader(vs), FragmentShader(fs))
{

}

Program::~Program()
{
    glDeleteProgram(_id);
}
