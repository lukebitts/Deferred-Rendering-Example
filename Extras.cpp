#include "Extras.hpp"

#ifdef __APPLE__
	#include <OpenGL/gl.h>
#else
	#include <GL/gl.h>
#endif

#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <exception>

#include <assimp/assimp.h>
#include <assimp/aiScene.h>
#include <assimp/aiPostProcess.h>
#include "lodepng/lodepng.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "Mesh.hpp"
#include "Vertex.hpp"
#include "Texture2D.hpp"
#include "Program.hpp"

std::unique_ptr<Mesh> extras::uv_sphere(float radius, unsigned int rings, unsigned int sectors)
{
    std::vector<GLfloat> vertices;
    std::vector<GLfloat> normals;
    std::vector<GLfloat> texcoords;
    std::vector<GLuint> indices;

	float const R = 1./(float)(rings-1);
	float const S = 1./(float)(sectors-1);
	int r, s;

	vertices.resize(rings * sectors * 3);
	normals.resize(rings * sectors * 3);
	texcoords.resize(rings * sectors * 2);
	std::vector<GLfloat>::iterator v = vertices.begin();
	std::vector<GLfloat>::iterator n = normals.begin();
	std::vector<GLfloat>::iterator t = texcoords.begin();
	for(r = 0; r < rings; r++) for(s = 0; s < sectors; s++) {
			float const y = sin( -M_PI_2 + M_PI * r * R );
			float const x = cos(2*M_PI * s * S) * sin( M_PI * r * R );
			float const z = sin(2*M_PI * s * S) * sin( M_PI * r * R );

			*t++ = s*S;
			*t++ = r*R;

			*v++ = x * radius;
			*v++ = y * radius;
			*v++ = z * radius;

			*n++ = x;
			*n++ = y;
			*n++ = z;
	}

	indices.resize(rings * sectors * 4);
	std::vector<GLuint>::iterator i = indices.begin();
	for(r = 0; r < rings; r++) for(s = 0; s < sectors; s++) {
			*i++ = r * sectors + s;
			*i++ = r * sectors + (s+1);
			*i++ = (r+1) * sectors + (s+1);
			*i++ = (r+1) * sectors + s;
	}

	auto indices_copy = indices;
	indices.clear();

	for(int i = 0; i < indices_copy.size(); i+=4)
	{
		indices.push_back(indices_copy[i+2]);
		indices.push_back(indices_copy[i+1]);
		indices.push_back(indices_copy[i]);

		indices.push_back(indices_copy[i+3]);
		indices.push_back(indices_copy[i+2]);
		indices.push_back(indices_copy[i]);
	}

	std::vector<Vertex> vcopy;
	int j = 0;
	for(int i = 0; i < vertices.size(); i+=3)
	{
		vcopy.push_back({vertices[i],vertices[i+1],vertices[i+2] , texcoords[j],texcoords[j+1] , normals[i],normals[i+1],normals[i+2] , 0,0,0 , 0,0,0});
		j+=2;
	}

	return std::unique_ptr<Mesh>(new Mesh(std::move(vcopy),std::move(indices)));
}

std::unique_ptr<Mesh> extras::plane(float size_x, float size_z)
{
    ///@todo: calculate tangent and bitangent
	std::vector<Vertex> vertices{
        {-size_x,-size_z,0 , 1, 1, 0,1,0 , 1,1,1 , 1,1,1},
        { size_x,-size_z,0 , 0, 1, 0,1,0 , 1,1,1 , 1,1,1},
        { size_x, size_z,0 , 0, 0, 0,1,0 , 1,1,1 , 1,1,1},
        {-size_x, size_z,0 , 1, 0, 0,1,0 , 1,1,1 , 1,1,1}};
	std::vector<GLuint> indices{0,1,2,0,2,3};

	return std::unique_ptr<Mesh>(new Mesh(std::move(vertices),std::move(indices)));
}

std::unique_ptr<Mesh> extras::mesh_from_file(const std::string& path)
{
	const aiScene* scene = aiImportFile(path.c_str(), aiProcessPreset_TargetRealtime_MaxQuality);
	if(!scene) return extras::uv_sphere(1,20,20);
    aiMesh* mesh = scene->mMeshes[scene->mNumMeshes-1];
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;

    if(mesh->HasPositions())
    {
        for(int i = 0; i < mesh->mNumVertices; ++i)
        {
            Vertex v{0,0,0,0,0,0,0,0,0,0,0,0,0,0};
            v.x = mesh->mVertices[i].x;
            v.y = mesh->mVertices[i].y;
            v.z = mesh->mVertices[i].z;

			if(mesh->HasTextureCoords(0))
			{
				v.u = mesh->mTextureCoords[0][i].x;
                v.v = mesh->mTextureCoords[0][i].y;
			}

            if(mesh->HasNormals() && mesh->HasTangentsAndBitangents())
            {
                v.nx = mesh->mNormals[i].x;
                v.ny = mesh->mNormals[i].y;
                v.nz = mesh->mNormals[i].z;

                v.tx = mesh->mTangents[i].x;
                v.ty = mesh->mTangents[i].x;
                v.tz = mesh->mTangents[i].x;

                v.tx = mesh->mTangents[i].x;
                v.ty = mesh->mTangents[i].x;
                v.tz = mesh->mTangents[i].x;
            }
            vertices.push_back(v);
        }
    }

    for(int i = 0; i < mesh->mNumFaces; ++i)
    {
        indices.push_back(mesh->mFaces[i].mIndices[0]);
        indices.push_back(mesh->mFaces[i].mIndices[1]);
        indices.push_back(mesh->mFaces[i].mIndices[2]);
    }
	
	aiReleaseImport(scene);

    return std::unique_ptr<Mesh>(new Mesh(std::move(vertices), indices));
}

