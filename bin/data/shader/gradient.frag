#version 120

uniform vec3 colorA;
uniform vec3 colorB;

uniform vec3 posA;
uniform vec3 posB;

uniform float minX;
uniform float maxX;
uniform float minY;
uniform float maxY;
uniform float windowHeight;
uniform float windowWidth;

uniform sampler2DRect img;

varying vec2 texCoordVarying;

uniform float rectArea;
uniform float time;
uniform float iVal;
uniform float jVal;

uniform float isLine;


//vec2 step1 = vec2(200, 200);
//vec2 step2 = vec2(400, 400);
//
//vec4 color1 = vec4(0.0,0.5,1.0,1.0);
//vec4 color2 = vec4(1.0,5.0,0.0,1.0);
//
//void mainImage( out vec4 fragColor, in vec2 fragCoord )
//{
//    //----constant, can pre compute------
//    vec2 gDrirection = step2 - step1;
//    float lenSq = dot(gDrirection, gDrirection);//len(gDrirection)^2
//    //-----------------------------------
//    vec2 relCoords = fragCoord - step1;
//    float t = dot (relCoords, gDrirection) /  lenSq;
//    fragColor = mix(color1, color2, t); //mix = lerp
//}


float minimum_distance(vec3 v, vec3 w, vec3 p) {
    // Return minimum distance between line segment vw and point p
    float l2 =  distance(v,w);
    l2 = l2*l2; //(v-w).lengthSquared();  // i.e. |w-v|^2 -  avoid a sqrt
    //if (l2 == 0.0) return distance(p,v);   // v == w case
    float t = dot((p-v),(w - v)) / l2;
    if (t < 0.0) return 0.0;
    else if (t > 1.0) return 1.0;  // Beyond the 'w' end of the segment
    return t;
}




void main() {
    vec2 pos = gl_FragCoord.xy;
    //pos.y = windowHeight - pos.y;
    float t =  minimum_distance(posA, posB, vec3(pos, 1.0));
    
    pos.x += sin(time + iVal*0.1)*400.0 * cos( time + iVal);
    pos.y += cos(time + iVal*0.1)*400.0 * sin( time + jVal);
    
    vec2 texTemp = texCoordVarying;
//    texTemp.x *= (1.0 + 0.01 * cos(jVal + time));
//    texTemp.y *= (1.0 + 0.01 * sin(jVal + time));
//
    
    float dist = distance(posB.xy, pos);
    
    float dd = sqrt(rectArea*0.003 / (pow(dist, 1.4))) -0.4;
    
     if (isLine > 0.5){
          dd = rectArea / (pow(dist, 1.2));
     } else {
    float dd2 = 100.0  / (pow(dist, 1.0));
         if (dd2 > 1.0) dd2 = 1.0;
    dd += dd2*0.5;
     }
//    dd *= 1.0; //sin(time + jVal*0.1 + iVal*0.1);
//   
    //if (dd > 1.1) dd = 1.1;
    vec4 vvv = texture2DRect(img, texTemp);
    if (isLine > 0.5){
        vvv = vec4(1.0, 1.0, 1.0, 1.0);
        gl_FragColor =  mix(vvv, vec4(1.0, 1.0, 1.0, 0.0), 1.0-dd);
    } else {
    
        gl_FragColor =  vec4(vvv.x*dd, vvv.y*dd, vvv.z*dd,0.5 + 3.0*dd);
        //vvv = vec4(1.0, 1.0, 1.0, 0.0);
        gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
    }
    //gl_FragColor = texture2DRect(img, texCoordVarying);
       //mix(vec4(vec3(vvv.xyz), 1.0), vec4(vec3(0.0), 1.0), t);
}