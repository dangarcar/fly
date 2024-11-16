#version 330 core

layout (location = 0) in float t;

out vec4 vertColor;

uniform mat4 projection;
uniform vec4 coords;
uniform float d;
uniform int n;
uniform float width;
uniform vec4 color;

const float M_PI = 3.14159265358979323846264338327950288;
const float M_PI_4 = 0.785398163397448309615660845819875721;
const float M_1_2PI = 1.0 / (2.0 * M_PI);

void main() 
{
    vec2 c1 = coords.rg;
    vec2 c2 = coords.ba;

    float A = sin((1-t) * d) / sin(d);
    float B = sin(t * d) / sin(d);
    float x = A * cos(c1.y) * cos(c1.x) + B * cos(c2.y) * cos(c2.x);
    float y = A * cos(c1.y) * sin(c1.x) + B * cos(c2.y) * sin(c2.x);
    float z = A * sin(c1.y) + B * sin(c2.y);
    
    vec2 c = vec2(atan(y, x), atan(z, sqrt(x*x + y*y)));
    vec2 proj = width * vec2( M_1_2PI * (M_PI + c.x),  M_1_2PI * (M_PI - log(tan(M_PI_4 + c.y/2))) );

    gl_Position = projection * vec4(proj.xy, 1.0, 1.0);
    
    int curr = int(n * t);
    float alpha = 1.0 * float(curr%2);
    vertColor = vec4(color.xyz, alpha);
}