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

	vec3 sunColor = vec3(1.0 , 200 / 255.0, 50 / 255.0);

	//calculate view 
	vec3 viewDir = normalize(worldPosition.rgb - cameraPosition);

	float sun = max(pow(dot(viewDir, lightDirection), 128), 0.0);
	FragColor = vec4(Lerp(botColor, topColor, abs(viewDir.y)) + sun * sunColor, 1);
}