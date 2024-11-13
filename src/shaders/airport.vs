#version 330 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTex;

out vec2 TexCoord;

uniform mat4 projection;

void main() 
{
    gl_Position = projection * vec4(aPos.xy, 1.0, 1.0);
    TexCoord = aTex;
}