#version 330

uniform mat4 view;
uniform mat4 projection;

in vec3 a_vertex;
out vec3 v_vertex;

void main()
{
	v_vertex = a_vertex;
	gl_Position = projection * view * vec4(a_vertex, 1.0);
}
