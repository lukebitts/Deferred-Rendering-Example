#ifndef _EXTRAS_H_
#define _EXTRAS_H_

#include <memory>
#include <string>

#define GL_GLEXT_PROTOTYPES

#ifdef __APPLE__
	#include <OpenGL/gl.h>
#else
	#include <GL/gl.h>
#endif

#ifdef __APPLE__
	#include <OpenGL/glext.h>
#else
	#include <GL/glext.h>
#endif

class Mesh;
class Program;

#include "glm/glm.hpp"
#include "glm/gtx/quaternion.hpp"
#include "Framebuffer.hpp"
#include "Texture2D.hpp"

namespace extras
{

std::unique_ptr<Mesh> uv_sphere(float radius, unsigned int rings, unsigned int sectors);
std::unique_ptr<Mesh> icosphere(float radius, unsigned subdivisions);
std::unique_ptr<Mesh> plane(float width, float height);
std::unique_ptr<Mesh> cube(float size);
std::unique_ptr<Mesh> cone(float base_radius, unsigned base_quality, float apex_y, float height);
std::string load_file(const std::string& path);
std::unique_ptr<Mesh> mesh_from_file(const std::string& path);
std::unique_ptr<Texture2D> texture2d_from_file(const std::string& path);
std::unique_ptr<Texture2D> texture2d_from_color(unsigned r, unsigned g, unsigned b, unsigned a);
Program program_from_file(const std::string& path_vs,const std::string& path_fs);

struct DeferredModel
{
	glm::vec3 position;
	glm::quat rotation;
	glm::vec3 scale;
	std::unique_ptr<Mesh> mesh;
	std::unique_ptr<Texture2D> color;
	std::unique_ptr<Texture2D> normal;
	std::unique_ptr<Texture2D> specular;
	DeferredModel(std::unique_ptr<Mesh> mesh, std::unique_ptr<Texture2D> color, std::unique_ptr<Texture2D> normal, std::unique_ptr<Texture2D> specular,
		glm::vec3 position = glm::vec3(), glm::quat rotation = glm::quat(), glm::vec3 scale = glm::vec3(1))
		: position(position), rotation(rotation), scale(scale), mesh(std::move(mesh)), color(std::move(color)), normal(std::move(normal)), specular(std::move(specular))
	{

	}
};

struct IDeferredLight
{
	virtual void shader_constants(glm::mat4 view, glm::mat4 projection, Program* p = nullptr) const = 0;
	virtual const Program& program() const = 0;
	virtual const std::unique_ptr<Mesh>& mesh() const = 0;
	virtual ~IDeferredLight(){}
};

struct ShadowCastingLight : public IDeferredLight
{
	ShadowCastingLight();
	virtual bool casts_shadow() const = 0;
	virtual glm::mat4 mvp() const = 0;
	private:
		Texture2D _textures[2];
		Framebuffer _fb;
};

struct PointLight : public IDeferredLight
{
	glm::vec3 position;
	glm::vec3 color;
	float power;
	float max_distance;
	void shader_constants(glm::mat4 view, glm::mat4 projection, Program* p = nullptr) const override;
	PointLight(glm::vec3 position, glm::vec3 color, float power, float max_distance);
	const Program& program() const override;
	const std::unique_ptr<Mesh>& mesh() const override;
};

struct SpotLight : public IDeferredLight
{
	glm::vec3 position;
	glm::vec3 color;
	glm::vec3 rotation;
	float power;
	float height;
	float radius;
	float falloff;
	void shader_constants(glm::mat4 view, glm::mat4 projection, Program* p = nullptr) const override;
	SpotLight(glm::vec3 position, glm::vec3 color, glm::vec3 rotation, float power, float height, float radius, float falloff);
	const Program& program() const override;
	const std::unique_ptr<Mesh>& mesh() const override;
};

void drawTexturedQuadToScreen(glm::vec2 position, glm::vec2 size, glm::vec2 win_size, GLuint tex_id);

}

#endif // EXTRAS_H
