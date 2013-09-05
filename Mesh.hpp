#ifndef _MESH_H_
#define _MESH_H_

#ifdef __APPLE__
	#include <OpenGL/gl.h>
#else
	#include <GL/gl.h>
#endif

#include <vector>

class Vertex;

struct Mesh
{
	///@todo separate into Model and Mesh (if necessary)
	enum BufferType{ VERTEX = 0, ELEMENT = 1 };
	GLuint buffers[ELEMENT+1];
	GLsizei element_number;
	Mesh(unsigned vertex_number, GLfloat* vertex_list, unsigned element_number, GLuint* element_list);
	Mesh(std::vector<Vertex> vertex_list, std::vector<GLuint> element_list);
	Mesh(const Mesh&) = delete;
	Mesh& operator=(const Mesh&) = delete;
	~Mesh();
};

#endif
