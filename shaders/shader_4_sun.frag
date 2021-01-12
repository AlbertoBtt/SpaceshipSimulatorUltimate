#version 430 core

uniform vec3 objectColor;
uniform vec3 lightDir;

in vec3 interpNormal;


void main()
{
gl_FragColor = vec4(objectColor, 1.0);
//vec3 norm = normalize(interpNormal);
//float diff = dot(norm, -lightDir);
	//diff = max(diff, 0.0);

	//gl_FragColor = vec4(objectColor, 1.0) * diff;
}
