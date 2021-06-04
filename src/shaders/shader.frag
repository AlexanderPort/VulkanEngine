
/*
layout(location = 0) in vec3 fragColor;

layout(location = 0) out vec4 outColor;

void main() {
    outColor = vec4(fragColor, 1.0);
}
*/

#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 fragColor;

layout(location = 0) out vec4 outColor;

const float Pi = 2.14159;
//uniform vec2 mouse;

const int   complexity      = 20;
const float mouse_factor    = 60.0;
const float mouse_offset    = 0.0;
const float fluid_speed     = 10000.0; 
const float color_intensity = 0.7;

layout(push_constant) uniform Push {
    vec2 resolution;
    vec3 data;
} push;
  
/*
void main()
{
    vec2 resolution = push.resolution;
    float time = push.data.x;
    vec2 mouse = push.data.yz;
  vec2 p=(2.0*gl_FragCoord.xy-resolution)/max(resolution.x,resolution.y);
  for(int i=1;i<complexity;i++)
  {
    vec2 newp=p + time*0.001;
    newp.x+=0.6/float(i)*sin(float(i)*p.y+time/fluid_speed+0.3*float(i)) + 0.5 + mouse.y/mouse_factor+mouse_offset;
    newp.y+=0.6/float(i)*sin(float(i)*p.x+time/fluid_speed+0.3*float(i+10)) - 0.5 - mouse.x/mouse_factor+mouse_offset;
    p=newp;
  }
  vec3 col=vec3(color_intensity*sin(3.0*p.x)+color_intensity,
                color_intensity*sin(3.0*p.y)+color_intensity,
                color_intensity*sin(p.x+p.y)+color_intensity);
  outColor=vec4(col, 1.0);
}
*/


mat2 r2d(float a) {
    float c = cos(a), s = sin(a);
    return mat2(c, s, -s, c);
}

vec2 path(float t) {
    float a = sin(t * 0.2 + 1.5), b = sin(t * 0.2);
    return vec2(2.0 * a, a * b);
}

float g = 0.0;

float de(vec3 p) {
    p.xy -= path(p.z);

    float d = -length(p.xy) + 4.0;

    g += 0.01 / (0.01 + d * d);
    return d;
}

 vec2 resolution = push.resolution;
float time = push.data.x;
    vec2 mouse = push.data.yz;

void mainImage(out vec4 fragColor, in vec2 fragCoord){
    vec2 uv = fragCoord / resolution.xy - 0.5;
    uv.x *= resolution.x / resolution.y;

    float dt = time * 4.0 / 10.0;
    vec3 ro = vec3(0, 0, -5.0 + dt);
    vec3 ta = vec3(0, 0, dt);

    ro.xy += path(ro.z);
    ta.xy += path(ta.z);

    vec3 fwd = normalize(ta - ro);
    vec3 right = cross(fwd, vec3(0, 1, 0));
    vec3 up = cross(right, fwd);
    vec3 rd = normalize(fwd * 0.25 + uv.x * right + uv.y * up);

    rd.xy *= r2d(sin(-ro.x / 3.14) * 0.3);
    vec3 p = floor(ro) + 0.5;
    vec3 mask;
    vec3 drd = 1.0 / abs(rd);
    rd = sign(rd);
    vec3 side = drd * (rd * (p - ro) + 0.5);

    float t = 0.0, ri = 0.0;
    for (float i = 0.0; i < 1.0; i += .01) {
        ri = i;
        if (de(p) < 0.0) break;
        mask = step(side, side.yzx) * step(side, side.zxy);

        side += drd * mask;
        p += rd * mask;
    }
    t = length(p - ro);

    vec3 c = vec3(1.0) * length(mask * vec3(1.0, 0.5, 0.75));
    c = mix(vec3(0.2, 0.2, 0.7), vec3(0.2, 0.1, 0.2), c);
    c += g * 0.4;
    c.r += 0.40 * sin(p.z * 0.25);
    c = mix(c, vec3(0.2, 0.1, 0.2), 1.0 - exp(-0.001 * t * t));

    fragColor = vec4(c, 1.0);
}

void main(void){
    mainImage(outColor, gl_FragCoord.xy);
}