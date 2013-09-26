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
    float power;
    float max_distance;
} light;

uniform struct Camera
{
    vec3 position;
} camera;

void main(void)
{
	vec2 screen_coordinates = gl_FragCoord.xy / vec2(window_width,window_height);

    vec4 frag_color = texture2D(tex_color,screen_coordinates);
    vec4 frag_position = texture2D(tex_position,screen_coordinates);
    vec3 frag_normal = normalize(texture2D(tex_normal,screen_coordinates).rgb);
    vec4 frag_specular = texture2D(tex_specular,screen_coordinates);

    vec3 n = normalize(frag_normal);
    vec3 l = normalize(light.position - frag_position.xyz);
    float lambert = max(dot(n,l),0.);

    vec3 r = normalize(reflect(light.position - frag_position.xyz, frag_normal));
    vec3 v = normalize(camera.position - frag_position.xyz);
    float phong = pow(max(dot(-r,v),0.),20.);

    float frag_distance = length(light.position - frag_position.xyz);

    float C = 1.;
    float L = 4.5/light.max_distance;
    float Q = 75./pow(light.max_distance,2.);
	
    float attenuation = C / ((1.+L*frag_distance)*(1.+Q*frag_distance*frag_distance));
    attenuation = max(attenuation - 0.002,0.);

    vec4 diffuse = frag_color * vec4(light.color,1.) * lambert;
    vec4 specular = frag_specular * vec4(light.color,1.) * phong;

    gl_FragColor = (diffuse + specular) * attenuation * light.power;
}