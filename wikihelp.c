#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "merc.h"

char *help_list[MAX_HELP];

void load_wikihelp(void)
{
    FILE *fp;
    int count = 0;
    char letter;
    char *tmp;

    if ((fp = fopen(WIKIHELP_INDEX,"r")) == NULL)
    {
	bug("Couldn't open wikihelp index file. Contact Ugha.",0);
	fclose(fp);
	return;
    }

    for (count = 0; count < MAX_HELP; count++)
    {

        letter = fread_letter(fp);
        if (letter == '#')
        {
            if (count < MAX_HELP)
                help_list[count] = NULL;
            fclose(fp);
            return;
        }
  	else
	    ungetc(letter,fp);

	tmp			=	fread_string(fp);
	help_list[count]	=	squash_html(tmp,FALSE);
   }
}
void do_showhelpindex(CHAR_DATA *ch, char *argument)
{
	int count;

	load_wikihelp();

	for (count = 0; count < MAX_HELP; count++)
	{

		if(help_list[count] == NULL)
		{
			send_to_char("End of helps.\n\r",ch);
			return;
		}
		send_to_char(help_list[count],ch);
	}
	return;
}
