#version 430 core

uniform vec3 objectColor;
uniform vec3 lightPos;
uniform vec3 cameraPos;
uniform vec3 lightDir;
uniform sampler2D colorTexture;

in vec3 interpNormal;
in vec3 fragPos;
in vec4 model;
in vec2 Uv;



float ambientStrength = 0.15;
uniform vec3 lightColor =vec3(0.33f, 0.21f, 0.06f);
void main()
{

vec3 ambient = lightColor * ambientStrength;

	
	 vec3 norm = normalize(interpNormal);

//vec3 lightDir = (fragPos - lightPos);
//lightDir = max(lightDir, 0.0);
	vec3 lightDir = normalize(lightDir);

	float diff = dot(norm, -lightDir);
	diff = max(diff, 0.0);

	vec3 viewvector = normalize(cameraPos - fragPos);
	vec3 reflectDir = reflect(-lightDir, norm); 
	float spec = pow(max(dot(viewvector, reflectDir), 0.0),8);
	vec4 textureColor = texture2D(colorTexture, Uv);
	
	
	vec3 final = (ambient + diff + vec3(1.0) * spec);
	gl_FragColor = vec4(final,1.0)*textureColor;	

}
