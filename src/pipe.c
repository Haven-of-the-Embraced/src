#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include "merc.h"
// Local functions.
char *fgetf( char *s, int n, FILE *iop );

char *fgetf( char *s, int n, FILE *iop )
{
    int c;
    char *cs;

    c = '\0';
    cs = s;
    while( --n > 0 && (c = getc(iop)) != EOF){
        if ((*cs++ = c) == '\0'){
            break;
        }
    }
    *cs = '\0';
    return((c == EOF && cs == s) ? (char*)"" : s);
}

/**************************************************************************/
void do_pipe( CHAR_DATA *ch, char *argument )
{
    char buf[16000], pbuf[MSL];

    FILE *fp;

    if (IS_NPC(ch)){
        do_huh(ch,"");
        return;
    }


    if (!IS_IMMORTAL(ch) && !IS_TRUSTED(ch,MAX_LEVEL))
    {
        do_huh(ch,"");
        return;
    }

    sprintf(pbuf,"%s", argument);

    cprintf(ch, "piping '%s'",pbuf);
    fp = popen( pbuf, "r" );
    if(!fp){
        sendch("Pipe failed!\n\r", ch);
        return;
    }

    fgetf( buf, 15000, fp );
    strcat (buf,"\r\n--=== END OF PIPE ===--\r\n");

    page_to_char(buf, ch);

    pclose( fp );

    return;
}
/**************************************************************************/


char * get_piperesult( char *cmd )
{
    static char buf[16000];
    char pbuf[MSL];

    FILE *fp;

    sprintf(pbuf,"%s", cmd);

    fp = popen( pbuf, "r" );
    if(!fp){
        return ("get_piperesult failed!");
    }

    fgetf( buf, 15900, fp );
    pclose( fp );
    return (buf);
}
