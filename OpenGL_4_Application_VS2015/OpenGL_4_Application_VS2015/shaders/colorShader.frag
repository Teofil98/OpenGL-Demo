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
uniform vec3 color;


vec3 ambient;
float ambientStrength = 0.1f;
vec3 diffuse;
vec3 specular;
float specularStrength = 0.5f;
float shininess = 32.0f;

float constant = 1.0f;
float linear = 0.35f;
float quadratic = 0.44f;



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
 float fogDensity = 0.02f;
 float fragmentDistance = length(fragPosEye);
 float fogFactor = exp(-pow(fragmentDistance * fogDensity, 2));

 return clamp(fogFactor, 0.0f, 1.0f);
}

void main() 
{
	computeLightComponents();
	
	
	ambient *= texture(diffuseTexture, passTexture).rgb;
	diffuse *= texture(diffuseTexture, passTexture).rgb;
	specular *= texture(specularTexture,passTexture).rgb;
	
	//ambient = 0;
	//diffuse = 0.2;
	//specular = 0.7;
	
	//modulate with shadow
	float shadow = computeShadow();
	//vec3 color = min((ambient + (1.0f - shadow)*diffuse) + (1.0f - shadow)*specular, 1.0f);
	
  //  vec3 color = vec3(0.0f,0.0f,255.0f);
	
	////////////////////////////
	
	if(texture(alphaTexture, passTexture).r < 0.1)
		discard;
	
	
	
	
	//fog
	
	float fogFactor = computeFog();
	vec4 fogColor = vec4(0.5f, 0.5f, 0.5f, 1.0f);
	
	
	
	vec4 newColor = vec4(color,1.0f);
	
	fColor = mix(fogColor, newColor, fogFactor);
		
	//fColor = vec4(0.0f,0.0f,255.0f,1.0f);
	
	//final color
	
	///////fColor = fogColor * (1 - fogFactor) + newColor * fogFactor;
	
	
	//fColor = vec4(calcPointLight(pointLights[0], normal,fragPosEye.xyz,viewDir),1.0f);
	
	
	//fColor = fogColor * (1 – fogFactor) + color * fogFactor;
   // fColor = vec4(color, 1.0f);
	//fColor = vec4(o, 1.0f);
}



/*#version 410 core

out vec4 fragColor;

uniform vec3 color;

void main()
{
   fragColor = vec4(color,1.0f);
}*/