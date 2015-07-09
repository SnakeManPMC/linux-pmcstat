#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <dd.h>
#include <ddlib.h>
#include <pmcparse.h>
#include <pmc.h>

//#define DEBUG

const char Version[] = "v0.2b r1 12-24-01"; /* version number /rev /date */

FILE *bulletin;
FILE *layout;

char buf[300];
char user_name[300][26];
char user_loct[300][26];
char LastLogged[300][20];
char up_bytes[80];
char up_files[80];
char dn_bytes[80];
char dn_files[80];
char UL_totalbytes[80];
char UL_totalfiles[80];
char DL_totalbytes[80];
char DL_totalfiles[80];
char Ccalls[80];
char Ctotal_calls[80];
char FK_bytes[80];
char FK_files[80];
char FK_totalbytes[80];
char FK_totalfiles[80];
char line[1024], org_line[1024], parsebuf[1024];
char dates[20];
char times[20];
char Boardname[50];
char BulletinPath[80];

/* group datas */
char GroupName[10][80], GroupLongName[10][80];
unsigned long long GroupULBytes[10], GroupULFiles[10];
unsigned long long GroupDLBytes[10], GroupDLFiles[10];
unsigned long long GroupWEEKBytes[10], GroupWEEKFiles[10];
unsigned long long GroupWeekBytesTotal = 0, GroupWeekFilesTotal = 0;
unsigned long long GroupMONTHBytes[10], GroupMONTHFiles[10];
unsigned long long GroupMonthBytesTotal = 0, GroupMonthFilesTotal = 0;
unsigned long long GroupYEARBytes[10], GroupYEARFiles[10];
unsigned long long GroupYearBytesTotal = 0, GroupYearFilesTotal = 0;
int Groups;

/* week month year datas */
unsigned long long ul_bytes[300], ul_files[300];
unsigned long long dl_bytes[300], dl_files[300];
unsigned long long week_ul_bytes[300], week_dl_bytes[300];
unsigned long long week_ul_files[300], week_dl_files[300];
unsigned long long week_ul_total, week_dl_total, week_files_total;
unsigned long long month_ul_bytes[300], month_dl_bytes[300];
unsigned long long month_ul_files[300], month_dl_files[300];
unsigned long long month_ul_total, month_dl_total, month_files_total;
unsigned long long year_ul_bytes[300], year_dl_bytes[300];
unsigned long long year_ul_files[300], year_dl_files[300];
unsigned long long year_ul_total, year_dl_total, year_files_total;
unsigned long long faked_bytes[300], faked_files[300];
unsigned long long UL_Total_Bytes, UL_Total_Files;
unsigned long long DL_Total_Bytes, DL_Total_Files;
unsigned long long Total_Calls;
unsigned long long Total_Faked_Bytes, Total_Faked_Files;
long Top_Cps, temp_cps;

/* for the weektop generation, old userdatas */
char old_user_name[300][26];
unsigned long long old_ul_bytes[300], old_ul_files[300];
unsigned long long old_dl_bytes[300], old_dl_files[300];
unsigned long long old_faked_bytes[300], old_faked_files[300];
int old_maxuser = 0, old_calls[300];

/* how many users to list, and some counters */
int UL_User_Num = 0, DL_User_Num = 0, Call_User_Num = 0, Faked_User_Num = 0;
int calls[300], i, maxuser, userbyte_pos[300], x, Pos, Exclude_Sysop = 0;
int weekcheck = 0, monthcheck = 0, yearcheck = 0;

/* highscores */
unsigned long long HS_week_UL_User = 0;
unsigned long long HS_week_DL_User = 0;
unsigned long long HS_month_UL_User = 0;
unsigned long long HS_month_DL_User = 0;
unsigned long long HS_year_UL_User = 0;
unsigned long long HS_year_DL_User = 0;
unsigned long long HS_week_Alltime_UL = 0;
unsigned long long HS_week_Alltime_DL = 0;
unsigned long long HS_month_Alltime_UL = 0;
unsigned long long HS_month_Alltime_DL = 0;
unsigned long long HS_year_Alltime_UL = 0;
unsigned long long HS_year_Alltime_DL = 0;
char HS_week_UL_Username[80];
char HS_week_DL_Username[80];
char HS_month_UL_Username[80];
char HS_month_DL_Username[80];
char HS_year_UL_Username[80];
char HS_year_DL_Username[80];
/* dates for user/bbs highscores */
char HS_week_UL_Date[80];
char HS_week_DL_Date[80];
char HS_month_UL_Date[80];
char HS_month_DL_Date[80];
char HS_year_UL_Date[80];
char HS_year_DL_Date[80];
char HS_week_Alltime_UL_Date[80];
char HS_week_Alltime_DL_Date[80];
char HS_month_Alltime_UL_Date[80];
char HS_month_Alltime_DL_Date[80];
char HS_year_Alltime_UL_Date[80];
char HS_year_Alltime_DL_Date[80];
/* and group highscore bytes */
unsigned long long HS_week_UL_Group = 0;
unsigned long long HS_week_DL_Group = 0;
unsigned long long HS_month_UL_Group = 0;
unsigned long long HS_month_DL_Group = 0;
unsigned long long HS_year_UL_Group = 0;
unsigned long long HS_year_DL_Group = 0;

char HS_week_UL_Groupname[80];
char HS_week_DL_Groupname[80];
char HS_month_UL_Groupname[80];
char HS_month_DL_Groupname[80];
char HS_year_UL_Groupname[80];
char HS_year_DL_Groupname[80];
/* dates for group highscores */
char HS_week_UL_Group_Date[80];
char HS_week_DL_Group_Date[80];
char HS_month_UL_Group_Date[80];
char HS_month_DL_Group_Date[80];
char HS_year_UL_Group_Date[80];
char HS_year_DL_Group_Date[80];

/* reads config file and if found blaablaa, then take a note */
int CFGread()
{
	char cfgtemp[80];
	char *cfg_p;
	char bbstemp[50];
	FILE *cfg_file;

	printf("reading configuration file... ");

	/* open c0nFiG file */
	cfg_file = fopen("pmctop.cfg", "rt");
	if (!cfg_file)
	{
		printf("error at pmctop.cfg file!\n");
		exit(1);
	}

	fgets(cfgtemp, 80, cfg_file);

	while (!feof(cfg_file)) /* read config until end of file */
	{
		if (strstr(cfgtemp, "Board_Name")) /* bbs name */
		{
			cfg_p = strtok(cfgtemp, " ");
			cfg_p = strtok(NULL, " "); /* first token of name */
			bzero(bbstemp, sizeof(bbstemp));
			while (cfg_p) /* do tokens until we dont have anything */
			{
				strcat(bbstemp, cfg_p);    /* append the next */
				strcat(bbstemp, " ");      /* some space... */
				cfg_p = strtok(NULL, " "); /* tokenize another */
			}
			bzero(Boardname, sizeof(Boardname));
			strncpy(Boardname, bbstemp, strlen(bbstemp) - 2); /* removing \0 */
		}

		if (strstr(cfgtemp, "Top_Upload_Users")) /* top uploaders number */
		{
			cfg_p = strtok(cfgtemp, " ");
			cfg_p = strtok(NULL, " ");
			UL_User_Num = atoi(cfg_p);
		}

		if (strstr(cfgtemp, "Top_Download_Users")) /* top downloaders number */
		{
			cfg_p = strtok(cfgtemp, " ");
			cfg_p = strtok(NULL, " ");
			DL_User_Num = atoi(cfg_p);
		}

		if (strstr(cfgtemp, "Top_Caller_Users")) /* top callers number */
		{
			cfg_p = strtok(cfgtemp, " ");
			cfg_p = strtok(NULL, " ");
			Call_User_Num = atoi(cfg_p);
		}

		if (strstr(cfgtemp, "Top_Faked_Users")) /* top faked number */
		{
			cfg_p = strtok(cfgtemp, " ");
			cfg_p = strtok(NULL, " ");
			Faked_User_Num = atoi(cfg_p);
		}

		if (strstr(cfgtemp, "Exclude_Sysop")) /* exclude sysop? */
		{
			cfg_p = strtok(cfgtemp, " ");
			cfg_p = strtok(NULL, " ");
			if (strstr(cfg_p, "yes"))
			{
				printf("Yup, sysop not included.");
				Exclude_Sysop = 1;
			}
		}

		if (strstr(cfgtemp, "bulletins_path")) /* path to place bulletins */
		{
			cfg_p = strtok(cfgtemp, " ");
			cfg_p = strtok(NULL, " ");
			bzero(BulletinPath, sizeof(BulletinPath));
			strcpy(BulletinPath, cfg_p);
		}

		fgets(cfgtemp, 80, cfg_file);
	}

	fclose(cfg_file);
	printf(" Done!\n");
	return 0;
}

int Headers()
{
/*    sprintf(buf,"\n\n\tPMCTOP %s (c) Snake Man, PMC 1999-2015.\n\n",Version);
    printf(buf);
 */
	printf("\n\n\tPMCTOP %s (c) Snake Man, PMC 1999-2015.\n\n", Version);

	return 0;
}

int Time_Check()
{
	struct tm *tm;
	time_t t;

	(void *) t = time(0);
	tm = localtime(&t);

	sprintf(dates, "%2.2d/%2.2d/%2.2d", tm->tm_mon + 1, tm->tm_mday, tm->tm_year % 100);
	sprintf(times, "%2.2d:%2.2d", tm->tm_hour, tm->tm_min);

	return 0;
}

int Generic_Footer()
{
	if (strstr(line, "BOARD_NAME")) /* name of the bbs */
	{
		sprintf(parsebuf, "%s", Boardname);
		pmcparse(line, '@', "BOARD_NAME", parsebuf);
	}

	if (strstr(line, "S_DATE")) /* in date/time part? */
	{
		sprintf(parsebuf, "%s", dates); /* system date 00/00/00 - month/day/year */
		pmcparse(line, '@', "S_DATE", parsebuf);

		sprintf(parsebuf, "%s", times); /* system time 00:00 - hour:min */
		pmcparse(line, '@', "S_TIME", parsebuf);
	}

	return 0;
}

/* scooping user information & creating all datas. */
int UserBase()
{
	struct userbase ub;
	struct tm *tm;
	time_t last;
	FILE *u_file;

	printf("reading DayDream userbase... ");

/*    sprintf(buf,"%s/data/userbase.dat",getenv("DAYDREAM"));*/
	strcpy(buf, "/home/bbs/data/userbase.dat");

	u_file = fopen(buf, "rb");
	if (!u_file)
	{
		printf("error at %s file!\n", buf);
		exit (1);
	}

	i = 1;
	fread(&ub, sizeof(struct userbase), 1, u_file);

	if (Exclude_Sysop)
	{
		//printf("dont use sysops account.");
		fread(&ub, sizeof(struct userbase), 1, u_file);
	}

	while (!feof(u_file))
	{
		strcpy(user_name[i], ub.user_handle);
		strcpy(user_loct[i], ub.user_organization);
		if (strstr(user_loct[i], "%S"))
			strcpy(user_loct[i], "pmctop modded location :)");
		if (strstr(user_loct[i], "%s"))
			strcpy(user_loct[i], "pmctop modded location :)");
		ul_bytes[i] = 0;
		ul_bytes[i] = ub.user_ulbytes;
		ul_files[i] = 0;
		ul_files[i] = ub.user_ulfiles;
		UL_Total_Bytes = (UL_Total_Bytes + ub.user_ulbytes);
		UL_Total_Files = (UL_Total_Files + ub.user_ulfiles);

		dl_bytes[i] = 0;
		dl_bytes[i] = ub.user_dlbytes;
		dl_files[i] = 0;
		dl_files[i] = ub.user_dlfiles;
		DL_Total_Bytes = (DL_Total_Bytes + ub.user_dlbytes);
		DL_Total_Files = (DL_Total_Files + ub.user_dlfiles);

		calls[i] = 0;
		calls[i] = ub.user_connections;
		Total_Calls = (Total_Calls + ub.user_connections);

		last = ub.user_lastcall;
		tm = localtime(&last);
		sprintf(LastLogged[i], "%-2.2d/%-2.2d/%-2.2d", tm->tm_mon + 1, tm->tm_mday, tm->tm_year % 100);

		faked_bytes[i] = 0;
		faked_bytes[i] = ub.user_fakedbytes;
		faked_files[i] = 0;
		faked_files[i] = ub.user_fakedfiles;
		Total_Faked_Bytes = (Total_Faked_Bytes + ub.user_fakedbytes);
		Total_Faked_Files = (Total_Faked_Files + ub.user_fakedfiles);

		fread(&ub, sizeof(struct userbase), 1, u_file);
		i++;
	}

	maxuser = i;
	fclose(u_file);
	printf(" Done!\n");
	return 0;
}

int DSZ_Check()
{
	FILE *dsz_file;

	dszlog dsz;
	Top_Cps = 0;

	printf("reading pmcdsz.dat logfile... ");

	dsz_file = fopen("pmcdsz.dat", "rb");
	if (!dsz_file)
	{
		printf("error at pmcdsz.dat file!\n");
		Top_Cps = 0;
		return 0;
	}

	fread(&dsz, sizeof(dszlog), 1, dsz_file);

	while (!feof(dsz_file))
	{
		temp_cps = dsz.file_cps; /* cps rate of current file record of pmcdsz.dat */
		if (Top_Cps < temp_cps)
			Top_Cps = temp_cps;
		fread(&dsz, sizeof(dszlog), 1, dsz_file);
	}

	fclose(dsz_file);
	printf("Done!\n");
	return 0;
}

/* creation of UPLOAD bulletin */
int Upload_List()
{
	FILE *top1lay;
	FILE *top1blt;
	printf("Writing Top Uploaders list... ");

	PMC_Sort(ul_bytes, userbyte_pos, maxuser);

/* read/write the damned example ansi file */
	top1lay = fopen("example//pmctop.1.gfx", "rt");
	if (!top1lay)
	{
		printf("error at pmctop.1.gfx!\n");
		exit(1);
	}

	top1blt = fopen("pmctop_uploader.gfx", "wt");
	if (!top1blt)
	{
		printf("error at pmctop_uploader.gfx!\n");
		exit (1);
	}

	bzero(line, sizeof(line));
	fgets(line, 1024, top1lay);

/* header part */
	while (!strstr(line, "U_NAME")) /* read until were on the macro part. */
	{
		if (feof(top1lay))
			break;
		fputs(line, top1blt);
		bzero(line, sizeof(line));
		fgets(line, 1024, top1lay);
	}
/* done, header is done! */
	bzero(org_line, sizeof(org_line));
	strcpy(org_line, line);

	Pos = 1;
/* displaying the sorted by Miz (c) PMC 1997. */
	while (Pos <= UL_User_Num)
	{
		for (x = 1; x <= maxuser; x++)
			if (userbyte_pos[x] == Pos)
			{
				if (ul_bytes[x] == 0)
					break;  /* no more guys, break the loop */

				sprintf(up_bytes, "%llu", ul_bytes[x]);
				strcpy(up_bytes, f_format(up_bytes));

				sprintf(up_files, "%llu", ul_files[x]);
				strcpy(up_files, f_format(up_files));

				strcpy(line, org_line);
				sprintf(parsebuf, "%i", Pos); /* position to parsebuf */
				pmcparse(line, '@', "U_POS", parsebuf);

				sprintf(parsebuf, "%s", user_name[x]); /* username to parsebuf */
				pmcparse(line, '@', "U_NAME", parsebuf);

				sprintf(parsebuf, "%s", user_loct[x]); /* user location for parsebuf */
				pmcparse(line, '@', "U_LOCAT", parsebuf);

				sprintf(parsebuf, "%s", up_files); /* files uploaded for parsebuf */
				pmcparse(line, '@', "U_FILES", parsebuf);

				sprintf(parsebuf, "%s", up_bytes); /* bytes uploaded for parsebuf */
				pmcparse(line, '@', "U_BYTES", parsebuf);

/*    sprintf(buf," %2i %-15s %-25s %8s %16s\n",Pos,user_name[x],user_loct[x],up_files,up_bytes); */
				fputs(line, top1blt);
			}
		Pos++;
	}
	sprintf(UL_totalbytes, "%llu", UL_Total_Bytes);
	strcpy(UL_totalbytes, f_format(UL_totalbytes));
	sprintf(UL_totalfiles, "%llu", UL_Total_Files);
	strcpy(UL_totalfiles, f_format(UL_totalfiles));

	bzero(line, sizeof(line));
	fgets(line, 1024, top1lay);
/* footer part */
	while (!feof(top1lay))
	{
		Generic_Footer();

		if (strstr(line, "TOT_BYTES")) /* in totalbytes/megs part? */
		{
			sprintf(parsebuf, "%s", UL_totalbytes);
			pmcparse(line, '@', "TOT_BYTES", parsebuf);

			sprintf(parsebuf, "%s", UL_totalfiles);
			pmcparse(line, '@', "TOT_FILES", parsebuf);

			sprintf(UL_totalbytes, "%llu", UL_Total_Bytes / (1024 * 1024));
			strcpy(UL_totalbytes, f_format(UL_totalbytes));
			sprintf(parsebuf, "%s", UL_totalbytes);
			pmcparse(line, '@', "TOT_MEG", parsebuf);

			sprintf(UL_totalbytes, "%llu", UL_Total_Bytes / (1024 * 1024 * 1024));
			strcpy(UL_totalbytes, f_format(UL_totalbytes));
			sprintf(parsebuf, "%s", UL_totalbytes);
			pmcparse(line, '@', "TOT_GIG", parsebuf);
		}

		if (feof(top1lay))
			break;
		fputs(line, top1blt);
		bzero(line, sizeof(line));
		fgets(line, 1024, top1lay);
	}
	fclose(top1blt);
	fclose(top1lay);

/* NEW NEW NEW NEW NEW NEW NEW NEW NEW NEW NEW NEW NEW NEW NEW NEW NEW NEW NEW

    fucking txt part of the shit */
/* read/write the damned example ansi file */
	top1lay = fopen("example//pmctop.1.txt", "rt");
	if (!top1lay)
	{
		printf("error at pmctop.1.txt!\n");
		exit(1);
	}

	top1blt = fopen("pmctop_uploader.txt", "wt");
	if (!top1blt)
	{
		printf("error at pmctop_uploader.txt!\n");
		exit (1);
	}

	bzero(line, sizeof(line));
	fgets(line, 1024, top1lay);

/* header part */
	while (!strstr(line, "U_NAME")) /* read until were on the macro part. */
	{
		if (feof(top1lay))
			break;
		fputs(line, top1blt);
		bzero(line, sizeof(line));
		fgets(line, 1024, top1lay);
	}
/* done, header is done! */
	bzero(org_line, sizeof(org_line));
	strcpy(org_line, line);

	Pos = 1;
/* displaying the sorted by Miz (c) PMC 1997. */
	while (Pos <= UL_User_Num)
	{
		for (x = 1; x <= maxuser; x++)
			if (userbyte_pos[x] == Pos)
			{
				if (ul_bytes[x] == 0)
					break;  /* no more guys, break the loop */

				sprintf(up_bytes, "%llu", ul_bytes[x]);
				strcpy(up_bytes, f_format(up_bytes));

				sprintf(up_files, "%llu", ul_files[x]);
				strcpy(up_files, f_format(up_files));

				strcpy(line, org_line);
				sprintf(parsebuf, "%i", Pos); /* position to parsebuf */
				pmcparse(line, '@', "U_POS", parsebuf);

				sprintf(parsebuf, "%s", user_name[x]); /* username to parsebuf */
				pmcparse(line, '@', "U_NAME", parsebuf);

				sprintf(parsebuf, "%s", user_loct[x]); /* user location for parsebuf */
				pmcparse(line, '@', "U_LOCAT", parsebuf);

				sprintf(parsebuf, "%s", up_files); /* files uploaded for parsebuf */
				pmcparse(line, '@', "U_FILES", parsebuf);

				sprintf(parsebuf, "%s", up_bytes); /* bytes uploaded for parsebuf */
				pmcparse(line, '@', "U_BYTES", parsebuf);

				fputs(line, top1blt);
			}
		Pos++;
	}
	sprintf(UL_totalbytes, "%llu", UL_Total_Bytes);
	strcpy(UL_totalbytes, f_format(UL_totalbytes));
	sprintf(UL_totalfiles, "%llu", UL_Total_Files);
	strcpy(UL_totalfiles, f_format(UL_totalfiles));

	bzero(line, sizeof(line));
	fgets(line, 1024, top1lay);
/* footer part */
	while (!feof(top1lay))
	{
		Generic_Footer();

		if (strstr(line, "TOT_BYTES")) /* in totalbytes/megs part? */
		{
			sprintf(parsebuf, "%s", UL_totalbytes);
			pmcparse(line, '@', "TOT_BYTES", parsebuf);

			sprintf(parsebuf, "%s", UL_totalfiles);
			pmcparse(line, '@', "TOT_FILES", parsebuf);

			sprintf(UL_totalbytes, "%llu", UL_Total_Bytes / (1024 * 1024));
			strcpy(UL_totalbytes, f_format(UL_totalbytes));
			sprintf(parsebuf, "%s", UL_totalbytes);
			pmcparse(line, '@', "TOT_MEG", parsebuf);

			sprintf(UL_totalbytes, "%llu", UL_Total_Bytes / (1024 * 1024 * 1024));
			strcpy(UL_totalbytes, f_format(UL_totalbytes));
			sprintf(parsebuf, "%s", UL_totalbytes);
			pmcparse(line, '@', "TOT_GIG", parsebuf);
		}

		if (feof(top1lay))
			break;
		fputs(line, top1blt);
		bzero(line, sizeof(line));
		fgets(line, 1024, top1lay);
	}
	fclose(top1blt);
	fclose(top1lay);
	printf("Done!\n");

	return 0;
}

