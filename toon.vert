

varying vec3 lightDir,normal;
uniform vec4 lightpos;
uniform int angle;
uniform sampler2D tex;

void main()
{

	
	lightDir = normalize( vec3(lightpos) );
	// == lightDir = normalize(vec3(gl_LightSource[0].position));
	normal = gl_NormalMatrix * gl_Normal;
	
	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_Position = ftransform();
} 
