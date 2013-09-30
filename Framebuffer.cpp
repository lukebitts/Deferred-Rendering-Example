#define GL_GLEXT_PROTOTYPES

#include "Framebuffer.hpp"

#ifdef __APPLE__
	#include <OpenGL/glext.h>
#else
	#include <GL/glext.h>
#endif

#include <exception>

#include "Texture2D.hpp"

Framebuffer::Framebuffer(std::initializer_list<std::tuple<GLenum, Texture2D>> textures)
{
	glGenFramebuffers(1, &_id);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, _id);
	_textures.reserve(textures.size());

	for(auto& texture : textures)
	{
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, std::get<0>(texture), GL_TEXTURE_2D, std::get<1>(texture).id(), 0);
		//todo: remove this const_cast
		_textures.push_back(const_cast<Texture2D&&>(std::get<1>(texture)));
	}

	//todo: throw a real exception
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER_EXT) != GL_FRAMEBUFFER_COMPLETE_EXT)
		throw std::runtime_error("");

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

GLuint Framebuffer::id() const
{
	return _id;
}

const Texture2D& Framebuffer::texture(size_t index) const
{
	return _textures.at(index);
}
