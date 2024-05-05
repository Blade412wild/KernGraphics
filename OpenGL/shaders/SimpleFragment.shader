#version 330 core
out vec4 FragColor;

in vec3 color;
in vec2 uv;
in vec3 normal;
in vec3 worldPosition;

uniform sampler2D mainTex;
uniform vec3 lightPosition;

void main() {
	//lighting
	vec3 lightDirection = normalize(worldPosition - lightPosition);
	float lightValue = max(-dot(normal, lightDirection), 0.0);

	FragColor = vec4(color, 1.0f) * texture(mainTex, uv) * min(lightValue + 0.1, 1.0);
}