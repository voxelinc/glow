#version 330

layout (location = 0) out vec4 fragColor;

in vec2 texCoord;

uniform sampler2D image;
uniform vec2 lensCenter;
uniform vec2 screenCenter;
uniform vec2 scale;
uniform vec2 scaleIn;
uniform vec4 hmdWarpParams;

vec2 hmdWarp(in vec2 vec)
{
	vec2 theta = (vec - lensCenter) * scaleIn;
	float rSq = theta.x * theta.x + theta.y * theta.y;
	
	return lensCenter + scale * theta * (
		hmdWarpParams.x +
		hmdWarpParams.y * rSq +
		hmdWarpParams.z * rSq * rSq +
		hmdWarpParams.w * rSq * rSq * rSq
	);
}

void main()
{
	vec2 warpedTextureCoord = hmdWarp(texCoord);
	
	if (any(clamp(warpedTextureCoord, screenCenter-vec2(0.25,0.5), screenCenter+vec2(0.25, 0.5)) - warpedTextureCoord))
	{
		discard;
	}
	
	fragColor = texture2D(image, warpedTextureCoord);
}
