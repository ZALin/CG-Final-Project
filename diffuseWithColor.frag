varying vec3 lightDir,normal;
uniform sampler2D tex;
uniform int angle;

void main()
{

	float intensity;
	vec4 colorbase = texture( tex, gl_TexCoord[0].st );;
	
	// normalizing the lights position to be on the safe side
	
	vec3 n = normalize(normal);
	
	intensity = dot(lightDir,n);
	

	
	if ( angle > 180)
		gl_FragColor = vec4( 0.05 , 0.05 , 0.05 , 1.0)*colorbase*intensity;
	else if( angle >= 150  ) //180-150
		gl_FragColor = vec4( 0.7+0.1*(angle-150)/30 ,0.2,0.0,1.0)*colorbase*intensity;
	else if( angle >= 140  ) //150-140
		gl_FragColor = vec4( 0.9-0.2*(angle-140)/10 , 0.9-0.7*(angle-140)/10 ,0.9-0.8*(angle-140)/10,1.0 )*colorbase*intensity;
	else if( angle >= 100  ) //140-100
		gl_FragColor = vec4(0.9,0.9,0.3 + 0.6*(angle-100)/40,1.0)*colorbase*intensity;
	else if( angle >= 80  ) //100-80
		gl_FragColor = vec4(1.0,1.0,0.3,1.0)*colorbase*intensity;
	else if( angle >= 40  ) //80-40
		gl_FragColor = vec4(0.9,0.9,0.9 - 0.6*(angle-40)/40,1.0)*colorbase*intensity;
	else if( angle >= 30  ) //40-30
		gl_FragColor = vec4( 0.7+0.2*(angle-30)/10 , 0.2+0.7*(angle-30)/10 ,0.1+0.8*(angle-30)/10,1.0 )*colorbase*intensity;
	else // ( angle >= 0) 30-0
		gl_FragColor = vec4(0.8-0.1*angle/30,0.2,0.0,1.0)*colorbase*intensity;
	
	

} 