std::unique_ptr<Texture2D> extras::texture2d_from_file(const std::string& path)
{
	std::vector<unsigned char> image;
	unsigned w, h;
	unsigned error = lodepng::decode(image, w, h, path);
	(void)error;
	return std::unique_ptr<Texture2D>(new Texture2D{0, GL_RGBA8,  w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*)&image[0],{{GL_TEXTURE_MIN_FILTER,GL_LINEAR},{GL_TEXTURE_MAG_FILTER,GL_LINEAR}}});
}

std::string extras::load_file(const std::string& path)
{
	std::ifstream in(path, std::ios::in | std::ios::binary);
	std::ostringstream contents;
	if (in)
	{
		contents << in.rdbuf();
		in.close();
		return contents.str();
	}
	return "";
}

Program extras::program_from_file(const std::string& path_vs, const std::string& path_fs)
{
	return Program{load_file(path_vs).c_str(),load_file(path_fs).c_str()};
}

void extras::drawTexturedQuadToScreen(glm::vec2 position, glm::vec2 size, GLuint tex_id)
{
	static Program* framebuffer_texture_shader = new Program(R"(
		#version 120

		attribute vec3 position;
		attribute vec2 texcoord;

		uniform mat4 projection, model, view;

		varying vec2 f_texcoord;

		void main(void)
		{
			gl_Position = projection * view * model * vec4(position, 1.0);
			f_texcoord = texcoord;
		}
	)",R"(
		#version 120

		uniform sampler2D albedo;

		varying vec2 f_texcoord;

		void main(void)
		{
			gl_FragColor = texture2D(albedo,f_texcoord);
		}
	)");
	static std::unique_ptr<Mesh> fbo_mesh = extras::plane(1,1);

	glm::mat4 mat_2d_projection = glm::ortho(0.f,800.f,0.f,600.f,0.1f,10.f);
	glm::mat4 mat_2d_view = glm::lookAt(glm::vec3(0,0,-1), glm::vec3(0,0,1), glm::vec3(0,1,0));

	glm::mat4 mat_model(1.f);
	glm::vec3 fbo_mesh_position((position*glm::vec2(-1,1))-(size/2.f),0.f);
	mat_model = glm::translate(mat_model,fbo_mesh_position);
	mat_model = glm::scale(mat_model, glm::vec3(size/glm::vec2(2.f,-2.f),0));

	Program* p = framebuffer_texture_shader;

	glUseProgram(p->id());

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex_id);
	glUniform1i(glGetUniformLocation(p->id(),"albedo"),0);

	glUniformMatrix4fv(glGetUniformLocation(p->id(),"model"), 1, GL_FALSE, glm::value_ptr(mat_model));
	glUniformMatrix4fv(glGetUniformLocation(p->id(),"view"), 1, GL_FALSE, glm::value_ptr(mat_2d_view));
	glUniformMatrix4fv(glGetUniformLocation(p->id(),"projection"), 1, GL_FALSE, glm::value_ptr(mat_2d_projection));

	glEnableVertexAttribArray(glGetAttribLocation(p->id(), "position"));
	glBindBuffer(GL_ARRAY_BUFFER, fbo_mesh->buffers[Mesh::BufferType::VERTEX]);
	glVertexAttribPointer(glGetAttribLocation(p->id(), "position"),3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void*)offsetof(Vertex, x));

	glEnableVertexAttribArray(glGetAttribLocation(p->id(), "texcoord"));
	glBindBuffer(GL_ARRAY_BUFFER, fbo_mesh->buffers[Mesh::BufferType::VERTEX]);
	glVertexAttribPointer(glGetAttribLocation(p->id(), "texcoord"),2,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void*)offsetof(Vertex, u));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, fbo_mesh->buffers[Mesh::BufferType::ELEMENT]);
	glDrawElements(GL_TRIANGLES, fbo_mesh->element_number, GL_UNSIGNED_INT, (void*)0);

	glDisableVertexAttribArray(glGetAttribLocation(p->id(), "position"));
	glDisableVertexAttribArray(glGetAttribLocation(p->id(), "texcoord"));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glUseProgram(0);
}

