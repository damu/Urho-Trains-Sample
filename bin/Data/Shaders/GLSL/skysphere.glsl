#include "Uniforms.glsl"
#include "Samplers.glsl"
#include "Transform.glsl"
#include "ScreenPos.glsl"
#include "Lighting.glsl"
#include "Fog.glsl"

varying vec2 vTexCoord;
varying vec4 vWorldPos;
varying vec3 vNormal;
uniform vec4 cSkyColor;
uniform vec3 cSunDir;
uniform vec4 cSunColor;
uniform sampler2D sDiffMap0;

void VS()
{
    mat4 modelMatrix = iModelMatrix;
    vec3 worldPos = GetWorldPos(modelMatrix);
    gl_Position = GetClipPos(worldPos);
    vTexCoord = GetTexCoord(iTexCoord);

    vWorldPos = vec4(worldPos, GetDepth(gl_Position));
    vNormal = GetWorldNormal(modelMatrix);
}

void PS()
{
    vec4 diffColor=cMatDiffColor*texture2D(sDiffMap,vTexCoord);
    gl_FragColor=diffColor;
    vec3 normal=normalize(vNormal);

    float d=1.0-vTexCoord.y;
    d=clamp(d,0.0,0.99);
//    d*=d;
//    d=length(vVertexLight);
    // d*=d;
    //gl_FragColor=vec4(d,d,d,1.0);
    //gl_FragColor=vec4(normal,1.0);
    float sun_intensity=clamp(dot(vNormal,cSunDir),0.0,1.0);
    gl_FragColor=cSunColor;//*sun_intensity;
    //gl_FragColor.a=(1.0-(0.5+cSunDir.y/2));
    //gl_FragColor.a=1.0-sun_intensity;
    //gl_FragColor.a=clamp(gl_FragColor.a*2,0.0,1.0);
    gl_FragColor.a=1.0-gl_FragColor.a;
    gl_FragColor.a*=0.5;
    //gl_FragColor=mix(vec4(GetFog(vec3(cSkyColor),1.0-d),d),gl_FragColor,gl_FragColor.a*2);
    vec4 sky=vec4(GetFog(vec3(cSkyColor),1.0-d),d);
    d=vTexCoord.y*2;
    d=1.0-clamp(d,0.0,1.0);
    gl_FragColor=vec4(sky.r+gl_FragColor.r*gl_FragColor.a,
                      sky.g+gl_FragColor.g*gl_FragColor.a,
                      sky.b+gl_FragColor.b*gl_FragColor.a,
                      clamp(1.0-(0.5+cSunDir.y),0.0,1.0)
                      //clamp(1.0-(0.5+cSunDir.y),0.0,1.0)+d);
                      //d);
                      );
}