/* creation of DOWNLOAD bulletin */
int Download_List()
{
	FILE *dlblt;
	FILE *dllay;
	printf("writing Top Downloaders list... ");

	PMC_Sort(dl_bytes, userbyte_pos, maxuser);

	dllay = fopen("example//pmctop.2.gfx", "rt");
	if (!dllay)
	{
		printf("error at pmctop.2.gfx!\n");
		exit(1);
	}

	dlblt = fopen("pmctop_downloader.gfx", "wt");
	if (!dlblt)
	{
		printf("error at pmctop_downloader.gfx!\n");
		exit (1);
	}

	bzero(line, sizeof(line));
	fgets(line, 1024, dllay);

/* header part */
	while (!strstr(line, "U_NAME")) /* read until were on the macro part. */
	{
		if (feof(dllay))
			break;
		fputs(line, dlblt);
		bzero(line, sizeof(line));
		fgets(line, 1024, dllay);
	}
/* done, header is done! */
	bzero(org_line, sizeof(org_line));
	strcpy(org_line, line);

	Pos = 1;
/* displaying the sorted by Miz (c) PMC 1997. */
	while (Pos <= DL_User_Num)
	{
		for (x = 1; x <= maxuser; x++)
			if (userbyte_pos[x] == Pos)
			{
				if (dl_bytes[x] == 0)
					break;  /* no more guys, break the loop */

				sprintf(dn_bytes, "%llu", dl_bytes[x]);
				strcpy(dn_bytes, f_format(dn_bytes));

				sprintf(dn_files, "%llu", dl_files[x]);
				strcpy(dn_files, f_format(dn_files));

				bzero(line, sizeof(line));
				strcpy(line, org_line);
				sprintf(parsebuf, "%i", Pos);
				pmcparse(line, '@', "U_POS", parsebuf);

				sprintf(parsebuf, "%s", user_name[x]);
				pmcparse(line, '@', "U_NAME", parsebuf);

				sprintf(parsebuf, "%s", user_loct[x]);
				pmcparse(line, '@', "U_LOCAT", parsebuf);

				sprintf(parsebuf, "%s", dn_files);
				pmcparse(line, '@', "D_FILES", parsebuf);

				sprintf(parsebuf, "%s", dn_bytes);
				pmcparse(line, '@', "D_BYTES", parsebuf);

				fputs(line, dlblt);
			}
		Pos++;
	}
	sprintf(DL_totalbytes, "%llu", DL_Total_Bytes);
	strcpy(DL_totalbytes, f_format(DL_totalbytes));
	sprintf(DL_totalfiles, "%llu", DL_Total_Files);
	strcpy(DL_totalfiles, f_format(DL_totalfiles));

	bzero(line, sizeof(line));
	fgets(line, 1024, dllay);
/* footer part */
	while (!feof(dllay))
	{
		Generic_Footer();

		if (strstr(line, "TOT_BYTES"))
		{
			sprintf(parsebuf, "%s", DL_totalbytes);
			pmcparse(line, '@', "TOT_BYTES", parsebuf);

			sprintf(parsebuf, "%s", DL_totalfiles);
			pmcparse(line, '@', "TOT_FILES", parsebuf);

			sprintf(DL_totalbytes, "%llu", DL_Total_Bytes / (1024 * 1024));
			strcpy(DL_totalbytes, f_format(DL_totalbytes));
			sprintf(parsebuf, "%s", DL_totalbytes);
			pmcparse(line, '@', "TOT_MEG", parsebuf);

			sprintf(DL_totalbytes, "%llu", DL_Total_Bytes / (1024 * 1024 * 1024));
			strcpy(DL_totalbytes, f_format(DL_totalbytes));
			sprintf(parsebuf, "%s", DL_totalbytes);
			pmcparse(line, '@', "TOT_GIG", parsebuf);
		}

		if (feof(dllay))
			break;
		fputs(line, dlblt);
		bzero(line, sizeof(line));
		fgets(line, 1024, dllay);
	}
	fclose(dlblt);
	fclose(dllay);

/* NEW NEW NEW NEW NEW NEW NEW NEW NEW NEW NEW ASCII part! */
	dllay = fopen("example//pmctop.2.txt", "rt");
	if (!dllay)
	{
		printf("error at pmctop.2.txt!\n");
		exit(1);
	}

	dlblt = fopen("pmctop_downloader.txt", "wt");
	if (!dlblt)
	{
		printf("error at pmctop_downloader.txt!\n");
		exit (1);
	}

	bzero(line, sizeof(line));
	fgets(line, 1024, dllay);

/* header part */
	while (!strstr(line, "U_NAME")) /* read until were on the macro part. */
	{
		if (feof(dllay))
			break;
		fputs(line, dlblt);
		bzero(line, sizeof(line));
		fgets(line, 1024, dllay);
	}
/* done, header is done! */
	bzero(org_line, sizeof(org_line));
	strcpy(org_line, line);

	Pos = 1;
/* displaying the sorted by Miz (c) PMC 1997. */
	while (Pos <= DL_User_Num)
	{
		for (x = 1; x <= maxuser; x++)
			if (userbyte_pos[x] == Pos)
			{
				if (dl_bytes[x] == 0)
					break;  /* no more guys, break the loop */

				sprintf(dn_bytes, "%llu", dl_bytes[x]);
				strcpy(dn_bytes, f_format(dn_bytes));

				sprintf(dn_files, "%llu", dl_files[x]);
				strcpy(dn_files, f_format(dn_files));

				bzero(line, sizeof(line));
				strcpy(line, org_line);
				sprintf(parsebuf, "%i", Pos);
				pmcparse(line, '@', "U_POS", parsebuf);

				sprintf(parsebuf, "%s", user_name[x]);
				pmcparse(line, '@', "U_NAME", parsebuf);

				sprintf(parsebuf, "%s", user_loct[x]);
				pmcparse(line, '@', "U_LOCAT", parsebuf);

				sprintf(parsebuf, "%s", dn_files);
				pmcparse(line, '@', "D_FILES", parsebuf);

				sprintf(parsebuf, "%s", dn_bytes);
				pmcparse(line, '@', "D_BYTES", parsebuf);

				fputs(line, dlblt);
			}
		Pos++;
	}
	sprintf(DL_totalbytes, "%llu", DL_Total_Bytes);
	strcpy(DL_totalbytes, f_format(DL_totalbytes));
	sprintf(DL_totalfiles, "%llu", DL_Total_Files);
	strcpy(DL_totalfiles, f_format(DL_totalfiles));

	bzero(line, sizeof(line));
	fgets(line, 1024, dllay);
/* footer part */
	while (!feof(dllay))
	{
		Generic_Footer();

		if (strstr(line, "TOT_BYTES"))
		{
			sprintf(parsebuf, "%s", DL_totalbytes);
			pmcparse(line, '@', "TOT_BYTES", parsebuf);

			sprintf(parsebuf, "%s", DL_totalfiles);
			pmcparse(line, '@', "TOT_FILES", parsebuf);

			sprintf(DL_totalbytes, "%llu", DL_Total_Bytes / (1024 * 1024));
			strcpy(DL_totalbytes, f_format(DL_totalbytes));
			sprintf(parsebuf, "%s", DL_totalbytes);
			pmcparse(line, '@', "TOT_MEG", parsebuf);

			sprintf(DL_totalbytes, "%llu", DL_Total_Bytes / (1024 * 1024 * 1024));
			strcpy(DL_totalbytes, f_format(DL_totalbytes));
			sprintf(parsebuf, "%s", DL_totalbytes);
			pmcparse(line, '@', "TOT_GIG", parsebuf);
		}

		if (feof(dllay))
			break;
		fputs(line, dlblt);
		bzero(line, sizeof(line));
		fgets(line, 1024, dllay);
	}
	fclose(dlblt);
	fclose(dllay);
	printf("Done!\n");

	return 0;
}

/* creation of CALLS bulletin */
int Calls_List()
{
	FILE *clay;
	FILE *cblt;
	printf("writing Top Callers list... ");

/*        PMC_Sort(calls, userbyte_pos, maxuser);*/

/* sorting routine by Miz (c) PMC 1997.	*/
	for (i = 1; i <= maxuser; i++)
	{
		userbyte_pos[i] = 1;

		for (x = 1; x <= maxuser; x++)
		{
			if (calls[i] < calls[x])
				userbyte_pos[i]++;
		}
	}

	clay = fopen("example//pmctop.3.gfx", "rt");
	if (!clay)
	{
		printf("error at pmctop.3.gfx!\n");
		exit(1);
	}

	cblt = fopen("pmctop_callers.gfx", "wt");
	if (!cblt)
	{
		printf("error at pmctop_callers.gfx!\n");
		exit (1);
	}

	bzero(line, sizeof(line));
	fgets(line, 1024, clay);

/* header part */
	while (!strstr(line, "U_NAME")) /* read until were on the macro part. */
	{
		if (feof(clay))
			break;
		fputs(line, cblt);
		bzero(line, sizeof(line));
		fgets(line, 1024, clay);
	}
/* done, header is done! */
	bzero(org_line, sizeof(org_line));
	strcpy(org_line, line);

	Pos = 1;
/* displaying the sorted by Miz (c) PMC 1997. */
	while (Pos <= Call_User_Num)
	{
		for (x = 1; x <= maxuser; x++)
			if (userbyte_pos[x] == Pos)
			{
				if (calls[x] == 0)
					break;  /* no more guys, break the loop */

				sprintf(Ccalls, "%i", calls[x]);
				strcpy(Ccalls, f_format(Ccalls));

				bzero(line, sizeof(line));
				strcpy(line, org_line);
				sprintf(parsebuf, "%i", Pos);
				pmcparse(line, '@', "U_POS", parsebuf);

				sprintf(parsebuf, "%s", user_name[x]);
				pmcparse(line, '@', "U_NAME", parsebuf);

				sprintf(parsebuf, "%s", user_loct[x]);
				pmcparse(line, '@', "U_LOCAT", parsebuf);

				sprintf(parsebuf, "%s", Ccalls);
				pmcparse(line, '@', "U_CALLS", parsebuf);

				sprintf(parsebuf, "%s", LastLogged[x]);
				pmcparse(line, '@', "D_LASTON", parsebuf);

				fputs(line, cblt);
			}
		Pos++;
	}
	sprintf(Ctotal_calls, "%llu", Total_Calls);
	strcpy(Ctotal_calls, f_format(Ctotal_calls));

	bzero(line, sizeof(line));
	fgets(line, 1024, clay);
/* footer part */
	while (!feof(clay))
	{
		Generic_Footer();

		if (strstr(line, "TOT_CALLS")) /* total calls */
		{
			sprintf(parsebuf, "%s", Ctotal_calls); /* system time 00:00 - hour:min */
			pmcparse(line, '@', "TOT_CALLS", parsebuf);
		}

		if (feof(clay))
			break;
		fputs(line, cblt);
		bzero(line, sizeof(line));
		fgets(line, 1024, clay);
	}
	fclose(cblt);
	fclose(clay);

/* NEW NEW NEW NEW NEW NEW NEW NEW NEW NEW ASCIIIII */
	clay = fopen("example//pmctop.3.txt", "rt");
	if (!clay)
	{
		printf("error at pmctop.3.txt!\n");
		exit(1);
	}

	cblt = fopen("pmctop_callers.txt", "wt");
	if (!cblt)
	{
		printf("error at pmctop_callers.txt!\n");
		exit (1);
	}

	bzero(line, sizeof(line));
	fgets(line, 1024, clay);

/* header part */
	while (!strstr(line, "U_NAME")) /* read until were on the macro part. */
	{
		if (feof(clay))
			break;
		fputs(line, cblt);
		bzero(line, sizeof(line));
		fgets(line, 1024, clay);
	}
/* done, header is done! */
	bzero(org_line, sizeof(org_line));
	strcpy(org_line, line);

	Pos = 1;
/* displaying the sorted by Miz (c) PMC 1997. */
	while (Pos <= Call_User_Num)
	{
		for (x = 1; x <= maxuser; x++)
			if (userbyte_pos[x] == Pos)
			{
				if (calls[x] == 0)
					break;  /* no more guys, break the loop */

				sprintf(Ccalls, "%i", calls[x]);
				strcpy(Ccalls, f_format(Ccalls));

				bzero(line, sizeof(line));
				strcpy(line, org_line);
				sprintf(parsebuf, "%i", Pos);
				pmcparse(line, '@', "U_POS", parsebuf);

				sprintf(parsebuf, "%s", user_name[x]);
				pmcparse(line, '@', "U_NAME", parsebuf);

				sprintf(parsebuf, "%s", user_loct[x]);
				pmcparse(line, '@', "U_LOCAT", parsebuf);

				sprintf(parsebuf, "%s", Ccalls);
				pmcparse(line, '@', "U_CALLS", parsebuf);

				sprintf(parsebuf, "%s", LastLogged[x]);
				pmcparse(line, '@', "D_LASTON", parsebuf);

				fputs(line, cblt);
			}
		Pos++;
	}
	sprintf(Ctotal_calls, "%llu", Total_Calls);
	strcpy(Ctotal_calls, f_format(Ctotal_calls));

	bzero(line, sizeof(line));
	fgets(line, 1024, clay);
/* footer part */
	while (!feof(clay))
	{
		Generic_Footer();

		if (strstr(line, "TOT_CALLS")) /* total calls */
		{
			sprintf(parsebuf, "%s", Ctotal_calls); /* system time 00:00 - hour:min */
			pmcparse(line, '@', "TOT_CALLS", parsebuf);
		}

		if (feof(clay))
			break;
		fputs(line, cblt);
		bzero(line, sizeof(line));
		fgets(line, 1024, clay);
	}
	fclose(cblt);
	fclose(clay);
	printf("Done!\n");

	return 0;
}

