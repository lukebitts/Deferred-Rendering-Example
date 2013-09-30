#ifndef _SPOTLIGHT_H_
#define _SPOTLIGHT_H_

#include "IDeferredLight.hpp"

#include "glm/glm.hpp"

struct SpotLight : public IDeferredLight
{
    SpotLight(glm::vec3 position, glm::vec3 rotation, glm::vec3 color, float power, float height, float radius, float falloff);
    glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 color;
	float power;
	float height;
	float radius;
	float falloff;
	virtual const Mesh& mesh();
    virtual const Program& program();
};

#endif // _SPOTLIGHT_H_
