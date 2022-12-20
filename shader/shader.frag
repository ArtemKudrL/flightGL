#version 330 core

in vec2 UV;
in vec3 POS;
in vec3 EYE;
in vec3 LIGHT;
in vec3 NORM;

out vec3 color;

uniform sampler2D sampler;
uniform vec3 lightColor;

void main()
{
    vec3 n = normalize(NORM);
    vec3 l = normalize(LIGHT);
    vec3 e = normalize(EYE);
    vec3 r = reflect(-l, n);
    float d = 1.0/length(LIGHT - POS);
    float cosTheta = clamp(dot(n, l), 0, 1);
    float cosAlpha = clamp(dot(e, r), 0, 1);
    vec3 diffuseColor = texture(sampler, UV).rgb;
    vec3 ambColor = diffuseColor * vec3(0.1, 0.1, 0.1);
    vec3 specColor = vec3(0.3, 0.3, 0.3);
    color = ambColor +
            diffuseColor * lightColor * cosTheta * d * d +
            specColor * lightColor * pow(cosAlpha, 5) * d * d;
}
