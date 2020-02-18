#version 410 core

in vec3 normal;
in vec4 fragPosEye;
in vec2 passTexture;
in vec4 fragPosLightSpace;
in vec4 fragPosit;
//////////////////////



out vec4 fColor;

//lighting
uniform	mat3 normalMatrix;

uniform	vec3 lightDir;
uniform	vec3 lightColor;
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform sampler2D alphaTexture;
uniform sampler2D shadowMap;


vec3 ambient;
float ambientStrength = 0.1f;
vec3 diffuse;
vec3 specular;
float specularStrength = 0.5f;
float shininess = 32.0f;

float constant = 1.0f;
float linear = 0.30;
float quadratic = 0.30f;

vec3 o;

struct PointLight 
{
	vec3 position;
	vec3 color;
	
	float constant;
	float linear;
	float quadratic;
	
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

#define NR_POINT_LIGHTS 1

uniform PointLight pointLights[NR_POINT_LIGHTS];

vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	
	vec3 lightDir = normalize(light.position - fragPos);
	//diffuse shading
	float diff = max(dot (normal, lightDir), 0.0);
	//specular shading
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
	//attenuation
	float dist = length(light.position - fragPos);
	float attenuation = 1.0 / (constant + linear * dist + 	quadratic * (dist * dist));
									
	//attenuation = 1.0f;
									
	// combine results
    vec3 ambientP  = ambient  * vec3(texture(diffuseTexture, passTexture).rgb) * light.color;
    vec3 diffuseP  = diffuse  * diff * vec3(texture(diffuseTexture, passTexture).rgb) * light.color;
    vec3 specularP = specular * spec * vec3(texture(specularTexture, passTexture).rgb) * light.color;
    ambientP  *= attenuation;
    diffuseP  *= attenuation;
    specularP *= attenuation;
    return (ambientP + diffuseP + specularP);
	
}

//........XXX..............//
vec3 viewDir;
//..........................//

void computeLightComponents()
{		

	vec3 lightPosEye = vec3(0, 0, 0);

	vec3 cameraPosEye = vec3(0.0f);//in eye coordinates, the viewer is situated at the origin
	
	
	//compute distance to light
	float dist = length(lightPosEye - fragPosEye.xyz);
	//compute attenuation
	float att = 1.0f;// / (constant + linear * dist + quadratic * (dist * dist));
	
	//att = 1.0f;
	
	//transform normal
	vec3 normalEye = normalize(normalMatrix * normal);	
	
	//compute light direction
	vec3 lightDirN = normalize(lightDir);
	
	//compute view direction 
	vec3 viewDirN = normalize(cameraPosEye - fragPosEye.xyz);
	
	//..................XXX........................//
	viewDir = viewDirN;
	//..............................................//
		
	//compute ambient light
	ambient = att * ambientStrength * lightColor;
	
	//compute diffuse light
	diffuse = att * max(dot(normalEye, lightDirN), 0.0f) * lightColor;
	
	//compute specular light
	vec3 reflection = reflect(-lightDirN, normalEye);
	float specCoeff = pow(max(dot(viewDirN, reflection), 0.0f), shininess);
	specular = att * specularStrength * specCoeff * lightColor;
}

float computeShadow()
{
	// perform perspective divide
	vec3 normalizedCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	
	// Transform to [0,1] range
	normalizedCoords = normalizedCoords * 0.5 + 0.5;
	
	if (normalizedCoords.z > 1.0f)
		return 0.0f;
	
	// Get closest depth value from light's perspective
	float closestDepth = texture(shadowMap, normalizedCoords.xy).r;
//o = vec3(closestDepth);	
	// Get depth of current fragment from light's perspective
	float currentDepth = normalizedCoords.z;
	
	// Check whether current frag pos is in shadow
	float bias = 0.005;
	float shadow = currentDepth -bias > closestDepth ? 1.0 : 0.0;
	
	//return 0.0f;
	return shadow;

}

float computeFog()
{
 float fogDensity = 0.05f;
 float fragmentDistance = length(fragPosEye);
 float fogFactor = exp(-pow(fragmentDistance * fogDensity, 2));

 return clamp(fogFactor, 0.0f, 1.0f);
}

void main() 
{
	computeLightComponents();
	
	vec3 baseColor = vec3(1.0f, 0.55f, 0.0f);//orange
	
	ambient *= texture(diffuseTexture, passTexture).rgb;
	diffuse *= texture(diffuseTexture, passTexture).rgb;
	specular *= texture(specularTexture,passTexture).rgb;
	
	
	
	//modulate with shadow
	float shadow = computeShadow();
	vec3 color = min((ambient + (1.0f - shadow)*diffuse) + (1.0f - shadow)*specular, 1.0f);
	
    
	
	////////////////////////////
	
	if(texture(alphaTexture, passTexture).r < 0.1)
		discard;
	
	////////////////////////////
	
	//..................XXX...................//
	for(int i = 0; i < NR_POINT_LIGHTS;i++)
		color += calcPointLight(pointLights[i], normal,fragPosit.xyz,viewDir);
		
	
	
	//fog
	
	float fogFactor = computeFog();
	vec4 fogColor = vec4(0.5f, 0.5f, 0.5f, 1.0f);
	
	
	
	vec4 newColor = vec4(color,1.0f);
	
	fColor = mix(fogColor, newColor, fogFactor);
		
	
	
	//final color
	
	fColor = fogColor * (1 - fogFactor) + newColor * fogFactor;
	
	
	//fColor = vec4(calcPointLight(pointLights[0], normal,fragPosEye.xyz,viewDir),1.0f);
	
	
	//fColor = fogColor * (1 – fogFactor) + color * fogFactor;
   // fColor = vec4(color, 1.0f);
	//fColor = vec4(o, 1.0f);
}
