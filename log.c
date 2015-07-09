/*

    this bitch displays the logfile into the screen.

 */
#include <stdio.h>
#include <pmc.h>

FILE *datafile;

dszlog dsz;

int ViewLog()
{
	int i = 0;

	datafile = fopen("pmcdsz.dat", "rb");
	if (!datafile)
	{
		printf("error at pmcdsz.dat!\n");
		exit (1);
		return 1;
	}

	while (!feof(datafile))
	{
		i++;
		fread(&dsz, sizeof(dszlog), 1, datafile);
		if (feof(datafile))
			break;

		printf("************************************\n");
		printf("user-record number: %i of database\n", i);
		printf("file_name:         %s\n", dsz.file_name);
		printf("file_size:         %lu\n", dsz.file_size);
		printf("file_cps:          %lu\n", dsz.file_cps);
		printf("file_errors:       %lu\n", dsz.file_errors);
		printf("flags:             %lu\n", dsz.flags);
		printf("user_slot:         %lu\n", dsz.user_slot);
	}
	printf("************************************\n");

	fclose(datafile);

	return 0;
}

int main()
{
	ViewLog();

	return 0;
}