#version 400 core


uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

uniform float time = 0.0f;

layout (triangles) in;
layout (triangle_strip, max_vertices = 200) out;

in Vertex
{
  vec4 colour;
  vec2 texCoord;
} IN[];

out Vertex
{
  vec4 colour;
  vec2 texCoord;
} OUT;

vec4 triPos(float i, float j)
{
  vec4 a = gl_in[0].gl_Position;
  vec4 b = gl_in[1].gl_Position;
  vec4 c = gl_in[2].gl_Position;

  return i * a + j * b + (1 - i - j) * c;
}

vec2 triTexCoord(float i, float j)
{
  vec2 a = IN[0].texCoord;
  vec2 b = IN[1].texCoord;
  vec2 c = IN[2].texCoord;

  return i * a + j * b + (1 - i - j) * c;
}

void cubeVertex(mat4 mvp, vec4 pos, vec2 tex, vec3 vPos)
{
  OUT.texCoord = tex + vPos.xy;
  gl_Position = mvp * vec4(pos.xyz + vPos, 1);
  EmitVertex();
}

void cubeAtPoint(vec4 p, float i, vec2 tex)
{
  vec3 upper = vec3(0, 0, i);
  vec3 lower = vec3(0, 0, -i);
  vec3 left = vec3(-i, 0, 0);
  vec3 right = vec3(i, 0, 0);
  vec3 front = vec3(0, i, 0);
  vec3 back = vec3(0, -i, 0);

  mat4 mvp = projMatrix * viewMatrix * modelMatrix;
  
  cubeVertex(mvp, p, tex, upper + left + back);
  cubeVertex(mvp, p, tex, lower + left + back);
  cubeVertex(mvp, p, tex, upper + left + front);
  cubeVertex(mvp, p, tex, lower + left + front);
  cubeVertex(mvp, p, tex, upper + right + front);
  cubeVertex(mvp, p, tex, lower + right + front);
  cubeVertex(mvp, p, tex, upper + right + back);
  cubeVertex(mvp, p, tex, lower + right + back);
  EndPrimitive();

  cubeVertex(mvp, p, tex, upper + left + front);
  cubeVertex(mvp, p, tex, upper + right + front);
  cubeVertex(mvp, p, tex, upper + left + back);
  cubeVertex(mvp, p, tex, upper + right + back);
  cubeVertex(mvp, p, tex, lower + left + back);
  cubeVertex(mvp, p, tex, lower + right + back);
  cubeVertex(mvp, p, tex, lower + left + front);
  cubeVertex(mvp, p, tex, lower + right + front);
  EndPrimitive();
}

void main()
{
  float d = 0.25;

  for (float i = 0; i < 1; i += d)
  {
    for (float j = 0; j < 1; j += d)
    {
      if (i + j > 1)
        continue;

      cubeAtPoint(triPos(i, j)*(0.4*time*projMatrix * viewMatrix * modelMatrix), d, triTexCoord(i, j));
    }
  }
}