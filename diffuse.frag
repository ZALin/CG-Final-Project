varying vec3 lightDir,normal;
uniform sampler2D tex;

void main()
{

	float intensity;
	vec4 colorbase = texture( tex, gl_TexCoord[0].st );
	//vec4 colorbase = texture( tex, vec2 );
	// normalizing the lights position to be on the safe side
	
	vec3 n = normalize(normal);
	
	intensity = dot(lightDir,n);
	
	if( intensity > 0.8)
	{
		gl_FragColor = colorbase*vec4(1.0,1.0,1.0,1.0);
	}
	else if(intensity > 0.5)
	{
		gl_FragColor = colorbase*vec4(0.8,0.8,0.8,0.8);
	}
	else
	{
		gl_FragColor = colorbase*vec4(0.5,0.5,0.5,0.5);
	}
	

} 