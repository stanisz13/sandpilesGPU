#version 330 core

out float res;

uniform sampler2D prevState;

void main()
{
	vec2 onePixel = vec2(1.0f) / textureSize(prevState, 0);
	vec2 pos = gl_FragCoord.xy/textureSize(prevState, 0);

	float mySand = texture(prevState, pos).r;
  	res = mySand;

  	if (mySand >= 4)
	{
		res -= 4;
	}

	if (texture(prevState, pos + vec2(onePixel.x, 0)).r >= 4)
   	res += 1;
   
	if (texture(prevState, pos + vec2(-onePixel.x, 0)).r >= 4)
   	res += 1;
   
	if (texture(prevState, pos + vec2(0, onePixel.y)).r >= 4)
   	res += 1;
   
	if (texture(prevState, pos + vec2(0, -onePixel.y)).r >= 4)
   	res += 1;
}
       

