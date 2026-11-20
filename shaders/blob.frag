#version 330 

#define MAX_POINTS 256
uniform vec2 points[MAX_POINTS];
uniform int count; 

uniform vec2 resolution;

uniform vec2 mouse;

uniform vec3 baseColor;

out vec4 finalColor;


float sdPolygon(vec2 p)
{
    float d = dot(p - points[0], p - points[0]);
    float s = 1.0;
    for (int i = 0, j = count - 1; i < count; j = i, i++)
    {
        vec2 e = points[j] - points[i];
        vec2 w = p - points[i];
        vec2 b = w - e * clamp(dot(w, e) / dot(e, e), 0.0, 1.0);
        d = min(d, dot(b, b));

        bvec3 c = bvec3(p.y >= points[i].y, p.y < points[j].y, e.x * w.y > e.y * w.x);
        if (all(c) || all(not(c))) s *= -1.0;
    }
    return s * sqrt(d);
}

vec2 sdGrad(vec2 p)
{
    float h = 8.0;
    float dx = sdPolygon(p + vec2(h, 0.0)) - sdPolygon(p - vec2(h, 0.0)); 
    float dy = sdPolygon(p + vec2(0.0, h)) - sdPolygon(p - vec2(0.0, h)); 
    return normalize(vec2(dx, dy));
}

void main () 
{
    vec3 bg = vec3(0.17, 0.18, 0.22);
    vec3 blob = baseColor;
    
    vec2 p = vec2(gl_FragCoord.x, resolution.y - gl_FragCoord.y);
    float d = sdPolygon(p) - 10.0;

    vec2 shadowOffset = vec2(12.0, 18.0);
    float ds = sdPolygon(p - shadowOffset);
    float shadow = 1.0 - smoothstep(-15.0, 25.0, ds);
    bg *= 1.0 - 0.5*shadow;

    if (d > 1.0)
    {
        finalColor = vec4(bg, 1.0);
        return;
    }

    float bevel = 75.0;
    float inside = clamp(-d / bevel, 0.0, 1.0);
    vec2 g = sdGrad(p);

    vec3 n;
    n.xy = g * (1.0 - inside);
    n.z = sqrt(1.0 - dot(n.xy, n.xy));

    float alpha = 1.0 - smoothstep(-1.0, 1.0, d);
    //finalColor = vec4(mix(bg, blob, alpha), 1.0);

    vec3 L = normalize(vec3(-0.4, -0.6, 0.7));
    float diffuse = max(dot(n, L), 0.0);

    vec3 H = normalize(L + vec3(0.0, 0.0, 1.0));
    float spec = pow(max(dot(n, H), 0.0), 60.0);

    float rim = pow(1.0 - n.z, 2.0);

    vec3 col = blob * (0.35 + 0.65 * diffuse);
    col += vec3(1.0) * spec * 0.8;
    col += blob * rim * 0.6;

    finalColor = vec4(mix(bg, col, alpha), 1.0);
}