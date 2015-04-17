// Fragment shader for multiple lights.

#version 430 core


struct LightProperties {
	bool isEnabled;
	bool isLocal;
	bool isSpot;
	vec3 ambient;
	vec3 color;
	vec3 position;
	vec3 halfVector;
	vec3 coneDirection;
	float spotCosCutoff;
	float spotExponent;
	float constantAttenuation;
	float linearAttenuation;
	float quadraticAttenuation;
};

// the set of lights to apply, per invocation of this shader
const int MAXLIGHTS = 4;
uniform LightProperties Lights[4];
uniform int numberOfLightsUsed;

uniform vec3 ambient_uniform;	
uniform vec3 diffuse_uniform;
uniform vec3 specular_uniform;
uniform float shininess_uniform;
vec3 ambient;
vec3 diffuse;
vec3 specular;
float shininess;

in vec2 tIndex;	// texture map indices
in vec3 Normal;		// normal in eye coordinates
in vec4 Position;	// vertex position in eye coordinates
uniform sampler2D tex;

out vec4 FragColor;

void main()
{
	ambient = ambient_uniform;
	diffuse = diffuse_uniform;
	specular = specular_uniform; 
	shininess = shininess_uniform;
    vec3 scatteredLight = vec3(0.0); // or, to a global ambient light
    vec3 reflectedLight = vec3(0.0);
	vec3 eyeDirection;
	vec3 lightDirection;
	vec3 halfVector;
	vec3 myNormal;
	float attenuation = 1.0f;
	float diffuseCoeff;
	float specularCoeff;
	vec3 rgb;// =vec3(1.0,0.0,0.0);

	vec4 outColor = texture(tex, tIndex);
	if (outColor.a == 0)
		discard;
	else
		FragColor = outColor;
    // loop over all the lights
	
}