/* creation of FAKED bulletin */
int Faked_List()
{
	FILE *fakblt;
	FILE *faklay;
	printf("writing Top Faked list... ");

	PMC_Sort(faked_bytes, userbyte_pos, maxuser);

	faklay = fopen("example//pmctop.4.gfx", "rt");
	if (!faklay)
	{
		printf("error at pmctop.4.gfx!\n");
		exit(1);
	}

	fakblt = fopen("pmctop_faked.gfx", "wt");
	if (!fakblt)
	{
		printf("error at pmctop_faked.gfx!\n");
		exit (1);
	}

	bzero(line, sizeof(line));
	fgets(line, 1024, faklay);

/* header part */
	while (!strstr(line, "U_NAME")) /* read until were on the macro part. */
	{
		if (feof(faklay))
			break;
		fputs(line, fakblt);
		bzero(line, sizeof(line));
		fgets(line, 1024, faklay);
	}
/* done, header is done! */
	bzero(org_line, sizeof(org_line));
	strcpy(org_line, line);

	Pos = 1;
/* displaying the sorted by Miz (c) PMC 1997. */
	while (Pos <= Faked_User_Num)
	{
		for (x = 1; x <= maxuser; x++)
			if (userbyte_pos[x] == Pos)
			{
				if (faked_bytes[x] == 0)
					break;  /* no more guys, break the loop */

				sprintf(FK_bytes, "%llu", faked_bytes[x]);
				strcpy(FK_bytes, f_format(FK_bytes));

				sprintf(FK_files, "%llu", faked_files[x]);
				strcpy(FK_files, f_format(FK_files));

				bzero(line, sizeof(line));
				strcpy(line, org_line);
				sprintf(parsebuf, "%i", Pos);
				pmcparse(line, '@', "U_POS", parsebuf);

				sprintf(parsebuf, "%s", user_name[x]);
				pmcparse(line, '@', "U_NAME", parsebuf);

				sprintf(parsebuf, "%s", user_loct[x]);
				pmcparse(line, '@', "U_LOCAT", parsebuf);

				sprintf(parsebuf, "%s", FK_bytes);
				pmcparse(line, '@', "B_FAKED", parsebuf);

				sprintf(parsebuf, "%s", FK_files);
				pmcparse(line, '@', "F_FAKED", parsebuf);

				fputs(line, fakblt);
			}
		Pos++;
	}

	sprintf(FK_totalbytes, "%llu", Total_Faked_Bytes);
	strcpy(FK_totalbytes, f_format(FK_totalbytes));

	sprintf(FK_totalfiles, "%llu", Total_Faked_Files);
	strcpy(FK_totalfiles, f_format(FK_totalfiles));

	bzero(line, sizeof(line));
	fgets(line, 1024, faklay);
/* footer part */
	while (!feof(faklay))
	{
		Generic_Footer();

		if (strstr(line, "TOTF_BYTES"))
		{
			sprintf(parsebuf, "%s", FK_totalbytes);
			pmcparse(line, '@', "TOTF_BYTES", parsebuf);

			sprintf(parsebuf, "%s", FK_totalfiles);
			pmcparse(line, '@', "TOTF_FILES", parsebuf);

			sprintf(FK_totalbytes, "%llu", Total_Faked_Bytes / (1024 * 1024));
			strcpy(FK_totalbytes, f_format(FK_totalbytes));
			sprintf(parsebuf, "%s", FK_totalbytes);
			pmcparse(line, '@', "TOTF_MEG", parsebuf);

			sprintf(FK_totalbytes, "%llu", Total_Faked_Bytes / (1024 * 1024 * 1024));
			strcpy(FK_totalbytes, f_format(FK_totalbytes));
			sprintf(parsebuf, "%s", FK_totalbytes);
			pmcparse(line, '@', "TOTF_GIG", parsebuf);
		}

		if (feof(faklay))
			break;
		fputs(line, fakblt);
		bzero(line, sizeof(line));
		fgets(line, 1024, faklay);
	}

	fclose(fakblt);
	fclose(faklay);

/* NEW NEW NEW NEW NEW NEW NEW NEW NEW ASCII */
	faklay = fopen("example//pmctop.4.txt", "rt");
	if (!faklay)
	{
		printf("error at pmctop.4.txt!\n");
		exit(1);
	}

	fakblt = fopen("pmctop_faked.txt", "wt");
	if (!fakblt)
	{
		printf("error at pmctop_faked.txt!\n");
		exit (1);
	}

	bzero(line, sizeof(line));
	fgets(line, 1024, faklay);

/* header part */
	while (!strstr(line, "U_NAME")) /* read until were on the macro part. */
	{
		if (feof(faklay))
			break;
		fputs(line, fakblt);
		bzero(line, sizeof(line));
		fgets(line, 1024, faklay);
	}
/* done, header is done! */
	bzero(org_line, sizeof(line));
	strcpy(org_line, line);

	Pos = 1;
/* displaying the sorted by Miz (c) PMC 1997. */
	while (Pos <= Faked_User_Num)
	{
		for (x = 1; x <= maxuser; x++)
			if (userbyte_pos[x] == Pos)
			{
				if (faked_bytes[x] == 0)
					break;  /* no more guys, break the loop */

				sprintf(FK_bytes, "%llu", faked_bytes[x]);
				strcpy(FK_bytes, f_format(FK_bytes));

				sprintf(FK_files, "%llu", faked_files[x]);
				strcpy(FK_files, f_format(FK_files));

				bzero(line, sizeof(line));
				strcpy(line, org_line);
				sprintf(parsebuf, "%i", Pos);
				pmcparse(line, '@', "U_POS", parsebuf);

				sprintf(parsebuf, "%s", user_name[x]);
				pmcparse(line, '@', "U_NAME", parsebuf);

				sprintf(parsebuf, "%s", user_loct[x]);
				pmcparse(line, '@', "U_LOCAT", parsebuf);

				sprintf(parsebuf, "%s", FK_bytes);
				pmcparse(line, '@', "B_FAKED", parsebuf);

				sprintf(parsebuf, "%s", FK_files);
				pmcparse(line, '@', "F_FAKED", parsebuf);

				fputs(line, fakblt);
			}
		Pos++;
	}

	sprintf(FK_totalbytes, "%llu", Total_Faked_Bytes);
	strcpy(FK_totalbytes, f_format(FK_totalbytes));

	sprintf(FK_totalfiles, "%llu", Total_Faked_Files);
	strcpy(FK_totalfiles, f_format(FK_totalfiles));

	bzero(line, sizeof(line));
	fgets(line, 1024, faklay);
/* footer part */
	while (!feof(faklay))
	{
		Generic_Footer();

		if (strstr(line, "TOTF_BYTES"))
		{
			sprintf(parsebuf, "%s", FK_totalbytes);
			pmcparse(line, '@', "TOTF_BYTES", parsebuf);

			sprintf(parsebuf, "%s", FK_totalfiles);
			pmcparse(line, '@', "TOTF_FILES", parsebuf);

			sprintf(FK_totalbytes, "%llu", Total_Faked_Bytes / (1024 * 1024));
			strcpy(FK_totalbytes, f_format(FK_totalbytes));
			sprintf(parsebuf, "%s", FK_totalbytes);
			pmcparse(line, '@', "TOTF_MEG", parsebuf);

			sprintf(FK_totalbytes, "%llu", Total_Faked_Bytes / (1024 * 1024 * 1024));
			strcpy(FK_totalbytes, f_format(FK_totalbytes));
			sprintf(parsebuf, "%s", FK_totalbytes);
			pmcparse(line, '@', "TOTF_GIG", parsebuf);
		}

		if (feof(faklay))
			break;
		fputs(line, fakblt);
		bzero(line, sizeof(line));
		fgets(line, 1024, faklay);
	}

	fclose(fakblt);
	fclose(faklay);
	printf("Done!\n");

	return 0;
}

int DSZ_Top()
{
	FILE *dszblt;

	printf("writing Top DSZ rates... ");

	dszblt = fopen("pmctop_cpstop.gfx", "wt");
	if (!dszblt)
	{
		printf("error at pmctop_cpstop.gfx!\n");
		exit (1);
	}

	sprintf(Ccalls, "%lu", Top_Cps);
	strcpy(Ccalls, f_format(Ccalls));

	sprintf(line, "\n\n\ttop cps is: %s\n\n", Ccalls);
	fputs(line, dszblt);

	fclose(dszblt);

/* NEW NEW NEW NEW NEW NEW NEW NEW ASCII */
	dszblt = fopen("pmctop_cpstop.txt", "wt");
	if (!dszblt)
	{
		printf("error at pmctop_cpstop.txt!\n");
		exit (1);
	}

	sprintf(Ccalls, "%lu", Top_Cps);
	strcpy(Ccalls, f_format(Ccalls));

	sprintf(line, "\n\n\ttop cps is: %s\n\n", Ccalls);
	fputs(line, dszblt);

	fclose(dszblt);
	printf("Done!\n");

	return 0;
}

/* grouptop alltime bulletin */
int Grouptop_Upload_List()
{
	FILE *gublt;
	FILE *gulay;
	printf("Writing Grouptop list... ");

	PMC_Sort(GroupULBytes, userbyte_pos, maxuser);

/* read/write the damned example ansi file */
	gulay = fopen("example//pmctop.8.gfx", "rt");
	if (!gulay)
	{
		printf("error at pmctop.8.gfx!\n");
		exit(1);
	}

	gublt = fopen("pmctop_gtop_uploader.gfx", "wt");
	if (!gublt)
	{
		printf("error at pmctop_gtop_uploader.gfx!\n");
		exit (1);
	}

	bzero(line, sizeof(line));
	fgets(line, 1024, gulay);

/* header part */
	while (!strstr(line, "U_NAME")) /* read until were on the macro part. */
	{
		if (feof(gulay))
			break;
		fputs(line, gublt);
		bzero(line, sizeof(line));
		fgets(line, 1024, gulay);
	}
/* done, header is done! */
	bzero(org_line, sizeof(org_line));
	strcpy(org_line, line);

	Pos = 1;
/* displaying the sorted by Miz (c) PMC 1997. */
	while (Pos <= Groups)
	{
		for (x = 1; x <= Groups; x++)
			if (userbyte_pos[x] == Pos)
			{
				if (GroupULBytes[x] == 0)
					break;  /* no more guys, break the loop */

				sprintf(up_bytes, "%llu", GroupULBytes[x]);
				strcpy(up_bytes, f_format(up_bytes));

				sprintf(up_files, "%llu", GroupULFiles[x]);
				strcpy(up_files, f_format(up_files));

				bzero(line, sizeof(line));
				strcpy(line, org_line);
				sprintf(parsebuf, "%i", Pos); /* position to parsebuf */
				pmcparse(line, '@', "U_POS", parsebuf);

				sprintf(parsebuf, "%s", GroupName[x]); /* username to parsebuf */
				pmcparse(line, '@', "U_NAME", parsebuf);

				sprintf(parsebuf, "%s", GroupLongName[x]); /* user location for parsebuf */
				pmcparse(line, '@', "U_LOCAT", parsebuf);

				sprintf(parsebuf, "%s", up_files); /* files uploaded for parsebuf */
				pmcparse(line, '@', "U_FILES", parsebuf);

				sprintf(parsebuf, "%s", up_bytes); /* bytes uploaded for parsebuf */
				pmcparse(line, '@', "U_BYTES", parsebuf);

/*    sprintf(buf," %2i %-15s %-25s %8s %16s\n",Pos,user_name[x],user_loct[x],up_files,up_bytes); */
				fputs(line, gublt);
			}
		Pos++;
	}
	sprintf(UL_totalbytes, "%llu", UL_Total_Bytes);
	strcpy(UL_totalbytes, f_format(UL_totalbytes));
	sprintf(UL_totalfiles, "%llu", UL_Total_Files);
	strcpy(UL_totalfiles, f_format(UL_totalfiles));

	bzero(line, sizeof(line));
	fgets(line, 1024, gulay);
/* footer part */
	while (!feof(gulay))
	{
		Generic_Footer();

		if (strstr(line, "TOT_BYTES")) /* in totalbytes/megs part? */
		{
			sprintf(parsebuf, "%s", UL_totalbytes);
			pmcparse(line, '@', "TOT_BYTES", parsebuf);

			sprintf(parsebuf, "%s", UL_totalfiles);
			pmcparse(line, '@', "TOT_FILES", parsebuf);

			sprintf(UL_totalbytes, "%llu", UL_Total_Bytes / (1024 * 1024));
			strcpy(UL_totalbytes, f_format(UL_totalbytes));
			sprintf(parsebuf, "%s", UL_totalbytes);
			pmcparse(line, '@', "TOT_MEG", parsebuf);

			sprintf(UL_totalbytes, "%llu", UL_Total_Bytes / (1024 * 1024 * 1024));
			strcpy(UL_totalbytes, f_format(UL_totalbytes));
			sprintf(parsebuf, "%s", UL_totalbytes);
			pmcparse(line, '@', "TOT_GIG", parsebuf);
		}

		if (feof(gulay))
			break;
		fputs(line, gublt);
		bzero(line, sizeof(line));
		fgets(line, 1024, gulay);
	}
	fclose(gublt);
	fclose(gulay);

/* NEW NEW NEW NEW NEW NEW NEW NEW NEW NEW NEW NEW NEW NEW NEW NEW NEW NEW NEW

    fucking txt part of the shit */
/* read/write the damned example ansi file */
	gulay = fopen("example//pmctop.8.txt", "rt");
	if (!gulay)
	{
		printf("error at pmctop.8.txt!\n");
		exit(1);
	}

	gublt = fopen("pmctop_gtop_uploader.txt", "wt");
	if (!gublt)
	{
		printf("error at pmctop_gtop_uploader.txt!\n");
		exit (1);
	}

	bzero(line, sizeof(line));
	fgets(line, 1024, gulay);

/* header part */
	while (!strstr(line, "U_NAME")) /* read until were on the macro part. */
	{
		if (feof(gulay))
			break;
		fputs(line, gublt);
		bzero(line, sizeof(line));
		fgets(line, 1024, gulay);
	}
/* done, header is done! */
	bzero(org_line, sizeof(org_line));
	strcpy(org_line, line);

	Pos = 1;
/* displaying the sorted by Miz (c) PMC 1997. */
	while (Pos <= Groups)
	{
		for (x = 1; x <= Groups; x++)
			if (userbyte_pos[x] == Pos)
			{
				if (GroupULBytes[x] == 0)
					break;  /* no more guys, break the loop */

				sprintf(up_bytes, "%llu", GroupULBytes[x]);
				strcpy(up_bytes, f_format(up_bytes));

				sprintf(up_files, "%llu", GroupULFiles[x]);
				strcpy(up_files, f_format(up_files));

				bzero(line, sizeof(line));
				strcpy(line, org_line);
				sprintf(parsebuf, "%i", Pos); /* position to parsebuf */
				pmcparse(line, '@', "U_POS", parsebuf);

				sprintf(parsebuf, "%s", GroupName[x]); /* username to parsebuf */
				pmcparse(line, '@', "U_NAME", parsebuf);

				sprintf(parsebuf, "%s", GroupLongName[x]); /* user location for parsebuf */
				pmcparse(line, '@', "U_LOCAT", parsebuf);

				sprintf(parsebuf, "%s", up_files); /* files uploaded for parsebuf */
				pmcparse(line, '@', "U_FILES", parsebuf);

				sprintf(parsebuf, "%s", up_bytes); /* bytes uploaded for parsebuf */
				pmcparse(line, '@', "U_BYTES", parsebuf);

				fputs(line, gublt);
			}
		Pos++;
	}
	sprintf(UL_totalbytes, "%llu", UL_Total_Bytes);
	strcpy(UL_totalbytes, f_format(UL_totalbytes));
	sprintf(UL_totalfiles, "%llu", UL_Total_Files);
	strcpy(UL_totalfiles, f_format(UL_totalfiles));

	bzero(line, sizeof(line));
	fgets(line, 1024, gulay);
/* footer part */
	while (!feof(gulay))
	{
		Generic_Footer();

		if (strstr(line, "TOT_BYTES")) /* in totalbytes/megs part? */
		{
			sprintf(parsebuf, "%s", UL_totalbytes);
			pmcparse(line, '@', "TOT_BYTES", parsebuf);

			sprintf(parsebuf, "%s", UL_totalfiles);
			pmcparse(line, '@', "TOT_FILES", parsebuf);

			sprintf(UL_totalbytes, "%llu", UL_Total_Bytes / (1024 * 1024));
			strcpy(UL_totalbytes, f_format(UL_totalbytes));
			sprintf(parsebuf, "%s", UL_totalbytes);
			pmcparse(line, '@', "TOT_MEG", parsebuf);

			sprintf(UL_totalbytes, "%llu", UL_Total_Bytes / (1024 * 1024 * 1024));
			strcpy(UL_totalbytes, f_format(UL_totalbytes));
			sprintf(parsebuf, "%s", UL_totalbytes);
			pmcparse(line, '@', "TOT_GIG", parsebuf);
		}

		if (feof(gulay))
			break;
		fputs(line, gublt);
		bzero(line, sizeof(line));
		fgets(line, 1024, gulay);
	}
	fclose(gublt);
	fclose(gulay);
	printf("Done!\n");

	return 0;
}

