#version 330

uniform mat4 modelView;
uniform mat4 projection;

in vec3 a_vertex;

void main()
{
	gl_Position = projection * modelView * vec4(a_vertex, 1.0);
}
