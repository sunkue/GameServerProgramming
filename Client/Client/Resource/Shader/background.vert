#version 450

layout(location = 0) in vec2 a_position;
layout(location = 1) in vec2 a_texcoord;

out vec2 texcoord;

void main()
{	
	gl_Position = vec4(a_position, 1, 1);
	texcoord = a_texcoord;


}
