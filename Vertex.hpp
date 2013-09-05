//
//  Vertex.h
//  OpenGLTest
//
//  Created by Lucas Bittencourt on 8/9/13.
//  Copyright (c) 2013 Lucas Bittencourt. All rights reserved.
//

#ifndef _VERTEX_H_
#define _VERTEX_H_

#ifdef __APPLE__
	#include <OpenGL/gl.h>
#else
	#include <GL/gl.h>
#endif

#include "GLErrors.hpp"

struct Vertex
{
	GLfloat x, y, z;
	GLfloat u, v;
	GLfloat nx, ny, nz;
	GLfloat tx, ty, tz;
	GLfloat bx, by, bz;
};

#endif
