#version 330 core

out vec4 FragColor;

uniform sampler2D tex;

bool cmp(const float a, const float b)
{
	return abs(a - b) < 0.0001f;
}

void main()
{
	vec2 pos = gl_FragCoord.xy/textureSize(tex, 0);
	float mySand = texture(tex, pos).r;
	
	if (mySand >= 4)
	FragColor = vec4(1);

	else if (cmp(mySand, 3.0f) )
	FragColor = vec4(0.92, 0.11, 0.22, 1);

	else if (cmp(mySand, 2.0f) )
	FragColor = vec4(0.45, 0.51, 0.84, 1);

	else if (cmp(mySand, 1.0f) )
	FragColor = vec4(0.02, 0.80, 0.11, 1);

	else if (cmp(mySand, 0.0f) )
	FragColor = vec4(0, 0, 0, 1);
}

