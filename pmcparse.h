#include <stdarg.h>

/* this is the MAX len a string+replacement can be. make it larger if you
   need to parse bigger strings. */
#define __maxstrlen__ 1024

/*

docs for pmcparse(8)

NAME
	pmcparse

SYNOPSIS
	#include pmcparse.h

	int pmcparse(char *str,char start,const char *end, ...)

DESCRIPTION
	The parse function replaces macros - defined by start and
	end in the str string with with the "..." input.

	Returns number of replacements done.

EXAMPLE
	char buffer[100];
	int i,j;

	strcpy(buffer,"user: @-10sUN | upload: @5dUL Mb\n");
	i=pmcparse(&buffer,'@',"UN","flowje");
	j=pmcparse(&buffer,'@',"UL",10);
	printf("replaced UN macro %d times and UL macro %d times.\n",i,j);
	printf(buffer);

BUGS
	1) start cannot be the character '%' due to use of sprintf(..).
           Any other char will work though, so ;p
	2) Only strings and decimals currently supported. But, one can
	   always convert to a string before calling pmcparse(..).

CODE-LABS-INFO
	Here is how it works;

	If you have a string with a macro, say;

		"@sME likes beer."

	And we want to replace the macro with ME with "flower". This
	would be done with a call like this:

		pmcparse(&buffer,'@',"ME","flower");

	What happens when we call pmcparse(..) is:

	1) We look though the string untill we find the macro. For the ME
	   macro we found it as:

		@sME

	2) Then we replace the start char with an %, and remove the
	   name of the macro. Now the string looks like this:

		"%s likes beer."

	3) Then we use sprintf to print the value we wanted to put instead
	   of the macro, and voila we have what we want.

		"flower likes beer."

	4) Since we use sprintf formatting we could put a formatting-code
	   into the macro too, so:

		"| @-20sME | <- likes beer."

	   .. would become ..

		"| flower              | <- likes beer."

AUTHOR
	flower/pmc-secret-codelabs ;p
	<cyberkid@ktsascii.net>

*/

int pmcparse(char *b,char start,const char *end, ...) {
	va_list ap;
	char *t=b,*f,*r;
	int found=0,endlen=strlen(end);
	char tmp[__maxstrlen__];
	int decimal;
	char *streng;
	float flyder;

	va_start(ap,end);

	while (*t)
		/* we found the start-char */
		if (*t==start) {
			f=t+1;
			while (*f)
				if (!strncmp(f,end,endlen)) {
					strcpy(tmp,b);
					r=f;
					found++;
					*t='%';
					r+=strlen(end);
					while (*r) {
						*(r-endlen)=*r;
						r++;
					}
					*(r-endlen)=0;
					switch (*(f-1)) {
						/* handle decimals */
						case 'd':
							decimal=va_arg(ap,int);
							sprintf(tmp,b,decimal);
							break;
						/* handle strings */
						case 's':
							streng=va_arg(ap,char*);
							sprintf(tmp,b,streng);
							break;
						/* handle floats (broken) */
						case 'f':
							flyder=va_arg(ap,float);
printf("flyder=%f\n",flyder);
							sprintf(tmp,b,flyder);
							break;
					}
					strcpy(b,tmp);
					break;
				} else if (*f==start)
					break;
				else
					f++;
			t++;
		} else
			t++;

	va_end(ap);
	return found;
}
