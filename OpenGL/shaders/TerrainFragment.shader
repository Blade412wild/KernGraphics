#version 330 core
out vec4 FragColor;

in vec2 uv;
in vec4 worldPosition;

uniform sampler2D mainTex;
uniform sampler2D normalTex;

uniform sampler2D dirt, sand, grass, rock, snow;

uniform vec3 lightDirection;
uniform vec3 cameraPosition;

vec3 lerp(vec3 a, vec3 b, float t) {
	return a + (b - a) * t;
}


void main() {

	//normal
	vec3 normal = texture(normalTex, uv).rgb;
	normal = normalize(normal * 2.0 * 1.0);
	normal.gb = normal.bg;
	normal.r = -normal.r;
	normal.b = -normal.b;


	//specular data
	vec3 viewDir = normalize(worldPosition.xyz - cameraPosition);
	//vec3 reflDir = normalize(reflect(lightDirection, normal));

	//lighting
	float lightValue = max(dot(normal, lightDirection), 0.0);
	//float specular = pow(max( - dot(reflDir, viewDir), 0.0), 8.0);

	//build Color!
	float y = worldPosition.y;

	float ds = clamp((y - 50) / 10, -1, 1) * .5 + .5;
	float sg = clamp((y - 75) / 10, -1, 1) * .5 + .5;
	float gr = clamp((y - 125) / 10, -1, 1) * .5 + .5;
	float rs = clamp((y - 200) / 10, -1, 1) * .5 + .5;

	float dist = length(worldPosition.xyz - cameraPosition);
	float uvLerp = clamp((dist - 250) / 150, -1, 1) * .5 + .5;

	vec3 dirtColorClose = texture(dirt, uv * 100).rgb;
	vec3 sandColorClose = texture(sand, uv * 100).rgb;
	vec3 grassColorCloser = texture(grass, uv * 100).rgb;
	vec3 rockColorClose = texture(rock, uv * 100).rgb;
	vec3 snowColorClose = texture(snow, uv * 100).rgb;

	vec3 dirtColorFar = texture(dirt, uv * 10).rgb;
	vec3 sandColorFar = texture(sand, uv * 10).rgb;
	vec3 grassColorFar = texture(grass, uv * 10).rgb;
	vec3 rockColorFar = texture(rock, uv * 10).rgb;
	vec3 snowColorFar = texture(snow, uv * 10).rgb;

	vec3 dirtColor = lerp(dirtColorClose, dirtColorFar, uvLerp);
	vec3 sandColor = lerp(sandColorClose, sandColorFar, uvLerp);
	vec3 grassColor = lerp(grassColorCloser, grassColorFar, uvLerp);
	vec3 rockColor = lerp(rockColorClose, rockColorFar, uvLerp);
	vec3 snowColor = lerp(snowColorClose, snowColorFar, uvLerp);


	vec3 diffuse = lerp(lerp(lerp(lerp(dirtColor, sandColor, ds), grassColor, sg), rockColor, gr), snowColor, rs);

	float fog = pow(clamp((dist - 250) / 1000, 0, 1), 2);

	vec3 topColor = vec3(68.0f / 255.0f, 118.0f / 255.0f, 189.0 / 255.0f);
	vec3 botColor = vec3(188.0f / 255.0f, 214.0f / 255.0f, 231.0 / 255.0f);

	vec3 fogColor = lerp(botColor, topColor, max(viewDir.y, 0.0));



	//seperate RGB and Aplha calculations
	//vec4 output = texture(grass, uv);
	//output.rgb = vec3(min(lightValue + 0.1, 1.0));
	vec4 output = vec4( lerp(diffuse * min(lightValue + 0.1, 1.0), fogColor, fog),  1.0);// + specular;

	FragColor = output;
}