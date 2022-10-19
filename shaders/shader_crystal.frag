#version 410 core

uniform vec3 cameraPos;
uniform samplerCube skybox;
uniform vec3 lightDir;
uniform bool lightDirUse;
uniform vec3 lightPos;

in vec3 Normal;
in vec3 Position;


void main()
{
	float ratio = 1.00 / 2.42;
	vec3 I = normalize(Position - cameraPos);
	vec3 I2 = normalize(-lightDir);
	vec3 I3 = normalize(lightPos - Position);
	I3=-I3;
	vec3 R;
	if(lightDirUse) R = refract(I, normalize(Normal), ratio) + refract(I2, normalize(Normal), ratio) + refract(I3, normalize(Normal), ratio);
	else R = refract(I, normalize(Normal), ratio) + refract(I3, normalize(Normal), ratio);
	gl_FragColor = vec4(texture(skybox, R).rgb, 1.0) * vec4(0,1,0,1);
}
