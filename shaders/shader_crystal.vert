#version 410 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexTexCoord;
layout(location = 2) in vec3 vertexNormal;

uniform mat4 transformation;
uniform mat4 modelMatrix;

out vec3 Normal;
out vec3 Position;

void main()
{
	gl_Position = transformation * vec4(vertexPosition, 1.0);
	mat4 model=modelMatrix;
	Position = vec3(model * vec4(vertexPosition, 1.0));
	Normal = mat3(transpose(inverse(model))) * vertexNormal;
	
}
