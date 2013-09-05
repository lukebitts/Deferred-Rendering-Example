//
//  Texture2D.h
//  OpenGLTest
//
//  Created by Lucas Bittencourt on 8/20/13.
//  Copyright (c) 2013 Lucas Bittencourt. All rights reserved.
//

#ifndef _TEXTURE2D_
#define _TEXTURE2D_

#ifdef __APPLE__
	#include <OpenGL/gl.h>
#else
	#include <GL/gl.h>
#endif

#include <initializer_list>
#include <utility>

///@todo create copy and move operations
struct Texture2D
{
	Texture2D(GLint level, GLint internalFormat, GLsizei width, GLsizei height,	GLint border, GLenum format, GLenum type, const GLvoid * data, std::initializer_list<std::pair<GLenum,GLint>> parametersi);
	~Texture2D();
	GLuint id;
};

#endif 
