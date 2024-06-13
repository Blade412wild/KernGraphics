#version 330 core
out vec4 FragColor;

in vec3 color;
in vec2 uv;
in mat3 tbn;
in vec4 worldPosition;

uniform sampler2D diffuseTex;
uniform sampler2D normalTex;

uniform vec3 lightDirection;
uniform vec3 cameraPosition;
uniform vec3 lightColor;


void main() {

	//normal
	vec3 normal = texture(normalTex, uv).rgb;
	normal = normalize(normal * 2.0 * 1.0);

	// transform with TBN
	normal = tbn * normal;
	normal = normalize(normal);

	vec3 ammbient =  0.1 * vec3(texture(diffuseTex, uv));
	float lightValue = max(dot(normal, lightDirection), 0.0);
	vec3 diffuse = 1.0 * vec3(texture(diffuseTex, uv)) * lightValue;

	//specular data
	vec3 viewDir = normalize(cameraPosition - worldPosition.xyz);
	vec3 reflDir = reflect(-lightDirection, normal);

	float specularLightValue = pow(max(dot(reflDir, viewDir), 0.0), 64.0);

	vec3 specular = 1.0 * vec3(texture(diffuseTex, uv)) * specularLightValue;

	//seperate RGB and Aplha calculations
	vec4 output = vec4(ammbient + diffuse + specular, 1.0);
	FragColor = output;
}