#version 400 core

uniform sampler2D 	mainTex;
uniform sampler2D	secondTex;

uniform float time;

in Vertex
{
	vec4 colour;
	vec2 texCoord;
} IN;

out vec4 fragColor;

void main(void)
{
	float blendFactor=0.1; 
	vec4 tex1 = texture(mainTex, IN.texCoord);
	vec4 tex2 = texture(secondTex, IN.texCoord);
	if(time*blendFactor<1) fragColor=mix(tex1,tex2, time*blendFactor);
	else fragColor=mix(tex1,tex2,1);
}