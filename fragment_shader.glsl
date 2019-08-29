#version 330 core
out vec4 FragColor;
  
in vec2 TexCoord;

uniform sampler2D Texture1;
uniform sampler2D Texture2;
uniform sampler2D Texture3;
uniform int textureChoice;

//uniform float texMix; wspolczynnik mieszania textur do FragColor = mix(texture(Texture1, TexCoord), texture(Texture2, TexCoord), texMix);

void main()
{
	if(textureChoice == 1){
	FragColor = texture(Texture1, TexCoord);
	}
	else if (textureChoice == 2){
	FragColor = texture(Texture2, TexCoord);
	}
	else if	(textureChoice == 3){
	FragColor = texture(Texture3, TexCoord);
	}
	
}