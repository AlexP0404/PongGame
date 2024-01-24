#version 450

struct VertexOutput
{
  vec2 LocalPosition;
  vec3 Color;
  float Radius;
};

layout(location = 0) in VertexOutput Input;

layout(location = 0) out vec4 outColor;

void main(){
  //outColor = vec4(fragColor,1.0);
  //vec3 col = vec3(smoothstep(0.0,0.05,fragRadius));
  //vec3 col = vec3(fragRadius);
  //outColor.rgb = col;
  float distance = 1.0 - length(Input.LocalPosition);
  float circle = smoothstep(0,Input.Radius,distance);
  if(circle == 0.0) discard;

  outColor = vec4(Input.Color,circle);
}
