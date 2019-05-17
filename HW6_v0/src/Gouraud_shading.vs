#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;

out vec3 LightingColor;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float ambientStrength;
uniform float diffuseStrength;
uniform float specularStrength;
uniform int reflectExtent;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0f);
	
	vec3 FragPos = vec3(model * vec4(aPos, 1.0f));
	vec3 Normal = mat3(transpose(inverse(model))) * aNormal;

	vec3 ambient = ambientStrength * lightColor;

	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);
	float diff = max(dot(norm, lightDir), 0.0f);
	vec3 diffuse = diffuseStrength * diff * lightColor;
	
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0f), reflectExtent);
	vec3 specular = specularStrength * spec * lightColor;

	LightingColor = ambient + diffuse + specular;
}