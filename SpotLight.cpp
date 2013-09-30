#include "SpotLight.hpp"
#include "Extras.hpp"
#include "Mesh.hpp"
#include "Program.hpp"

SpotLight::SpotLight(glm::vec3 position, glm::vec3 rotation, glm::vec3 color, float power, float height, float radius, float falloff)
    : position(position), rotation(rotation), color(color), power(power), height(height), radius(radius), falloff(falloff)
{

}

const Mesh& SpotLight::mesh()
{
    static auto mesh = extras::mesh_from_file("assets/models/cone.obj");
    return *mesh.get();
}

const Program& SpotLight::program()
{
    static const Program p = extras::program_from_file("assets/shaders/deferred_spot_light.vs.glsl", "assets/shaders/deferred_spot_light.fs.glsl");
	return p;
}
