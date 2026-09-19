#version 450 core
layout(location = 0) in vec3 pos;
layout(location = 2) in vec3 normal;
uniform mat4 model, view, projection;
out vec3 viewNormal;

void main()
{
    gl_Position = projection * view * model * vec4(pos, 1.0);
    viewNormal = mat3(transpose(inverse(view * model))) * normal;
}
