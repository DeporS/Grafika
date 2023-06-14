#version 330

uniform sampler2D textureMap0;
uniform sampler2D textureMap1;
uniform sampler2D textureMap2;

out vec4 pixelColor; //Zmienna wyjsciowa fragment shadera. Zapisuje sie do niej ostateczny (prawie) kolor piksela

in vec4 ic; 
in vec4 n;
in vec4 l;
in vec4 l2;
in vec4 v;
in vec2 iTexCoord0;

vec2 parallax(vec4 v, vec2 t, float s, float h){
	vec2 ti =-v.xy/s;
	float hi =-v.z/s;

	vec2 tc = t;
	float hc = h;

	float ht = h*texture(textureMap2,tc).r;

	if(v.z<=0){
		discard;
	}else{
		while (hc>ht){
			tc= tc + ti;
			if(tc.x < 0 || tc.x > 1 || tc.y < 0 || tc.y > 1){
				break;
			}
			hc = hc + hi;
			ht = h*texture(textureMap2,tc).r;
		}
		if(tc.x < 0 || tc.x > 1 || tc.y < 0 || tc.y > 1){
			discard;
		}
	}

	return tc;
}

void main(void) {

	//Znormalizowane interpolowane wektory
	vec4 ml = normalize(l);
	vec4 ml2 = normalize(l2);
	vec4 mv = normalize(v);
	vec2 tc = parallax(mv, iTexCoord0, 700, 0.03);
	vec4 mn = normalize(vec4(texture(textureMap1, tc).rgb*2-1,0));
	
	//Wektor odbity
	vec4 mr = reflect(-ml, mn);
	vec4 mr2 = reflect(-ml2, mn);

	//Parametry powierzchni
	vec4 kd = texture(textureMap0, tc); 
	//vec4 ks = texture(textureMap1, tc);
	vec4 ks = vec4(1,1,1,1);

	//Obliczenie modelu oświetlenia
	float nl = clamp(dot(mn, ml), 0, 1);
	float rv = pow(clamp(dot(mr, mv), 0, 1),25);
	pixelColor = vec4(kd.rgb * nl, kd.a) + vec4(ks.rgb*rv, 0);

	//Obliczenie modelu oświetlenia
	float nl2 = clamp(dot(mn, ml2), 0, 1);
	float rv2 = pow(clamp(dot(mr2, mv), 0, 1),25);
	pixelColor += vec4(kd.rgb * nl2, kd.a) + vec4(ks.rgb*rv2, 0);
}
