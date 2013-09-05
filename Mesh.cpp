#define GL_GLEXT_PROTOTYPES

#include "Mesh.hpp"
#include "Vertex.hpp"

#ifdef __APPLE__
	#include <OpenGL/glext.h>
#else
	#include <GL/glext.h>
#endif

#include "GLErrors.hpp"

#include <GL/glfw.h>
#include <iostream>

Mesh::Mesh(unsigned vertex_number, GLfloat* vertex_list, unsigned element_number, GLuint* element_list): element_number(element_number)
{
	glGenBuffers(2, buffers);

	glBindBuffer(GL_ARRAY_BUFFER, buffers[VERTEX]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*3*vertex_number, vertex_list, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[ELEMENT]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*element_number, element_list, GL_STATIC_DRAW);
}

Mesh::Mesh(std::vector<Vertex> vertex_list, std::vector<GLuint> element_list) : element_number((GLuint)element_list.size())
{
	glGenBuffers(2, buffers);

	glBindBuffer(GL_ARRAY_BUFFER, buffers[VERTEX]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*vertex_list.size(), &vertex_list[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[ELEMENT]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*element_number, &element_list[0], GL_STATIC_DRAW);
}

Mesh::~Mesh()
{
	glDeleteBuffers(6, buffers);
}