//POINT LIGHT

extras::PointLight::PointLight(glm::vec3 position, glm::vec3 color, float power, float max_distance)
	: position(position), color(color), power(power), max_distance(max_distance)
{

}

const Program& extras::PointLight::program()
{
	static const Program p = extras::program_from_file("assets/shaders/deferred_point_light.vs.glsl", "assets/shaders/deferred_point_light.fs.glsl");
	return p;
}

const std::unique_ptr<Mesh>& extras::PointLight::mesh()
{
	static const std::unique_ptr<Mesh> m = extras::mesh_from_file("assets/models/sphere.obj");
	return m;
}

void extras::PointLight::shader_constants(glm::mat4 view, glm::mat4 projection, Program* p) 
{
	glm::mat4 mat_model(1.f);
	mat_model = glm::translate(mat_model, position);
	mat_model = glm::scale(mat_model, glm::vec3(max_distance));
	
	if(p)
	{
		glUseProgram(p->id());
		glUniformMatrix4fv(glGetUniformLocation(p->id(),"model"), 1, GL_FALSE, glm::value_ptr(mat_model));
		glUniformMatrix4fv(glGetUniformLocation(p->id(),"view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(p->id(),"projection"), 1, GL_FALSE, glm::value_ptr(projection));
		
		return;
	}
	
	glUseProgram(program().id());
	glUniformMatrix4fv(glGetUniformLocation(program().id(),"model"), 1, GL_FALSE, glm::value_ptr(mat_model));
	glUniformMatrix4fv(glGetUniformLocation(program().id(),"view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(program().id(),"projection"), 1, GL_FALSE, glm::value_ptr(projection));

	glUniform3f(glGetUniformLocation(program().id(), "light.position"), position.x, position.y, position.z);
	glUniform3f(glGetUniformLocation(program().id(), "light.color"), color.x, color.y, color.z);
	glUniform1f(glGetUniformLocation(program().id(), "light.power"), power);
	glUniform1f(glGetUniformLocation(program().id(), "light.max_distance"), max_distance);
}

//SPOT LIGHT

extras::SpotLight::SpotLight(glm::vec3 position, glm::vec3 color, glm::vec3 rotation, float power, float height, float radius, float falloff)
	: position(position), color(color), rotation(rotation), power(power), height(height), radius(radius), falloff(falloff)
{

}

const Program& extras::SpotLight::program()
{
	static const Program p = extras::program_from_file("assets/shaders/deferred_spot_light.vs.glsl", "assets/shaders/deferred_spot_light.fs.glsl");
	return p;
}

const std::unique_ptr<Mesh>& extras::SpotLight::mesh()
{
	static const std::unique_ptr<Mesh> m = extras::mesh_from_file("assets/models/cone.obj");
	return m;
}

void extras::SpotLight::shader_constants(glm::mat4 view, glm::mat4 projection, Program* p)
{
	glm::mat4 mat_model(1.f);
	mat_model = glm::translate(mat_model, position);
	mat_model = glm::rotate(mat_model, rotation.x, glm::vec3(1,0,0));
	mat_model = glm::rotate(mat_model, rotation.y, glm::vec3(0,1,0));
	mat_model = glm::rotate(mat_model, rotation.z, glm::vec3(0,0,1));
	mat_model = glm::scale(mat_model, glm::vec3(height*tanf(falloff*3.14/180)+1,height,height*tanf(falloff*3.14/180)+1));
	
	if(p)
	{
		glUseProgram(p->id());
		glUniformMatrix4fv(glGetUniformLocation(p->id(),"model"), 1, GL_FALSE, glm::value_ptr(mat_model));
		glUniformMatrix4fv(glGetUniformLocation(p->id(),"view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(p->id(),"projection"), 1, GL_FALSE, glm::value_ptr(projection));
		
		return;
	}
	
	glUseProgram(program().id());
	glUniformMatrix4fv(glGetUniformLocation(program().id(),"model"), 1, GL_FALSE, glm::value_ptr(mat_model));
	glUniformMatrix4fv(glGetUniformLocation(program().id(),"view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(program().id(),"projection"), 1, GL_FALSE, glm::value_ptr(projection));

	glUniform3f(glGetUniformLocation(program().id(), "light.color"), color.x, color.y, color.z);
	glUniform3f(glGetUniformLocation(program().id(), "light.position"), position.x, position.y, position.z);
	glUniform3f(glGetUniformLocation(program().id(), "light.rotation"), rotation.x, rotation.y, rotation.z);
	glUniform1f(glGetUniformLocation(program().id(), "light.power"), power);
	glUniform1f(glGetUniformLocation(program().id(), "light.height"), height);
	glUniform1f(glGetUniformLocation(program().id(), "light.radius"), radius*(3.14)/(180));
	glUniform1f(glGetUniformLocation(program().id(), "light.falloff"), falloff*(3.14)/(180));
}