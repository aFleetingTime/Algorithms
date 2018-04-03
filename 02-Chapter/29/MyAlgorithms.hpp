#pragma once

unsigned d(int x[], int n)
{
	static unsigned count = 0;
	count = 0;
	for(int i = 0; i < n; i += 2)
		count += 2;
	int i = 1;
	while(i <= n / 2)
	{
		count += 3;
		++i;
	}
	count += 4;
	return count;
}
