#version 450 core
layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 texture;
layout(location = 2) in vec3 norm;

out vec3 normal;
out vec3 FragPos;
out vec4 directionalLightSpacePos;

uniform mat4 model, view, projection;
uniform mat4 directionalLightTransform;

void main()
{
    gl_Position = projection * view * model * vec4(pos, 1.0);

    normal = mat3(transpose(inverse(model))) * norm;
    FragPos = (model * vec4(pos, 1.0)).xyz;

    directionalLightSpacePos = directionalLightTransform * model * vec4(pos, 1.0);
}

