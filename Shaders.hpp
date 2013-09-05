// DEFERRED SHADING
#ifndef _SHADERS_HPP_
#define _SHADERS_HPP_

const char* deferred_first_pass_vs = R"(
    #version 120

    attribute vec3 position;
    attribute vec2 texcoord;
    attribute vec3 normal;
    attribute vec3 tangent;
    attribute vec3 bitangent;

    uniform mat4 projection, model, view;
    uniform mat3 model3x3;

    varying vec3 f_position;
    varying vec2 f_texcoord;
    varying mat3 f_tbn;

    void main(void)
    {
        mat4 model_view = view * model;
        gl_Position = projection * model_view * vec4(position, 1.0);

        f_position = (model * vec4(position,1.0)).xyz;
        f_texcoord = texcoord;

        vec3 f_normal = (model3x3 * normal);
        vec3 f_tangent = (model3x3 * tangent);
        vec3 f_bitangent = (model3x3 * bitangent);

        f_tbn = mat3(f_tangent,f_bitangent,f_normal);
    }
)";

const char* deferred_first_pass_fs = R"(
    #version 120

	uniform sampler2D diffuse_map;
    uniform sampler2D normal_map;
    uniform sampler2D specular_map;
    //uniform sampler2D emit_map;

    varying vec3 f_position;
    varying vec2 f_texcoord;
    varying mat3 f_tbn;

    void main(void)
    {
        gl_FragData[0] = texture2D(diffuse_map,f_texcoord);
        gl_FragData[1] = vec4(f_position,1.0);
		
        vec3 normal_pixel = normalize(texture2D(normal_map, f_texcoord).rgb * 2.0 - 1.);
        normal_pixel.g *= -1.;
        vec3 normal = normalize(f_tbn * normal_pixel);
        //normal = f_tbn[2];

        gl_FragData[2] = vec4(normal,0.);
        gl_FragData[3] = texture2D(specular_map,f_texcoord);
    }
)";

//Simple Texture

const char* framebuffer_texture_vs = R"(
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
)";

const char* framebuffer_texture_fs = R"(
    #version 120

    uniform sampler2D albedo;

    varying vec2 f_texcoord;

    void main(void)
    {
        gl_FragColor = texture2D(albedo,f_texcoord);
    }
)";

//Wireframe

const char* wireframe_vs = R"(
	#version 120

	attribute vec3 position;

	uniform mat4 projection, model, view;

	void main(void)
	{
		gl_Position = projection * view * model * vec4(position, 1.0);
	}
)";

const char* wireframe_fs = R"(
	#version 120

	void main(void)
	{
		gl_FragColor = vec4(1.,1.,1.,0.1);
	}
)";

//Depth

const char* depth_vs = R"(
    #version 120

    attribute vec3 position;
    uniform mat4 projection, model, view;

    void main(void)
    {
        gl_Position = projection * view * model * vec4(position, 1.0);
    }
)";

const char* depth_fs = R"(
    #version 120

    void main(void)
    {
        gl_FragColor = vec4(gl_FragCoord.z);
    }
)";

#endif
