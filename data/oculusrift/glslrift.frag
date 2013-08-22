#version 330

in vec3 v_vertex;
layout (location = 0) out vec4 fragColor;

void main()
{
	fragColor = vec4(v_vertex * 0.5 + 0.5, 1.f);
}
