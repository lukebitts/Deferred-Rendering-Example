#ifndef _COMPILABLESHADER_H_
#define _COMPILABLESHADER_H_

#ifdef __APPLE__
	#include <OpenGL/gl.h>
#else
	#include <GL/gl.h>
#endif

class CompilableShader
{
	public:
		CompilableShader(const char* source, GLenum type);
		CompilableShader(const CompilableShader&) = delete;
		CompilableShader& operator=(const CompilableShader&) = delete;
		CompilableShader(CompilableShader&&);
		CompilableShader& operator=(CompilableShader&&);
		inline GLuint id() const { return _id; }
		~CompilableShader();
	private:
		GLuint _id;
};

class VertexShader : public CompilableShader
{
	public:
		VertexShader(const char* source);
};

class FragmentShader : public CompilableShader
{
	public:
		FragmentShader(const char* source);
};


#endif // _COMPILABLESHADER_H_