/* grouptop alltime bulletin */
int Grouptop_Download_List()
{
	printf("Writing Grouptop list... ");

	PMC_Sort(GroupDLBytes, userbyte_pos, maxuser);

/* read/write the damned example ansi file */
	layout = fopen("example//pmctop.8.gfx", "rt");
	if (!layout)
	{
		printf("error at pmctop.8.gfx!\n");
		exit(1);
	}

	bulletin = fopen("pmctop_gtop_downloader.gfx", "wt");
	if (!bulletin)
	{
		printf("error at pmctop_gtop_downloader.gfx!\n");
		exit (1);
	}

	fgets(line, 1024, layout);

/* header part */
	while (!strstr(line, "U_NAME")) /* read until were on the macro part. */
	{
		if (feof(layout))
			break;
		fputs(line, bulletin);
		fgets(line, 1024, layout);
	}
/* done, header is done! */
	strcpy(org_line, line);

	Pos = 1;
/* displaying the sorted by Miz (c) PMC 1997. */
	while (Pos <= Groups)
	{
		for (x = 1; x <= Groups; x++)
			if (userbyte_pos[x] == Pos)
			{
				if (GroupDLBytes[x] == 0)
					break;  /* no more guys, break the loop */

				sprintf(up_bytes, "%llu", GroupDLBytes[x]);
				strcpy(up_bytes, f_format(up_bytes));

				sprintf(up_files, "%llu", GroupDLFiles[x]);
				strcpy(up_files, f_format(up_files));

				strcpy(line, org_line);
				sprintf(parsebuf, "%i", Pos); /* position to parsebuf */
				pmcparse(line, '@', "U_POS", parsebuf);

				sprintf(parsebuf, "%s", GroupName[x]); /* username to parsebuf */
				pmcparse(line, '@', "U_NAME", parsebuf);

				sprintf(parsebuf, "%s", GroupLongName[x]); /* user location for parsebuf */
				pmcparse(line, '@', "U_LOCAT", parsebuf);

				sprintf(parsebuf, "%s", up_files); /* files uploaded for parsebuf */
				pmcparse(line, '@', "U_FILES", parsebuf);

				sprintf(parsebuf, "%s", up_bytes); /* bytes uploaded for parsebuf */
				pmcparse(line, '@', "U_BYTES", parsebuf);

/*    sprintf(buf," %2i %-15s %-25s %8s %16s\n",Pos,user_name[x],user_loct[x],up_files,up_bytes); */
				fputs(line, bulletin);
			}
		Pos++;
	}
	sprintf(UL_totalbytes, "%llu", UL_Total_Bytes);
	strcpy(UL_totalbytes, f_format(UL_totalbytes));
	sprintf(UL_totalfiles, "%llu", UL_Total_Files);
	strcpy(UL_totalfiles, f_format(UL_totalfiles));

	fgets(line, 1024, layout);
/* footer part */
	while (!feof(layout))
	{
		Generic_Footer();

		if (strstr(line, "TOT_BYTES")) /* in totalbytes/megs part? */
		{
			sprintf(parsebuf, "%s", UL_totalbytes);
			pmcparse(line, '@', "TOT_BYTES", parsebuf);

			sprintf(parsebuf, "%s", UL_totalfiles);
			pmcparse(line, '@', "TOT_FILES", parsebuf);

			sprintf(UL_totalbytes, "%llu", UL_Total_Bytes / (1024 * 1024));
			strcpy(UL_totalbytes, f_format(UL_totalbytes));
			sprintf(parsebuf, "%s", UL_totalbytes);
			pmcparse(line, '@', "TOT_MEG", parsebuf);

			sprintf(UL_totalbytes, "%llu", UL_Total_Bytes / (1024 * 1024 * 1024));
			strcpy(UL_totalbytes, f_format(UL_totalbytes));
			sprintf(parsebuf, "%s", UL_totalbytes);
			pmcparse(line, '@', "TOT_GIG", parsebuf);
		}

		if (feof(layout))
			break;
		fputs(line, bulletin);
		fgets(line, 1024, layout);
	}
	fclose(bulletin);
	fclose(layout);

/* NEW NEW NEW NEW NEW NEW NEW NEW NEW NEW NEW NEW NEW NEW NEW NEW NEW NEW NEW

    fucking txt part of the shit */
/* read/write the damned example ansi file */
	layout = fopen("example//pmctop.8.txt", "rt");
	if (!layout)
	{
		printf("error at pmctop.8.txt!\n");
		exit(1);
	}

	bulletin = fopen("pmctop_gtop_downloader.txt", "wt");
	if (!bulletin)
	{
		printf("error at pmctop_gtop_downloader.txt!\n");
		exit (1);
	}

	fgets(line, 1024, layout);

/* header part */
	while (!strstr(line, "U_NAME")) /* read until were on the macro part. */
	{
		if (feof(layout))
			break;
		fputs(line, bulletin);
		fgets(line, 1024, layout);
	}
/* done, header is done! */
	strcpy(org_line, line);

	Pos = 1;
/* displaying the sorted by Miz (c) PMC 1997. */
	while (Pos <= Groups)
	{
		for (x = 1; x <= Groups; x++)
			if (userbyte_pos[x] == Pos)
			{
				if (GroupDLBytes[x] == 0)
					break;  /* no more guys, break the loop */

				sprintf(up_bytes, "%llu", GroupDLBytes[x]);
				strcpy(up_bytes, f_format(up_bytes));

				sprintf(up_files, "%llu", GroupDLFiles[x]);
				strcpy(up_files, f_format(up_files));

				strcpy(line, org_line);
				sprintf(parsebuf, "%i", Pos); /* position to parsebuf */
				pmcparse(line, '@', "U_POS", parsebuf);

				sprintf(parsebuf, "%s", GroupName[x]); /* username to parsebuf */
				pmcparse(line, '@', "U_NAME", parsebuf);

				sprintf(parsebuf, "%s", GroupLongName[x]); /* user location for parsebuf */
				pmcparse(line, '@', "U_LOCAT", parsebuf);

				sprintf(parsebuf, "%s", up_files); /* files uploaded for parsebuf */
				pmcparse(line, '@', "U_FILES", parsebuf);

				sprintf(parsebuf, "%s", up_bytes); /* bytes uploaded for parsebuf */
				pmcparse(line, '@', "U_BYTES", parsebuf);

				fputs(line, bulletin);
			}
		Pos++;
	}
	sprintf(UL_totalbytes, "%llu", UL_Total_Bytes);
	strcpy(UL_totalbytes, f_format(UL_totalbytes));
	sprintf(UL_totalfiles, "%llu", UL_Total_Files);
	strcpy(UL_totalfiles, f_format(UL_totalfiles));

	fgets(line, 1024, layout);
/* footer part */
	while (!feof(layout))
	{
		Generic_Footer();

		if (strstr(line, "TOT_BYTES")) /* in totalbytes/megs part? */
		{
			sprintf(parsebuf, "%s", UL_totalbytes);
			pmcparse(line, '@', "TOT_BYTES", parsebuf);

			sprintf(parsebuf, "%s", UL_totalfiles);
			pmcparse(line, '@', "TOT_FILES", parsebuf);

			sprintf(UL_totalbytes, "%llu", UL_Total_Bytes / (1024 * 1024));
			strcpy(UL_totalbytes, f_format(UL_totalbytes));
			sprintf(parsebuf, "%s", UL_totalbytes);
			pmcparse(line, '@', "TOT_MEG", parsebuf);

			sprintf(UL_totalbytes, "%llu", UL_Total_Bytes / (1024 * 1024 * 1024));
			strcpy(UL_totalbytes, f_format(UL_totalbytes));
			sprintf(parsebuf, "%s", UL_totalbytes);
			pmcparse(line, '@', "TOT_GIG", parsebuf);
		}

		if (feof(layout))
			break;
		fputs(line, bulletin);
		fgets(line, 1024, layout);
	}
	fclose(bulletin);
	fclose(layout);
	printf("Done!\n");

	return 0;
}

int grouptoppen()
{
	FILE *PCGROUP;

	char UserName[80];
	char temp[80];
	int MemNumber = 0, All_Members = 0, gi = 0;
	Groups = 0;
#ifdef DEBUG
	printf("at grouptop...\n");
#endif

	GroupWeekBytesTotal = 0;
	GroupWeekFilesTotal = 0;
	GroupMonthBytesTotal = 0;
	GroupMonthFilesTotal = 0;
	GroupYearBytesTotal = 0;
	GroupYearFilesTotal = 0;

	PCGROUP = fopen("pmcgroup.cfg", "rt");
	if (!PCGROUP)
	{
		printf("error at pmcgroup.cfg, exiting.\n");
		return 0;
	}

#ifdef DEBUG
	printf("starting looper\n");
#endif
	while (!feof(PCGROUP))
	{
		Groups++;
		fgets(temp, 80, PCGROUP);
		bzero(GroupName[Groups], sizeof(UserName));
		strncpy(GroupName[Groups], temp, strlen(temp) - 1);

		fgets(temp, 80, PCGROUP);
		bzero(GroupLongName[Groups], sizeof(GroupLongName));
		strncpy(GroupLongName[Groups], temp, strlen(temp) - 1);
#ifdef DEBUG
		printf
		        ("GROUP---name: %s\n", GroupName[Groups]);
#endif
		if (strstr(GroupName[Groups], "ends"))
			break;

		fgets(temp, 80, PCGROUP);
		MemNumber = 0;
		MemNumber = atoi(temp);
		All_Members = (All_Members + MemNumber); /* adds the number of members */
		gi = 1;

		while (gi <= MemNumber)
		{
			bzero(UserName, sizeof(UserName));
			fgets(temp, 80, PCGROUP);
			strncpy(UserName, temp, strlen(temp) - 1);
#ifdef DEBUG
			printf("UserName: %s\n", UserName);
#endif
			gi++;
			Addor_Group_Bytes(UserName);
		}

		MemNumber = 0;
	}

#ifdef DEBUG
	printf("prepping to do the group TOTAL crap\n");
#endif
	gi = 1;
	while (gi < Groups)
	{
		/* group weektop TOTAL bytes & files */
		GroupWeekBytesTotal += GroupWEEKBytes[gi];
		GroupWeekFilesTotal += GroupWEEKFiles[gi];
		/* group monthtop TOTAL bytes & files */
		GroupMonthBytesTotal += GroupMONTHBytes[gi];
		GroupMonthFilesTotal += GroupMONTHFiles[gi];
		/* group yeartop TOTAL bytes & files */
		GroupYearBytesTotal += GroupYEARBytes[gi];
		GroupYearFilesTotal += GroupYEARFiles[gi];
		gi++;
	}
#ifdef DEBUG
	printf("done the group TOTAL crap\n");
#endif

	fclose(PCGROUP);
#ifdef DEBUG
	printf("pcgroup is closed, exiting grouptoppen() NOW!\n");
#endif
	return 0;
}

int Addor_Group_Bytes(char *dude)
{
	int gg = 0;

	while (gg < maxuser) /* rewind the users until found match */
	{
		gg++;
		if (!strcasecmp(dude, user_name[gg]))
		{
#ifdef DEBUG
			printf("match: %s ==> %s\n", dude, user_name[gg]);
#endif
			GroupULBytes[Groups] += ul_bytes[gg]; /* add UL bytes to group */
			GroupULFiles[Groups] += ul_files[gg]; /* add UL files to group */
			GroupDLBytes[Groups] += dl_bytes[gg]; /* add DL bytes to group */
			GroupDLFiles[Groups] += dl_files[gg]; /* add.. well figure out */

			/* group weektop */
			GroupWEEKBytes[Groups] += week_ul_bytes[gg];
			GroupWEEKFiles[Groups] += week_ul_files[gg];

			/* group monthtop */
			GroupMONTHBytes[Groups] += month_ul_bytes[gg];
			GroupMONTHFiles[Groups] += month_ul_files[gg];

			/* group yeartop */
			GroupYEARBytes[Groups] += year_ul_bytes[gg];
			GroupYEARFiles[Groups] += year_ul_files[gg];

			break; /* abort, ready for next user SIR! */
		}
	}

	return 0;
}

int Extract_old_data(char *oldie)
{
	struct userbase ub;
	FILE *u_file;

	strcpy(buf, oldie);

	u_file = fopen(buf, "rb");
	if (!u_file)
	{
		printf("error at %s file!\n", buf);
		exit (1);
	}
#ifdef DEBUG
	printf("doing the magic: %s\n", buf);
#endif

	i = 0;
	fread(&ub, sizeof(struct userbase), 1, u_file);

	if (Exclude_Sysop)
	{
		//printf("dont use sysops account.\n");
		fread(&ub, sizeof(struct userbase), 1, u_file);
	}

	while (!feof(u_file))
	{
		i++;
		bzero(old_user_name[i], sizeof(old_user_name));
		strcpy(old_user_name[i], ub.user_handle);
		old_ul_bytes[i] = 0;
		old_ul_bytes[i] = ub.user_ulbytes;
		old_ul_files[i] = 0;
		old_ul_files[i] = ub.user_ulfiles;

		old_dl_bytes[i] = 0;
		old_dl_bytes[i] = ub.user_dlbytes;
		old_dl_files[i] = 0;
		old_dl_files[i] = ub.user_dlfiles;

		old_calls[i] = 0;
		old_calls[i] = ub.user_connections;

		old_faked_bytes[i] = 0;
		old_faked_bytes[i] = ub.user_fakedbytes;
		old_faked_files[i] = 0;
		old_faked_files[i] = ub.user_fakedfiles;
#ifdef DEBUG
		printf("GRABBER -> username: %s [%i] - bytes: %llu\n", old_user_name[i], i, old_ul_bytes[i]);
#endif

		fread(&ub, sizeof(struct userbase), 1, u_file);
	}

	old_maxuser = i;
	fclose(u_file);
	return old_maxuser;
}

int Week_Top(int jep)
{
	int px = 1;

	week_ul_total = 0;
	week_files_total = 0;

	while (px < jep)
	{
		if ((!strcasecmp(old_user_name[px], user_name[px])) && (old_ul_bytes[px] < ul_bytes[px]))
		{
/*	if (old_ul_bytes[px] < ul_bytes[px]) {*/
			week_ul_bytes[px] = 0;
			week_ul_bytes[px] = (ul_bytes[px] - old_ul_bytes[px]);
			week_ul_files[px] = 0;
			week_ul_files[px] = (ul_files[px] - old_ul_files[px]);
			week_ul_total += week_ul_bytes[px];
			week_files_total += week_ul_files[px];
		}
		px++;
	}
#ifdef DEBUG
	printf("we are at week_top() and jep is: %i\n", jep);
#endif

	if (jep < maxuser)
	{
#ifdef DEBUG
		printf("doing the JEP thing\n");
#endif
		while (jep < maxuser)
		{
#ifdef DEBUG
			printf("(wkup) username: %s [%i] - ALLTIME bytes: %llu\n", user_name[jep], jep, ul_bytes[jep]);
#endif
			week_ul_bytes[jep] = 0;
			week_ul_bytes[jep] = ul_bytes[jep];
			week_ul_files[jep] = 0;
			week_ul_files[jep] = ul_files[jep];
			week_ul_total += week_ul_bytes[jep];
			week_files_total += week_ul_files[jep];
			jep++;
		}
	}

	return 0;
}

int Month_Top(int jep)
{
	int px = 1;

	month_ul_total = 0;
	month_files_total = 0;

	while (px < jep)
	{
		if ((!strcasecmp(old_user_name[px], user_name[px])) && (old_ul_bytes[px] < ul_bytes[px]))
		{
/*	if (old_ul_bytes[px] < ul_bytes[px]) {*/
			month_ul_bytes[px] = 0;
			month_ul_bytes[px] = (ul_bytes[px] - old_ul_bytes[px]);
			month_ul_files[px] = 0;
			month_ul_files[px] = (ul_files[px] - old_ul_files[px]);
			month_ul_total += month_ul_bytes[px];
			month_files_total += month_ul_files[px];
		}
		px++;
	}
#ifdef DEBUG
	printf("we are at month_top() and jep is: %i\n", jep);
#endif
	if (jep < maxuser)
	{
#ifdef DEBUG
		printf("doing the JEP thing\n");
#endif
		while (jep < maxuser)
		{
#ifdef DEBUG
			printf("(mup) username: %s [%i] - ALLTIME bytes %llu\n", user_name[jep], jep, ul_bytes[jep]);
#endif
			month_ul_bytes[jep] = 0;
			month_ul_bytes[jep] = ul_bytes[jep];
			month_ul_files[jep] = 0;
			month_ul_files[jep] = ul_files[jep];
			month_ul_total += month_ul_bytes[jep];
			month_files_total += month_ul_files[jep];
			jep++;
		}
	}

	return 0;
}

int Year_Top(int jep)
{
	int px = 1;

	year_ul_total = 0;
	year_files_total = 0;

	while (px < jep)
	{
		if ((!strcasecmp(old_user_name[px], user_name[px])) && (old_ul_bytes[px] < ul_bytes[px]))
		{
			year_ul_bytes[px] = 0;
			year_ul_bytes[px] = (ul_bytes[px] - old_ul_bytes[px]);
			year_ul_files[px] = 0;
			year_ul_files[px] = (ul_files[px] - old_ul_files[px]);
			year_ul_total += year_ul_bytes[px];
			year_files_total += year_ul_files[px];
		}
		px++;
	}
#ifdef DEBUG
	printf("we are at year_top() and jep is: %i\n", jep);
#endif

	if (jep < maxuser)
	{
#ifdef DEBUG
		printf("doing the JEP thing ********************************************\n");
#endif
		while (jep < maxuser)
		{
#ifdef DEBUG
			printf("(yr) username: %s [%i] - ALLTIME bytes %llu\n", user_name[jep], jep, ul_bytes[jep]);
#endif
			year_ul_bytes[jep] = 0;
			year_ul_bytes[jep] = ul_bytes[jep];
			year_ul_files[jep] = 0;
			year_ul_files[jep] = ul_files[jep];
			year_ul_total += year_ul_bytes[jep];
			year_files_total += year_ul_files[jep];
			jep++;
		}
	}

#ifdef DEBUG
	px = 1;
	printf("yep one more check, the while 0 < maxuser for yearguys\n");
	while (px < maxuser)
	{
		printf("username: %s [%i] - bytes: %llu\n", user_name[px], px, year_ul_bytes[px]);
		px++;
	}
#endif

	return 0;
}

