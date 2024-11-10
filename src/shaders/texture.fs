#version 330 core

in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D sampler;
uniform vec4 modColor;
uniform bool useTexture;

void main()
{
    FragColor = modColor;
    
    if(useTexture)
        FragColor *= texture(sampler, TexCoord);
}
