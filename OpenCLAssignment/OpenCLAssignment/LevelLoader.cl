
__kernel void loadLevel(__global const int *chars,
	__global int *result)
{
	int i = get_global_id(0);
	
	result[i] = chars[i];

	/*
	if (chars[i] == 88) // X
	{
		result[i] = 0;
	}
	else if (chars[i] == 32) // Space
	{
		result[i] = 1;
	}
	else if (chars[i] == 71) // G
	{
		result[i] = 2;
	}
	else if (chars[i] == 83) // S
	{
		result[i] = 3;
	}
	else // \n
	{
		result[i] = 4;
	}
	*/
}
