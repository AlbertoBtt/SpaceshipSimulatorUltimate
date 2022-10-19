#version 410 core

in vec2 Uv;

uniform sampler2D colorTexture;
uniform bool HDR;
uniform float exposure;

void main()
{             
    const float gamma = 2.2;
    vec3 hdrColor = texture(colorTexture, Uv).rgb;
    if(HDR)
    {
        vec3 result = vec3(1.0) - exp(-hdrColor * exposure);     
        result = pow(result, vec3(1.0 / gamma));
        gl_FragColor = vec4(result, 1.0);
    }
    else
    {
        vec3 result = pow(hdrColor, vec3(1.0 / gamma));
        gl_FragColor = vec4(result, 1.0);
    }
}