/*

    PMC tools for the trade... 1999

 */

/* for dszlog routine that is used with pmcdata.c / pmctop */
#define FILENAMELENGTH    50

typedef struct
{
	char file_name[FILENAMELENGTH];
	long file_size;
	long file_cps;
	long file_errors;
	int flags;
	long user_slot;
} dszlog;
/* dszlog end */

/* for Fastest Uploaders */

/* datafile format */
typedef struct
{
	char User_Name[25];
	unsigned long long ZeroDay; /* what the fuck is this long long ?? :) */
	unsigned long long OneDay;  /* flowers note: its 64bit integer == huge ;) */
	unsigned long long TwoDay;
	unsigned long long ThreeDay;
	unsigned long long FourDay;
	unsigned long long Older;
} Fastest;
/* Fastest end */

/* Baddest Batches datafile */
typedef struct
{
	char User_Name[25];
	unsigned long Megs;
	unsigned long Cps;
	unsigned long Files;
	unsigned long Time;
	char Date[10];
} Baddest;
/* Baddest end */

/* to add . into the file size & cps etc */
/* routine by flower the pimp, pmc 1999  */
char *f_format(char *s)
{
	char *c, *t = (char *)malloc(strlen(s) + (strlen(s) / 3) + 10); /* +10 just to be sure. */
	int i = 0;

	t += strlen(s) + (strlen(s) / 3) + 9;
	c = s + strlen(s);

	while (c >= s)
	{
		*t = *c;
		t--;
		c--;
		if ((i % 3 == 0) && (i))
		{
			*t = '.';
			t--;
		}
		i++;
	}

	t++;
	if (*t == '.')
		t++;

	return t;
}

/* sorting routine by Miz (c) PMC 1997.	*/
int PMC_Sort(unsigned long long sorted[], int position[], int max_user)
{
	int so_i = 1, so_x = 1;

	for (so_i = 1; so_i <= max_user; so_i++)
	{
		position[so_i] = 1;

		for (so_x = 1; so_x <= max_user; so_x++)
		{
			if (sorted[so_i] < sorted[so_x])
				position[so_i]++;
		}
	}
	return 0;
}

/* to stript the path from userfiles (names actually) */
int splitpath(char **args, char *p) {
	int c = 0;

	while (*p == '/')
		p++;

	args[c++] = p;
	while (*p)
	{
		if (*p == '/')
		{
			*(p++) = 0;
			args[c++] = p;
			while(*p == '/')
				p++;
		}
		else
			p++;
	}
	args[c] = 0;

	return c - 1;
}