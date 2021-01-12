#version 430 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexTexCoord;
layout(location = 2) in vec3 vertexNormal;

uniform mat4 transformation;
uniform mat4 modelmatrix;

out vec3 interpNormal;
out vec3 fragPos;
out vec4 model;
out vec4 vPositions;

void main()
{
    gl_Position = transformation * vec4(vertexPosition, 1.0);
    interpNormal = vertexNormal;
    fragPos = vertexPosition;
    model =  vec4(vertexNormal, 0.0) * modelmatrix;
    vPositions = vec4(vertexPosition, 1.0) * modelmatrix;
}
