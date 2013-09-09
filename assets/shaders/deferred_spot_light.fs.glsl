#version 120

uniform sampler2D tex_color;
uniform sampler2D tex_position;
uniform sampler2D tex_normal;
uniform sampler2D tex_specular;
uniform sampler2D tex_depth;

uniform int window_width;
uniform int window_height;

uniform struct Light
{
    vec3 color;
    vec3 position;
	vec3 rotation;
    float power;
	float height;
	float radius;
	float falloff;
	sampler2D tex_depth;
	mat4 projection;
	mat4 view;
} light;

uniform struct Camera
{
    vec3 position;
} camera;

uniform mat4 model;
mat3 model3x3 = mat3(model);

void main(void)
{
	vec2 screen_coordinates = gl_FragCoord.xy / vec2(window_width,window_height);

    vec4 frag_color = texture2D(tex_color,screen_coordinates);
    vec4 frag_position = texture2D(tex_position,screen_coordinates);
    vec3 frag_normal = normalize(texture2D(tex_normal,screen_coordinates).rgb);
    vec4 frag_specular = texture2D(tex_specular,screen_coordinates);
    vec4 frag_depth = texture2D(tex_depth,screen_coordinates);

	vec3 n = normalize(frag_normal);
    vec3 l = normalize(light.position - frag_position.xyz);
    float lambert = max(dot(n,l),0.);

	vec3 r = normalize(reflect(light.position - frag_position.xyz, frag_normal));
    vec3 v = normalize(camera.position - frag_position.xyz);
    float phong = pow(max(dot(-r,v),0.),20.);

	float spot_alpha = dot(-l,normalize(model3x3 * vec3(0,-1,0)));
	float inner_alpha = cos(light.falloff);
	float outer_alpha = cos(light.radius);
	float spot = 1 - clamp((spot_alpha - outer_alpha) / (inner_alpha - outer_alpha), 0., 1.);

	vec4 diffuse = frag_color * vec4(light.color, 1.) * light.power * lambert * spot;
	vec4 specular = frag_specular * vec4(light.color, 1.) * phong * spot;

    mat4 bias = mat4(
        0.5, 0.0, 0.0, 0.0,
        0.0, 0.5, 0.0, 0.0,
        0.0, 0.0, 0.5, 0.0,
        0.5, 0.5, 0.5, 1.0
    );
    vec4 frag_shadow_position = bias * (light.projection * light.view * frag_position);

    if(texture2D(light.tex_depth,frag_shadow_position.xy/frag_shadow_position.w).z < frag_depth.z)
        gl_FragColor = vec4(0,0,0,0);
    else
        gl_FragColor = diffuse + specular;
	gl_FragColor = texture2D(light.tex_depth,frag_shadow_position.xy/frag_shadow_position.w);//vec4(vec2(frag_shadow_position.xy),0,1);// diffuse + specular;
}
