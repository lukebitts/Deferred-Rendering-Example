#ifndef PROGRAM_H
#define PROGRAM_H

#ifdef __APPLE__
	#include <OpenGL/gl.h>
#else
	#include <GL/gl.h>
#endif

class VertexShader;
class FragmentShader;

class Program
{
	///@todo disable copy and enable move operations
	public:
		Program(const VertexShader& vs, const FragmentShader& fs);
		Program(const char* vs, const char* fs);
		inline GLuint id() const { return _id; }
		~Program();
	private:
		GLuint _id;
};

#endif // PROGRAM_H
