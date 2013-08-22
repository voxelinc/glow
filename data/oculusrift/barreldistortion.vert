#version 330

in vec2 in_texCoord;

out vec2 texCoord;

void main()
{
	gl_Position = vec4(textureCoordinate, 0.0, 1.0);
	texCoord = in_texCoord;
}
