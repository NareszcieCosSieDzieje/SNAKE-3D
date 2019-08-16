#version 330 core
out vec4 FragColor;
  
in vec3 ourColor;
in vec4 TexCoord;

uniform sampler2D Texture3;
uniform sampler2D Texture4;
uniform float texMix1;

vec2 WoodCoords;
vec2 FaceCoords;

void main()
{
	WoodCoords = vec2(TexCoord.x, TexCoord.y);
	FaceCoords = vec2(TexCoord.z, TexCoord.w);
    FragColor = mix(texture(Texture3, WoodCoords), texture(Texture4, FaceCoords), texMix1);
}