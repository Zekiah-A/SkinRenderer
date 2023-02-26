#version 330

in vec2 frag_uv;
out vec4 frag_color;

uniform sampler2D textureSampler;

void main()
{
    frag_color = texture(textureSampler, frag_uv);
}