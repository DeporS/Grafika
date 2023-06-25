#version 330

//Zmienne jednorodne
uniform mat4 P;
uniform mat4 V;
uniform mat4 M;

//Atrybuty
in vec4 vertex; //wspolrzedne wierzcholka w przestrzeni modelu
in vec4 color; //kolor związany z wierzchołkiem
in vec4 normal; //wektor normalny w przestrzeni modelu
in vec2 texCoord0;
in vec4 c1;
in vec4 c2;
in vec4 c3;

//Zmienne interpolowane
out vec4 ic;
out vec4 l;
out vec4 l2;
out vec4 l3;
out vec4 l4;
out vec4 l5;
out vec4 n;
out vec4 v;
out vec2 iTexCoord0;



void main(void) {
    mat4 invTBN = mat4(c1,c2,c3,vec4(0,0,0,1));
    vec4 lp = vec4(-50, 15, 50, 1); //przestrzeń świata
    vec4 lp2 = vec4(50, 15, -50, 1); //przestrzeń świata
    vec4 lp3 = vec4(-50, 15, -50, 1); //przestrzeń świata
    vec4 lp4 = vec4(50, 15, 50, 1); //przestrzeń świata
    vec4 lp5 = vec4(0, 200, 0, 1); //przestrzeń świata
    //l = normalize(V * lp - V*M*vertex); //wektor do światła w przestrzeni oka
    //v = normalize(vec4(0, 0, 0, 1) - V * M * vertex); //wektor do obserwatora w przestrzeni oka
    l = normalize(invTBN*(inverse(M)*lp-vertex));
    l2 = normalize(invTBN*(inverse(M)*lp2-vertex));
    l3 = normalize(invTBN*(inverse(M)*lp3-vertex));
    l4 = normalize(invTBN*(inverse(M)*lp4-vertex));
    l5 = normalize(invTBN*(inverse(M)*lp5-vertex));
    v = normalize(invTBN*(inverse(V*M)*vec4(0,0,0,1)-vertex));
    n = vec4(0,0,1,0);
    //n = normalize(V * M * normal); //wektor normalny w przestrzeni oka
    
    
    iTexCoord0 = texCoord0;
    
    ic = color;
    
    gl_Position=P*V*M*vertex;
}
