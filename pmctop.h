/* creation of UPLOAD bulletin */
int fishUpload_List(ul_bytes, userbyte_pos, maxuser)
{
	FILE *top1lay;
	FILE *top1blt;
	printf("Writing Top Uploaders list... ");

	PMC_Sort(ul_bytes, userbyte_pos, maxuser);

/* sorting routine by Miz (c) PMC 1997.
   for (i=1;i<=maxuser;i++)
   {
    userbyte_pos[i]=1;

        for (x=1;x<=maxuser;x++)
        {
            if (ul_bytes[i] < ul_bytes[x]) userbyte_pos[i]++;
        }
   }
 */

/* read/write the damned example ansi file */
	top1lay = fopen("pmctop.1.gfx", "rt");
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
	top1lay = fopen("pmctop.1.txt", "rt");
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