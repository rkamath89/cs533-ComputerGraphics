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
uniform LightProperties Lights[MAXLIGHTS];
uniform int numberOfLightsUsed;

uniform vec3 ambient_uniform;	
uniform vec3 diffuse_uniform;
uniform vec3 specular_uniform;
uniform float shininess_uniform;
vec3 ambient;
vec3 diffuse;
vec3 specular;
float shininess;
vec4 outColor ;
in vec2 tIndex;	// texture map indices
in vec3 Normal;		// normal in eye coordinates
in vec4 Position;	// vertex position in eye coordinates
uniform sampler2D tex;
out vec4 FragColor;
uniform int enableColor;

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

    // loop over all the lights
	int light = 0;
    for (light = 0; light <= numberOfLightsUsed;light++) 
	{
		
        if (!Lights[light].isEnabled)
		{
					continue;
		}
           /* if(Lights[light].color.x == 1 && Lights[light].color.y == 1 && Lights[light].color.z == 1 &&
			Lights[light].position.x == 5 && Lights[light].position.y == 5 && Lights[light].position.z == 0 &&   
			 Lights[light].constantAttenuation == 0.001 && Lights[light].linearAttenuation == 0.001 && Lights[light].quadraticAttenuation== 0.002 
			&& Lights[light].coneDirection.x == -5 && Lights[light].coneDirection.y == -5 && Lights[light].coneDirection.z == 0 
			&& Lights[light].spotCosCutoff == .867 && Lights[light].spotExponent == 25)
		{
			rgb =vec3(0.0,1.0,0.0);
		}*/
        attenuation = 1.0;

        eyeDirection = normalize(-vec3(Position));	// since we are in eye coordinates
													// eye position is 0,0,0
        // for local lights, compute per-fragment direction,
        // halfVector, and attenuation
        if (Lights[light].isLocal) 
		{

		    lightDirection = Lights[light].position - vec3(Position);
            float lightDistance = length(lightDirection);
            lightDirection = normalize(lightDirection);

			attenuation = 1.0 /
				(Lights[light].constantAttenuation
				+ Lights[light].linearAttenuation    * lightDistance
				+ Lights[light].quadraticAttenuation * lightDistance
						                                  * lightDistance);
				if (Lights[light].isSpot) 
				{
					
			//if(Lights[light].position.y == 5)
			 //rgb =vec3(0.0,1.0,0.0);
					vec3 myConeDirection = normalize(Lights[light].coneDirection);
					float spotCos = dot(lightDirection,
										-myConeDirection);
					if (spotCos < Lights[light].spotCosCutoff)
					{
						//rgb =vec3(0.0,0.0,1.0);
						attenuation = 0.0;
					}
						
					else 
						attenuation *= pow(spotCos,
										   Lights[light].spotExponent);
				}
            halfVector = normalize(lightDirection + eyeDirection);
        } 
		else
		// directional light
		{
			
			lightDirection = normalize(Lights[light].position);
			halfVector = normalize(lightDirection + eyeDirection);
        }

		myNormal = normalize(Normal);
        diffuseCoeff  = max(0.0, dot(myNormal, lightDirection));
        specularCoeff = max(0.0, dot(myNormal, halfVector));

        if (diffuseCoeff == 0.0)
            specularCoeff = 0.0;
        else 
            specularCoeff = pow(specularCoeff, shininess); // * Strength;

        // Accumulate all the lights’ effects as it interacts with material properties
		
        scatteredLight += Lights[light].ambient * ambient * attenuation +
                          Lights[light].color * (diffuseCoeff * diffuse) * attenuation;
        reflectedLight += Lights[light].color * (specularCoeff * specular) * attenuation;
    }

	rgb = min(scatteredLight + reflectedLight, vec3(1.0));
	if(enableColor != 1)
	{
		outColor = texture(tex, tIndex);
		if (outColor.a == 0)
			discard;
		else
			FragColor = outColor * vec4(rgb, 0.0f);
	}
	else
	{
		FragColor = vec4(rgb, 0.0f);
	}

    
}