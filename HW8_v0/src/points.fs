#version 330 core

uniform vec3 MyColor;
out vec4 FragColor;

void main()
{
	FragColor = vec4(MyColor, 1.0f);
}