/* creation of UPLOAD bulletin */
int WEEK_Upload_List()
{
	FILE *wkuplay;
	FILE *wkupblt;

	printf("Writing WeekTop Uploaders list... ");
#ifdef DEBUG
	printf("at WEEK_Upload_List(), starting sorting loop!\n");
#endif

	PMC_Sort(week_ul_bytes, userbyte_pos, maxuser);

#ifdef DEBUG
	printf("ok done, now opening pmctop.5.gfx file\n");
#endif
/* read/write the damned example ansi file */
	wkuplay = fopen("example//pmctop.5.gfx", "rt");
	if (!wkuplay)
	{
		printf("error at pmctop.5.gfx!\n");
		exit(1);
	}
#ifdef DEBUG
	printf("okay pmctop.5.gfx is now OPEN... hmm, weirdos people\n");
#endif

	wkupblt = fopen("pmctop_WEEK_uploader.gfx", "wt");
#ifdef DEBUG
	printf("attempted we have... sigh!\n");
#endif
	if (!wkupblt)
	{
		printf("error at pmctop_WEEK_uploader.gfx!\n");
		exit (1);
	}

#ifdef DEBUG
	printf("starting to read into the LINE, stuff from layout\n");
#endif
	bzero(line, sizeof(line));
	fgets(line, 1024, wkuplay);

	sprintf(UL_totalbytes, "%llu", week_ul_total);
	strcpy(UL_totalbytes, f_format(UL_totalbytes));
	sprintf(UL_totalfiles, "%llu", week_files_total);
	strcpy(UL_totalfiles, f_format(UL_totalfiles));

/* header part */
	while (!strstr(line, "U_NAME")) /* read until were on the macro part. */
	{
		if (feof(wkuplay))
			break;
		fputs(line, wkupblt);
		bzero(line, sizeof(line));
		fgets(line, 1024, wkuplay);
	}
/* done, header is done! */
	bzero(org_line, sizeof(org_line));
	strcpy(org_line, line);

	Pos = 1;
/* displaying the sorted by Miz (c) PMC 1997. */
	while (Pos <= UL_User_Num)
	{
		for (x = 1; x <= maxuser; x++)
			if (userbyte_pos[x] == Pos)
			{
				if (week_ul_bytes[x] == 0)
					break;  /* no more guys, break the loop */

				sprintf(up_bytes, "%llu", week_ul_bytes[x]);
				strcpy(up_bytes, f_format(up_bytes));

				sprintf(up_files, "%llu", week_ul_files[x]);
				strcpy(up_files, f_format(up_files));

				strcpy(line, org_line);
				sprintf(parsebuf, "%i", Pos); /* position to parsebuf */
				pmcparse(line, '@', "U_POS", parsebuf);

				sprintf(parsebuf, "%s", user_name[x]); /* username to parsebuf */
				pmcparse(line, '@', "U_NAME", parsebuf);

				sprintf(parsebuf, "%s", user_loct[x]); /* user location for parsebuf */
				pmcparse(line, '@', "U_LOCAT", parsebuf);

				sprintf(parsebuf, "%s", up_files); /* files uploaded for parsebuf */
				pmcparse(line, '@', "U_FILES", parsebuf);

				sprintf(parsebuf, "%s", up_bytes); /* bytes uploaded for parsebuf */
				pmcparse(line, '@', "U_BYTES", parsebuf);

/*    sprintf(buf," %2i %-15s %-25s %8s %16s\n",Pos,user_name[x],user_loct[x],up_files,up_bytes); */
				fputs(line, wkupblt);
				if (weekcheck)
					DataBase_EEEK("week", user_name[x], user_loct[x], up_bytes, up_files, UL_totalbytes, UL_totalfiles);
			}
		Pos++;
	}
/*
    sprintf(UL_totalbytes,"%llu",week_ul_total);
    strcpy(UL_totalbytes,f_format(UL_totalbytes));
    sprintf(UL_totalfiles,"%llu",week_files_total);
    strcpy(UL_totalfiles,f_format(UL_totalfiles));
 */

	bzero(line, sizeof(line));
	fgets(line, 1024, wkuplay);
/* footer part */
	while (!feof(wkuplay))
	{
		Generic_Footer();

		if (strstr(line, "TOT_BYTES")) /* in totalbytes/megs part? */
		{
			sprintf(parsebuf, "%s", UL_totalbytes);
			pmcparse(line, '@', "TOT_BYTES", parsebuf);

			sprintf(parsebuf, "%s", UL_totalfiles);
			pmcparse(line, '@', "TOT_FILES", parsebuf);

			sprintf(UL_totalbytes, "%llu", week_ul_total / (1024 * 1024));
			strcpy(UL_totalbytes, f_format(UL_totalbytes));
			sprintf(parsebuf, "%s", UL_totalbytes);
			pmcparse(line, '@', "TOT_MEG", parsebuf);

			sprintf(UL_totalbytes, "%llu", week_ul_total / (1024 * 1024 * 1024));
			strcpy(UL_totalbytes, f_format(UL_totalbytes));
			sprintf(parsebuf, "%s", UL_totalbytes);
			pmcparse(line, '@', "TOT_GIG", parsebuf);
		}

		if (feof(wkuplay))
			break;
		fputs(line, wkupblt);
		bzero(line, sizeof(line));
		fgets(line, 1024, wkuplay);
	}
	fclose(wkupblt);
	fclose(wkuplay);

/* NEW NEW NEW NEW NEW NEW NEW NEW NEW NEW NEW NEW NEW NEW NEW NEW NEW NEW NEW

    fucking txt part of the shit */
/* read/write the damned example ansi file */
	wkuplay = fopen("example//pmctop.5.txt", "rt");
	if (!wkuplay)
	{
		printf("error at pmctop.5.txt!\n");
		exit(1);
	}

	wkupblt = fopen("pmctop_WEEK_uploader.txt", "wt");
	if (!wkupblt)
	{
		printf("error at pmctop_WEEK_uploader.txt!\n");
		exit (1);
	}

	bzero(line, sizeof(line));
	fgets(line, 1024, wkuplay);

/* header part */
	while (!strstr(line, "U_NAME")) /* read until were on the macro part. */
	{
		if (feof(wkuplay))
			break;
		fputs(line, wkupblt);
		bzero(line, sizeof(line));
		fgets(line, 1024, wkuplay);
	}
/* done, header is done! */
	bzero(org_line, sizeof(org_line));
	strcpy(org_line, line);

	Pos = 1;
/* displaying the sorted by Miz (c) PMC 1997. */
	while (Pos <= UL_User_Num)
	{
		for (x = 1; x <= maxuser; x++)
			if (userbyte_pos[x] == Pos)
			{
				if (week_ul_bytes[x] == 0)
					break;  /* no more guys, break the loop */

				sprintf(up_bytes, "%llu", week_ul_bytes[x]);
				strcpy(up_bytes, f_format(up_bytes));

				sprintf(up_files, "%llu", week_ul_files[x]);
				strcpy(up_files, f_format(up_files));

				strcpy(line, org_line);
				sprintf(parsebuf, "%i", Pos); /* position to parsebuf */
				pmcparse(line, '@', "U_POS", parsebuf);

				sprintf(parsebuf, "%s", user_name[x]); /* username to parsebuf */
				pmcparse(line, '@', "U_NAME", parsebuf);

				sprintf(parsebuf, "%s", user_loct[x]); /* user location for parsebuf */
				pmcparse(line, '@', "U_LOCAT", parsebuf);

				sprintf(parsebuf, "%s", up_files); /* files uploaded for parsebuf */
				pmcparse(line, '@', "U_FILES", parsebuf);

				sprintf(parsebuf, "%s", up_bytes); /* bytes uploaded for parsebuf */
				pmcparse(line, '@', "U_BYTES", parsebuf);

				fputs(line, wkupblt);
			}
		Pos++;
	}
	sprintf(UL_totalbytes, "%llu", week_ul_total);
	strcpy(UL_totalbytes, f_format(UL_totalbytes));
	sprintf(UL_totalfiles, "%llu", week_files_total);
	strcpy(UL_totalfiles, f_format(UL_totalfiles));

	bzero(line, sizeof(line));
	fgets(line, 1024, wkuplay);
/* footer part */
	while (!feof(wkuplay))
	{
		Generic_Footer();

		if (strstr(line, "TOT_BYTES")) /* in totalbytes/megs part? */
		{
			sprintf(parsebuf, "%s", UL_totalbytes);
			pmcparse(line, '@', "TOT_BYTES", parsebuf);

			sprintf(parsebuf, "%s", UL_totalfiles);
			pmcparse(line, '@', "TOT_FILES", parsebuf);

			sprintf(UL_totalbytes, "%llu", week_ul_total / (1024 * 1024));
			strcpy(UL_totalbytes, f_format(UL_totalbytes));
			sprintf(parsebuf, "%s", UL_totalbytes);
			pmcparse(line, '@', "TOT_MEG", parsebuf);

			sprintf(UL_totalbytes, "%llu", week_ul_total / (1024 * 1024 * 1024));
			strcpy(UL_totalbytes, f_format(UL_totalbytes));
			sprintf(parsebuf, "%s", UL_totalbytes);
			pmcparse(line, '@', "TOT_GIG", parsebuf);
		}

		if (feof(wkuplay))
			break;
		fputs(line, wkupblt);
		bzero(line, sizeof(line));
		fgets(line, 1024, wkuplay);
	}
	fclose(wkupblt);
	fclose(wkuplay);
	printf("Done!\n");

	return 0;
}

/* creation of UPLOAD bulletin */
int MONTH_Upload_List()
{
	printf("Writing MonthTop Uploaders list... ");

	PMC_Sort(month_ul_bytes, userbyte_pos, maxuser);

/* read/write the damned example ansi file */
	layout = fopen("example//pmctop.6.gfx", "rt");
	if (!layout)
	{
		printf("error at pmctop.6.gfx!\n");
		exit(1);
	}

	bulletin = fopen("pmctop_MONTH_uploader.gfx", "wt");
	if (!bulletin)
	{
		printf("error at pmctop_MONTH_uploader.gfx!\n");
		exit (1);
	}

	fgets(line, 1024, layout);

	sprintf(UL_totalbytes, "%llu", month_ul_total);
	strcpy(UL_totalbytes, f_format(UL_totalbytes));
	sprintf(UL_totalfiles, "%llu", month_files_total);
	strcpy(UL_totalfiles, f_format(UL_totalfiles));

/* header part */
	while (!strstr(line, "U_NAME")) /* read until were on the macro part. */
	{
		if (feof(layout))
			break;
		fputs(line, bulletin);
		fgets(line, 1024, layout);
	}
/* done, header is done! */
	strcpy(org_line, line);

	Pos = 1;
/* displaying the sorted by Miz (c) PMC 1997. */
	while (Pos <= UL_User_Num)
	{
		for (x = 1; x <= maxuser; x++)
			if (userbyte_pos[x] == Pos)
			{
				if (month_ul_bytes[x] == 0)
					break;  /* no more guys, break the loop */

				sprintf(up_bytes, "%llu", month_ul_bytes[x]);
				strcpy(up_bytes, f_format(up_bytes));

				sprintf(up_files, "%llu", month_ul_files[x]);
				strcpy(up_files, f_format(up_files));

				strcpy(line, org_line);
				sprintf(parsebuf, "%i", Pos); /* position to parsebuf */
				pmcparse(line, '@', "U_POS", parsebuf);

				sprintf(parsebuf, "%s", user_name[x]); /* username to parsebuf */
				pmcparse(line, '@', "U_NAME", parsebuf);

				sprintf(parsebuf, "%s", user_loct[x]); /* user location for parsebuf */
				pmcparse(line, '@', "U_LOCAT", parsebuf);

				sprintf(parsebuf, "%s", up_files); /* files uploaded for parsebuf */
				pmcparse(line, '@', "U_FILES", parsebuf);

				sprintf(parsebuf, "%s", up_bytes); /* bytes uploaded for parsebuf */
				pmcparse(line, '@', "U_BYTES", parsebuf);

/*    sprintf(buf," %2i %-15s %-25s %8s %16s\n",Pos,user_name[x],user_loct[x],up_files,up_bytes); */
				fputs(line, bulletin);
				if (monthcheck)
					DataBase_EEEK("month", user_name[x], user_loct[x], up_bytes, up_files, UL_totalbytes, UL_totalfiles);
			}
		Pos++;
	}
/*
    sprintf(UL_totalbytes,"%llu",month_ul_total);
    strcpy(UL_totalbytes,f_format(UL_totalbytes));
    sprintf(UL_totalfiles,"%llu",month_files_total);
    strcpy(UL_totalfiles,f_format(UL_totalfiles));
 */

	fgets(line, 1024, layout);
/* footer part */
	while (!feof(layout))
	{
		Generic_Footer();

		if (strstr(line, "TOT_BYTES")) /* in totalbytes/megs part? */
		{
			sprintf(parsebuf, "%s", UL_totalbytes);
			pmcparse(line, '@', "TOT_BYTES", parsebuf);

			sprintf(parsebuf, "%s", UL_totalfiles);
			pmcparse(line, '@', "TOT_FILES", parsebuf);

			sprintf(UL_totalbytes, "%llu", month_ul_total / (1024 * 1024));
			strcpy(UL_totalbytes, f_format(UL_totalbytes));
			sprintf(parsebuf, "%s", UL_totalbytes);
			pmcparse(line, '@', "TOT_MEG", parsebuf);

			sprintf(UL_totalbytes, "%llu", month_ul_total / (1024 * 1024 * 1024));
			strcpy(UL_totalbytes, f_format(UL_totalbytes));
			sprintf(parsebuf, "%s", UL_totalbytes);
			pmcparse(line, '@', "TOT_GIG", parsebuf);
		}

		if (feof(layout))
			break;
		fputs(line, bulletin);
		fgets(line, 1024, layout);
	}
	fclose(bulletin);
	fclose(layout);

/* NEW NEW NEW NEW NEW NEW NEW NEW NEW NEW NEW NEW NEW NEW NEW NEW NEW NEW NEW

    fucking txt part of the shit */
/* read/write the damned example ansi file */
	layout = fopen("example//pmctop.6.txt", "rt");
	if (!layout)
	{
		printf("error at pmctop.6.txt!\n");
		exit(1);
	}

	bulletin = fopen("pmctop_MONTH_uploader.txt", "wt");
	if (!bulletin)
	{
		printf("error at pmctop_MONTH_uploader.txt!\n");
		exit (1);
	}

	fgets(line, 1024, layout);

/* header part */
	while (!strstr(line, "U_NAME")) /* read until were on the macro part. */
	{
		if (feof(layout))
			break;
		fputs(line, bulletin);
		fgets(line, 1024, layout);
	}
/* done, header is done! */
	strcpy(org_line, line);

	Pos = 1;
/* displaying the sorted by Miz (c) PMC 1997. */
	while (Pos <= UL_User_Num)
	{
		for (x = 1; x <= maxuser; x++)
			if (userbyte_pos[x] == Pos)
			{
				if (month_ul_bytes[x] == 0)
					break;  /* no more guys, break the loop */

				sprintf(up_bytes, "%llu", month_ul_bytes[x]);
				strcpy(up_bytes, f_format(up_bytes));

				sprintf(up_files, "%llu", month_ul_files[x]);
				strcpy(up_files, f_format(up_files));

				strcpy(line, org_line);
				sprintf(parsebuf, "%i", Pos); /* position to parsebuf */
				pmcparse(line, '@', "U_POS", parsebuf);

				sprintf(parsebuf, "%s", user_name[x]); /* username to parsebuf */
				pmcparse(line, '@', "U_NAME", parsebuf);

				sprintf(parsebuf, "%s", user_loct[x]); /* user location for parsebuf */
				pmcparse(line, '@', "U_LOCAT", parsebuf);

				sprintf(parsebuf, "%s", up_files); /* files uploaded for parsebuf */
				pmcparse(line, '@', "U_FILES", parsebuf);

				sprintf(parsebuf, "%s", up_bytes); /* bytes uploaded for parsebuf */
				pmcparse(line, '@', "U_BYTES", parsebuf);

				fputs(line, bulletin);
			}
		Pos++;
	}
	sprintf(UL_totalbytes, "%llu", month_ul_total);
	strcpy(UL_totalbytes, f_format(UL_totalbytes));
	sprintf(UL_totalfiles, "%llu", month_files_total);
	strcpy(UL_totalfiles, f_format(UL_totalfiles));

	fgets(line, 1024, layout);
/* footer part */
	while (!feof(layout))
	{
		Generic_Footer();

		if (strstr(line, "TOT_BYTES")) /* in totalbytes/megs part? */
		{
			sprintf(parsebuf, "%s", UL_totalbytes);
			pmcparse(line, '@', "TOT_BYTES", parsebuf);

			sprintf(parsebuf, "%s", UL_totalfiles);
			pmcparse(line, '@', "TOT_FILES", parsebuf);

			sprintf(UL_totalbytes, "%llu", month_ul_total / (1024 * 1024));
			strcpy(UL_totalbytes, f_format(UL_totalbytes));
			sprintf(parsebuf, "%s", UL_totalbytes);
			pmcparse(line, '@', "TOT_MEG", parsebuf);

			sprintf(UL_totalbytes, "%llu", month_ul_total / (1024 * 1024 * 1024));
			strcpy(UL_totalbytes, f_format(UL_totalbytes));
			sprintf(parsebuf, "%s", UL_totalbytes);
			pmcparse(line, '@', "TOT_GIG", parsebuf);
		}

		if (feof(layout))
			break;
		fputs(line, bulletin);
		fgets(line, 1024, layout);
	}
	fclose(bulletin);
	fclose(layout);
	printf("Done!\n");

	return 0;
}

