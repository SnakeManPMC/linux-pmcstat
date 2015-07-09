/*
    dszlog datacollector for PMCTOP
 */
#include <dd.h>
#include <ddlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pmc.h> /* this is pmc header dir :) */

FILE *pmcdszfile;

struct dif *d;
dszlog dsz; /* from pmc.h */

char buf[300];
char temp_username[25];
char templine[80];
char filename[80];
char Cpsrate[80];
char filesize[80];
int Node;

/* routine to read dsz-log file */
int DSZ_Read_Routine()
{
	int cx;
	char *p;

	p = strtok(templine, " ");

	if (strcmp(p, "S"))
		/* upload or download? */
		dsz.flags = 0;  /* download */
	else
		dsz.flags = 1;
	/* upload */

	p = strtok(NULL, " ");

	strcpy(filesize, p); /* filesize from token */

	for (cx = 1; cx < 4; cx++) p = strtok(NULL, " ");
	/* do three times strtok */

	strcpy(Cpsrate, p);
	/* get cps rate */

	for (cx = 1; cx < 7; cx++) p = strtok(NULL, " ");
	/* six times here */

	strcpy(filename, p);
	/* get filename */

	return 0;
}

/* loop, read the log and write datas */
int Log_Looper()
{
	char node_data_name[80];       /* name for nodeinfo?.data file */
	FILE *cur_dsz_log;             /* dszlog.[nodenum] file */
	FILE *node_info;               /* nodeinfo?.data file open */
	struct DayDream_NodeInfo rock; /* crapp by DD */

	sprintf(node_data_name, "%snodeinfo%i.data", DDTMP, Node); /* create nodeinfo */

	node_info = fopen(node_data_name, "rb");
	/* open nodeinfo?.data */

	fread(&rock, sizeof(struct DayDream_NodeInfo), 1, node_info);
	/* read the user stats */
	fclose(node_info);

	sprintf(buf, "%sdszlog.%i", DDTMP, Node); /* create correct dszlog name */

	cur_dsz_log = fopen(buf, "rt");
	/* open DSZLOG.[nodenum] */
	if (!cur_dsz_log)
	{
		dd_sendstring(d, "error at dszlog\n");
		dd_close(d);
		exit(1);
	}

	/* here is the loop that reads one line of DSZLOG */
	fgets(templine, 80, cur_dsz_log);
	while (!feof(cur_dsz_log)) /* until we are end of dszlog file */
	{
		DSZ_Read_Routine()
		; /* actual DSZLOG reading function here */

		strcpy(dsz.file_name, filename);
		sscanf(filesize, "%lu", &dsz.file_size);
		sscanf(Cpsrate, "%lu", &dsz.file_cps);
		dsz.file_errors = 0;
		/*  dsz.flags= is done in dsz_read_routine() */
		dsz.user_slot = rock.ddn_userslot;

		fwrite(&dsz, sizeof(dszlog), 1, pmcdszfile);
		/* write to pmc_database_log */
		fgets(templine, 80, cur_dsz_log); /* read the next line */
	}

	fclose(cur_dsz_log);
	sprintf(buf, "%sdszlog.%i", DDTMP, Node);
	unlink(buf); /* delete the /tempdir/dszlog.[node] */
	return 0;
}

/* main bitch whore shit */
int main(int argv, char *argc[])

{
	if (argv == 1)
	{
		printf("need to run from DayDream\n");
		exit(1);
	}

	d = dd_initdoor(argc[1]);

	sprintf(buf, "%s/doors/pmcdsz.dat", getenv("DAYDREAM"));

	pmcdszfile = fopen(buf, "ab");
	if (!pmcdszfile)
	{
		dd_sendstring(d, "error at pmcdsz.dat, inform sysop!\n");
		dd_close(d);
		exit(1);
	}

	sscanf(argc[1], "%d", &Node);

	Log_Looper();

	fclose(pmcdszfile);
	dd_close(d);
	return 0;
}
