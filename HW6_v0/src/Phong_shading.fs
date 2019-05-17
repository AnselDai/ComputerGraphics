#version 330 core
in vec3 Normal;
in vec3 FragPos;
out vec4 FragColor;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 objectColor;
uniform vec3 lightColor;

uniform float ambientStrength;
uniform float diffuseStrength;
uniform float specularStregth;
uniform int reflectExtent;

void main()
{
	// 环境光照
	vec3 ambient = ambientStrength * lightColor;
	// 漫反射
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);
	float diff = max(dot(norm, lightDir), 0.0f);
	vec3 diffuse = diffuseStrength * diff * lightColor;
	// 镜面反射
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), reflectExtent);
	vec3 specular = specularStregth * spec * lightColor;
	// 融合
	vec3 result = (ambient + diffuse + specular) * objectColor;
	FragColor = vec4(result, 1.0f);
}
