#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pmcparse.h>
#include <pmc.h>

char line[1024];

/* creation of UPLOAD bulletin */
int Stats_List(char *whas)
{
	FILE *top1lay;
	FILE *top1blt;
	FILE *daatta;

	char wst[80], tmpc[80], org_line[1024];

	if (strcmp(whas, "week") == 0)
	{
		top1lay = fopen("example//pmctop.12.gfx", "rt");
		if (!top1lay)
		{
			printf("error at pmctop.12.gfx!\n");
			exit(1);
		}

		top1blt = fopen("pmctop_stat_weeks.gfx", "wt");
		if (!top1blt)
		{
			printf("error at pmctop_stat_weeks.gfx!\n");
			exit (1);
		}

		daatta = fopen("weektop.dat", "rt");
		if (!daatta)
		{
			printf("error at weektop.dat!\n");
			exit (1);
		}
	}

	if (strcmp(whas, "month") == 0)
	{
		top1lay = fopen("example//pmctop.13.gfx", "rt");
		if (!top1lay)
		{
			printf("error at pmctop.13.gfx!\n");
			exit(1);
		}

		top1blt = fopen("pmctop_stat_months.gfx", "wt");
		if (!top1blt)
		{
			printf("error at pmctop_stat_weeks.gfx!\n");
			exit (1);
		}

		daatta = fopen("monthtop.dat", "rt");
		if (!daatta)
		{
			printf("error at monthtop.dat!\n");
			exit (1);
		}
	}

	if (strcmp(whas, "year") == 0)
	{
		top1lay = fopen("example//pmctop.14.gfx", "rt");
		if (!top1lay)
		{
			printf("error at pmctop.14.gfx!\n");
			exit(1);
		}

		top1blt = fopen("pmctop_stat_years.gfx", "wt");
		if (!top1blt)
		{
			printf("error at pmctop_stat_years.gfx!\n");
			exit (1);
		}

		daatta = fopen("yeartop.dat", "rt");
		if (!daatta)
		{
			printf("error at yeartop.dat!\n");
			exit (1);
		}
	}

/* ascii */
	if (strcmp(whas, "weekasc") == 0)
	{
		top1lay = fopen("example//pmctop.12.txt", "rt");
		if (!top1lay)
		{
			printf("error at pmctop.12.txt!\n");
			exit(1);
		}

		top1blt = fopen("pmctop_stat_weeks.txt", "wt");
		if (!top1blt)
		{
			printf("error at pmctop_stat_weeks.txt!\n");
			exit (1);
		}

		daatta = fopen("weektop.dat", "rt");
		if (!daatta)
		{
			printf("error at weektop.dat!\n");
			exit (1);
		}
	}

	if (strcmp(whas, "monthasc") == 0)
	{
		top1lay = fopen("example//pmctop.13.txt", "rt");
		if (!top1lay)
		{
			printf("error at pmctop.13.txt!\n");
			exit(1);
		}

		top1blt = fopen("pmctop_stat_months.txt", "wt");
		if (!top1blt)
		{
			printf("error at pmctop_stat_weeks.txt!\n");
			exit (1);
		}

		daatta = fopen("monthtop.dat", "rt");
		if (!daatta)
		{
			printf("error at monthtop.dat!\n");
			exit (1);
		}
	}

	if (strcmp(whas, "yearasc") == 0)
	{
		top1lay = fopen("example//pmctop.14.txt", "rt");
		if (!top1lay)
		{
			printf("error at pmctop.14.txt!\n");
			exit(1);
		}

		top1blt = fopen("pmctop_stat_years.txt", "wt");
		if (!top1blt)
		{
			printf("error at pmctop_stat_years.txt!\n");
			exit (1);
		}

		daatta = fopen("yeartop.dat", "rt");
		if (!daatta)
		{
			printf("error at yeartop.dat!\n");
			exit (1);
		}
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

	if (feof(daatta))
	{
		printf("We have no week/monthtop data yet, exiting.\n");
		fclose(top1blt);
		fclose(top1lay);
		fclose(daatta);
		return 0;
	}
/* hmm quite slowly working, still segfaulting if
   we dont have any data on weektop.dat or monthtop.dat
   hehe... sigh.
 */

	bzero(wst, sizeof(wst));
	bzero(tmpc, sizeof(tmpc));
	fgets(wst, 80, daatta); /* date */
	strncpy(tmpc, wst, 8);
	pmcparse(line, '@', "U_POS", tmpc);

	while (1)
	{
		bzero(wst, sizeof(wst));
		bzero(tmpc, sizeof(tmpc));
		fgets(wst, 80, daatta); /* date */
		strncpy(tmpc, wst, strlen(wst) - 1);
		pmcparse(line, '@', "U_NAME", tmpc);

		bzero(wst, sizeof(wst));
		bzero(tmpc, sizeof(tmpc));
		fgets(wst, 80, daatta); /* date */
		strncpy(tmpc, wst, strlen(wst) - 1);
		if (strlen(wst) > 15)
		{
			bzero(tmpc, sizeof(tmpc));
			strncpy(tmpc, wst, 15);
		}
		pmcparse(line, '@', "U_LOCAT", tmpc);

		bzero(wst, sizeof(wst));
		bzero(tmpc, sizeof(tmpc));
		fgets(wst, 80, daatta); /* date */
		strncpy(tmpc, wst, strlen(wst) - 1);
		pmcparse(line, '@', "U_BYTES", tmpc);

		bzero(wst, sizeof(wst));
		bzero(tmpc, sizeof(tmpc));
		fgets(wst, 80, daatta); /* date */
		strncpy(tmpc, wst, strlen(wst) - 1);
		pmcparse(line, '@', "U_FILES", tmpc);

		bzero(wst, sizeof(wst));
		bzero(tmpc, sizeof(tmpc));
		fgets(wst, 80, daatta); /* date */
		strncpy(tmpc, wst, strlen(wst) - 1);
		pmcparse(line, '@', "U_ATES", tmpc);

		bzero(wst, sizeof(wst));
		bzero(tmpc, sizeof(tmpc));
		fgets(wst, 80, daatta); /* date */
		strncpy(tmpc, wst, strlen(wst) - 1);
		pmcparse(line, '@', "U_ALES", tmpc);

		fputs(line, top1blt);
		bzero(line, sizeof(line));
		strcpy(line, org_line);

		bzero(wst, sizeof(wst));
		bzero(tmpc, sizeof(tmpc));
		fgets(wst, 80, daatta); /* date */

		if (feof(daatta))
			break;

		strncpy(tmpc, wst, 8);
		pmcparse(line, '@', "U_POS", tmpc);
	}

	bzero(line, sizeof(line));
	fgets(line, 1024, top1lay);

/* footer part */
	while (!feof(top1lay))
	{
//    Generic_Footer();
		if (feof(top1lay))
			break;
		fputs(line, top1blt);
		bzero(line, sizeof(line));
		fgets(line, 1024, top1lay);
	}
	fclose(top1blt);
	fclose(top1lay);
	fclose(daatta);

	return 0;
}

/*

    highscores shiet

 */
int HighScores_List(char *styl)
{
	FILE *highlay;
	FILE *highblt;
	FILE *scores;

	char sdata[80], stmp[80];

	if (strcmp(styl, "ansii") == 0)
	{
		highlay = fopen("example//pmctop.11.gfx", "rt");
		if (!highlay)
		{
			printf("error at pmctop.11.gfx!\n");
			exit(1);
		}

		highblt = fopen("pmctop_highscores.gfx", "wt");
		if (!highblt)
		{
			printf("error at pmctop_highscores.gfx!\n");
			exit (1);
		}
	}

	if (strcmp(styl, "ascii") == 0)
	{
		highlay = fopen("example//pmctop.11.txt", "rt");
		if (!highlay)
		{
			printf("error at pmctop.11.txt!\n");
			exit(1);
		}

		highblt = fopen("pmctop_highscores.txt", "wt");
		if (!highblt)
		{
			printf("error at pmctop_highscores.txt!\n");
			exit (1);
		}
	}

	scores = fopen("highscores.dat", "rt");
	if (!scores)
	{
		printf("error at highscores.dat!\n");
		exit (1);
	}

	bzero(line, sizeof(line));
	fgets(line, 1024, highlay);

	while (!strstr(line, "WKUP_UL_USER")) /* read until were on the macro part. */
	{
		if (feof(highlay))
			break;
		fputs(line, highblt);
		bzero(line, sizeof(line));
		fgets(line, 1024, highlay);
	}

	bzero(sdata, sizeof(sdata));
	bzero(stmp, sizeof(stmp));
	fgets(sdata, 80, scores); /* week username */
	strncpy(stmp, sdata, strlen(sdata) - 1);
	pmcparse(line, '@', "WKUP_UL_USER", stmp);

	bzero(sdata, sizeof(sdata));
	bzero(stmp, sizeof(stmp));
	fgets(sdata, 80, scores); /* week bytes */
	strncpy(stmp, sdata, strlen(sdata) - 1);
	pmcparse(line, '@', "WKUP_UL_BYTES", stmp);

	bzero(sdata, sizeof(sdata));
	bzero(stmp, sizeof(stmp));
	fgets(sdata, 80, scores); /* week date */
	strncpy(stmp, sdata, strlen(sdata) - 1);
	pmcparse(line, '@', "WKUP_UL_DATE", stmp);

	fputs(line, highblt);
	bzero(line, sizeof(line));
	fgets(line, 1024, highlay);

	bzero(sdata, sizeof(sdata));
	bzero(stmp, sizeof(stmp));
	fgets(sdata, 80, scores); /* month username */
	strncpy(stmp, sdata, strlen(sdata) - 1);
	pmcparse(line, '@', "MUP_UL_USER", stmp);

	bzero(sdata, sizeof(sdata));
	bzero(stmp, sizeof(stmp));
	fgets(sdata, 80, scores); /* month bytes */
	strncpy(stmp, sdata, strlen(sdata) - 1);
	pmcparse(line, '@', "MUP_UL_BYTES", stmp);

	bzero(sdata, sizeof(sdata));
	bzero(stmp, sizeof(stmp));
	fgets(sdata, 80, scores); /* month date */
	strncpy(stmp, sdata, strlen(sdata) - 1);
	pmcparse(line, '@', "MUP_UL_DATE", stmp);

	fputs(line, highblt);
	bzero(line, sizeof(line));
	fgets(line, 1024, highlay);

	bzero(sdata, sizeof(sdata));
	bzero(stmp, sizeof(stmp));
	fgets(sdata, 80, scores); /* year date */
	strncpy(stmp, sdata, strlen(sdata) - 1);
	pmcparse(line, '@', "YEAR_UL_USER", stmp);

	bzero(sdata, sizeof(sdata));
	bzero(stmp, sizeof(stmp));
	fgets(sdata, 80, scores); /* year date */
	strncpy(stmp, sdata, strlen(sdata) - 1);
	pmcparse(line, '@', "YEAR_UL_BYTES", stmp);

	bzero(sdata, sizeof(sdata));
	bzero(stmp, sizeof(stmp));
	fgets(sdata, 80, scores); /* year date */
	strncpy(stmp, sdata, strlen(sdata) - 1);
	pmcparse(line, '@', "YEAR_UL_DATE", stmp);

	fputs(line, highblt);
	bzero(line, sizeof(line));
	fgets(line, 1024, highlay);

	bzero(sdata, sizeof(sdata));
	bzero(stmp, sizeof(stmp));
	fgets(sdata, 80, scores); /* week alltime */
	strncpy(stmp, sdata, strlen(sdata) - 1);
	pmcparse(line, '@', "WEEK_ALLTIME_PUL", stmp);

	bzero(sdata, sizeof(sdata));
	bzero(stmp, sizeof(stmp));
	fgets(sdata, 80, scores); /* week alltime date */
	strncpy(stmp, sdata, strlen(sdata) - 1);
	pmcparse(line, '@', "WEEK_UL_ALLTIME_PDATE", stmp);

	fputs(line, highblt);
	bzero(line, sizeof(line));
	fgets(line, 1024, highlay);

	bzero(sdata, sizeof(sdata));
	bzero(stmp, sizeof(stmp));
	fgets(sdata, 80, scores); /* month alltime */
	strncpy(stmp, sdata, strlen(sdata) - 1);
	pmcparse(line, '@', "MONTH_ALLTIME_UL", stmp);

	bzero(sdata, sizeof(sdata));
	bzero(stmp, sizeof(stmp));
	fgets(sdata, 80, scores); /* month alltime date */
	strncpy(stmp, sdata, strlen(sdata) - 1);
	pmcparse(line, '@', "MONTH_UL_ALLTIME_DATE", stmp);

	fputs(line, highblt);
	bzero(line, sizeof(line));
	fgets(line, 1024, highlay);

	bzero(sdata, sizeof(sdata));
	bzero(stmp, sizeof(stmp));
	fgets(sdata, 80, scores); /* year alltime */
	strncpy(stmp, sdata, strlen(sdata) - 1);
	pmcparse(line, '@', "YEAR_ALLTIME_PUL", stmp);

	bzero(sdata, sizeof(sdata));
	bzero(stmp, sizeof(stmp));
	fgets(sdata, 80, scores); /* year alltime date */
	strncpy(stmp, sdata, strlen(sdata) - 1);
	pmcparse(line, '@', "YEAR_UL_ALLTIME_PDATE", stmp);

	fputs(line, highblt);
	bzero(line, sizeof(line));
	fgets(line, 1024, highlay);

	bzero(sdata, sizeof(sdata));
	bzero(stmp, sizeof(stmp));
	fgets(sdata, 80, scores); /* week groupname  */
	strncpy(stmp, sdata, strlen(sdata) - 1);
	pmcparse(line, '@', "WKUP_GP_NAME", stmp);

	bzero(sdata, sizeof(sdata));
	bzero(stmp, sizeof(stmp));
	fgets(sdata, 80, scores); /* week group bytes */
	strncpy(stmp, sdata, strlen(sdata) - 1);
	pmcparse(line, '@', "WKUP_GP_BYTES", stmp);

	bzero(sdata, sizeof(sdata));
	bzero(stmp, sizeof(stmp));
	fgets(sdata, 80, scores); /* week group date */
	strncpy(stmp, sdata, strlen(sdata) - 1);
	pmcparse(line, '@', "WKUP_GP_DATE", stmp);

	fputs(line, highblt);
	bzero(line, sizeof(line));
	fgets(line, 1024, highlay);

	bzero(sdata, sizeof(sdata));
	bzero(stmp, sizeof(stmp));
	fgets(sdata, 80, scores); /* month groupname */
	strncpy(stmp, sdata, strlen(sdata) - 1);
	pmcparse(line, '@', "MUP_GP_NAME", stmp);

	bzero(sdata, sizeof(sdata));
	bzero(stmp, sizeof(stmp));
	fgets(sdata, 80, scores); /* month group bytes */
	strncpy(stmp, sdata, strlen(sdata) - 1);
	pmcparse(line, '@', "MUP_GP_BYTES", stmp);

	bzero(sdata, sizeof(sdata));
	bzero(stmp, sizeof(stmp));
	fgets(sdata, 80, scores); /* month group date */
	strncpy(stmp, sdata, strlen(sdata) - 1);
	pmcparse(line, '@', "MUP_GP_DATE", stmp);

	fputs(line, highblt);
	bzero(line, sizeof(line));
	fgets(line, 1024, highlay);

	bzero(sdata, sizeof(sdata));
	bzero(stmp, sizeof(stmp));
	fgets(sdata, 80, scores); /* year group name */
	strncpy(stmp, sdata, strlen(sdata) - 1);
	pmcparse(line, '@', "YEARUP_GP_NAME", stmp);

	bzero(sdata, sizeof(sdata));
	bzero(stmp, sizeof(stmp));
	fgets(sdata, 80, scores); /* year group bytes */
	strncpy(stmp, sdata, strlen(sdata) - 1);
	pmcparse(line, '@', "YEARUP_GP_BYTES", stmp);

	bzero(sdata, sizeof(sdata));
	bzero(stmp, sizeof(stmp));
	fgets(sdata, 80, scores); /* year group date */
	strncpy(stmp, sdata, strlen(sdata) - 1);
	pmcparse(line, '@', "YEARUP_GP_DATE", stmp);

	fputs(line, highblt);
	bzero(line, sizeof(line));
	fgets(line, 1024, highlay);

/* footer part */
	while (!feof(highlay))
	{
		if (feof(highlay))
			break;
		fputs(line, highblt);
		bzero(line, sizeof(line));
		fgets(line, 1024, highlay);
	}

	fclose(highblt);
	fclose(highlay);
	fclose(scores);

	return 0;
}

int main()
{
	Stats_List("week");
	Stats_List("month");
	Stats_List("year");
	/* ascii */
	Stats_List("weekasc");
	Stats_List("monthasc");
	Stats_List("yearasc");

	HighScores_List("ansii");
	HighScores_List("ascii");

	return 0;
}