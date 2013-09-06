//
//  main.cpp
//  OpenGLTest
//
//  Created by Lucas Bittencourt on 6/4/13.
//  Copyright (c) 2013 Lucas Bittencourt. All rights reserved.
//

#define GL_GLEXT_PROTOTYPES
#include <GL/glfw.h>

#ifdef __APPLE__
	#include <OpenGL/glext.h>
#else
	#include <GL/glext.h>
#endif

#include <sstream>
#include <iostream>

#include "glm/glm.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "Mesh.hpp"
#include "Vertex.hpp"
#include "Texture2D.hpp"
#include "Shaders.hpp"
#include "Program.hpp"
#include "Extras.hpp"

#include "GLErrors.hpp"

template <typename T>
constexpr T degToRad(T deg)
{
	static_assert(std::is_same<T, float>::value || std::is_same<T, double>::value,"");
	return deg*((T)3.14)/((T)180);
}

bool wireframe = false;

int main(int argc, char* argv[])
{
	glfwInit();
	glfwOpenWindowHint(GLFW_VERSION_MINOR, 2);
	glfwOpenWindowHint(GLFW_VERSION_MAJOR, 1);
	glfwOpenWindowHint(GLFW_FSAA_SAMPLES, 0);
	glfwOpenWindow(800, 600, 0, 0, 0, 0, 8, 0, GLFW_WINDOW);
	glfwSwapInterval(1);
	
	glfwSetWindowPos(2000, 100);
	
	glfwSetKeyCallback([](int key, int press) -> void {
		extern bool wireframe;
		
		if(key == 'W' && press)
			wireframe = !wireframe;
	});

	/*******/
    ///@todo find an alternative to the framebuffers extension when it's not available
	///@todo cleanup the framebuffer
	
	GLuint fbo;
	Texture2D imgs[5] = {
		{0, GL_RGBA8,  800, 600, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL, {{GL_TEXTURE_MIN_FILTER,GL_LINEAR}}},
		{0, GL_RGBA32F_ARB,  800, 600, 0, GL_RGBA, GL_FLOAT, NULL, {{GL_TEXTURE_MIN_FILTER,GL_LINEAR}}},
		{0, GL_RGBA16F_ARB,  800, 600, 0, GL_RGBA, GL_FLOAT, NULL, {{GL_TEXTURE_MIN_FILTER,GL_LINEAR}}},
		{0, GL_RGB8,  800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL, {{GL_TEXTURE_MIN_FILTER,GL_LINEAR}}},
		{0, GL_DEPTH_COMPONENT16,  800, 600, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL, {{GL_TEXTURE_MIN_FILTER,GL_LINEAR}}}
	};
	glGenFramebuffersEXT(1, &fbo);

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);

    {
        GLuint buffer;
        glGenRenderbuffersEXT(1, &buffer);
        glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, buffer);
        glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, 800, 600);

        glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, buffer);

        glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, imgs[0].id, 0);
        glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT1_EXT, GL_TEXTURE_2D, imgs[1].id, 0);
        glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT2_EXT, GL_TEXTURE_2D, imgs[2].id, 0);
        glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT3_EXT, GL_TEXTURE_2D, imgs[3].id, 0);
        glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT , GL_TEXTURE_2D, imgs[4].id, 0);
		
        if(glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT) != GL_FRAMEBUFFER_COMPLETE_EXT)
            return 1;
    }

    glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	
	GLuint light_fbo;
	Texture2D light_img{0, GL_DEPTH_COMPONENT16,  800, 600, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL, {{GL_TEXTURE_MIN_FILTER,GL_LINEAR}}};
	Texture2D lightcolor_img{0, GL_RGB8,  800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL, {{GL_TEXTURE_MIN_FILTER,GL_LINEAR}}};
	glGenFramebuffersEXT(1, &light_fbo);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, light_fbo);

    {	
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, lightcolor_img.id, 0);
        glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT , GL_TEXTURE_2D, light_img.id, 0);
		
		glDrawBuffer(GL_NONE);
		
		std::cout<<glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
		
        if(glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT) != GL_FRAMEBUFFER_COMPLETE_EXT)
			return 1;
    }

    glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	
	glDrawBuffer(GL_BACK);
	glReadBuffer(GL_FRONT);

	/*******/

	std::vector<extras::DeferredModel> models;
	
	models.emplace_back(
		extras::mesh_from_file("assets/models/chesterfield.obj"),
		extras::texture2d_from_file("assets/textures/chesterfield_color.png"),
		extras::texture2d_from_file("assets/textures/chesterfield_normal.png"),
		extras::texture2d_from_file("assets/textures/chesterfield_specular.png")
	);
	
	models.emplace_back(
		extras::mesh_from_file("assets/models/dragon.obj"),
		extras::texture2d_from_file("assets/textures/chesterfield_color.png"),
		extras::texture2d_from_file("assets/textures/chesterfield_normal.png"),
		extras::texture2d_from_file("assets/textures/chesterfield_specular.png")
	);
	models.back().scale = glm::vec3(3,3,3);
	
	std::vector<std::unique_ptr<extras::IDeferredLight>> lights;

	//lights.emplace_back(new extras::PointLight{glm::vec3(3,4,0),glm::vec3(1),200.f,10.f});
	lights.emplace_back(new extras::PointLight{glm::vec3(-3,2,-5),glm::vec3(1.f,1.f,1.f),200.f,10.f});
	//lights.emplace_back(new extras::SpotLight{glm::vec3(0,5.0,0), glm::vec3(1), glm::vec3(0,0,0), 1.f, 5.f, 1.f, 45.f});
	//glm::vec3 position, glm::vec3 color, glm::vec3 rotation, float power, float height, float radius, float falloff
	
	extras::SpotLight* spot = new extras::SpotLight{glm::vec3(0,5.0,0), glm::vec3(0.3,0,0), glm::vec3(0,0,0), 10.f, 10.f, 15.f, 25.f};//static_cast<extras::SpotLight*>(lights[1].get());

	Program deferred_first_pass_shader(deferred_first_pass_vs,deferred_first_pass_fs);
	Program framebuffer_texture_shader(framebuffer_texture_vs,framebuffer_texture_fs);
	Program wireframe_shader(wireframe_vs,wireframe_fs);
	Program depth_shader(depth_vs,depth_fs);

	glm::mat4 mat_projection = glm::perspective(75.f, (float)800 / (float)600, 1.0f, 100.f);
	
	glm::vec3 cam_pos{0,6,-6};

	int frame = 0;
	int realFrame = 0;
	double accum = 0;
	while(!glfwGetKey(GLFW_KEY_ESC) && glfwGetWindowParam(GLFW_OPENED))
	{
		/*FPS*/
		frame++;
		realFrame++;
		std::stringstream ss;
		ss<<(frame/(glfwGetTime()-accum));
		if(frame >= 100) {
			frame = 0;
			accum = glfwGetTime();
		}
		glfwSetWindowTitle(ss.str().c_str());
		/*END FPS*/

		glfwPollEvents();

		glm::mat4 mat_view(1);
		mat_view = glm::lookAt(cam_pos, glm::vec3(0,0,0), glm::vec3(0,1,0));
		
		//((extras::PointLight*)lights[0].get())->position.x = sinf(realFrame/50.f)*5.f;
		//((extras::SpotLight*)lights[0].get())->position.x = sinf(realFrame/50.f)*5.f;
		cam_pos.x = cosf(realFrame/300.f)*5.f;
		spot->rotation.z = sinf(realFrame/20.f)*30.f;
		spot->rotation.x = sinf(realFrame/80.f)*30.f;

        //first pass (draw information to the framebuffer)
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);
        GLenum buffers[] = {GL_COLOR_ATTACHMENT0_EXT, GL_COLOR_ATTACHMENT1_EXT, GL_COLOR_ATTACHMENT2_EXT, GL_COLOR_ATTACHMENT3_EXT};
        glDrawBuffers(4,buffers);

		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glDisable(GL_BLEND);

        glClearColor(.0f,.0f,.0f,1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        for(auto&& model : models)
        {
            Program* p = &deferred_first_pass_shader;

            glm::mat4 mat_model(1.f);
            mat_model = glm::translate(mat_model, model.position);
            mat_model = mat_model * glm::toMat4(model.rotation);
            mat_model = glm::scale(mat_model, model.scale);

            glm::mat3 model3x3 = glm::mat3(mat_model);

            glUseProgram(p->id());
            glUniformMatrix4fv(glGetUniformLocation(p->id(),"model"), 1, GL_FALSE, glm::value_ptr(mat_model));
            glUniformMatrix4fv(glGetUniformLocation(p->id(),"view"), 1, GL_FALSE, glm::value_ptr(mat_view));
            glUniformMatrix4fv(glGetUniformLocation(p->id(),"projection"), 1, GL_FALSE, glm::value_ptr(mat_projection));
            glUniformMatrix3fv(glGetUniformLocation(p->id(),"model3x3"), 1, GL_FALSE, glm::value_ptr(model3x3));
			
			glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, model.color->id);
            glUniform1i(glGetUniformLocation(p->id(),"diffuse_map"),0);
			
			glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, model.normal->id);
            glUniform1i(glGetUniformLocation(p->id(),"normal_map"),1);
			
			glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, model.specular->id);
            glUniform1i(glGetUniformLocation(p->id(),"specular_map"),2);

            glEnableVertexAttribArray(glGetAttribLocation(p->id(), "position"));
            glBindBuffer(GL_ARRAY_BUFFER, model.mesh->buffers[Mesh::BufferType::VERTEX]);
            glVertexAttribPointer(glGetAttribLocation(p->id(), "position"),3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void*)offsetof(Vertex, x));

            glEnableVertexAttribArray(glGetAttribLocation(p->id(), "texcoord"));
            glBindBuffer(GL_ARRAY_BUFFER, model.mesh->buffers[Mesh::BufferType::VERTEX]);
            glVertexAttribPointer(glGetAttribLocation(p->id(), "texcoord"),2,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void*)offsetof(Vertex, u));

            glEnableVertexAttribArray(glGetAttribLocation(p->id(), "normal"));
            glBindBuffer(GL_ARRAY_BUFFER, model.mesh->buffers[Mesh::BufferType::VERTEX]);
            glVertexAttribPointer(glGetAttribLocation(p->id(), "normal"),3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void*)offsetof(Vertex, nx));

            glEnableVertexAttribArray(glGetAttribLocation(p->id(), "tangent"));
            glBindBuffer(GL_ARRAY_BUFFER, model.mesh->buffers[Mesh::BufferType::VERTEX]);
            glVertexAttribPointer(glGetAttribLocation(p->id(), "tangent"),3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void*)offsetof(Vertex, tx));

            glEnableVertexAttribArray(glGetAttribLocation(p->id(), "bitangent"));
            glBindBuffer(GL_ARRAY_BUFFER, model.mesh->buffers[Mesh::BufferType::VERTEX]);
            glVertexAttribPointer(glGetAttribLocation(p->id(), "bitangent"),3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void*)offsetof(Vertex, bx));

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model.mesh->buffers[Mesh::BufferType::ELEMENT]);
            glDrawElements(GL_TRIANGLES, model.mesh->element_number, GL_UNSIGNED_INT, (void*)0);

            glDisableVertexAttribArray(glGetAttribLocation(p->id(), "position"));
            glDisableVertexAttribArray(glGetAttribLocation(p->id(), "texcoord"));
            glDisableVertexAttribArray(glGetAttribLocation(p->id(), "normal"));
            glDisableVertexAttribArray(glGetAttribLocation(p->id(), "tangent"));
            glDisableVertexAttribArray(glGetAttribLocation(p->id(), "bitangent"));
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            glUseProgram(0);
        }

		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
		
		//second pass: render geometry depth from the point of view of every shadow casting light
		
		glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);
		glCullFace(GL_FRONT);
		
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, light_fbo);
        GLenum light_buffers[] = {GL_COLOR_ATTACHMENT0_EXT};
        glDrawBuffers(1,light_buffers);
		
		glClearColor(.0f,.0f,.0f,1.f);
        glClear(GL_DEPTH_BUFFER_BIT);
		
		glm::mat4 mat_light_view;
		mat_light_view = glm::rotate(mat_light_view, spot->rotation.x + 90.f, glm::vec3(1.f,0.f,0.f));
		mat_light_view = glm::rotate(mat_light_view, spot->rotation.y, glm::vec3(0.f,1.f,0.f));
		mat_light_view = glm::rotate(mat_light_view, spot->rotation.z, glm::vec3(0.f,0.f,1.f));
		mat_light_view = glm::translate(mat_light_view, -spot->position);
		
		glm::mat4 mat_light_projection = glm::perspective(2*spot->falloff, (float)800 / (float)600, 1.0f, 100.f);
		
		for(auto&& model : models)
        {
            Program* p = &depth_shader;

            glm::mat4 mat_model(1.f);
            mat_model = glm::translate(mat_model, model.position);
            mat_model = mat_model * glm::toMat4(model.rotation);
            mat_model = glm::scale(mat_model, model.scale);

            glUseProgram(p->id());
            glUniformMatrix4fv(glGetUniformLocation(p->id(),"model"), 1, GL_FALSE, glm::value_ptr(mat_model));
            glUniformMatrix4fv(glGetUniformLocation(p->id(),"view"), 1, GL_FALSE, glm::value_ptr(mat_light_view));
            glUniformMatrix4fv(glGetUniformLocation(p->id(),"projection"), 1, GL_FALSE, glm::value_ptr(mat_light_projection));

            glEnableVertexAttribArray(glGetAttribLocation(p->id(), "position"));
            glBindBuffer(GL_ARRAY_BUFFER, model.mesh->buffers[Mesh::BufferType::VERTEX]);
            glVertexAttribPointer(glGetAttribLocation(p->id(), "position"),3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void*)offsetof(Vertex, x));

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model.mesh->buffers[Mesh::BufferType::ELEMENT]);
            glDrawElements(GL_TRIANGLES, model.mesh->element_number, GL_UNSIGNED_INT, (void*)0);

            glDisableVertexAttribArray(glGetAttribLocation(p->id(), "position"));
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            glUseProgram(0);
        }
		
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
		
		glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
		
		//third pass: render the light geometry with the information from the first and seconds passes
		glClearColor(.0f,.0f,.0f,0.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		
		glEnable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
		glBlendFunc(GL_ONE, GL_ONE);
		glDepthMask(GL_FALSE);
		//glCullFace(GL_FRONT);
		glDepthFunc(GL_GEQUAL);
		
		glBindFramebufferEXT(GL_READ_FRAMEBUFFER_EXT, fbo);
		glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER_EXT, 0);
		glBlitFramebufferEXT(0, 0, 800, 600, 0, 0, 800, 600, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
		
		for(auto&& light : lights)
		{
			const Program* p = &light->program();
			
			glUseProgram(p->id());
			
			light->shader_constants(mat_view, mat_projection);
			
			glUniform1i(glGetUniformLocation(p->id(), "window_width"),800);
			glUniform1i(glGetUniformLocation(p->id(), "window_height"),600);
			
			glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, imgs[0].id);
            glUniform1i(glGetUniformLocation(p->id(),"tex_color"),0);
			
			glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, imgs[1].id);
            glUniform1i(glGetUniformLocation(p->id(),"tex_position"),1);
			
			glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, imgs[2].id);
            glUniform1i(glGetUniformLocation(p->id(),"tex_normal"),2);
			
			glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_2D, imgs[3].id);
            glUniform1i(glGetUniformLocation(p->id(),"tex_specular"),3);
			
			glActiveTexture(GL_TEXTURE4);
            glBindTexture(GL_TEXTURE_2D, imgs[4].id);
            glUniform1i(glGetUniformLocation(p->id(),"tex_depth"),4);
			
			glUniform3f(glGetUniformLocation(p->id(), "camera.position"), cam_pos.x, cam_pos.y, cam_pos.z);
			
			glEnableVertexAttribArray(glGetAttribLocation(p->id(), "position"));
            glBindBuffer(GL_ARRAY_BUFFER, light->mesh()->buffers[Mesh::BufferType::VERTEX]);
            glVertexAttribPointer(glGetAttribLocation(p->id(), "position"),3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void*)offsetof(Vertex, x));
			
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, light->mesh()->buffers[Mesh::BufferType::ELEMENT]);
            glDrawElements(GL_TRIANGLES, light->mesh()->element_number, GL_UNSIGNED_INT, (void*)0);

            glDisableVertexAttribArray(glGetAttribLocation(p->id(), "position"));
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            glUseProgram(0);
		}
		
		//render the spot light
		{
			const Program* p = &spot->program();
			
			glUseProgram(p->id());
			
			spot->shader_constants(mat_view, mat_projection);
			
			glUniform1i(glGetUniformLocation(p->id(), "window_width"),800);
			glUniform1i(glGetUniformLocation(p->id(), "window_height"),600);
			
			glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, imgs[0].id);
            glUniform1i(glGetUniformLocation(p->id(),"tex_color"),0);
			
			glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, imgs[1].id);
            glUniform1i(glGetUniformLocation(p->id(),"tex_position"),1);
			
			glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, imgs[2].id);
            glUniform1i(glGetUniformLocation(p->id(),"tex_normal"),2);
			
			glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_2D, imgs[3].id);
            glUniform1i(glGetUniformLocation(p->id(),"tex_specular"),3);
			
			glActiveTexture(GL_TEXTURE4);
            glBindTexture(GL_TEXTURE_2D, imgs[4].id);
            glUniform1i(glGetUniformLocation(p->id(),"tex_depth"),4);
			
			//The shader should receive the depth map from the prev pass and the mvp used
			glActiveTexture(GL_TEXTURE5);
            glBindTexture(GL_TEXTURE_2D, light_img.id);
            glUniform1i(glGetUniformLocation(p->id(),"light_depth"),4);
			
			glUniformMatrix4fv(glGetUniformLocation(p->id(),"light_projection"), 1, GL_FALSE, glm::value_ptr(mat_light_projection));
			glUniformMatrix4fv(glGetUniformLocation(p->id(),"light_view"), 1, GL_FALSE, glm::value_ptr(mat_light_view));
			
			glUniform3f(glGetUniformLocation(p->id(), "camera.position"), cam_pos.x, cam_pos.y, cam_pos.z);
			
			glEnableVertexAttribArray(glGetAttribLocation(p->id(), "position"));
            glBindBuffer(GL_ARRAY_BUFFER, spot->mesh()->buffers[Mesh::BufferType::VERTEX]);
            glVertexAttribPointer(glGetAttribLocation(p->id(), "position"),3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void*)offsetof(Vertex, x));
			
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, spot->mesh()->buffers[Mesh::BufferType::ELEMENT]);
            glDrawElements(GL_TRIANGLES, spot->mesh()->element_number, GL_UNSIGNED_INT, (void*)0);

            glDisableVertexAttribArray(glGetAttribLocation(p->id(), "position"));
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            glUseProgram(0);
		}
		
        //fourth pass (draw the to quads on the screen)
        
		glDepthMask(GL_TRUE);
		glDepthFunc(GL_LESS);
		glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
		
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		glDisable(GL_BLEND);
		glDisable(GL_STENCIL_TEST);
		
		//extras::drawTexturedQuadToScreen(glm::vec2(0,600), glm::vec2(400,300), light_img.id);
		//extras::drawTexturedQuadToScreen(glm::vec2(400,600), glm::vec2(400,300), imgs[4].id);

        /*glm::mat4 mat_2d_projection = glm::ortho(0.f,800.f,600.f,0.f,0.1f,10.f);
        glm::mat4 mat_2d_view = glm::lookAt(glm::vec3(0,0,-1), glm::vec3(0,0,1), glm::vec3(0,1,0));

        for(int tex = 0; tex < 5; ++tex) {
            glm::mat4 mat_model(1.f);
            glm::vec3 fbo_mesh_position((tex)*-800/4.f,50.f,0.f);
            mat_model = glm::translate(mat_model,fbo_mesh_position);
            mat_model = glm::scale(mat_model, glm::vec3(800/8,600/8,0));

            Program* p = &framebuffer_texture_shader;

            glUseProgram(p->id());

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, imgs[tex].id);
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
        }*/

        //fifth pass (geometry edges)
		if(wireframe)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			for(auto&& model : models)
			{
				Program* p = &wireframe_shader;

				glm::mat4 mat_model(1.f);
				mat_model = glm::translate(mat_model, model.position);
				mat_model = mat_model * glm::toMat4(model.rotation);
				mat_model = glm::scale(mat_model, model.scale);

				glUseProgram(p->id());
				glUniformMatrix4fv(glGetUniformLocation(p->id(),"model"), 1, GL_FALSE, glm::value_ptr(mat_model));
				glUniformMatrix4fv(glGetUniformLocation(p->id(),"view"), 1, GL_FALSE, glm::value_ptr(mat_view));
				glUniformMatrix4fv(glGetUniformLocation(p->id(),"projection"), 1, GL_FALSE, glm::value_ptr(mat_projection));

				glEnableVertexAttribArray(glGetAttribLocation(p->id(), "position"));
				glBindBuffer(GL_ARRAY_BUFFER, model.mesh->buffers[Mesh::BufferType::VERTEX]);
				glVertexAttribPointer(glGetAttribLocation(p->id(), "position"),3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void*)offsetof(Vertex, x));

				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model.mesh->buffers[Mesh::BufferType::ELEMENT]);
				glDrawElements(GL_TRIANGLES, model.mesh->element_number, GL_UNSIGNED_INT, (void*)0);

				glDisableVertexAttribArray(glGetAttribLocation(p->id(), "position"));
				glBindBuffer(GL_ARRAY_BUFFER, 0);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
				glUseProgram(0);
			}
			for(auto&& light : lights)
			{
				Program* p = &wireframe_shader;
				
				glUseProgram(p->id());
				
				light->shader_constants(mat_view, mat_projection, p);
				
				glEnableVertexAttribArray(glGetAttribLocation(p->id(), "position"));
				glBindBuffer(GL_ARRAY_BUFFER, light->mesh()->buffers[Mesh::BufferType::VERTEX]);
				glVertexAttribPointer(glGetAttribLocation(p->id(), "position"),3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void*)offsetof(Vertex, x));
				
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, light->mesh()->buffers[Mesh::BufferType::ELEMENT]);
				glDrawElements(GL_TRIANGLES, light->mesh()->element_number, GL_UNSIGNED_INT, (void*)0);

				glDisableVertexAttribArray(glGetAttribLocation(p->id(), "position"));
				glBindBuffer(GL_ARRAY_BUFFER, 0);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
				glUseProgram(0);
			}
			{
				Program* p = &wireframe_shader;
				
				glUseProgram(p->id());
				
				spot->shader_constants(mat_view, mat_projection, p);
				
				glEnableVertexAttribArray(glGetAttribLocation(p->id(), "position"));
				glBindBuffer(GL_ARRAY_BUFFER, spot->mesh()->buffers[Mesh::BufferType::VERTEX]);
				glVertexAttribPointer(glGetAttribLocation(p->id(), "position"),3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void*)offsetof(Vertex, x));
				
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, spot->mesh()->buffers[Mesh::BufferType::ELEMENT]);
				glDrawElements(GL_TRIANGLES, spot->mesh()->element_number, GL_UNSIGNED_INT, (void*)0);

				glDisableVertexAttribArray(glGetAttribLocation(p->id(), "position"));
				glBindBuffer(GL_ARRAY_BUFFER, 0);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
				glUseProgram(0);
			}
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glDisable(GL_BLEND);
		}
		glfwSwapBuffers();
	}

	glfwCloseWindow();
	glfwTerminate();

    return 0;
}
