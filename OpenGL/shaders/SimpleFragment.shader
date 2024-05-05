#version 330 core
out vec4 FragColor;

in vec3 color;
//in vec2 uv

uniform sampler2D mainTex;

void main() {
	FragColor = vec4(color, 1.0f);// *texture(mainTex, uv);
}