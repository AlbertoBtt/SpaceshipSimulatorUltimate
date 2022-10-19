#version 410 core

uniform vec3 objectColor;
uniform vec3 lightPos;
uniform vec3 cameraPos;
//uniform vec3 lightDir;
uniform sampler2D colorTexture;
uniform sampler2D normalSampler;

in vec3 interpNormal;
in vec3 fragPos;
in vec2 Uv;
in mat3 TBN;
in vec3 lightDirTS;
in vec3 viewDirTS;

float ambientStrength = 0.15;
//uniform vec3 lightColor =vec3(0.33f, 0.21f, 0.06f);

void main()
{
	vec3 L = -lightDirTS;
	vec3 V = normalize(viewDirTS);
	vec3 N = texture2D(normalSampler, Uv).rgb;
	N = normalize(N * 2.0 - 1.0);
	vec3 R = reflect(-normalize(L), N);

	float diffuse = max(0, dot(N, L));
	
	float specular_pow = 10;
	float specular = pow(max(0, dot(R, V)), specular_pow);

	vec3 color = texture2D(colorTexture, Uv).rgb;

	vec3 lightColor = vec3(1);
	vec3 shadedColor = color * diffuse + lightColor * specular;
	
	float ambient = 0.2;

	gl_FragColor = vec4(mix(color, shadedColor, 1.0 - ambient), 1.0);
}