int YEAR_Upload_List()
{
	printf("Writing YearTop Uploaders list... ");

	PMC_Sort(year_ul_bytes, userbyte_pos, maxuser);

/* read/write the damned example ansi file */
	layout = fopen("example//pmctop.7.gfx", "rt");
	if (!layout)
	{
		printf("error at pmctop.7.gfx!\n");
		exit(1);
	}

	bulletin = fopen("pmctop_YEAR_uploader.gfx", "wt");
	if (!bulletin)
	{
		printf("error at pmctop_YEAR_uploader.gfx!\n");
		exit (1);
	}

	fgets(line, 1024, layout);

	sprintf(UL_totalbytes, "%llu", year_ul_total);
	strcpy(UL_totalbytes, f_format(UL_totalbytes));
	sprintf(UL_totalfiles, "%llu", year_files_total);
	strcpy(UL_totalfiles, f_format(UL_totalfiles));

/* header part */
	while (!strstr(line, "U_NAME")) /* read until were on the macro part. */
	{
		if (feof(layout))
			break;
		fputs(line, bulletin);
		fgets(line, 1024, layout);
	}
/* done, header is done! */
	strcpy(org_line, line);

	Pos = 1;
/* displaying the sorted by Miz (c) PMC 1997. */
	while (Pos <= UL_User_Num)
	{
		for (x = 1; x <= maxuser; x++)
			if (userbyte_pos[x] == Pos)
			{
				if (year_ul_bytes[x] == 0)
					break;  /* no more guys, break the loop */

				sprintf(up_bytes, "%llu", year_ul_bytes[x]);
				strcpy(up_bytes, f_format(up_bytes));

				sprintf(up_files, "%llu", year_ul_files[x]);
				strcpy(up_files, f_format(up_files));

				strcpy(line, org_line);
				sprintf(parsebuf, "%i", Pos); /* position to parsebuf */
				pmcparse(line, '@', "U_POS", parsebuf);

				sprintf(parsebuf, "%s", user_name[x]); /* username to parsebuf */
				pmcparse(line, '@', "U_NAME", parsebuf);

				sprintf(parsebuf, "%s", user_loct[x]); /* user location for parsebuf */
				pmcparse(line, '@', "U_LOCAT", parsebuf);

				sprintf(parsebuf, "%s", up_files); /* files uploaded for parsebuf */
				pmcparse(line, '@', "U_FILES", parsebuf);

				sprintf(parsebuf, "%s", up_bytes); /* bytes uploaded for parsebuf */
				pmcparse(line, '@', "U_BYTES", parsebuf);

/*    sprintf(buf," %2i %-15s %-25s %8s %16s\n",Pos,user_name[x],user_loct[x],up_files,up_bytes); */
				fputs(line, bulletin);
				if (yearcheck)
					DataBase_EEEK("year", user_name[x], user_loct[x], up_bytes, up_files, UL_totalbytes, UL_totalfiles);
			}
		Pos++;
	}
/*
    sprintf(UL_totalbytes,"%llu",year_ul_total);
    strcpy(UL_totalbytes,f_format(UL_totalbytes));
    sprintf(UL_totalfiles,"%llu",year_files_total);
    strcpy(UL_totalfiles,f_format(UL_totalfiles));
 */

	fgets(line, 1024, layout);
/* footer part */
	while (!feof(layout))
	{
		Generic_Footer();

		if (strstr(line, "TOT_BYTES")) /* in totalbytes/megs part? */
		{
			sprintf(parsebuf, "%s", UL_totalbytes);
			pmcparse(line, '@', "TOT_BYTES", parsebuf);

			sprintf(parsebuf, "%s", UL_totalfiles);
			pmcparse(line, '@', "TOT_FILES", parsebuf);

			sprintf(UL_totalbytes, "%llu", year_ul_total / (1024 * 1024));
			strcpy(UL_totalbytes, f_format(UL_totalbytes));
			sprintf(parsebuf, "%s", UL_totalbytes);
			pmcparse(line, '@', "TOT_MEG", parsebuf);

			sprintf(UL_totalbytes, "%llu", year_ul_total / (1024 * 1024 * 1024));
			strcpy(UL_totalbytes, f_format(UL_totalbytes));
			sprintf(parsebuf, "%s", UL_totalbytes);
			pmcparse(line, '@', "TOT_GIG", parsebuf);
		}

		if (feof(layout))
			break;
		fputs(line, bulletin);
		fgets(line, 1024, layout);
	}
	fclose(bulletin);
	fclose(layout);

/* NEW NEW NEW NEW NEW NEW NEW NEW NEW NEW NEW NEW NEW NEW NEW NEW NEW NEW NEW

    fucking txt part of the shit */
/* read/write the damned example ansi file */
	layout = fopen("example//pmctop.7.txt", "rt");
	if (!layout)
	{
		printf("error at pmctop.7.txt!\n");
		exit(1);
	}

	bulletin = fopen("pmctop_YEAR_uploader.txt", "wt");
	if (!bulletin)
	{
		printf("error at pmctop_YEAR_uploader.txt!\n");
		exit (1);
	}

	fgets(line, 1024, layout);

/* header part */
	while (!strstr(line, "U_NAME")) /* read until were on the macro part. */
	{
		if (feof(layout))
			break;
		fputs(line, bulletin);
		fgets(line, 1024, layout);
	}
/* done, header is done! */
	strcpy(org_line, line);

	Pos = 1;
/* displaying the sorted by Miz (c) PMC 1997. */
	while (Pos <= UL_User_Num)
	{
		for (x = 1; x <= maxuser; x++)
			if (userbyte_pos[x] == Pos)
			{
				if (year_ul_bytes[x] == 0)
					break;  /* no more guys, break the loop */

				sprintf(up_bytes, "%llu", year_ul_bytes[x]);
				strcpy(up_bytes, f_format(up_bytes));

				sprintf(up_files, "%llu", year_ul_files[x]);
				strcpy(up_files, f_format(up_files));

				strcpy(line, org_line);
				sprintf(parsebuf, "%i", Pos); /* position to parsebuf */
				pmcparse(line, '@', "U_POS", parsebuf);

				sprintf(parsebuf, "%s", user_name[x]); /* username to parsebuf */
				pmcparse(line, '@', "U_NAME", parsebuf);

				sprintf(parsebuf, "%s", user_loct[x]); /* user location for parsebuf */
				pmcparse(line, '@', "U_LOCAT", parsebuf);

				sprintf(parsebuf, "%s", up_files); /* files uploaded for parsebuf */
				pmcparse(line, '@', "U_FILES", parsebuf);

				sprintf(parsebuf, "%s", up_bytes); /* bytes uploaded for parsebuf */
				pmcparse(line, '@', "U_BYTES", parsebuf);

				fputs(line, bulletin);
			}
		Pos++;
	}
	sprintf(UL_totalbytes, "%llu", year_ul_total);
	strcpy(UL_totalbytes, f_format(UL_totalbytes));
	sprintf(UL_totalfiles, "%llu", year_files_total);
	strcpy(UL_totalfiles, f_format(UL_totalfiles));

	fgets(line, 1024, layout);
/* footer part */
	while (!feof(layout))
	{
		Generic_Footer();

		if (strstr(line, "TOT_BYTES")) /* in totalbytes/megs part? */
		{
			sprintf(parsebuf, "%s", UL_totalbytes);
			pmcparse(line, '@', "TOT_BYTES", parsebuf);

			sprintf(parsebuf, "%s", UL_totalfiles);
			pmcparse(line, '@', "TOT_FILES", parsebuf);

			sprintf(UL_totalbytes, "%llu", year_ul_total / (1024 * 1024));
			strcpy(UL_totalbytes, f_format(UL_totalbytes));
			sprintf(parsebuf, "%s", UL_totalbytes);
			pmcparse(line, '@', "TOT_MEG", parsebuf);

			sprintf(UL_totalbytes, "%llu", year_ul_total / (1024 * 1024 * 1024));
			strcpy(UL_totalbytes, f_format(UL_totalbytes));
			sprintf(parsebuf, "%s", UL_totalbytes);
			pmcparse(line, '@', "TOT_GIG", parsebuf);
		}

		if (feof(layout))
			break;
		fputs(line, bulletin);
		fgets(line, 1024, layout);
	}
	fclose(bulletin);
	fclose(layout);
	printf("Done!\n");

	return 0;
}

int sumThing(char *thingy)
{
	int xx = 1;
	printf("********* IN STEREO ********** %s\n", thingy);

	while (xx < maxuser)
	{
		printf("old_user[%i]: %s\n", xx, old_user_name[xx]);
		xx++;
	}

	return 0;
}

/*
   int addor_me_newbie()
   {
   int px=1;


        week_ul_bytes[px]=0;
        week_ul_bytes[px]=(ul_bytes[px]-old_ul_bytes[px]);
        week_ul_files[px]=0;
        week_ul_files[px]=(ul_files[px]-old_ul_files[px]);
        week_ul_total += week_ul_bytes[px];
        week_files_total += week_ul_files[px];

    return 0;
   }
 */

/* grouptop weektop bulletin */
int Grouptop_WEEKTOP_Upload_List()
{
	printf("Writing Group WEEKTOP list... ");

	PMC_Sort(GroupWEEKBytes, userbyte_pos, maxuser);

/* read/write the damned example ansi file */
	layout = fopen("example//pmctop.9.gfx", "rt");
	if (!layout)
	{
		printf("error at pmctop.8.gfx!\n");
		exit(1);
	}

	bulletin = fopen("pmctop_gtopweek_uploader.gfx", "wt");
	if (!bulletin)
	{
		printf("error at pmctop_gtopweek_uploader.gfx!\n");
		exit (1);
	}

	fgets(line, 1024, layout);

/* header part */
	while (!strstr(line, "U_NAME")) /* read until were on the macro part. */
	{
		if (feof(layout))
			break;
		fputs(line, bulletin);
		fgets(line, 1024, layout);
	}
/* done, header is done! */
	strcpy(org_line, line);

	Pos = 1;
/* displaying the sorted by Miz (c) PMC 1997. */
	while (Pos <= Groups)
	{
		for (x = 1; x <= Groups; x++)
			if (userbyte_pos[x] == Pos)
			{
				if (GroupWEEKBytes[x] == 0)
					break;  /* no more guys, break the loop */

				sprintf(up_bytes, "%llu", GroupWEEKBytes[x]);
				strcpy(up_bytes, f_format(up_bytes));

				sprintf(up_files, "%llu", GroupWEEKFiles[x]);
				strcpy(up_files, f_format(up_files));

				strcpy(line, org_line);
				sprintf(parsebuf, "%i", Pos); /* position to parsebuf */
				pmcparse(line, '@', "U_POS", parsebuf);

				sprintf(parsebuf, "%s", GroupName[x]); /* username to parsebuf */
				pmcparse(line, '@', "U_NAME", parsebuf);

				sprintf(parsebuf, "%s", GroupLongName[x]); /* user location for parsebuf */
				pmcparse(line, '@', "U_LOCAT", parsebuf);

				sprintf(parsebuf, "%s", up_files); /* files uploaded for parsebuf */
				pmcparse(line, '@', "U_FILES", parsebuf);

				sprintf(parsebuf, "%s", up_bytes); /* bytes uploaded for parsebuf */
				pmcparse(line, '@', "U_BYTES", parsebuf);

/*    sprintf(buf," %2i %-15s %-25s %8s %16s\n",Pos,user_name[x],user_loct[x],up_files,up_bytes); */
				fputs(line, bulletin);
			}
		Pos++;
	}

	sprintf(UL_totalbytes, "%llu", GroupWeekBytesTotal);
	strcpy(UL_totalbytes, f_format(UL_totalbytes));
	sprintf(UL_totalfiles, "%llu", GroupWeekFilesTotal);
	strcpy(UL_totalfiles, f_format(UL_totalfiles));

	fgets(line, 1024, layout);
/* footer part */
	while (!feof(layout))
	{
		Generic_Footer();

		if (strstr(line, "TOT_BYTES")) /* in totalbytes/megs part? */
		{
			sprintf(parsebuf, "%s", UL_totalbytes);
			pmcparse(line, '@', "TOT_BYTES", parsebuf);

			sprintf(parsebuf, "%s", UL_totalfiles);
			pmcparse(line, '@', "TOT_FILES", parsebuf);

			sprintf(UL_totalbytes, "%llu", GroupWeekBytesTotal / (1024 * 1024));
			strcpy(UL_totalbytes, f_format(UL_totalbytes));
			sprintf(parsebuf, "%s", UL_totalbytes);
			pmcparse(line, '@', "TOT_MEG", parsebuf);

			sprintf(UL_totalbytes, "%llu", GroupWeekBytesTotal / (1024 * 1024 * 1024));
			strcpy(UL_totalbytes, f_format(UL_totalbytes));
			sprintf(parsebuf, "%s", UL_totalbytes);
			pmcparse(line, '@', "TOT_GIG", parsebuf);
		}

		if (feof(layout))
			break;
		fputs(line, bulletin);
		fgets(line, 1024, layout);
	}
	fclose(bulletin);
	fclose(layout);

/* NEW NEW NEW NEW NEW NEW NEW NEW NEW NEW NEW NEW NEW NEW NEW NEW NEW NEW NEW

    fucking txt part of the shit */
/* read/write the damned example ansi file */
	layout = fopen("example//pmctop.9.txt", "rt");
	if (!layout)
	{
		printf("error at pmctop.9.txt!\n");
		exit(1);
	}

	bulletin = fopen("pmctop_gtopweek_uploader.txt", "wt");
	if (!bulletin)
	{
		printf("error at pmctop_gtopweek_uploader.txt!\n");
		exit (1);
	}

	fgets(line, 1024, layout);

/* header part */
	while (!strstr(line, "U_NAME")) /* read until were on the macro part. */
	{
		if (feof(layout))
			break;
		fputs(line, bulletin);
		fgets(line, 1024, layout);
	}
/* done, header is done! */
	strcpy(org_line, line);

	Pos = 1;
/* displaying the sorted by Miz (c) PMC 1997. */
	while (Pos <= Groups)
	{
		for (x = 1; x <= Groups; x++)
			if (userbyte_pos[x] == Pos)
			{
				if (GroupWEEKBytes[x] == 0)
					break;  /* no more guys, break the loop */

				sprintf(up_bytes, "%llu", GroupWEEKBytes[x]);
				strcpy(up_bytes, f_format(up_bytes));

				sprintf(up_files, "%llu", GroupWEEKFiles[x]);
				strcpy(up_files, f_format(up_files));

				strcpy(line, org_line);
				sprintf(parsebuf, "%i", Pos); /* position to parsebuf */
				pmcparse(line, '@', "U_POS", parsebuf);

				sprintf(parsebuf, "%s", GroupName[x]); /* username to parsebuf */
				pmcparse(line, '@', "U_NAME", parsebuf);

				sprintf(parsebuf, "%s", GroupLongName[x]); /* user location for parsebuf */
				pmcparse(line, '@', "U_LOCAT", parsebuf);

				sprintf(parsebuf, "%s", up_files); /* files uploaded for parsebuf */
				pmcparse(line, '@', "U_FILES", parsebuf);

				sprintf(parsebuf, "%s", up_bytes); /* bytes uploaded for parsebuf */
				pmcparse(line, '@', "U_BYTES", parsebuf);

				fputs(line, bulletin);
			}
		Pos++;
	}
	sprintf(UL_totalbytes, "%llu", GroupWeekBytesTotal);
	strcpy(UL_totalbytes, f_format(UL_totalbytes));
	sprintf(UL_totalfiles, "%llu", GroupWeekFilesTotal);
	strcpy(UL_totalfiles, f_format(UL_totalfiles));

	fgets(line, 1024, layout);
/* footer part */
	while (!feof(layout))
	{
		Generic_Footer();

		if (strstr(line, "TOT_BYTES")) /* in totalbytes/megs part? */
		{
			sprintf(parsebuf, "%s", UL_totalbytes);
			pmcparse(line, '@', "TOT_BYTES", parsebuf);

			sprintf(parsebuf, "%s", UL_totalfiles);
			pmcparse(line, '@', "TOT_FILES", parsebuf);

			sprintf(UL_totalbytes, "%llu", GroupWeekBytesTotal / (1024 * 1024));
			strcpy(UL_totalbytes, f_format(UL_totalbytes));
			sprintf(parsebuf, "%s", UL_totalbytes);
			pmcparse(line, '@', "TOT_MEG", parsebuf);

			sprintf(UL_totalbytes, "%llu", GroupWeekBytesTotal / (1024 * 1024 * 1024));
			strcpy(UL_totalbytes, f_format(UL_totalbytes));
			sprintf(parsebuf, "%s", UL_totalbytes);
			pmcparse(line, '@', "TOT_GIG", parsebuf);
		}

		if (feof(layout))
			break;
		fputs(line, bulletin);
		fgets(line, 1024, layout);
	}
	fclose(bulletin);
	fclose(layout);
	printf("Done!\n");

	return 0;
}

