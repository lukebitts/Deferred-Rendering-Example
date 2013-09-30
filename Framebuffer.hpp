//
//  Vertex.h
//  OpenGLTest
//
//  Created by Lucas Bittencourt on 8/9/13.
//  Copyright (c) 2013 Lucas Bittencourt. All rights reserved.
//

#ifndef _FRAMEBUFFER_H_
#define _FRAMEBUFFER_H_

#ifdef __APPLE__
	#include <OpenGL/gl.h>
#else
	#include <GL/gl.h>
#endif

#include <vector>
#include <tuple>

class Texture2D;

//todo: disable copy and enable move

class Framebuffer
{
	public:
		Framebuffer(std::initializer_list<std::tuple<GLenum, Texture2D>>);
		GLuint id() const;
		const Texture2D& texture(size_t index) const;
	private:
		GLuint _id;
		std::vector<Texture2D> _textures;
};

#endif

