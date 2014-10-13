#include <ctype.h>

//hacked together
//if string has pattern: char, digit, char --> ur fucked
static bool stringcmp(const char* a, const char* b)
{
	uint i=0;
	//skipt to first digit
	while(!isdigit(a[i]) && !isdigit(b[i]))
	{
		if(a[i] != b[i])
			return (a[i] < b[i]);

		if(!a[i])
			return 1;

		if(!b[i])
			return 0;

		++i;
	}
	//strip digits
	int da, db;
	da = atoi(&a[i]);
	db = atoi(&b[i]);
	return (da<db);
};
