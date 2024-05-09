#version 330 core
out vec4 FragColor;

in vec4 worldPosition;

uniform vec3 lightDirection;
uniform vec3 cameraPosition;

vec3 Lerp(vec3 a, vec3 b, float t) {
	return a + (b - a) * t;
}


void main() {

	vec3 topColor = vec3(68.0f / 255.0f, 118.0f / 255.0f, 189.0 / 255.0f);
	vec3 botColor = vec3(188.0f / 255.0f, 214.0f / 255.0f, 231.0 / 255.0f);


	//specular data
	vec3 viewDir = normalize(worldPosition - cameraPosition);
	FragColor = vec4(Lerp(botColor, topColor, viewDir.y, 0.0, 1);
}