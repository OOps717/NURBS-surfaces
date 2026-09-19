#version 450 core
in vec3 viewNormal;
out vec4 FragColor;
uniform vec3 markerColour;

void main()
{
    vec3 normal = normalize(viewNormal);
    vec3 lightDirection = normalize(vec3(-0.4, 0.7, 1.0));
    float diffuse = max(dot(normal, lightDirection), 0.0);
    vec3 colour = markerColour * (0.25 + 0.75 * diffuse);
    FragColor = vec4(colour, 1.0);
}
