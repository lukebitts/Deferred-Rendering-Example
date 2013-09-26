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
    float lambert = clamp(dot(n,l),0,1);

	vec3 r = normalize(reflect(light.position - frag_position.xyz, frag_normal));
    vec3 v = normalize(camera.position - frag_position.xyz);
    float phong = pow(max(dot(-r,v),0.),20.);

	float spot_alpha = dot(-l,normalize(model3x3 * vec3(0,-1,0)));
	float inner_alpha = cos(light.falloff);
	float outer_alpha = cos(light.radius);
	float spot = clamp((spot_alpha - outer_alpha) / (inner_alpha - outer_alpha), 0., 1.);

	vec4 diffuse = frag_color * vec4(light.color, 1.) * light.power * lambert * spot;
	vec4 specular = frag_specular * vec4(light.color, 1.) * phong * spot;

    vec4 frag_shadow_position = light.projection * light.view * frag_position;
    frag_shadow_position = frag_shadow_position / frag_shadow_position.w / 2.f + 0.5f;
	
	float frag_distance = length(light.position - frag_position.xyz);
	float C = 1.;
    float L = 4.5/light.height;
    float Q = 75./pow(light.height,2.);

	float attenuation = C / ((1.+L*frag_distance)*(1.+Q*frag_distance*frag_distance));
    attenuation = max(attenuation - 0.002,0.);
	attenuation *= light.power;

    float bias = 0;//clamp(0.005*tan(acos(clamp(dot(n,l),0,1))),0,0.001);

    if(texture2D(light.tex_depth,frag_shadow_position.xy).z < frag_shadow_position.z - bias)
        gl_FragColor = (diffuse + specular) * attenuation * 0.2;
    else
        gl_FragColor = (diffuse + specular) * attenuation;
}
