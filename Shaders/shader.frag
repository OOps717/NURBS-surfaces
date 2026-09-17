#version 450 core

in vec3 normal;
in vec3 FragPos;
in vec4 directionalLightSpacePos;

out vec4 FragColor;

const int MAX_POINT_LIGHTS = 2;
const int MAX_SPOT_LIGHTS = 2;

struct Light
{
    vec3 colour;
    float ambientIntensity;
    float diffuseIntensity;
};

struct DirectionalLight
{
    Light base;
    vec3 direction;
};

struct Material
{
    float specularIntensity;
    float shininess;
};

uniform sampler2D directionalShadowMap;

uniform Material material;
uniform vec3 eyePosition;
uniform vec3 objectColour;

uniform DirectionalLight directionalLight;

float calcDirectionalShadowFactor(DirectionalLight light)
{
	vec3 projCoords = directionalLightSpacePos.xyz / directionalLightSpacePos.w;
	projCoords = (projCoords * 0.5) + 0.5;

	float current = projCoords.z;

	vec3 fragNormal = normalize(normal);
    vec3 lightDir = normalize(directionalLight.direction);
	//vec3 lightDir = normalize(directionalLight.direction - FragPos);

	float bias = max(0.05 * (1.0 - dot(fragNormal, lightDir)), 0.0005);


	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(directionalShadowMap, 0);
	for(int x = -1; x <= 1; ++x)
	{
		for(int y = -1; y <= 1; ++y)
		{
			float pcfDepth = texture(directionalShadowMap, projCoords.xy + vec2(x,y) * texelSize).r;
			shadow += current - bias > pcfDepth ? 1.0 : 0.0;
		}
	}

	shadow /= 9.0;

	if(projCoords.z > 1.0)
	{
		shadow = 0.0;
	}

	return shadow;
}

vec3 calcLightByDirection(Light light, vec3 direction, float shadowFactor)
{
	vec4 ambientColour = vec4(light.colour, 1.0f) * light.ambientIntensity;
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(-direction);

	float diffuseFactor = max(dot(norm, lightDir), 0.0f);
	vec4 diffuseColour = vec4(light.colour, 1.0f) * light.diffuseIntensity * diffuseFactor;

	vec4 specularColour = vec4(0, 0, 0, 0);
	if(diffuseFactor > 0.0f)
	{
		vec3 fragToEye = normalize(eyePosition - FragPos);
		vec3 reflectedVertex = normalize(reflect(-lightDir, norm));

		float specularFactor = max(dot(fragToEye, reflectedVertex), 0.0);
		if(specularFactor > 0.0f)
		{
			specularFactor = pow(specularFactor, material.shininess);
			specularColour = vec4(light.colour * material.specularIntensity * specularFactor, 1.0f);
		}
	}

	return objectColour * (ambientColour.rgb + (1.0f - shadowFactor) * diffuseColour.rgb)
         + (1.0f - shadowFactor) * specularColour.rgb;
}

vec3 calcDirectionalLight()
{
    float shadowFactor = calcDirectionalShadowFactor(directionalLight);
    return calcLightByDirection(directionalLight.base, directionalLight.direction, shadowFactor);
}

void main()
{
    vec3 finalColour = calcDirectionalLight();

    FragColor = vec4(finalColour, 1.0);
}