/* grouptop monthtop bulletin */
int Grouptop_MONTHTOP_Upload_List()
{
	printf("Writing Group MONTHTOP list... ");

	PMC_Sort(GroupMONTHBytes, userbyte_pos, maxuser);

/* read/write the damned example ansi file */
	layout = fopen("example//pmctop.10.gfx", "rt");
	if (!layout)
	{
		printf("error at pmctop.10.gfx!\n");
		exit(1);
	}

	bulletin = fopen("pmctop_gtopmonth_uploader.gfx", "wt");
	if (!bulletin)
	{
		printf("error at pmctop_gtopmonth_uploader.gfx!\n");
		exit (1);
	}

	fgets(line, 1024, layout);

/* header part */
	while (!strstr(line, "U_NAME")) /* read until were on the macro part. */
	{
		if (feof(layout))
			break;
		fputs(line, bulletin);
		fgets(line, 1024, layout);
	}
/* done, header is done! */
	strcpy(org_line, line);

	Pos = 1;
/* displaying the sorted by Miz (c) PMC 1997. */
	while (Pos <= Groups)
	{
		for (x = 1; x <= Groups; x++)
			if (userbyte_pos[x] == Pos)
			{
				if (GroupMONTHBytes[x] == 0)
					break;  /* no more guys, break the loop */

				sprintf(up_bytes, "%llu", GroupMONTHBytes[x]);
				strcpy(up_bytes, f_format(up_bytes));

				sprintf(up_files, "%llu", GroupMONTHFiles[x]);
				strcpy(up_files, f_format(up_files));

				strcpy(line, org_line);
				sprintf(parsebuf, "%i", Pos); /* position to parsebuf */
				pmcparse(line, '@', "U_POS", parsebuf);

				sprintf(parsebuf, "%s", GroupName[x]); /* username to parsebuf */
				pmcparse(line, '@', "U_NAME", parsebuf);

				sprintf(parsebuf, "%s", GroupLongName[x]); /* user location for parsebuf */
				pmcparse(line, '@', "U_LOCAT", parsebuf);

				sprintf(parsebuf, "%s", up_files); /* files uploaded for parsebuf */
				pmcparse(line, '@', "U_FILES", parsebuf);

				sprintf(parsebuf, "%s", up_bytes); /* bytes uploaded for parsebuf */
				pmcparse(line, '@', "U_BYTES", parsebuf);

/*    sprintf(buf," %2i %-15s %-25s %8s %16s\n",Pos,user_name[x],user_loct[x],up_files,up_bytes); */
				fputs(line, bulletin);
			}
		Pos++;
	}

	sprintf(UL_totalbytes, "%llu", GroupMonthBytesTotal);
	strcpy(UL_totalbytes, f_format(UL_totalbytes));
	sprintf(UL_totalfiles, "%llu", GroupMonthFilesTotal);
	strcpy(UL_totalfiles, f_format(UL_totalfiles));

	fgets(line, 1024, layout);
/* footer part */
	while (!feof(layout))
	{
		Generic_Footer();

		if (strstr(line, "TOT_BYTES")) /* in totalbytes/megs part? */
		{
			sprintf(parsebuf, "%s", UL_totalbytes);
			pmcparse(line, '@', "TOT_BYTES", parsebuf);

			sprintf(parsebuf, "%s", UL_totalfiles);
			pmcparse(line, '@', "TOT_FILES", parsebuf);

			sprintf(UL_totalbytes, "%llu", GroupMonthBytesTotal / (1024 * 1024));
			strcpy(UL_totalbytes, f_format(UL_totalbytes));
			sprintf(parsebuf, "%s", UL_totalbytes);
			pmcparse(line, '@', "TOT_MEG", parsebuf);

			sprintf(UL_totalbytes, "%llu", GroupMonthBytesTotal / (1024 * 1024 * 1024));
			strcpy(UL_totalbytes, f_format(UL_totalbytes));
			sprintf(parsebuf, "%s", UL_totalbytes);
			pmcparse(line, '@', "TOT_GIG", parsebuf);
		}

		if (feof(layout))
			break;
		fputs(line, bulletin);
		fgets(line, 1024, layout);
	}
	fclose(bulletin);
	fclose(layout);

/* NEW NEW NEW NEW NEW NEW NEW NEW NEW NEW NEW NEW NEW NEW NEW NEW NEW NEW NEW

    fucking txt part of the shit */
/* read/write the damned example ansi file */
	layout = fopen("example//pmctop.10.txt", "rt");
	if (!layout)
	{
		printf("error at pmctop.10.txt!\n");
		exit(1);
	}

	bulletin = fopen("pmctop_gtopmonth_uploader.txt", "wt");
	if (!bulletin)
	{
		printf("error at pmctop_gtopmonth_uploader.txt!\n");
		exit (1);
	}

	fgets(line, 1024, layout);

/* header part */
	while (!strstr(line, "U_NAME")) /* read until were on the macro part. */
	{
		if (feof(layout))
			break;
		fputs(line, bulletin);
		fgets(line, 1024, layout);
	}
/* done, header is done! */
	strcpy(org_line, line);

	Pos = 1;
/* displaying the sorted by Miz (c) PMC 1997. */
	while (Pos <= Groups)
	{
		for (x = 1; x <= Groups; x++)
			if (userbyte_pos[x] == Pos)
			{
				if (GroupMONTHBytes[x] == 0)
					break;  /* no more guys, break the loop */

				sprintf(up_bytes, "%llu", GroupMONTHBytes[x]);
				strcpy(up_bytes, f_format(up_bytes));

				sprintf(up_files, "%llu", GroupMONTHFiles[x]);
				strcpy(up_files, f_format(up_files));

				strcpy(line, org_line);
				sprintf(parsebuf, "%i", Pos); /* position to parsebuf */
				pmcparse(line, '@', "U_POS", parsebuf);

				sprintf(parsebuf, "%s", GroupName[x]); /* username to parsebuf */
				pmcparse(line, '@', "U_NAME", parsebuf);

				sprintf(parsebuf, "%s", GroupLongName[x]); /* user location for parsebuf */
				pmcparse(line, '@', "U_LOCAT", parsebuf);

				sprintf(parsebuf, "%s", up_files); /* files uploaded for parsebuf */
				pmcparse(line, '@', "U_FILES", parsebuf);

				sprintf(parsebuf, "%s", up_bytes); /* bytes uploaded for parsebuf */
				pmcparse(line, '@', "U_BYTES", parsebuf);

				fputs(line, bulletin);
			}
		Pos++;
	}
	sprintf(UL_totalbytes, "%llu", GroupMonthBytesTotal);
	strcpy(UL_totalbytes, f_format(UL_totalbytes));
	sprintf(UL_totalfiles, "%llu", GroupMonthFilesTotal);
	strcpy(UL_totalfiles, f_format(UL_totalfiles));

	fgets(line, 1024, layout);
/* footer part */
	while (!feof(layout))
	{
		Generic_Footer();

		if (strstr(line, "TOT_BYTES")) /* in totalbytes/megs part? */
		{
			sprintf(parsebuf, "%s", UL_totalbytes);
			pmcparse(line, '@', "TOT_BYTES", parsebuf);

			sprintf(parsebuf, "%s", UL_totalfiles);
			pmcparse(line, '@', "TOT_FILES", parsebuf);

			sprintf(UL_totalbytes, "%llu", GroupMonthBytesTotal / (1024 * 1024));
			strcpy(UL_totalbytes, f_format(UL_totalbytes));
			sprintf(parsebuf, "%s", UL_totalbytes);
			pmcparse(line, '@', "TOT_MEG", parsebuf);

			sprintf(UL_totalbytes, "%llu", GroupMonthBytesTotal / (1024 * 1024 * 1024));
			strcpy(UL_totalbytes, f_format(UL_totalbytes));
			sprintf(parsebuf, "%s", UL_totalbytes);
			pmcparse(line, '@', "TOT_GIG", parsebuf);
		}

		if (feof(layout))
			break;
		fputs(line, bulletin);
		fgets(line, 1024, layout);
	}
	fclose(bulletin);
	fclose(layout);
	printf("Done!\n");

	return 0;
}

/*

    high score crap that motherfucking dont format itself :(

 */
int Write_Highscores()
{
	FILE *highblt;
	char ptemp[80];

	highblt = fopen("highscores.dat", "wt");
	if (!highblt)
	{
		printf("error at highscores.dat!\n");
		exit (1);
	}

	bzero(parsebuf, sizeof(parsebuf));
	bzero(ptemp, sizeof(ptemp));
	sprintf(parsebuf, "%llu", HS_week_UL_User);
	strcpy(ptemp, f_format(parsebuf));

	fputs(HS_week_UL_Username, highblt); /* week username */
	fputs("\n", highblt);
	fputs(ptemp, highblt);          /* week ul bytes */
	fputs("\n", highblt);
	fputs(HS_week_UL_Date, highblt); /* week ul date  */
	fputs("\n", highblt);

	bzero(parsebuf, sizeof(parsebuf));
	bzero(ptemp, sizeof(ptemp));
	sprintf(parsebuf, "%llu", HS_month_UL_User);
	strcpy(ptemp, f_format(parsebuf));

	fputs(HS_month_UL_Username, highblt); /* month username */
	fputs("\n", highblt);
	fputs(ptemp, highblt);          /* month ul bytes */
	fputs("\n", highblt);
	fputs(HS_month_UL_Date, highblt); /* month ul date  */
	fputs("\n", highblt);

	bzero(parsebuf, sizeof(parsebuf));
	bzero(ptemp, sizeof(ptemp));
	sprintf(parsebuf, "%llu", HS_year_UL_User);
	strcpy(ptemp, f_format(parsebuf));

	fputs(HS_year_UL_Username, highblt); /* year username */
	fputs("\n", highblt);
	fputs(ptemp, highblt);          /* year ul bytes */
	fputs("\n", highblt);
	fputs(HS_year_UL_Date, highblt); /* year ul date  */
	fputs("\n", highblt);

	bzero(parsebuf, sizeof(parsebuf));
	bzero(ptemp, sizeof(ptemp));
	sprintf(parsebuf, "%llu", HS_week_Alltime_UL);
	strcpy(ptemp, f_format(parsebuf));

	fputs(ptemp, highblt);                  /* week alltime         */
	fputs("\n", highblt);
	fputs(HS_week_Alltime_UL_Date, highblt); /* week alltime date   */
	fputs("\n", highblt);

	bzero(parsebuf, sizeof(parsebuf));
	bzero(ptemp, sizeof(ptemp));
	sprintf(parsebuf, "%llu", HS_month_Alltime_UL);
	strcpy(ptemp, f_format(parsebuf));

	fputs(ptemp, highblt);                  /* month alltime        */
	fputs("\n", highblt);
	fputs(HS_month_Alltime_UL_Date, highblt); /* month alltime date	*/
	fputs("\n", highblt);

	bzero(parsebuf, sizeof(parsebuf));
	bzero(ptemp, sizeof(ptemp));
	sprintf(parsebuf, "%llu", HS_year_Alltime_UL);
	strcpy(ptemp, f_format(parsebuf));

	fputs(ptemp, highblt);                  /* year alltime         */
	fputs("\n", highblt);
	fputs(HS_year_Alltime_UL_Date, highblt); /* year alltime date	*/
	fputs("\n", highblt);

	bzero(parsebuf, sizeof(parsebuf));
	bzero(ptemp, sizeof(ptemp));
	sprintf(parsebuf, "%llu", HS_week_UL_Group);
	strcpy(ptemp, f_format(parsebuf));

	fputs(HS_week_UL_Groupname, highblt);   /* week groupname	*/
	fputs("\n", highblt);
	fputs(ptemp, highblt);                  /* week group ul bytes  */
	fputs("\n", highblt);
	fputs(HS_week_UL_Group_Date, highblt);  /* week group ul date   */
	fputs("\n", highblt);

	bzero(parsebuf, sizeof(parsebuf));
	bzero(ptemp, sizeof(ptemp));
	sprintf(parsebuf, "%llu", HS_month_UL_Group);
	strcpy(ptemp, f_format(parsebuf));

	fputs(HS_month_UL_Groupname, highblt);  /* month groupname	*/
	fputs("\n", highblt);
	fputs(ptemp, highblt);                  /* month group ul bytes */
	fputs("\n", highblt);
	fputs(HS_month_UL_Group_Date, highblt); /* month group ul date  */
	fputs("\n", highblt);

	bzero(parsebuf, sizeof(parsebuf));
	bzero(ptemp, sizeof(ptemp));
	sprintf(parsebuf, "%llu", HS_year_UL_Group);
	strcpy(ptemp, f_format(parsebuf));

	fputs(HS_year_UL_Groupname, highblt);   /* year groupname	*/
	fputs("\n", highblt);
	fputs(ptemp, highblt);                  /* year group ul bytes  */
	fputs("\n", highblt);
	fputs(HS_year_UL_Group_Date, highblt);  /* year group ul date   */
	fputs("\n", highblt);

	fclose(highblt);
	return 0;
}

