#version 330 core

in vec3 vertexPos;
in vec3 vertexNorm;
in vec3 vertexColor;

out vec4 FragColor;

uniform vec3 uCameraPos;
uniform vec3 uLightCol;
uniform vec3 uLightInt;
uniform vec3 uLightDir;

vec3 CalculateLightPower(vec3 lightDir, vec3 viewDir, vec3 ambient, vec3 diffuse, vec3 specular)
{
    vec3 normLightDir = normalize(lightDir);
    float diff = max(dot(vertexNorm, normLightDir), 0.0);

    // specular shading
    vec3 halfwayDir = normalize(normLightDir + viewDir);
    float spec = pow(max(dot(halfwayDir, vertexNorm), 0.0), 32.0);

    return ambient + (diff * diffuse) + (spec * specular);
}

void main()
{
    vec3 viewDirection = normalize((uCameraPos - vertexPos));
    FragColor = vec4(CalculateLightPower(uLightDir, viewDirection, uLightCol * uLightInt.x, uLightCol * uLightInt.y, uLightCol * uLightInt.z), 1.0);
}
