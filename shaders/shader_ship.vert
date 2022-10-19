#version 410 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexTexCoord;
layout(location = 2) in vec3 vertexNormal;
layout(location = 3) in vec3 vertexTangent;
layout(location = 4) in vec3 vertexBitangent;

uniform mat4 transformation;
uniform mat4 modelMatrix;
uniform vec3 cameraPos;
uniform vec3 lightDir;
uniform vec3 lightPos;

out vec3 interpNormal;
out vec3 fragPos;
out vec4 vPositions;
out vec2 Uv;
out vec3 lightDirTS;
out vec3 viewDirTS;
out mat3 TBN;
void main()
{

    gl_Position = transformation * vec4(vertexPosition, 1.0);
    interpNormal = mat3(transpose(inverse(modelMatrix))) * vertexNormal;
    fragPos = vec3(modelMatrix * vec4(vertexPosition, 1.0f));
    vPositions = vec4(vertexPosition, 1.0) * modelMatrix;
    Uv=vertexTexCoord;

    vec3 normal =  (modelMatrix * vec4(vertexNormal, 0.0)).xyz;
	vec3 tangent =  (modelMatrix * vec4(vertexTangent, 0.0)).xyz;
	vec3 bitangent =  (modelMatrix * vec4(vertexBitangent, 0.0)).xyz;

	mat3 TBN = transpose(mat3(tangent,bitangent,normal));
    vec3 viewDir = normalize(cameraPos - fragPos);
	vec3 lightPos2 = normalize(lightPos - fragPos);
	lightPos2=-lightPos2;

   	 lightDirTS = TBN * lightPos2;
	lightDirTS = normalize(lightDirTS);
	viewDirTS = TBN * viewDir;
	viewDirTS = normalize(viewDirTS);

	
}
