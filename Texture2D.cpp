//
//  Texture2D.cpp
//  OpenGLTest
//
//  Created by Lucas Bittencourt on 8/20/13.
//  Copyright (c) 2013 Lucas Bittencourt. All rights reserved.
//

#include "Texture2D.hpp"

#include "GLErrors.hpp"

Texture2D::Texture2D(GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid * data,
	std::initializer_list<std::pair<GLenum,GLint>> parametersi) : _id(0)
{
	glGenTextures(1, &_id);
	glBindTexture(GL_TEXTURE_2D, _id);
	
	for(auto&& p : parametersi)
		glTexParameteri(GL_TEXTURE_2D, p.first, p.second);

	glTexImage2D(GL_TEXTURE_2D, level, internalFormat, width, height, border, format, type, data);
	
	glBindTexture(GL_TEXTURE_2D, 0);
}

Texture2D::Texture2D(Texture2D&& other)
{
	this->_id = other._id;
	other._id = 0;
}

Texture2D& Texture2D::operator=(Texture2D&& other)
{
	this->_id = other._id;
	other._id = 0;
	
	return *this;
}

GLuint Texture2D::id() const
{
	return _id;
}

Texture2D::~Texture2D()
{
	glDeleteTextures(1, &_id);
}