int HighScores_read()
{
	FILE *database;
	char temp[80];
	int recmaybe = 0, hs = 1;

	database = fopen("pmctop.dat", "rt");
	if (!database)
	{
		printf("error at pmctop.dat file!\n");
		exit(1);
	}

	fgets(temp, 80, database);
	sscanf(temp, "%llu", &HS_week_UL_User);
	fgets(temp, 80, database);
	strncpy(HS_week_UL_Username, temp, strlen(temp) - 1);
	fgets(temp, 80, database);
	strncpy(HS_week_UL_Date, temp, strlen(temp) - 1);

	fgets(temp, 80, database);
	sscanf(temp, "%llu", &HS_week_DL_User);
	fgets(temp, 80, database);
	strncpy(HS_week_DL_Username, temp, strlen(temp) - 1);
	fgets(temp, 80, database);
	strncpy(HS_week_DL_Date, temp, strlen(temp) - 1);

	fgets(temp, 80, database);
	sscanf(temp, "%llu", &HS_month_UL_User);
	fgets(temp, 80, database);
	strncpy(HS_month_UL_Username, temp, strlen(temp) - 1);
	fgets(temp, 80, database);
	strncpy(HS_month_UL_Date, temp, strlen(temp) - 1);

	fgets(temp, 80, database);
	sscanf(temp, "%llu", &HS_month_DL_User);
	fgets(temp, 80, database);
	strncpy(HS_month_DL_Username, temp, strlen(temp) - 1);
	fgets(temp, 80, database);
	strncpy(HS_month_DL_Date, temp, strlen(temp) - 1);

	fgets(temp, 80, database);
	sscanf(temp, "%llu", &HS_year_UL_User);
	fgets(temp, 80, database);
	strncpy(HS_year_UL_Username, temp, strlen(temp) - 1);
	fgets(temp, 80, database);
	strncpy(HS_year_UL_Date, temp, strlen(temp) - 1);

	fgets(temp, 80, database);
	sscanf(temp, "%llu", &HS_year_DL_User);
	fgets(temp, 80, database);
	strncpy(HS_year_DL_Username, temp, strlen(temp) - 1);
	fgets(temp, 80, database);
	strncpy(HS_year_DL_Date, temp, strlen(temp) - 1);

	fgets(temp, 80, database);
	sscanf(temp, "%llu", &HS_week_Alltime_UL);
	fgets(temp, 80, database);
	strncpy(HS_week_Alltime_UL_Date, temp, strlen(temp) - 1);

	fgets(temp, 80, database);
	sscanf(temp, "%llu", &HS_week_Alltime_DL);
	fgets(temp, 80, database);
	strncpy(HS_week_Alltime_DL_Date, temp, strlen(temp) - 1);

	fgets(temp, 80, database);
	sscanf(temp, "%llu", &HS_month_Alltime_UL);
	fgets(temp, 80, database);
	strncpy(HS_month_Alltime_UL_Date, temp, strlen(temp) - 1);

	fgets(temp, 80, database);
	sscanf(temp, "%llu", &HS_month_Alltime_DL);
	fgets(temp, 80, database);
	strncpy(HS_month_Alltime_DL_Date, temp, strlen(temp) - 1);

	fgets(temp, 80, database);
	sscanf(temp, "%llu", &HS_year_Alltime_UL);
	fgets(temp, 80, database);
	strncpy(HS_year_Alltime_UL_Date, temp, strlen(temp) - 1);

	fgets(temp, 80, database);
	sscanf(temp, "%llu", &HS_year_Alltime_DL);
	fgets(temp, 80, database);
	strncpy(HS_year_Alltime_DL_Date, temp, strlen(temp) - 1);

/* group reading part */
	fgets(temp, 80, database);
	sscanf(temp, "%llu", &HS_week_UL_Group);
	fgets(temp, 80, database);
	strncpy(HS_week_UL_Groupname, temp, strlen(temp) - 1);
	fgets(temp, 80, database);
	strncpy(HS_week_UL_Group_Date, temp, strlen(temp) - 1);

	fgets(temp, 80, database);
	sscanf(temp, "%llu", &HS_week_DL_Group);
	fgets(temp, 80, database);
	strncpy(HS_week_DL_Groupname, temp, strlen(temp) - 1);
	fgets(temp, 80, database);
	strncpy(HS_week_DL_Group_Date, temp, strlen(temp) - 1);

	fgets(temp, 80, database);
	sscanf(temp, "%llu", &HS_month_UL_Group);
	fgets(temp, 80, database);
	strncpy(HS_month_UL_Groupname, temp, strlen(temp) - 1);
	fgets(temp, 80, database);
	strncpy(HS_month_UL_Group_Date, temp, strlen(temp) - 1);

	fgets(temp, 80, database);
	sscanf(temp, "%llu", &HS_month_DL_Group);
	fgets(temp, 80, database);
	strncpy(HS_month_DL_Groupname, temp, strlen(temp) - 1);
	fgets(temp, 80, database);
	strncpy(HS_month_DL_Group_Date, temp, strlen(temp) - 1);

	fgets(temp, 80, database);
	sscanf(temp, "%llu", &HS_year_UL_Group);
	fgets(temp, 80, database);
	strncpy(HS_year_UL_Groupname, temp, strlen(temp) - 1);
	fgets(temp, 80, database);
	strncpy(HS_year_UL_Group_Date, temp, strlen(temp) - 1);

	fgets(temp, 80, database);
	sscanf(temp, "%llu", &HS_year_DL_Group);
	fgets(temp, 80, database);
	strncpy(HS_year_DL_Groupname, temp, strlen(temp) - 1);
	fgets(temp, 80, database);
	strncpy(HS_year_DL_Group_Date, temp, strlen(temp) - 1);

	fclose(database); /* reading done, lets close the datafile */

	while (hs < maxuser)
	{
/* weektop upload user */
		if (week_ul_bytes[hs] > HS_week_UL_User)
		{
#ifdef DEBUG
			printf("new WEEKTOP UL highscore: %s - %llu\n", user_name[hs], week_ul_bytes[hs]);
#endif
			HS_week_UL_User = 0;
			HS_week_UL_User = week_ul_bytes[hs];
			bzero(HS_week_UL_Username, sizeof(HS_week_UL_Username));
			strcpy(HS_week_UL_Username, user_name[hs]);
			strcpy(HS_week_UL_Date, dates);
			recmaybe = 1;
		}

/* weektop download user */
		if (week_dl_bytes[hs] > HS_week_DL_User)
		{
#ifdef DEBUG
			printf("new WEEKTOP DL highscore: %s - %llu\n", user_name[hs], week_dl_bytes[hs]);
#endif
			HS_week_DL_User = 0;
			HS_week_DL_User = week_ul_bytes[hs];
			bzero(HS_week_DL_Username, sizeof(HS_week_DL_Username));
			strcpy(HS_week_DL_Username, user_name[hs]);
			strcpy(HS_week_DL_Date, dates);
			recmaybe = 1;
		}

/* monthtop upload user */
		if (month_ul_bytes[hs] > HS_month_UL_User)
		{
#ifdef DEBUG
			printf("new MONTHTOP UL highscore: %s - %llu\n", user_name[hs], month_ul_bytes[hs]);
#endif
			HS_month_UL_User = 0;
			HS_month_UL_User = month_ul_bytes[hs];
			bzero(HS_month_UL_Username, sizeof(HS_month_UL_Username));
			strcpy(HS_month_UL_Username, user_name[hs]);
			strcpy(HS_month_UL_Date, dates);
			recmaybe = 1;
		}

/* monthtop download user */
		if (month_dl_bytes[hs] > HS_month_DL_User)
		{
#ifdef DEBUG
			printf("new MONTHTOP DL highscore: %s - %llu\n", user_name[hs], month_dl_bytes[hs]);
#endif
			HS_month_DL_User = 0;
			HS_month_DL_User = month_dl_bytes[hs];
			bzero(HS_month_DL_Username, sizeof(HS_month_DL_Username));
			strcpy(HS_month_DL_Username, user_name[hs]);
			strcpy(HS_month_DL_Date, dates);
			recmaybe = 1;
		}

/* yeartop upload user */
		if (year_ul_bytes[hs] > HS_year_UL_User)
		{
#ifdef DEBUG
			printf("new YEARTOP UL highscore: %s - %llu\n", user_name[hs], year_ul_bytes[hs]);
#endif
			HS_year_UL_User = 0;
			HS_year_UL_User = year_ul_bytes[hs];
			bzero(HS_year_UL_Username, sizeof(HS_year_UL_Username));
			strcpy(HS_year_UL_Username, user_name[hs]);
			strcpy(HS_year_UL_Date, dates);
			recmaybe = 1;
		}

/* yeartop download user */
		if (year_dl_bytes[hs] > HS_year_DL_User)
		{
#ifdef DEBUG
			printf("new YEARTOP DL highscore: %s - %llu\n", user_name[hs], year_dl_bytes[hs]);
#endif
			HS_year_DL_User = 0;
			HS_year_DL_User = year_dl_bytes[hs];
			bzero(HS_year_DL_Username, sizeof(HS_year_DL_Username));
			strcpy(HS_year_DL_Username, user_name[hs]);
			strcpy(HS_year_DL_Date, dates);
			recmaybe = 1;
		}

		hs++;
	}

/* weektop ALLTIME uploads */
	if (week_ul_total > HS_week_Alltime_UL)
	{
		HS_week_Alltime_UL = 0;
		HS_week_Alltime_UL = week_ul_total;
		strcpy(HS_week_Alltime_UL_Date, dates);
#ifdef DEBUG
		printf("new WEEKTOP ALLTIME UL highscore: %llu\n", HS_week_Alltime_UL);
#endif
		recmaybe = 1;
	}

/* weektop ALLTIME downloads */
	if (week_dl_total > HS_week_Alltime_DL)
	{
		HS_week_Alltime_DL = 0;
		HS_week_Alltime_DL = week_dl_total;
		strcpy(HS_week_Alltime_DL_Date, dates);
#ifdef DEBUG
		printf("new WEEKTOP ALLTIME DL highscore: %llu\n", HS_week_Alltime_DL);
#endif
		recmaybe = 1;
	}

/* monthtop ALLTIME uploads */
	if (month_ul_total > HS_month_Alltime_UL)
	{
		HS_month_Alltime_UL = 0;
		HS_month_Alltime_UL = month_ul_total;
		strcpy(HS_month_Alltime_UL_Date, dates);
#ifdef DEBUG
		printf("new MONTHTOP ALLTIME UL highscore: %llu\n", HS_month_Alltime_UL);
#endif
		recmaybe = 1;
	}

/* monthtop ALLTIME downloads */
	if (month_dl_total > HS_month_Alltime_DL)
	{
		HS_month_Alltime_DL = 0;
		HS_month_Alltime_DL = month_dl_total;
		strcpy(HS_month_Alltime_DL_Date, dates);
#ifdef DEBUG
		printf("new MONTHTOP ALLTIME DL highscore: %llu\n", HS_month_Alltime_DL);
#endif
		recmaybe = 1;
	}

/* yeartop ALLTIME uploads */
	if (year_ul_total > HS_year_Alltime_UL)
	{
		HS_year_Alltime_UL = 0;
		HS_year_Alltime_UL = year_ul_total;
		strcpy(HS_year_Alltime_UL_Date, dates);
#ifdef DEBUG
		printf("new YEARTOP ALLTIME UL highscore: %llu\n", HS_year_Alltime_UL);
#endif
		recmaybe = 1;
	}

/* yeartop ALLTIME downloads */
	if (year_dl_total > HS_year_Alltime_DL)
	{
		HS_year_Alltime_DL = 0;
		HS_year_Alltime_DL = year_dl_total;
		strcpy(HS_year_Alltime_DL_Date, dates);
#ifdef DEBUG
		printf("new YEARTOP ALLTIME DL highscore: %llu\n", HS_year_Alltime_DL);
#endif
		recmaybe = 1;
	}

/* lets do the same record shit to GROUPS now */
	hs = 1;

	while (hs < Groups)
	{
/* weektop upload group */
		if (GroupWEEKBytes[hs] > HS_week_UL_Group)
		{
#ifdef DEBUG
			printf("new GROUP WEEKTOP UL highscore: %s - %llu\n", GroupName[hs], GroupWEEKBytes[hs]);
#endif
			HS_week_UL_Group = 0;
			HS_week_UL_Group = GroupWEEKBytes[hs];
			bzero(HS_week_UL_Groupname, sizeof(HS_week_UL_Groupname));
			strcpy(HS_week_UL_Groupname, GroupName[hs]);
			strcpy(HS_week_UL_Group_Date, dates);
			recmaybe = 1;
		}

/* monthtop upload group */
		if (GroupMONTHBytes[hs] > HS_month_UL_Group)
		{
#ifdef DEBUG
			printf("new GROUP MONTHTOP UL highscore: %s - %llu\n", GroupName[hs], GroupMONTHBytes[hs]);
#endif
			HS_month_UL_Group = 0;
			HS_month_UL_Group = GroupMONTHBytes[hs];
			bzero(HS_month_UL_Groupname, sizeof(HS_month_UL_Groupname));
			strcpy(HS_month_UL_Groupname, GroupName[hs]);
			strcpy(HS_month_UL_Group_Date, dates);
			recmaybe = 1;
		}

/* yeartop upload group */
		if (GroupYEARBytes[hs] > HS_year_UL_Group)
		{
#ifdef DEBUG
			printf("new GROUP YEARTOP UL highscore: %s - %llu\n", GroupName[hs], GroupYEARBytes[hs]);
#endif
			HS_year_UL_Group = 0;
			HS_year_UL_Group = GroupYEARBytes[hs];
			bzero(HS_year_UL_Groupname, sizeof(HS_year_UL_Groupname));
			strcpy(HS_year_UL_Groupname, GroupName[hs]);
			strcpy(HS_year_UL_Group_Date, dates);
			recmaybe = 1;
		}

		hs++;
	}

	if (recmaybe)
	{
		database = fopen("pmctop.dat", "wt");
		if (!database)
		{
			printf("error at pmctop.dat file!\n");
			exit(1);
		}

		fprintf(database, "%llu\n", HS_week_UL_User);
		fprintf(database, "%s\n", HS_week_UL_Username);
		fprintf(database, "%s\n", HS_week_UL_Date);

		fprintf(database, "%llu\n", HS_week_DL_User);
		fprintf(database, "%s\n", HS_week_DL_Username);
		fprintf(database, "%s\n", HS_week_DL_Date);

		fprintf(database, "%llu\n", HS_month_UL_User);
		fprintf(database, "%s\n", HS_month_UL_Username);
		fprintf(database, "%s\n", HS_month_UL_Date);

		fprintf(database, "%llu\n", HS_month_DL_User);
		fprintf(database, "%s\n", HS_month_DL_Username);
		fprintf(database, "%s\n", HS_month_DL_Date);

		fprintf(database, "%llu\n", HS_year_UL_User);
		fprintf(database, "%s\n", HS_year_UL_Username);
		fprintf(database, "%s\n", HS_year_UL_Date);

		fprintf(database, "%llu\n", HS_year_DL_User);
		fprintf(database, "%s\n", HS_year_DL_Username);
		fprintf(database, "%s\n", HS_year_DL_Date);

		fprintf(database, "%llu\n", HS_week_Alltime_UL);
		fprintf(database, "%s\n", HS_week_Alltime_UL_Date);

		fprintf(database, "%llu\n", HS_week_Alltime_DL);
		fprintf(database, "%s\n", HS_week_Alltime_DL_Date);

		fprintf(database, "%llu\n", HS_month_Alltime_UL);
		fprintf(database, "%s\n", HS_month_Alltime_UL_Date);

		fprintf(database, "%llu\n", HS_month_Alltime_DL);
		fprintf(database, "%s\n", HS_month_Alltime_DL_Date);

		fprintf(database, "%llu\n", HS_year_Alltime_UL);
		fprintf(database, "%s\n", HS_year_Alltime_UL_Date);

		fprintf(database, "%llu\n", HS_year_Alltime_DL);
		fprintf(database, "%s\n", HS_year_Alltime_DL_Date);

		fprintf(database, "%llu\n", HS_week_UL_Group);
		fprintf(database, "%s\n", HS_week_UL_Groupname);
		fprintf(database, "%s\n", HS_week_UL_Group_Date);

		fprintf(database, "%llu\n", HS_week_DL_Group);
		fprintf(database, "%s\n", HS_week_DL_Groupname);
		fprintf(database, "%s\n", HS_week_DL_Group_Date);

		fprintf(database, "%llu\n", HS_month_UL_Group);
		fprintf(database, "%s\n", HS_month_UL_Groupname);
		fprintf(database, "%s\n", HS_month_UL_Group_Date);

		fprintf(database, "%llu\n", HS_month_DL_Group);
		fprintf(database, "%s\n", HS_month_DL_Groupname);
		fprintf(database, "%s\n", HS_month_DL_Group_Date);

		fprintf(database, "%llu\n", HS_year_UL_Group);
		fprintf(database, "%s\n", HS_year_UL_Groupname);
		fprintf(database, "%s\n", HS_year_UL_Group_Date);

		fprintf(database, "%llu\n", HS_year_DL_Group);
		fprintf(database, "%s\n", HS_year_DL_Groupname);
		fprintf(database, "%s\n", HS_year_DL_Group_Date);
	}

	fclose(database);

#ifdef DEBUG
	printf("highscores...\n");
	printf("HS_week_UL_User: %llu\n", HS_week_UL_User);
	printf("hs week ul username: %s\n", HS_week_UL_Username);
	printf("hs week ul date: %s\n", HS_week_UL_Date);
	printf("hs week dl user: %llu\n", HS_week_DL_User);
	printf("hs week dl username: %s\n", HS_week_DL_Username);
	printf("hs week dl date: %s\n", HS_week_DL_Date);
	printf("hs month ul user: %llu\n", HS_month_UL_User);
	printf("hs month ul username: %s\n", HS_month_UL_Username);
	printf("hs month ul date: %s\n", HS_month_UL_Date);
	printf("hs monh dl user: %llu\n", HS_month_DL_User);
	printf("hs month dl username: %s\n", HS_month_DL_Username);
	printf("hs month dl date: %s\n", HS_month_DL_Date);
	printf("hs year ul user: %llu\n", HS_year_UL_User);
	printf("hs year ul username: %s\n", HS_year_UL_Username);
	printf("hs year ul date: %s\n", HS_year_UL_Date);
	printf("hs year dl user: %llu\n", HS_year_DL_User);
	printf("hs year dl username: %s\n", HS_year_DL_Username);
	printf("hs year dl date: %s\n", HS_year_DL_Date);

	printf("hs week alltime ul: %llu - %s\n", HS_week_Alltime_UL, HS_week_Alltime_UL_Date);
	printf("hs week alltime dl: %llu - %s\n", HS_week_Alltime_DL, HS_week_Alltime_DL_Date);
	printf("hs month alltime ul: %llu - %s\n", HS_month_Alltime_UL, HS_month_Alltime_UL_Date);
	printf("hs month alltime dl: %llu - %s\n", HS_month_Alltime_DL, HS_month_Alltime_DL_Date);
	printf("hs year alltime ul: %llu - %s\n", HS_year_Alltime_UL, HS_year_Alltime_UL_Date);
	printf("hs year alltime dl: %llu - %s\n", HS_year_Alltime_DL, HS_year_Alltime_DL_Date);

	printf("hs week ul group: %llu\n", HS_week_UL_Group);
	printf("hs week ul groupname: %s\n", HS_week_UL_Groupname);
	printf("hs week ul gdate: %s\n", HS_week_UL_Group_Date);
	printf("hs week dl group: %llu\n", HS_week_DL_Group);
	printf("hs week dl groupname: %s\n", HS_week_DL_Groupname);
	printf("hs week dl gdate: %s\n", HS_week_DL_Group_Date);
	printf("hs month ul group: %llu\n", HS_month_UL_Group);
	printf("hs month ul groupname: %s\n", HS_month_UL_Groupname);
	printf("hs month ul gdate: %s\n", HS_month_UL_Group_Date);
	printf("hs month dl group: %llu\n", HS_month_DL_Group);
	printf("hs month dl groupname: %s\n", HS_month_DL_Groupname);
	printf("hs month dl gdate: %s\n", HS_month_DL_Group_Date);
	printf("hs year ul group: %llu\n", HS_year_UL_Group);
	printf("hs year ul groupname: %s\n", HS_year_UL_Groupname);
	printf("hs year ul gdate: %s\n", HS_year_UL_Group_Date);
	printf("hs year dl group: %llu\n", HS_year_DL_Group);
	printf("hs year dl groupname: %s\n", HS_year_DL_Groupname);
	printf("hs year dl gdate: %s\n", HS_year_DL_Group_Date);
#endif
	return 0;
}

/*  Weektop Database  */
int DataBase_EEEK(char *wus, char *winner, char *locat, char *bytes, char *files, char *albytes, char *alfiles)
{
	FILE *datdata;
/*
   format for the text file weektop database

   01/31/01 -- 23:58 hrs
   cleaner
   Mafia Jeopardy
   5.193.519.175
   1.941
   9.135.758.913
   3.453
 */
#ifdef DEBUG
	printf("yeppers, at DataBase_EEEK...\n");
#endif
	/* weektop recording */
	if (strstr(wus, "week"))
	{
		datdata = fopen("weektop.dat", "at");
		if (!datdata)
		{
			printf("error at weektop.dat, exiting\n");
			exit(1);
		}

		fputs(dates, datdata);
		fputs(" -- ", datdata);
		fputs(times, datdata);
		fputs(" hrs\n", datdata);
		fputs(winner, datdata);
		fputs("\n", datdata);
		fputs(locat, datdata);
		fputs("\n", datdata);
		fputs(bytes, datdata);
		fputs("\n", datdata);
		fputs(files, datdata);
		fputs("\n", datdata);
		fputs(albytes, datdata);
		fputs("\n", datdata);
		fputs(alfiles, datdata);
		fputs("\n", datdata);

		fclose(datdata);
		weekcheck = 0;

		printf("*UPDATED* week stat ...");
	}

	/* monthtop recording */
	if (strstr(wus, "month"))
	{
		datdata = fopen("monthtop.dat", "at");
		if (!datdata)
		{
			printf("error at monthtop.dat, exiting\n");
			exit(1);
		}

		fputs(dates, datdata);
		fputs(" - ", datdata);
		fputs(times, datdata);
		fputs("\n", datdata);
		fputs(winner, datdata);
		fputs("\n", datdata);
		fputs(locat, datdata);
		fputs("\n", datdata);
		fputs(bytes, datdata);
		fputs("\n", datdata);
		fputs(files, datdata);
		fputs("\n", datdata);
		fputs(albytes, datdata);
		fputs("\n", datdata);
		fputs(alfiles, datdata);
		fputs("\n", datdata);

		fclose(datdata);
		monthcheck = 0;

		printf("*UPDATED* month stat ...");
	}

	/* yeartop recording */
	if (strstr(wus, "year"))
	{
		datdata = fopen("yeartop.dat", "at");
		if (!datdata)
		{
			printf("error at yeartop.dat, exiting\n");
			exit(1);
		}

		fputs(dates, datdata);
		fputs(" - ", datdata);
		fputs(times, datdata);
		fputs("\n", datdata);
		fputs(winner, datdata);
		fputs("\n", datdata);
		fputs(locat, datdata);
		fputs("\n", datdata);
		fputs(bytes, datdata);
		fputs("\n", datdata);
		fputs(files, datdata);
		fputs("\n", datdata);
		fputs(albytes, datdata);
		fputs("\n", datdata);
		fputs(alfiles, datdata);
		fputs("\n", datdata);

		fclose(datdata);
		yearcheck = 0;

		printf("*UPDATED* year stat ...");
	}

	return 0;
}

/* **************************************************************************
**************************************************************************
**************************************************************************
************************************************************************** */

/* this is the heart of the bitch */
int main(int argv, char *argc[])
{
	int ouser = 0;

	Headers(); /* what we are, and doing */

	CFGread();
	Time_Check();
	UserBase();
	DSZ_Check();

	/* top lists */
	Upload_List();
	Download_List();
	Calls_List();
	Faked_List();
	DSZ_Top();

	ouser = Extract_old_data("userbase.weekly.dat");
#ifdef DEBUG
	sumThing("week");
	printf("going to week_top() and ouser == %i\n", ouser);
#endif
	Week_Top(ouser);
	ouser = 0;
	ouser = Extract_old_data("userbase.monthly.dat");
#ifdef DEBUG
	sumThing("month");
	printf("going to month_top() and ouser == %i\n", ouser);
#endif
	Month_Top(ouser);
	ouser = 0;
	ouser = Extract_old_data("userbase.yearly.dat");
#ifdef DEBUG
	sumThing("year");
	printf("going to year_top() and ouser == %i\n", ouser);
#endif
	Year_Top(ouser);

	grouptoppen();

#ifdef DEBUG
	printf("ok exited grouptoppen\n... entering WEEK_Upload_List()\n");
#endif

#ifdef DEBUG
	printf("argc: %s\n", argc[1]);
#endif
	/* new database shit */
	if (argv == 2)
	{
//        strcpy(cmdlineh,argc[1]); /* get possible parameter */

		if (strstr(argc[1], "/weektop"))
			weekcheck = 1;
		if (strstr(argc[1], "/monthtop"))
			monthcheck = 1;
		if (strstr(argc[1], "/yeartop"))
			yearcheck = 1;
	}

	WEEK_Upload_List();
	MONTH_Upload_List();
	YEAR_Upload_List();

	Grouptop_Upload_List();
	Grouptop_Download_List();
	Grouptop_WEEKTOP_Upload_List();
	Grouptop_MONTHTOP_Upload_List();

	HighScores_read();
#ifdef DEBUG
	printf("entering write_highscores() function\n");
#endif
	Write_Highscores();
#ifdef DEBUG
	printf("shit works\n");
#endif

	return 0;
}
