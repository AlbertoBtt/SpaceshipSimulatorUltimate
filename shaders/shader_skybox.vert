#version 410

layout (location = 0) in vec3 VertexPosition;

uniform mat4 Transformation;

out vec3 TexCoords;

void main()
{
	gl_Position = Transformation * vec4(VertexPosition, 1.0);

    TexCoords = VertexPosition;
}
