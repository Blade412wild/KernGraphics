#version 330 core
out vec4 FragColor;

in vec3 color;
in vec2 uv;
in mat3 tbn;
in vec3 worldPosition;

uniform sampler2D mainTex;
uniform sampler2D normalTex;

uniform vec3 lightPosition;

void main() {

	//normal
	vec3 normal = texture(normalTex, uv).rgb;
	normal = normalize(normal * 2.0 * 1.0);
	normal = tbn * normal;

	//lighting
	vec3 lightDirection = normalize(worldPosition - lightPosition);
	float lightValue = max(-dot(normal, lightDirection), 0.0);

	FragColor = vec4(color, 1.0f) * texture(mainTex, uv) * min(lightValue + 0.1, 1.0);
}