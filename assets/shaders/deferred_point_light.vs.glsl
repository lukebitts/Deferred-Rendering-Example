#version 120

attribute vec3 position;

uniform mat4 projection, model, view;

void main(void)
{
	gl_Position = projection * view * model * vec4(position,1.0);
}