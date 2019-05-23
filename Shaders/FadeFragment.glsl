#version 400 core

uniform sampler2D 	mainTex;

uniform float time = 0.0f;

in Vertex
{
	vec4 colour;
	vec2 texCoord;
} IN;

out vec4 fragColor;

void main(void)
{	
	float fade = 0.1f;
	
	if(textureSize(mainTex, 1).x < 1.0f) {
		fragColor = IN.colour;
		fragColor.w-=fade*time;
	}
	else {
		fragColor = texture(mainTex, IN.texCoord) ;
		fragColor.w-=fade*time;
	}
}