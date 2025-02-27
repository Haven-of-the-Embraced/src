/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
 *                                                                         *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael          *
 *  Chastain, Michael Quan, and Mitchell Tse.                              *
 *                                                                         *
 *  In order to use any part of this Merc Diku Mud, you must comply with   *
 *  both the original Diku license in 'license.doc' as well the Merc       *
 *  license in 'license.txt'.  In particular, you may not remove either of *
 *  these copyright notices.                                               *
 *                                                                         *
 *  Thanks to abaddon for proof-reading our comm.c and pointing out bugs.  *
 *  Any remaining bugs are, of course, our work, not his.  :)              *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 ***************************************************************************/

/***************************************************************************
*   ROM 2.4 is copyright 1993-1998 Russ Taylor             *
*   ROM has been brought to you by the ROM consortium          *
*       Russ Taylor (rtaylor@hypercube.org)                *
*       Gabrielle Taylor (gtaylor@hypercube.org)               *
*       Brian Moore (zump@rom.org)                     *
*   By using this code, you have agreed to follow the terms of the     *
*   ROM license, in the file Rom24/doc/rom.license             *
***************************************************************************/

/*
 * This file contains all of the OS-dependent stuff:
 *   startup, signals, BSD sockets for tcp/ip, i/o, timing.
 *
 * The data flow for input is:
 *    Game_loop ---> Read_from_descriptor ---> Read
 *    Game_loop ---> Read_from_buffer
 *
 * The data flow for output is:
 *    Game_loop ---> Process_Output ---> Write_to_descriptor -> Write
 *
 * The OS-dependent functions are Read_from_descriptor and Write_to_descriptor.
 * -- Furey  26 Jan 1993
 */

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#endif

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h> /* OLC -- for close read write etc */
#include <stdarg.h> /* printf_to_char */
#include <assert.h>

#include "merc.h"
#include "interp.h"
#include "recycle.h"
#include "tables.h"
#include "lookup.h"
#include "addict.h"

/*
 * Malloc debugging stuff.
 */
#if defined(sun)
#undef MALLOC_DEBUG
#endif

#if defined(MALLOC_DEBUG)
#include <malloc.h>
extern  int malloc_debug    args( ( int  ) );
extern  int malloc_verify   args( ( void ) );
#endif



/*
 * Signal handling.
 * Apollo has a problem with __attribute(atomic) in signal.h,
 *   I dance around it.
 */
#if defined(apollo)
#define __attribute(x)
#endif

#if defined(unix)
#include <signal.h>
#endif

#if defined(apollo)
#undef __attribute
#endif



/*
 * Socket and TCP/IP stuff.
 */
#if defined(macintosh) || defined(MSDOS)
const   char    echo_off_str    [] = { '\0' };
const   char    echo_on_str [] = { '\0' };
const   char    go_ahead_str    [] = { '\0' };
#endif

#if defined(unix)
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include "telnet.h"
const   char    echo_off_str    [] = { IAC, WILL, TELOPT_ECHO, '\0' };
const   char    echo_on_str [] = { IAC, WONT, TELOPT_ECHO, '\0' };
const   char    go_ahead_str    [] = { IAC, GA, '\0' };
#endif



/*
 * OS-dependent declarations.
 */
#if defined(_AIX)
#include <sys/select.h>
//int   accept      args( ( int s, struct sockaddr *addr, int *addrlen ) );
//int   bind        args( ( int s, struct sockaddr *name, int namelen ) );
void    bzero       args( ( char *b, int length ) );
int getpeername args( ( int s, struct sockaddr *name, int *namelen ) );
int getsockname args( ( int s, struct sockaddr *name, int *namelen ) );
int gettimeofday    args( ( struct timeval *tp, struct timezone *tzp ) );
int listen      args( ( int s, int backlog ) );
int setsockopt  args( ( int s, int level, int optname, void *optval,
                int optlen ) );
int socket      args( ( int domain, int type, int protocol ) );
#endif

#if defined(apollo)
#include <unistd.h>
void    bzero       args( ( char *b, int length ) );
#endif

#if defined(__hpux)
int accept      args( ( int s, void *addr, int *addrlen ) );
int bind        args( ( int s, const void *addr, int addrlen ) );
void    bzero       args( ( char *b, int length ) );
int getpeername args( ( int s, void *addr, int *addrlen ) );
int getsockname args( ( int s, void *name, int *addrlen ) );
int gettimeofday    args( ( struct timeval *tp, struct timezone *tzp ) );
int listen      args( ( int s, int backlog ) );
int setsockopt  args( ( int s, int level, int optname,
                const void *optval, int optlen ) );
int socket      args( ( int domain, int type, int protocol ) );
#endif

#if defined(interactive)
#include <net/errno.h>
#include <sys/fnctl.h>
#endif

#if defined(linux)
/*
    Linux shouldn't need these. If you have a problem compiling, try
    uncommenting these functions.
*/
/*
int accept      args( ( int s, struct sockaddr *addr, int *addrlen ) );
int bind        args( ( int s, struct sockaddr *name, int namelen ) );
int getpeername args( ( int s, struct sockaddr *name, int *namelen ) );
int getsockname args( ( int s, struct sockaddr *name, int *namelen ) );
int listen      args( ( int s, int backlog ) );
*/

int close       args( ( int fd ) );
//int gettimeofday    args( ( struct timeval *tp, void *tzp ) );
/* int  read        args( ( int fd, char *buf, int nbyte ) ); */
int select      args( ( int width, fd_set *readfds, fd_set *writefds,
                fd_set *exceptfds, struct timeval *timeout ) );
int socket      args( ( int domain, int type, int protocol ) );
/* int  write       args( ( int fd, char *buf, int nbyte ) ); */ /* read,write in unistd.h */
#endif
/* Needs to be global because of do_copyover */
int port, control;

#if defined(macintosh)
#include <console.h>
#include <fcntl.h>
#include <unix.h>
struct  timeval
{
    time_t  tv_sec;
    time_t  tv_usec;
};
#if !defined(isascii)
#define isascii(c)      ( (c) < 0200 )
#endif
static  long            theKeys [4];

int gettimeofday        args( ( struct timeval *tp, void *tzp ) );
#endif

#if defined(MIPS_OS)
extern  int     errno;
#endif

#if defined(MSDOS)
int gettimeofday    args( ( struct timeval *tp, void *tzp ) );
int kbhit       args( ( void ) );
#endif

#if defined(NeXT)
int close       args( ( int fd ) );
int fcntl       args( ( int fd, int cmd, int arg ) );
#if !defined(htons)
u_short htons       args( ( u_short hostshort ) );
#endif
#if !defined(ntohl)
u_long  ntohl       args( ( u_long hostlong ) );
#endif
int read        args( ( int fd, char *buf, int nbyte ) );
int select      args( ( int width, fd_set *readfds, fd_set *writefds,
                fd_set *exceptfds, struct timeval *timeout ) );
int write       args( ( int fd, char *buf, int nbyte ) );
#endif

#if defined(sequent)
int accept      args( ( int s, struct sockaddr *addr, int *addrlen ) );
int bind        args( ( int s, struct sockaddr *name, int namelen ) );
int close       args( ( int fd ) );
int fcntl       args( ( int fd, int cmd, int arg ) );
int getpeername args( ( int s, struct sockaddr *name, int *namelen ) );
int getsockname args( ( int s, struct sockaddr *name, int *namelen ) );
int gettimeofday    args( ( struct timeval *tp, struct timezone *tzp ) );
#if !defined(htons)
u_short htons       args( ( u_short hostshort ) );
#endif
int listen      args( ( int s, int backlog ) );
#if !defined(ntohl)
u_long  ntohl       args( ( u_long hostlong ) );
#endif
int read        args( ( int fd, char *buf, int nbyte ) );
int select      args( ( int width, fd_set *readfds, fd_set *writefds,
                fd_set *exceptfds, struct timeval *timeout ) );
int setsockopt  args( ( int s, int level, int optname, caddr_t optval,
                int optlen ) );
int socket      args( ( int domain, int type, int protocol ) );
int write       args( ( int fd, char *buf, int nbyte ) );
#endif

/* This includes Solaris Sys V as well */
#if defined(sun)
int accept      args( ( int s, struct sockaddr *addr, int *addrlen ) );
int bind        args( ( int s, struct sockaddr *name, int namelen ) );
void    bzero       args( ( char *b, int length ) );
int close       args( ( int fd ) );
int getpeername args( ( int s, struct sockaddr *name, int *namelen ) );
int getsockname args( ( int s, struct sockaddr *name, int *namelen ) );
int listen      args( ( int s, int backlog ) );
int read        args( ( int fd, char *buf, int nbyte ) );
int select      args( ( int width, fd_set *readfds, fd_set *writefds,
                fd_set *exceptfds, struct timeval *timeout ) );

#if !defined(__SVR4)
int gettimeofday    args( ( struct timeval *tp, struct timezone *tzp ) );

#if defined(SYSV)
int setsockopt      args( ( int s, int level, int optname,
                const char *optval, int optlen ) );
#else
int setsockopt  args( ( int s, int level, int optname, void *optval,
                int optlen ) );
#endif
#endif
int socket      args( ( int domain, int type, int protocol ) );
int write       args( ( int fd, char *buf, int nbyte ) );
#endif

#if defined(ultrix)
int accept      args( ( int s, struct sockaddr *addr, int *addrlen ) );
int bind        args( ( int s, struct sockaddr *name, int namelen ) );
void    bzero       args( ( char *b, int length ) );
int close       args( ( int fd ) );
int getpeername args( ( int s, struct sockaddr *name, int *namelen ) );
int getsockname args( ( int s, struct sockaddr *name, int *namelen ) );
int gettimeofday    args( ( struct timeval *tp, struct timezone *tzp ) );
int listen      args( ( int s, int backlog ) );
int read        args( ( int fd, char *buf, int nbyte ) );
int select      args( ( int width, fd_set *readfds, fd_set *writefds,
                fd_set *exceptfds, struct timeval *timeout ) );
int setsockopt  args( ( int s, int level, int optname, void *optval,
                int optlen ) );
int socket      args( ( int domain, int type, int protocol ) );
int write       args( ( int fd, char *buf, int nbyte ) );
#endif



/*
 * Global variables.
 */
DESCRIPTOR_DATA *   descriptor_list;    /* All open descriptors     */
DESCRIPTOR_DATA *   descriptor_tsil;    /* Like descriptor_list, but backwards! */
DESCRIPTOR_DATA *   d_next;     /* Next descriptor in loop  */
DESCRIPTOR_DATA *   d_prev;     /* Previous descriptor in loop */
FILE *          fpReserve;      /* Reserved file handle     */
bool            god;        /* All new chars are gods!  */
bool            merc_down;      /* Shutdown         */
bool            test_run;
bool            wizlock;        /* Game is wizlocked        */
bool            newlock;        /* Game is newlocked        */
char            str_boot_time[MAX_INPUT_LENGTH];
/*char          boot_time_short[MAX_INPUT_LENGTH];*/
time_t          current_time;   /* time of this pulse */
time_t          boot_time;
bool            MOBtrigger = TRUE;  /* act() switch                 */
int             most_players = 0;   /* most players on since reboot */
int             max_players = 0; /* Max players on, EVEWRR!*/
int             tree_count = 0;
int             vein_count = 0;
struct  clan_type       clan_table[MAX_CLAN];

/* Imm globals */
bool            arena=FALSE;              /* Arena is closed      */
bool            nosun=FALSE;        /* No sun damage for vamps  */
bool            doubleexp=FALSE;    /* Double exp and cap   */
bool            manualxp=FALSE; /*manually set doublexp */
bool            slaughter=FALSE;        /* Removes damcap and quad damage */
bool            doubledam=FALSE;    /* double damage for players */
bool            spookums=FALSE;     /* Halloween Bonus */
bool            carnival=FALSE;     /* Mardi Gras Bonus */
bool            resolver=TRUE;
bool            DEBUG_MESSAGES=FALSE;
CMD_DATA * cmd_first;
CMD_DATA * cmd_last;
int global_qp;
int global_xp;
int qpmult; //Sets rate that QP is added to global qp
int xpmult; //sets rate that XP is added to global qp
int xpawardmult; // Set double xp multiplier
int xpstat; //Sets the milestone status for firing double xp
/* config file (config.cfg in the area directory) */
CONFIG_DATA     *config;


/*
 * OS-dependent local functions.
 */
#if defined(macintosh) || defined(MSDOS)
void    game_loop_mac_msdos args( ( void ) );
bool    read_from_descriptor    args( ( DESCRIPTOR_DATA *d ) );
bool    write_to_descriptor args( ( int desc, char *txt, int length ) );
#endif

#if defined(unix)
void    game_loop_unix      args( ( int control ) );
int init_socket     args( ( int port ) );
void    init_descriptor     args( ( int control ) );
bool    read_from_descriptor    args( ( DESCRIPTOR_DATA *d ) );
bool    write_to_descriptor args( ( int desc, char *txt, int length ) );
#endif



char randomcolors[MAX_RANDOM+2] = "RrGgYyBbMmCcWwD";

/*
 * Other local functions (OS-independent).
 */
bool    check_parse_name    args( ( char *name ) );
bool    check_reconnect     args( ( DESCRIPTOR_DATA *d, char *name,
                    bool fConn ) );
bool    check_playing       args( ( DESCRIPTOR_DATA *d, char *name ) );
int main            args( ( int argc, char **argv ) );
void    nanny           args( ( DESCRIPTOR_DATA *d, char *argument ) );
bool    process_output      args( ( DESCRIPTOR_DATA *d, bool fPrompt ) );
void    read_from_buffer    args( ( DESCRIPTOR_DATA *d ) );
void    stop_idling     args( ( CHAR_DATA *ch ) );
void    bust_a_prompt           args( ( CHAR_DATA *ch ) );
void    write_config        args ( ( void ) );
void    load_config     args ( ( void ) );
void    init_signals            args( ( void ) );
void    do_auto_shutdown        args( ( void ) );
char colour2(char type, CHAR_DATA *ch, char *string);
char *shorttime args( (time_t *tm) );


int main( int argc, char **argv )
{
    struct timeval now_time;
    bool fCopyOver = FALSE;

    /*
     * Initialize Signal Handler
     */

    // init_signals();
    /*
     * Memory debugging if needed.
     */
#if defined(MALLOC_DEBUG)
    malloc_debug( 2 );
#endif

    /*
     * Init time.
     */
/*    struct tm *now = localtime(&now_time);
    sprintf( boot_time_short, "%2d:%2d on %2d/%2d/%d", now->tm_hour, now->tm_min, now->tm_year+1900, now->tm_mon+1, now->tm_mday);*/
    gettimeofday( &now_time, NULL );
    current_time    = (time_t) now_time.tv_sec;
    boot_time       = current_time;
    strcpy( str_boot_time, ctime( &current_time ) );

    /*
     * Macintosh console initialization.
     */
#if defined(macintosh)
    console_options.nrows = 31;
    cshow( stdout );
    csetmode( C_RAW, stdin );
    cecho2file( "log file", 1, stderr );
#endif

    //Initialize xp multiplier.
    xpawardmult = 1;
    xpstat = 0;
    qpmult = 1;
    xpmult = 1;
    global_xp = 0;
    global_qp = 0;

    /*
     * Reserve one channel for our use.
     */
    if ( ( fpReserve = fopen( NULL_FILE, "r" ) ) == NULL )
    {
    perror( NULL_FILE );
    exit( 1 );
    }

    /*
     * Get the port number.
     */
    port = 2000;
    if ( argc > 1 )
    {
        if ( !is_number( argv[1] ) )
        {
            fprintf( stderr, "Usage: %s [port #]\n", argv[0] );
            exit( 1 );
        }
        else if ( ( port = atoi( argv[1] ) ) <= 1024 )
        {
            fprintf( stderr, "Port number must be above 1024.\n" );
            exit( 1 );
        }

        /* Are we being passed some arguments? */
        if (argv[2]) {

            /* Are we recovering from a copyover? */
            if (!str_cmp(argv[2], "copyover"))
            {
                fCopyOver = TRUE;
                control = atoi(argv[3]);
            } else if (!str_cmp(argv[2], "testrun"))
            {
                test_run = TRUE;
            } else {
                fprintf(stderr, "Unrecognized argument. Booting MUD normally...\n" );
            }
        }
    }

    /*
     * Run the game.
     */
#if defined(macintosh) || defined(MSDOS)
    boot_db();
    log_string( "Merc is ready to rock." );
    game_loop_mac_msdos( );
#endif

#if defined(unix)
    if (!fCopyOver)
        control = init_socket( port );

    boot_db();
    sprintf( log_buf, "Haven is ready to rock on port %d.", port );
    log_string( log_buf );

    load_config();

    if (fCopyOver)
        copyover_recover();


    game_loop_unix( control );
    close (control);
#endif
    save_config();
    /*
     * That's all, folks.
     */
    log_string( "Normal termination of game." );
    exit( 0 );
    return 0;
}



#if defined(unix)
int init_socket( int port )
{
    static struct sockaddr_in sa_zero;
    struct sockaddr_in sa;
    int x = 1;
    int keepidle = 90;
    int keepintvl = 60;
    int keepcnt = 20;
    int fd;

    if ( ( fd = socket( AF_INET, SOCK_STREAM, 0 ) ) < 0 )
    {
    perror( "Init_socket: socket" );
    exit( 1 );
    }

    if ( setsockopt( fd, SOL_SOCKET, SO_REUSEADDR,
    (char *) &x, sizeof(x) ) < 0 )
    {
    perror( "Init_socket: SO_REUSEADDR" );
    close(fd);
    exit( 1 );
    }

   if(setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, (char *) &x, sizeof(x) ) < 0) {
      perror("Init_socket: SO_KEEPALIVE");
      close(fd);
      exit(1);
   }

      if(setsockopt(fd, SOL_TCP, TCP_KEEPIDLE, (char *) &keepidle, sizeof(keepidle) ) < 0) {
      perror("Init_socket: TCP_KEEPIDLE");
      close(fd);
      exit(1);
   }

      if(setsockopt(fd, SOL_TCP, TCP_KEEPINTVL, (char *) &keepintvl, sizeof(keepintvl) ) < 0) {
      perror("Init_socket: TCP_KEEPINTVL");
      close(fd);
      exit(1);
   }

      if(setsockopt(fd, SOL_TCP, TCP_KEEPCNT, (char *) &keepcnt, sizeof(keepcnt) ) < 0) {
      perror("Init_socket: TCP_KEEPCNT");
      close(fd);
      exit(1);
   }


#if defined(SO_DONTLINGER) && !defined(SYSV)
    {
    struct  linger  ld;

    ld.l_onoff  = 1;
    ld.l_linger = 1000;

    if ( setsockopt( fd, SOL_SOCKET, SO_DONTLINGER,
    (char *) &ld, sizeof(ld) ) < 0 )
    {
        perror( "Init_socket: SO_DONTLINGER" );
        close(fd);
        exit( 1 );
    }
    }
#endif

    sa          = sa_zero;
    sa.sin_family   = AF_INET;
    sa.sin_port     = htons( port );

    if ( bind( fd, (struct sockaddr *) &sa, sizeof(sa) ) < 0 )
    {
    perror("Init socket: bind" );
    close(fd);
    exit(1);
    }


    if ( listen( fd, 3 ) < 0 )
    {
    perror("Init socket: listen");
    close(fd);
    exit(1);
    }

    return fd;
}
#endif



#if defined(macintosh) || defined(MSDOS)
void game_loop_mac_msdos( void )
{
    struct timeval last_time;
    struct timeval now_time;
    static DESCRIPTOR_DATA dcon;

    gettimeofday( &last_time, NULL );
    current_time = (time_t) last_time.tv_sec;

    /*
     * New_descriptor analogue.
     */
    dcon.descriptor = 0;
    dcon.connected  = CON_GET_NAME;
    dcon.host       = str_dup( "localhost" );
    dcon.outsize    = 2000;
    dcon.outbuf     = alloc_mem( dcon.outsize );
    dcon.next       = descriptor_list;
    dcon.showstr_head   = NULL;
    dcon.showstr_point  = NULL;
    dcon.pEdit      = NULL;         /* OLC */
    dcon.pString    = NULL;         /* OLC */
    dcon.editor     = 0;            /* OLC */
    descriptor_list = &dcon;

    /*
     * Send the greeting.
     */
    {
    extern char * help_greeting;
    if ( help_greeting[0] == '.' )
        write_to_buffer( &dcon, help_greeting+1, 0 );
    else
        write_to_buffer( &dcon, help_greeting  , 0 );
    }

    init_signals();
    /* Main loop */
    while ( !merc_down )
    {
    DESCRIPTOR_DATA *d;

    /*
     * Process input.
     */
    for ( d = descriptor_list; d != NULL; d = d_next )
    {
        d_next  = d->next;
        d->fcommand = FALSE;

#if defined(MSDOS)
        if ( kbhit( ) )
#endif
        {
        if ( d->character != NULL )
            d->character->timer = 0;
        if ( !read_from_descriptor( d ) )
        {
            if ( d->character != NULL && d->connected == CON_PLAYING)
            save_char_obj( d->character );
            d->outtop   = 0;
            close_socket( d );
            continue;
        }
        }

        if (d->character != NULL && d->character->daze > 0)
        --d->character->daze;

        if ( d->character != NULL && d->character->wait > 0 && !IS_IMMORTAL(d->character) )
        {
        --d->character->wait;
        continue;
        }

        read_from_buffer( d );
        if ( d->incomm[0] != '\0' )
        {
        d->fcommand = TRUE;
        stop_idling( d->character );

            /* OLC */
            if ( d->showstr_point )
                show_string( d, d->incomm );
            else
            if ( d->pString )
                string_add( d->character, d->incomm );
            else
                switch ( d->connected )
                {
                    case CON_PLAYING:
                if ( !run_olc_editor( d ) )
                substitute_alias( d, d->incomm );
                break;
                    default:
                nanny( d, d->incomm );
                break;
                }

        d->incomm[0]    = '\0';
        }
    }



    /*
     * Autonomous game motion.
     */
    update_handler( );



    /*
     * Output.
     */
    for ( d = descriptor_list; d != NULL; d = d_next )
    {
        d_next = d->next;

        if ( ( d->fcommand || d->outtop > 0 ) )
        {
        if ( !process_output( d, TRUE ) )
        {
            if ( d->character != NULL && d->connected == CON_PLAYING)
            save_char_obj( d->character );
            d->outtop   = 0;
            close_socket( d );
        }
        }
    }



    /*
     * Synchronize to a clock.
     * Busy wait (blargh).
     */
    now_time = last_time;
    for ( ; ; )
    {
        int delta;

#if defined(MSDOS)
        if ( kbhit( ) )
#endif
        {
        if ( dcon.character != NULL )
            dcon.character->timer = 0;
        if ( !read_from_descriptor( &dcon ) )
        {
            if ( dcon.character != NULL && d->connected == CON_PLAYING)
            save_char_obj( d->character );
            dcon.outtop = 0;
            close_socket( &dcon );
        }
#if defined(MSDOS)
        break;
#endif
        }

        gettimeofday( &now_time, NULL );
        delta = ( now_time.tv_sec  - last_time.tv_sec  ) * 1000 * 1000
          + ( now_time.tv_usec - last_time.tv_usec );
        if ( delta >= 1000000 / PULSE_PER_SECOND )
        break;
    }
    last_time    = now_time;
    current_time = (time_t) last_time.tv_sec;
    }

    return;
}
#endif



#if defined(unix)
void game_loop_unix( int control )
{
    sh_int test_loops = MAX_TEST_LOOPS;
    static struct timeval null_time;
    struct timeval last_time;

    signal( SIGPIPE, SIG_IGN );
    gettimeofday( &last_time, NULL );
    current_time = (time_t) last_time.tv_sec;

    /* Main loop */
    while ( !merc_down )
    {
    fd_set in_set;
    fd_set out_set;
    fd_set exc_set;
    DESCRIPTOR_DATA *d;
    int maxdesc;

#if defined(MALLOC_DEBUG)
    if ( malloc_verify( ) != 1 )
        abort( );
#endif

    /*
     * Poll all active descriptors.
     */
    FD_ZERO( &in_set  );
    FD_ZERO( &out_set );
    FD_ZERO( &exc_set );
    FD_SET( control, &in_set );
    maxdesc = control;
    for ( d = descriptor_list; d; d = d->next )
    {
        maxdesc = UMAX( maxdesc, d->descriptor );
        FD_SET( d->descriptor, &in_set  );
        FD_SET( d->descriptor, &out_set );
        FD_SET( d->descriptor, &exc_set );
    }

    if ( select( maxdesc+1, &in_set, &out_set, &exc_set, &null_time ) < 0 )
    {
        perror( "Game_loop: select: poll" );
        exit( 1 );
    }

    /*
     * New connection?
     */
    if ( FD_ISSET( control, &in_set ) )
        init_descriptor( control );

    /*
     * Kick out the freaky folks.
     */
    for ( d = descriptor_list; d != NULL; d = d_next )
    {
        d_next = d->next;
        if ( FD_ISSET( d->descriptor, &exc_set ) )
        {
        FD_CLR( d->descriptor, &in_set  );
        FD_CLR( d->descriptor, &out_set );
        if ( d->character && d->connected == CON_PLAYING)
            save_char_obj( d->character );
        d->outtop   = 0;
        close_socket( d );
        }
    }

    /*
     * Process input.
     */
    for ( d = descriptor_list; d != NULL; d = d_next )
    {
        d_next  = d->next;
        d->fcommand = FALSE;

        if ( FD_ISSET( d->descriptor, &in_set ) )
        {
        if ( d->character != NULL )
            d->character->timer = 0;
        if ( !read_from_descriptor( d ) )
        {
            FD_CLR( d->descriptor, &out_set );
            if ( d->character != NULL && d->connected == CON_PLAYING)
            save_char_obj( d->character );
            d->outtop   = 0;
            close_socket( d );
            continue;
        }
        }

        if (d->character != NULL && d->character->daze > 0)
        --d->character->daze;

        if ( d->character != NULL && d->character->wait > 0 )
        {
        --d->character->wait;
        if (!IS_IMMORTAL(d->character))
            continue;
        }

        if ( d->character != NULL && d->character->stopped > 0 )
        {
        --d->character->stopped;

        if (!IS_IMMORTAL(d->character))
            continue;
        }

        read_from_buffer( d );
        if ( d->incomm[0] != '\0' )
        {
        d->fcommand = TRUE;
        stop_idling( d->character );

    /* OLC */
    if ( d->showstr_point )
        show_string( d, d->incomm );
    else
    if ( d->pString )
        string_add( d->character, d->incomm );
    else
        switch ( d->connected )
        {
            case CON_PLAYING:
            if ( !run_olc_editor( d ) )
                    substitute_alias( d, d->incomm );
            break;
            default:
            nanny( d, d->incomm );
            break;
        }

        d->incomm[0]    = '\0';
        }
    }



    /*
     * Autonomous game motion.
     */
    update_handler( );



    /*
     * Output.
     */
    for ( d = descriptor_list; d != NULL; d = d_next )
    {
        d_next = d->next;

        if ( ( d->fcommand || d->outtop > 0 )
        &&   FD_ISSET(d->descriptor, &out_set) )
        {
        if ( !process_output( d, TRUE ) )
        {
            if ( d->character != NULL && d->connected == CON_PLAYING)
            save_char_obj( d->character );
            d->outtop   = 0;
            close_socket( d );
        }
        }
    }



    /*
     * Synchronize to a clock.
     * Sleep( last_time + 1/PULSE_PER_SECOND - now ).
     * Careful here of signed versus unsigned arithmetic.
     */
    {
        struct timeval now_time;
        long secDelta;
        long usecDelta;

        gettimeofday( &now_time, NULL );
        usecDelta   = ((int) last_time.tv_usec) - ((int) now_time.tv_usec)
            + 1000000 / PULSE_PER_SECOND;
        secDelta    = ((int) last_time.tv_sec ) - ((int) now_time.tv_sec );
        while ( usecDelta < 0 )
        {
        usecDelta += 1000000;
        secDelta  -= 1;
        }

        while ( usecDelta >= 1000000 )
        {
        usecDelta -= 1000000;
        secDelta  += 1;
        }

        if ( secDelta > 0 || ( secDelta == 0 && usecDelta > 0 ) )
        {
        struct timeval stall_time;

        stall_time.tv_usec = usecDelta;
        stall_time.tv_sec  = secDelta;
        if ( select( 0, NULL, NULL, NULL, &stall_time ) < 0 )
        {
            perror( "Game_loop: select: stall" );
            exit( 1 );
        }
        }
    }

    gettimeofday( &last_time, NULL );
    current_time = (time_t) last_time.tv_sec;

    if (test_run)
	if (test_loops <= 0) {
        	merc_down = TRUE;
	} else {
		test_loops--;
	}
    }

    return;
}
#endif



#if defined(unix)

void init_descriptor( int control )
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *dnew;
    struct sockaddr_in sock;
    struct hostent *from;
    int desc;
    socklen_t size;

    size = sizeof(sock);
    getsockname( control, (struct sockaddr *) &sock, &size );
    if ( ( desc = accept( control, (struct sockaddr *) &sock, &size) ) < 0 )
    {
    perror( "New_descriptor: accept" );
    return;
    }

#if !defined(FNDELAY)
#define FNDELAY O_NDELAY
#endif

    if ( fcntl( desc, F_SETFL, FNDELAY ) == -1 )
    {
    perror( "New_descriptor: fcntl: FNDELAY" );
    return;
    }

    /*
     * Cons a new descriptor.
     */
    dnew = new_descriptor();
    dnew->descriptor    = desc;

    size = sizeof(sock);
    if ( getpeername( desc, (struct sockaddr *) &sock, &size ) < 0 )
    {
    perror( "New_descriptor: getpeername" );
    dnew->host = str_dup( "(unknown)" );
    }
    else
    {
    /*
     * Would be nice to use inet_ntoa here but it takes a struct arg,
     * which ain't very compatible between gcc and system libraries.
     */
    int addr;

    addr = ntohl( sock.sin_addr.s_addr );
    sprintf( buf, "%d.%d.%d.%d",
        ( addr >> 24 ) & 0xFF, ( addr >> 16 ) & 0xFF,
        ( addr >>  8 ) & 0xFF, ( addr       ) & 0xFF
        );
    sprintf( log_buf, "Sock.sinaddr:  %s", buf );
    log_string( log_buf );
    if (!resolver)
    {from = NULL;}
    else
    {from = gethostbyaddr( (char *) &sock.sin_addr, sizeof(sock.sin_addr), AF_INET );}

    dnew->host = str_dup( from ? from->h_name : buf );
    }

    /*
     * Swiftest: I added the following to ban sites.  I don't
     * endorse banning of sites, but Copper has few descriptors now
     * and some people from certain sites keep abusing access by
     * using automated 'autodialers' and leaving connections hanging.
     *
     * Furey: added suffix check by request of Nickel of HiddenWorlds.
     */
    if ( check_ban(dnew->host,BAN_ALL))
    {
    write_to_descriptor( desc,"Your site has been banned from this mud. If you feel you were unfairly banned. Please email haven@haven.wolfpaw.net for more information.\n\r", 0 );
    close( desc );
    free_descriptor(dnew);
    return;
    }
    /*
     * Init descriptor data.
     */
    dnew->next              = descriptor_list;
    if (descriptor_list)
        descriptor_list->prev   = dnew;
    if (descriptor_tsil == NULL)
        descriptor_tsil = dnew;
    descriptor_list     = dnew;

    /*
     * Send the greeting.
     */
    {
    extern char * help_greeting;
    if ( help_greeting[0] == '.' )
        write_to_buffer( dnew, help_greeting+1, 0 );
    else
        write_to_buffer( dnew, help_greeting  , 0 );
    }

    return;
}
#endif



void close_socket( DESCRIPTOR_DATA *dclose )
{
    CHAR_DATA *ch;

    if ( dclose->outtop > 0 )
    process_output( dclose, FALSE );

    if ( dclose->snoop_by != NULL )
    {
    write_to_buffer( dclose->snoop_by,
        "Your victim has left the game.\n\r", 0 );
    }

    {
    DESCRIPTOR_DATA *d;

    for ( d = descriptor_list; d != NULL; d = d->next )
    {
        if ( d->snoop_by == dclose )
        d->snoop_by = NULL;
    }
    }

    if ( ( ch = dclose->character ) != NULL )
    {
    sprintf( log_buf, "Closing link to %s.", ch->name );
    log_string( log_buf );
    /* cut down on wiznet spam when rebooting */
    if ( dclose->connected == CON_PLAYING && !merc_down)
    {
        act( "$n has lost $s link.", ch, NULL, NULL, TO_ROOM );
        wiznet("Net death has claimed $N.",ch,NULL,WIZ_LINKS,0,0);
        ch->desc = NULL;
    }
    else
    {
        free_char(dclose->original ? dclose->original :
        dclose->character );
    }
    }

    if ( d_next == dclose )
    d_next = d_next->next;

    if ( dclose == descriptor_list ) // NEWEST descriptor
    {
    descriptor_list = descriptor_list->next;
    if (descriptor_list)
        descriptor_list->prev = NULL;
    }
    else if (dclose == descriptor_tsil) // Oldest descriptor
    {
        descriptor_tsil = descriptor_tsil->prev;
        if (descriptor_tsil)
            descriptor_tsil->next = NULL;
    }
    else
    {
        dclose->next->prev = dclose->prev;
        dclose->prev->next = dclose->next;
    }
    /*DESCRIPTOR_DATA *d;

    for ( d = descriptor_list; d && d->next != dclose; d = d->next )
        ;
    if ( d != NULL )
        d->next = dclose->next;
        d->next->prev = d;
    else
        bug( "Close_socket: dclose not found.", 0 );
    }
*/
    close( dclose->descriptor );
    free_descriptor(dclose);
#if defined(MSDOS) || defined(macintosh)
    exit(1);
#endif
    return;
}



bool read_from_descriptor( DESCRIPTOR_DATA *d )
{
    int iStart;

    /* Hold horses if pending command already. */
    if ( d->incomm[0] != '\0' )
    return TRUE;

    /* Check for overflow. */
    iStart = strlen(d->inbuf);
    if ( iStart >= sizeof(d->inbuf) - 10 )
    {
    sprintf( log_buf, "%s input overflow!", d->host );
    log_string( log_buf );
    write_to_descriptor( d->descriptor,
        "\n\r*** PUT A LID ON IT!!! ***\n\r", 0 );
    return FALSE;
    }

    /* Snarf input. */
#if defined(macintosh)
    for ( ; ; )
    {
    int c;
    c = getc( stdin );
    if ( c == '\0' || c == EOF )
        break;
    putc( c, stdout );
    if ( c == '\r' )
        putc( '\n', stdout );
    d->inbuf[iStart++] = c;
    if ( iStart > sizeof(d->inbuf) - 10 )
        break;
    }
#endif

#if defined(MSDOS) || defined(unix)
    for ( ; ; )
    {
    int nRead;

    nRead = read( d->descriptor, d->inbuf + iStart,
        sizeof(d->inbuf) - 10 - iStart );
    if ( nRead > 0 )
    {
        iStart += nRead;
        if ( d->inbuf[iStart-1] == '\n' || d->inbuf[iStart-1] == '\r' )
        break;
    }
    else if ( nRead == 0 )
    {
        log_string( "EOF encountered on read." );
        return FALSE;
    }
    else if ( errno == EWOULDBLOCK )
        break;
    else
    {
        perror( "Read_from_descriptor" );
        return FALSE;
    }
    }
#endif

    d->inbuf[iStart] = '\0';
    return TRUE;
}



/*
 * Transfer one line from input buffer to input line.
 */
void read_from_buffer( DESCRIPTOR_DATA *d )
{
    int i, j, k;

    /*
     * Hold horses if pending command already.
     */
    if ( d->incomm[0] != '\0' )
    return;

    /*
     * Look for at least one new line.
     */
    for ( i = 0; d->inbuf[i] != '\n' && d->inbuf[i] != '\r'; i++ )
    {
    if ( d->inbuf[i] == '\0' )
        return;
    }

    /*
     * Canonical input processing.
     */
    for ( i = 0, k = 0; d->inbuf[i] != '\n' && d->inbuf[i] != '\r'; i++ )
    {
    if ( k >= MAX_INPUT_LENGTH - 2 )
    {
        write_to_descriptor( d->descriptor, "Line too long.\n\r", 0 );

        /* skip the rest of the line */
        for ( ; d->inbuf[i] != '\0'; i++ )
        {
        if ( d->inbuf[i] == '\n' || d->inbuf[i] == '\r' )
            break;
        }
        d->inbuf[i]   = '\n';
        d->inbuf[i+1] = '\0';
        break;
    }

    if ( d->inbuf[i] == '\b' && k > 0 )
        --k;
    else if ( isascii(d->inbuf[i]) && isprint(d->inbuf[i]) )
        d->incomm[k++] = d->inbuf[i];
    }

    /*
     * Finish off the line.
     */
    if ( k == 0 )
    d->incomm[k++] = ' ';
    d->incomm[k] = '\0';

    /*
     * Deal with bozos with #repeat 1000 ...
     */

    if ( k > 1 || d->incomm[0] == '!' )
    {
        if ( d->incomm[0] != '!' && strcmp( d->incomm, d->inlast ) )
    {
        d->repeat = 0;
    }
    else
    {
        if (++d->repeat >= 25 && d->character
        &&  d->connected == CON_PLAYING && strlen(d->incomm) > 2 &&
          str_prefix(d->incomm, "north")  && str_prefix(d->incomm, "south")  &&
          str_prefix(d->incomm, "east")  && str_prefix(d->incomm, "west")  &&
          str_prefix(d->incomm, "up" ) && str_prefix(d->incomm, "down" ) )
        {
        sprintf( log_buf, "%s input spamming!", d->host );
        log_string( log_buf );
        wiznet("Spam spam spam $N spam spam spam spam spam!",
               d->character,NULL,WIZ_SPAM,0,get_trust(d->character));
        if (d->incomm[0] == '!')
            wiznet(d->inlast,d->character,NULL,WIZ_SPAM,0,
            get_trust(d->character));
        else
            wiznet(d->incomm,d->character,NULL,WIZ_SPAM,0,
            get_trust(d->character));

        d->repeat = 0;

        write_to_descriptor( d->descriptor,
            "\n\r*** PUT A LID ON IT!!! ***\n\r", 0 );
        strcpy( d->incomm, "quit" );

        }
    }
    }


    /*
     * Do '!' substitution.
     */
    if ( d->incomm[0] == '!' )
    strcpy( d->incomm, d->inlast );
    else
    strcpy( d->inlast, d->incomm );

    /*
     * Shift the input buffer.
     */
    while ( d->inbuf[i] == '\n' || d->inbuf[i] == '\r' )
    i++;
    for ( j = 0; ( d->inbuf[j] = d->inbuf[i+j] ) != '\0'; j++ )
    ;
    return;
}



/*
 * Low level output function.
 */
bool process_output( DESCRIPTOR_DATA *d, bool fPrompt )
{
    extern bool merc_down;

    /*
     * Bust a prompt.
     */
    if ( !merc_down )
    {
    if ( d->showstr_point )
        write_to_buffer( d, "[Hit Return to continue]\n\r", 0 );
    else if ( fPrompt && d->pString && d->connected == CON_PLAYING )
        write_to_buffer( d, "> ", 2 );
    else if ( fPrompt && d->connected == CON_PLAYING )
    {
        CHAR_DATA *ch;
    CHAR_DATA *victim;

    ch = d->character;

        /* battle prompt */
        if ((victim = ch->fighting) != NULL && can_see(ch,victim))
        {
            int percent;
            char wound[100];
        char *pbuff;
        char buf[MAX_STRING_LENGTH];
        char buffer[MAX_STRING_LENGTH*2];

            if (victim->max_hit > 0)
                percent = victim->hit * 100 / victim->max_hit;
            else
                percent = -1;

            if (percent >= 100)
                sprintf(wound,"is in excellent condition.");
            else if (percent >= 90)
                sprintf(wound,"has a few scratches.");
            else if (percent >= 75)
                sprintf(wound,"has some small wounds and bruises.");
            else if (percent >= 50)
                sprintf(wound,"has quite a few wounds.");
            else if (percent >= 30)
                sprintf(wound,"has some big nasty wounds and scratches.");
            else if (percent >= 15)
                sprintf(wound,"looks pretty hurt.");
            else if (percent >= 0)
                sprintf(wound,"is in awful condition.");
            else
                sprintf(wound,"is bleeding to death.");

            sprintf(buf,"%s %s \n\r",
                IS_NPC(victim) ? victim->short_descr : victim->name,wound);
        buf[0]  = UPPER( buf[0] );
        pbuff   = buffer;
        colourconv( pbuff, buf, d->character );
            write_to_buffer( d, buffer, 0);
        }


    ch = d->original ? d->original : d->character;
    if (!IS_SET(ch->comm, COMM_COMPACT) )
        write_to_buffer( d, "\n\r", 2 );

    CHAR_DATA *target;
    int tarmove;
    int tarhit;
    if (victim && IS_SET(ch->act2, PLR2_BATTLEPROMPT))
    {
        target = victim->fighting;
        if (target)
        {
            tarhit = (100*target->hit)/(target->max_hit);
            tarmove = (100*target->move)/(target->max_move);
        }
    }

            if (ch->fighting && target && target != ch && IS_SET(ch->act2, PLR2_BATTLEPROMPT))
            {
                if (!IS_SET(ch->comm, COMM_COMPACT))
                    send_to_char("{D _______________________________________________________{x\n\r", ch);
                cprintf(ch, "{D[{m*{cTanking{m*{D] {W%s {xHp:%s%d% {xMv:%s%d%\n\r", IS_NPC(target) ? target->short_descr : target->name,
                        tarhit >= 75 ? "{G" : tarhit >= 33 ? "{Y" : "{R", tarhit, 
                        tarmove >= 75 ? "{G" : tarmove >= 33 ? "{Y" : "{R", tarmove);
                if (!IS_SET(ch->comm, COMM_COMPACT))
                    send_to_char("{D ```````````````````````````````````````````````````````{x\n\r", ch);
            }
        if ( IS_SET(ch->comm, COMM_PROMPT) )
            bust_a_prompt( d->character );

    if (IS_SET(ch->comm,COMM_TELNET_GA))
        write_to_buffer(d,go_ahead_str,0);
    }
    }

    /*
     * Short-circuit if nothing to write.
     */
    if ( d->outtop == 0 )
    return TRUE;

    /*
     * Snoop-o-rama.
     */
    if ( d->snoop_by != NULL )
    {
    if (d->character != NULL)
        write_to_buffer( d->snoop_by, d->character->name,0);
    write_to_buffer( d->snoop_by, "> ", 2 );
    write_to_buffer( d->snoop_by, d->outbuf, d->outtop );
    }

    /*
     * OS-dependent output.
     */
    if ( !write_to_descriptor( d->descriptor, d->outbuf, d->outtop ) )
    {
    d->outtop = 0;
    return FALSE;
    }
    else
    {
    d->outtop = 0;
    return TRUE;
    }
}

/*
 * Bust a prompt (player settable prompt)
 * coded by Morgenes for Aldara Mud
 */
void bust_a_prompt( CHAR_DATA *ch )
{

    char buf[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    const char *str;
    const char *i;
    char *point;
    char *pbuff;
    char buffer[ MAX_STRING_LENGTH*2 ];
    char doors[MAX_INPUT_LENGTH];
    EXIT_DATA *pexit;
    bool found;
    const char *dir_name[] = {"N","E","S","W","U","D"};
    int door;
    struct tm * timeinfo;
    const char *health_level[] =
    { "Incapacitated", "Crippled", "Mauled", "Wounded",
        "Injured", "Hurt", "Bruised", "Immaculate"};

    int cshealth = (ch->hit / HLEVELS(ch, 1));
    if (cshealth < 0)
        cshealth = 0;
    if (cshealth > 7)
        cshealth = 7;

    point = buf;
    str = ch->prompt;
    if( !str || str[0] == '\0')
    {
        sprintf( buf, "{c<%dhp %dm %dmv>{x %s",
        ch->hit, ch->mana, ch->move, ch->prefix );
    send_to_char( buf, ch );
    return;
    }


   if (IS_SET(ch->comm,COMM_AFK))
   {
    send_to_char("<AFK> ",ch);
    return;
   }

   while( *str != '\0' )
   {
      if( *str != '%' )
      {
         *point++ = *str++;
         continue;
      }
      ++str;
      switch( *str )
      {
         default :
            i = " "; break;
    case 'e':
        found = FALSE;
        doors[0] = '\0';
        for (door = 0; door < 6; door++)
        {
        if ((pexit = ch->in_room->exit[door]) != NULL
        &&  pexit ->u1.to_room != NULL
        &&  (can_see_room(ch,pexit->u1.to_room)
        ||   (IS_AFFECTED(ch,AFF_INFRARED)
        &&    !IS_AFFECTED(ch,AFF_BLIND)))
        &&  !IS_SET(pexit->exit_info,EX_CLOSED))
        {
            found = TRUE;
            strcat(doors,dir_name[door]);
        }
        }
        if (!found)
        strcat(buf,"none");
        sprintf(buf2,"%s",doors);
        i = buf2; break;
     case 'c' :
        sprintf(buf2,"%s","\n\r");
        i = buf2; break;
    case 'C' :
        sprintf(buf2, "%s", health_level[cshealth]);
        i = buf2; break;
     case 'E' :
        sprintf(buf2, "%d", global_xp);
        i = buf2; break;
         case 'h' :
            sprintf( buf2, "%d", ch->hit );
            i = buf2; break;
         case 'H' :
            sprintf( buf2, "%d", ch->max_hit );
            i = buf2; break;
         case 'm' :
            sprintf( buf2, "%d", ch->mana );
            i = buf2; break;
         case 'M' :
            sprintf( buf2, "%d", ch->max_mana );
            i = buf2; break;
         case 'v' :
            sprintf( buf2, "%d", ch->move );
            i = buf2; break;
         case 'V' :
            sprintf( buf2, "%d", ch->max_move );
            i = buf2; break;
         case 'x' :
            sprintf( buf2, "%d", ch->exp );
            i = buf2; break;
    case 'A' :
        sprintf( buf2, "%d", ch->agg_dam );
        i = buf2; break;

    case 'S' :
        sprintf(buf2, "%d", ch->remorts);
        i = buf2; break;

    case 't' :
    sprintf(buf2, "%d%s", (time_info.hour % 12 == 0) ? 12 : time_info.hour %12, time_info.hour >= 12 ? "pm" : "am");
    i = buf2;break;

    case 'T':
    	timeinfo = localtime (&current_time);
    	strftime(buf2, 10, "%I:%M%p", timeinfo);
	i = buf2;
    	break;

    case 'i' :
    sprintf(buf2, "%d",  IS_NPC(ch) ? 0 : (int)ch->pcdata->IC_total/60);
    i = buf2;break;

    case 'q' :
    sprintf(buf2, "%d", ch->qpoints);
    i = buf2;break;

    case 'I' :
        sprintf(buf2, "%s", IS_OUTSIDE(ch) ? "Outdoors" : "Indoors");
        i = buf2; break;


     case 'X' :
        sprintf(buf2, "%d", IS_NPC(ch) ? 0 :
        (ch->level + 1) * exp_per_level(ch,ch->pcdata->points) - ch->exp);
        i = buf2; break;
         case 'g' :
            sprintf( buf2, "%ld", ch->gold);
            i = buf2; break;
     case 's' :
        sprintf( buf2, "%ld", ch->silver);
        i = buf2; break;
         case 'a' :
            if( ch->level > 9 )
               sprintf( buf2, "%d", ch->alignment );
            else
               sprintf( buf2, "%s", IS_GOOD(ch) ? "good" : IS_EVIL(ch) ?
                "evil" : "neutral" );
            i = buf2; break;
         case 'r' :
            if( ch->in_room != NULL )
               sprintf( buf2, "%s",
        ((!IS_NPC(ch) && IS_SET(ch->act,PLR_HOLYLIGHT)) ||
         (!IS_AFFECTED(ch,AFF_BLIND) && !room_is_dark( ch->in_room )))
        ? ch->in_room->name : "darkness");
            else
               sprintf( buf2, " " );
            i = buf2; break;
         case 'R' :
            if( IS_IMMORTAL( ch ) && ch->in_room != NULL )
               sprintf( buf2, "%d", ch->in_room->vnum );
            else
               sprintf( buf2, " " );
            i = buf2; break;
         case 'z' :
            if( ch->in_room != NULL )
               sprintf( buf2, "%s", ch->in_room->area->name );
            else
               sprintf( buf2, " " );
            i = buf2; break;
         case '%' :
            sprintf( buf2, "%%" );
            i = buf2; break;
         case 'o' :
            sprintf( buf2, "%s", olc_ed_name(ch) );
            i = buf2; break;
     case 'O' :
        sprintf( buf2, "%s", olc_ed_vnum(ch) );
        i = buf2; break;
     case 'b' :
        sprintf( buf2, "%d", ch->pblood/10 );
        i = buf2; break;
     case 'G' :
        if (!IS_NPC(ch))
            sprintf( buf2, "%d", ch->pcdata->rage[TEMP] );
        else
            sprintf( buf2, "(NONE)");
        i = buf2; break;
    case 'N' :
       if (!IS_NPC(ch))
           sprintf( buf2, "%d", ch->pcdata->gnosis[TEMP] );
       else
           sprintf( buf2, "(NONE)");
       i = buf2;
       break;

     case 'u' :
        if (!IS_NPC(ch))
            sprintf( buf2, "%d", ch->pcdata->condition[COND_FULL] );
        else
            sprintf( buf2, "100");

        i = buf2; break;
     case 'U' :
        if (!IS_NPC(ch))
        {
            if ( ch->pcdata->condition[COND_FULL] >= 45 )
                sprintf(buf2, "Stuffed");
            else if ( ch->pcdata->condition[COND_FULL] >= 30 )
                sprintf(buf2, "Full");
            else if ( ch->pcdata->condition[COND_FULL] >= 20 )
                sprintf(buf2, "Satisfied");
            else if ( ch->pcdata->condition[COND_FULL] >= 10 )
                sprintf(buf2, "Hungry");
            else if ( ch->pcdata->condition[COND_FULL] < 10 )
                sprintf(buf2, "Starving");
        } else {
            sprintf( buf2, "(None)");
            }

        i = buf2; break;
     case 'F' :
        if (!IS_NPC(ch))
        {
        if(ch->pcdata->shiftform == HOMID)
            sprintf( buf2, "Homid" );
        else if(ch->pcdata->shiftform == GLABRO)
            sprintf( buf2, "Glabro" );
        else if(ch->pcdata->shiftform == CRINOS)
            sprintf( buf2, "Crinos" );
        else if(ch->pcdata->shiftform == HISPO)
            sprintf( buf2, "Hispo" );
        else if(ch->pcdata->shiftform == LUPUS)
            sprintf( buf2, "Lupus" );
        else
            sprintf( buf2, " ");
        } else {sprintf(buf2, " ");}

        i = buf2; break;

     case 'Q' :
        sprintf( buf2, "%d", ch->quintessence );
        i = buf2; break;

     case 'P' :
        sprintf( buf2, "%d", ch->paradox );
        i = buf2; break;

    case 'w' :
        if (!IS_NPC(ch))
            sprintf( buf2, "%d", ch->cswillpower );
        else
            sprintf( buf2, "(NONE)" );
        i = buf2; break;

    case 'W' :
        if (!IS_NPC(ch))
            sprintf( buf2, "%d", ch->csmax_willpower );
        else
            sprintf( buf2, "(NO MAX)" );
        i = buf2; break;

     case 'l' :
        if(room_is_dark( ch->in_room ))
            sprintf( buf2, "Dark" );
        else
            sprintf( buf2, "Light" );
        i = buf2; break;
    case 'L' :
        sprintf(buf2, "%d", ch->level);
        i = buf2; break;
    case 'f' :
        if(IS_AFFECTED(ch,AFF_FANGS))
            sprintf( buf2, "F" );
        else
            sprintf( buf2, " " );
        i = buf2; break;
    case '&' :
        sprintf(buf2,"{</");
        i = buf2; break;
    case '(' :
        sprintf(buf2,"<");
        i = buf2; break;
    case ')' :
        sprintf(buf2,">");
        i = buf2; break;
      }
      ++str;
      while( (*point = *i) != '\0' )
         ++point, ++i;
   }
   *point   = '\0';
   pbuff    = buffer;

   colourconv( pbuff, buf, ch );
    write_to_buffer( ch->desc, buffer, 0 );
   if (ch->prefix[0] != '\0')
        write_to_buffer(ch->desc,ch->prefix,0);
   return;
}



/*
 * Append onto an output buffer.
 */
void write_to_buffer( DESCRIPTOR_DATA *d, const char *txt, int length )
{
    /*
     * Find length in case caller didn't.
     */
    if ( length <= 0 )
    length = strlen(txt);

    /*
     * Initial \n\r if needed.
     */
    if ( d->outtop == 0 && !d->fcommand )
    {
    d->outbuf[0]    = '\n';
    d->outbuf[1]    = '\r';
    d->outtop   = 2;
    }

    /*
     * Expand the buffer as needed.
     */
    while ( d->outtop + length >= d->outsize )
    {
    char *outbuf;

        if (d->outsize >= 32000)
    {
        bug("Buffer overflow. Closing.\n\r",0);
        close_socket(d);
        return;
    }
    outbuf      = alloc_mem( 2 * d->outsize );
    strncpy( outbuf, d->outbuf, d->outtop );
    free_mem( d->outbuf, d->outsize );
    d->outbuf   = outbuf;
    d->outsize *= 2;
    }

    /*
     * Copy.
     */
    strncpy( d->outbuf + d->outtop, txt, length );
    d->outtop += length;
    return;
}



/*
 * Lowest level output function.
 * Write a block of text to the file descriptor.
 * If this gives errors on very long blocks (like 'ofind all'),
 *   try lowering the max block size.
 */
bool write_to_descriptor( int desc, char *txt, int length )
{
    int iStart;
    int nWrite;
    int nBlock;

#if defined(macintosh) || defined(MSDOS)
    if ( desc == 0 )
    desc = 1;
#endif

    if ( length <= 0 )
    length = strlen(txt);

    for ( iStart = 0; iStart < length; iStart += nWrite )
    {
    nBlock = UMIN( length - iStart, 4096 );
    if ( ( nWrite = write( desc, txt + iStart, nBlock ) ) < 0 )
        { perror( "Write_to_descriptor" ); return FALSE; }
    }

    return TRUE;
}



/*
 * Deal with sockets that haven't logged in yet.
 */
void nanny( DESCRIPTOR_DATA *d, char *argument )
{
    DESCRIPTOR_DATA *d_old, *d_next;
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *ch;
    char *pwdnew;
    char *p;
    int group,iClass,race,i,weapon, num, num1;
    bool fOld;
    PC_DATA *pcdata, *prev, *next;

    while ( isspace(*argument) )
    argument++;

    ch = d->character;

    switch ( d->connected )
    {

    default:
    bug( "Nanny: bad d->connected %d.", d->connected );
    close_socket( d );
    return;

    case CON_GET_NAME:
    if ( argument[0] == '\0' )
    {
        close_socket( d );
        return;
    }

    argument[0] = UPPER(argument[0]);
    if ( !check_parse_name( argument ) )
    {
        write_to_buffer( d, "Illegal name, try another.\n\rName: ", 0 );
        return;
    }

    fOld = load_char_obj( d, argument );
    ch   = d->character;
    SET_BIT( ch->act, PLR_COLOUR );
    if (IS_SET(ch->act, PLR_DENY))
    {
        sprintf( log_buf, "Denying access to %s@%s.", argument, d->host );
        log_string( log_buf );
        write_to_buffer( d, "You are denied access.\n\r", 0 );
        close_socket( d );
        return;
    }

    if (check_ban(d->host,BAN_PERMIT) && !IS_SET(ch->act,PLR_PERMIT))
    {
        write_to_buffer(d,"Your site has been banned from this mud. If you feel you were unfairly banned. Please email zelan@haavenmud.net for more information.\n\r",0);
        close_socket(d);
        return;
    }

    if ( check_reconnect( d, argument, FALSE ) )
    {
        fOld = TRUE;
    }
    else
    {
        if ( wizlock && !IS_IMMORTAL(ch))
        {
        write_to_buffer( d, "The game is wizlocked. If there have been several crashes in the last couple\n\r minutes, this may be a result of a corrupt pfile. If the mud crashes when you type in your character name\n\r or password, please log in a different char when the mud is unwizlocked and contact a level 108+\n\r", 0 );
        close_socket( d );
        return;
        }
    }

    if ( fOld )
    {
        /* Old player */
        write_to_buffer( d, "Password: ", 0 );
        write_to_buffer( d, echo_off_str, 0 );
        d->connected = CON_GET_OLD_PASSWORD;
        return;
    }
    else
    {
        /* New player */
        if (newlock)
        {
                write_to_buffer( d, "The game is newlocked.\n\r", 0 );
                close_socket( d );
                return;
            }

        if (check_ban(d->host,BAN_NEWBIES))
        {
        write_to_buffer(d,
            "New players are not allowed from your site.\n\r",0);
        close_socket(d);
        return;
        }
        write_to_buffer(d,"\n\rName Guidelines:\n\r",0);
        write_to_buffer(d,"1. This is a White Wolf theme based MUD, therefore names should be based\n\r",0);
        write_to_buffer(d,"around this theme.\n\r",0);
        write_to_buffer(d,"2. Compound words are NOT acceptable as character names, this includes but\n\r",0);
        write_to_buffer(d,"isn't limited to MrHappy, Superstud, and other stupid names of this nature.\n\r",0);
        write_to_buffer(d,"3. Names taken from popular cartoons, movies, and popular literature are\n\r",0);
        write_to_buffer(d,"not acceptable as player names.\n\r",0);
        write_to_buffer(d,"4. Names with profane language are not acceptable as player names.\n\r",0);
        write_to_buffer(d,"5. Names which mimic, insult or otherwise emulate another Immortal or\n\r",0);
        write_to_buffer(d,"player are not acceptable.\n\r",0);
        write_to_buffer(d,"6. The Immortal Staff, reserves the right to determine wether a name is\n\r",0);
        write_to_buffer(d,"acceptable or not, and may act on their own descretion, petitions are to\n\r",0);
        write_to_buffer(d,"be made to the Administration only.\n\r",0);
        write_to_buffer(d,"7. The Administration reserve the right to make changes to these rules at any\n\r",0);
        write_to_buffer(d,"time with or without notification, warning or evidence of sanity.\n\r",0);

        sprintf( buf, "Did I get that right, %s (Y/N)? ", argument );
        write_to_buffer( d, buf, 0 );
        d->connected = CON_CONFIRM_NEW_NAME;
        return;
    }
    break;

    case CON_GET_OLD_PASSWORD:
#if defined(unix)
    write_to_buffer( d, "\n\r", 2 );
#endif
    update_csstats(ch);

    if (strcmp( argument, ch->pcdata->pwd)) {
        write_to_buffer( d, "Wrong password.", 0);/*\n\rIf you have forgotten your password and have a valid email address registered to this character,\n\ryou may log in with the password FORGOTTEN (in all caps) to have the password mailed to you.\n\r", 0 );*/
        close_socket( d );
        return;
    }

    free_string(ch->pcdata->pwd);
    ch->pcdata->pwd = str_dup(argument);


    write_to_buffer( d, echo_on_str, 0 );

    if (check_playing(d,ch->name))
        return;

    if ( check_reconnect( d, ch->name, TRUE ) )
        return;

    sprintf( log_buf, "%s@%s has connected.", ch->name, d->host );
    log_string( log_buf );
    if(!IS_SET(ch->act,PLR_AUTOVAMP) || ch->incog_level == 0 || ch->invis_level == 0)
    wiznet(log_buf,NULL,NULL,WIZ_SITES,0,get_trust(ch));

    if ( IS_IMMORTAL(ch) )
    {
        do_function(ch, &do_help, "imotd" );
        d->connected = CON_READ_IMOTD;
    }
    else
    {
        do_function(ch, &do_help, "motd" );
        d->connected = CON_READ_MOTD;
    }
    announce(ch, NULL, WIZ_LOGINS);
    //if( d->character->max_pblood < 100) d->character->max_pblood = 100;
    break;

/* RT code for breaking link */

    case CON_BREAK_CONNECT:
    switch( *argument )
    {
    case 'y' : case 'Y':
            for ( d_old = descriptor_list; d_old != NULL; d_old = d_next )
        {
        d_next = d_old->next;
        if (d_old == d || d_old->character == NULL)
            continue;

        if (str_cmp(ch->name,d_old->original ?
            d_old->original->name : d_old->character->name))
            continue;

        close_socket(d_old);
        }
        if (check_reconnect(d,ch->name,TRUE))
            return;
        write_to_buffer(d,"Reconnect attempt failed.\n\rName: ",0);
            if ( d->character != NULL )
            {
                free_char( d->character );
                d->character = NULL;
            }
        d->connected = CON_GET_NAME;
        break;

    case 'n' : case 'N':
        write_to_buffer(d,"Name: ",0);
            if ( d->character != NULL )
            {
                free_char( d->character );
                d->character = NULL;
            }
        d->connected = CON_GET_NAME;
        break;

    default:
        write_to_buffer(d,"Please type Y or N? ",0);
        break;
    }
    break;

    case CON_CONFIRM_NEW_NAME:
    switch ( *argument )
    {
    case 'y': case 'Y':
    /* dict check stuff */
        write_to_buffer(d, "New character.\n\rChecking for Dictionary entry...", 0);
        sprintf(buf,"grep -i %s %s",ch->name, DICT_FILE);
        if(0==system(buf))
        {
            write_to_buffer(d,"\n\r This name is listed in the dictionary.\n\rPlease choose another.\n\r\n\rName:\n\r",0);
            d->connected = CON_GET_NAME;
            break;
        }
        sprintf( buf, "Character passed.\n\rGive me a password for %s: %s",
        ch->name, echo_off_str );
        write_to_buffer( d, buf, 0 );
        d->connected = CON_GET_NEW_PASSWORD;
        break;

    case 'n': case 'N':
        write_to_buffer( d, "Ok, what IS it, then? ", 0 );
        free_char( d->character );
        d->character = NULL;
        d->connected = CON_GET_NAME;
        break;

    default:
        write_to_buffer( d, "Please type Yes or No? ", 0 );
        break;
    }
    break;

    case CON_GET_NEW_PASSWORD:
#if defined(unix)
    write_to_buffer( d, "\n\r", 2 );
#endif

    if ( strlen(argument) < 5 )
    {
        write_to_buffer( d,
        "Password must be at least five characters long.\n\rPassword: ",
        0 );
        return;
    }

//    pwdnew = crypt( argument, ch->name );
    pwdnew = str_dup(argument);
    for ( p = pwdnew; *p != '\0'; p++ )
    {
        if ( *p == '~' || *p == ' ' )
        {
        write_to_buffer( d,
            "New password not acceptable, try again.\n\rPassword: ",
            0 );
        return;
        }
    }

    free_string( ch->pcdata->pwd );
    ch->pcdata->pwd = str_dup( pwdnew );
    write_to_buffer( d, "Please retype password: ", 0 );
    d->connected = CON_CONFIRM_NEW_PASSWORD;
    break;

    case CON_CONFIRM_NEW_PASSWORD:
#if defined(unix)
    write_to_buffer( d, "\n\r", 2 );
#endif

   // if ( strcmp( crypt( argument, ch->pcdata->pwd ), ch->pcdata->pwd ) )
      if (strcmp(ch->pcdata->pwd, argument))
    {
        write_to_buffer( d, "Passwords don't match.\n\rRetype password: ",
        0 );
        d->connected = CON_GET_NEW_PASSWORD;
        return;
    }

    write_to_buffer( d, echo_on_str, 0 );


        race = race_lookup("human");
        ch->race = race_lookup("human");

    for (i = 0; i < MAX_STATS; i++)
        ch->perm_stat[i] = 15;


    ch->affected_by = ch->affected_by|race_table[race].aff;
    ch->imm_flags   = ch->imm_flags|race_table[race].imm;
    ch->res_flags   = ch->res_flags|race_table[race].res;
    ch->vuln_flags  = ch->vuln_flags|race_table[race].vuln;
    ch->form    = race_table[race].form;
    ch->parts   = race_table[race].parts;
    ch->act2    = PLR2_NEWBIE;

        write_to_buffer( d, "Does your client support ANSI color?[Y/N] ", 0 );
        d->connected = CON_GET_COLOUR;
      break;

    case CON_GET_COLOUR:
    switch ( argument[0] )
    {
    case 'y': case 'Y': SET_BIT( ch->act, PLR_COLOUR );
                    send_to_char( "\n\r{bC{ro{yl{co{mu{gr{x is now {rON{x, Way Cool!\n\r\n\r", ch );
                    break;
    case 'n': case 'N':
                    break;
    default:
            write_to_buffer( d, "\n\rThat is not a valid choice.\n\rPlease choose [Y]es or [N]o: ", 0 );
        return;
    }

        write_to_buffer( d, "What is your sex (M/F)? ", 0 );
        d->connected = CON_GET_NEW_SEX;
        break;


    case CON_GET_NEW_SEX:
    switch ( argument[0] )
    {
    case 'm': case 'M': ch->sex = SEX_MALE;
                ch->pcdata->true_sex = SEX_MALE;
                break;
    case 'f': case 'F': ch->sex = SEX_FEMALE;
                ch->pcdata->true_sex = SEX_FEMALE;
                break;
    default:
        write_to_buffer( d, "That's not a sex.\n\rWhat IS your sex? ", 0 );
        return;
    }

    wiznet("Newbie alert!  $N sighted.",ch,NULL,WIZ_NEWBIE,0,0);
    sprintf( log_buf, "Newbie alert: %s@%s", ch->name, d->host );
    wiznet(log_buf,NULL,NULL,WIZ_SITES,0,get_trust(ch));
    log_string( log_buf);


    write_to_buffer(d,"Haven provides character Archetypes to assign your starting attributes and\n\r",0);
    write_to_buffer(d,"abilities. This will give your Character a preset Character Sheet to get you\n\r",0);
    write_to_buffer(d,"started. Afterwards You may then spend 'Freebie' points to further customize \n\r",0);
    write_to_buffer(d,"your character.\n\r\n\r",0);

    write_to_buffer(d,"Or, you may skip this step and create a completely custom character sheet\n\r", 0);
    write_to_buffer(d,"in-game. *Warning* You will have NO SKILLS until you complete the creation\n\r", 0);
    write_to_buffer(d,"process in-game using the 'create' command.\n\r", 0);

    write_to_buffer(d,"\n\rAvailable Archetypes:\n\r\n\r",0);
    write_to_buffer(d,"[0] Warrior\n\r",0);
    write_to_buffer(d,"[1] Rogue\n\r",0);
    write_to_buffer(d,"[2] Clergy\n\r\n\r",0);

    write_to_buffer(d,"Please select the number of the archetype that best fits you Character.\n\r",0);
    write_to_buffer(d,"Or, enter 'CUSTOM' to skip this step and create a custom sheet later:\n\r",0);
    d->connected = CON_PICK_CHILDHOOD;
    break;

    case CON_PICK_CHILDHOOD:
    if(is_number(argument))
        num = atoi(argument);
    else {
        if (!str_cmp(argument, "custom"))
            {
                write_to_buffer(d,"\n\rHaven of the Embraced is focused on Vampires, but you may choose to start the\n\r",0);
                write_to_buffer(d,"\n\rgame as a human, if you wish.  You may become embraced later, or convert to a\n\r",0);
                write_to_buffer(d,"\n\rGarou or Mage upon reaching level 100.  Vampires are far more powerful at creation\n\r",0);
                write_to_buffer(d,"\n\rover humans, and are highly recommended as your starting race if you are new to\n\r",0);
                write_to_buffer(d,"\n\rHaven of the Embraced.  The added survivability allows you to explore more easily,\n\r",0);
                write_to_buffer(d,"\n\rlearning the MUD at a better pace.\n\r",0);

                write_to_buffer(d,"Do you wish to start the game as a Vampire (Recommended): [Y/N]? ",0);

                d->connected = CON_CHOICE_VAMP;
                ch->pcdata->progress = 0;
                break;

            } else
                num = 99;
        }
    if(num < 0 || num > 9)
    {
        write_to_buffer(d,"That is not a valid choice.\n\r\n\r",0);
        write_to_buffer(d,"\n\rAvailable Archetypes:\n\r\n\r",0);
        write_to_buffer(d,"[0] Warrior\n\r",0);
        write_to_buffer(d,"[1] Rogue\n\r",0);
        write_to_buffer(d,"[2] Clergy\n\r\n\r",0);
        write_to_buffer(d,"Please select the number of the archetype that best fits you Character.\n\r",0);
        write_to_buffer(d,"Or, enter 'CUSTOM' to skip this step and create a custom sheet later:\n\r",0);
        d->connected = CON_PICK_CHILDHOOD;
        break;
    }
    for(i = 0; i < MAX_ATTRIBUTES; i++)
        ch->csattributes[i] = cr_attribute_table[num].attribute[i];

    for(;;)
    {
        num1 = number_range(0,MAX_ATTRIBUTES - 1);
        if(ch->csattributes[num1] < 4)
        {
            ch->csattributes[num1]++;
            break;
        }
    }

    for(i = 0; i < MAX_ABILITIES; i++)
        ch->csabilities[i] = cr_abil_table[num].abil[i];

    for(i = 0; i <= 2;i++)
        ch->pcdata->csvirtues[i] = cr_abil_table[num].virtue[i];
    num1 = 2;
    while (num1 > 0)
    {
        if(ch->csabilities[(i = number_range(0,MAX_ABILITIES - 1))] < 4)
        {
            ch->csabilities[i]++;
            num1--;
        }
    }
    for(;;)
    {
        num1 = number_range(0,2);
        if(ch->pcdata->csvirtues[num1] < 4)
        {
            ch->pcdata->csvirtues[num1]++;
            break;
        }
    }
    ch->pcdata->progress = 8;
    write_to_buffer(d,"\n\r\n\rYou've finished setting your charsheet archetypes and have been given 5\n\r",0);
    write_to_buffer(d,"'background' points that you can spend on background traits in-game. Once you've \n\r", 0);
    write_to_buffer(d,"done that, you may use the 'freebie' command to further increase your traits using \n\r",0);
    write_to_buffer(d,"experience points called 'freebies'.\n\r", 0);
    ch->csmax_willpower = ch->pcdata->csvirtues[COURAGE];
    ch->pcdata->cshumanity = ch->pcdata->csvirtues[CONSCIENCE]+ch->pcdata->csvirtues[SELF_CONTROL];
    ch->cswillpower = ch->csmax_willpower;
    ch->dpoints = 5;

    write_to_buffer(d,"\n\rHaven of the Embraced is focused on Vampires, but you may choose to start the\n\r",0);
    write_to_buffer(d,"\n\rgame as a human, if you wish.  You may become embraced later, or convert to a\n\r",0);
    write_to_buffer(d,"\n\rGarou or Mage upon reaching level 100.  Vampires are far more powerful at creation\n\r",0);
    write_to_buffer(d,"\n\rover humans, and are highly recommended as your starting race if you are new to\n\r",0);
    write_to_buffer(d,"\n\rHaven of the Embraced.  The added survivability allows you to explore more easily,\n\r",0);
    write_to_buffer(d,"\n\rlearning the MUD at a better pace.\n\r",0);

    write_to_buffer(d,"Do you wish to start the game as a Vampire (Recommended): [Y/N]? ",0);
    d->connected = CON_CHOICE_VAMP;
    break;


    case CON_CHOICE_VAMP:

    if(argument[0] == 'Y' || argument[0] == 'y')
    {
        write_to_buffer(d,"Vampires are divided into Clans. A clan is like a family line, a tie in blood\n\r",0);
        write_to_buffer(d,"passed from Sire to Childer. Each clan is has a unique set of traits and different\n\r",0);
        write_to_buffer(d,"abilities. Please select a clan for your character. Please note that you may never \n\r",0);
        write_to_buffer(d,"change your clan. Some clans are not available at creation and you must be 'embraced'\n\r",0);
        write_to_buffer(d,"or made into a vampire in-game by another player.  You may type help <clan> to gain\n\r",0);
        write_to_buffer(d,"more information about each clan. IE: help Nosfertatu\n\rThe available clan choices are:\n\r\n\r",0);

        write_to_buffer(d,"Assamite                      Brujah\n\r",0);
        write_to_buffer(d,"Gangrel                       Malkavian\n\r",0);
        write_to_buffer(d,"Nosferatu                     Ravnos\n\r",0);
        write_to_buffer(d,"Toreador                      Tremere\n\r",0);
        write_to_buffer(d,"Ventrue\n\r",0);

        write_to_buffer(d,"\n\r(Newbie Recommended) clans, in order:  1) Brujah 2) Nosferatu 3) Gangrel",0);
        write_to_buffer(d,"\n\rPlease choose a clan: ",0);

        d->connected = CON_PICK_CLAN;
        break;
    }
    else if(argument[0] == 'N' || argument[0] == 'n')
    {
    ch->freebie = 15;
        ch->class = 0;
    update_csstats(ch);
    write_to_buffer(d,"\n\r",2);
    SET_BIT( ch->act, PLR_AUTOEXIT );
    do_function(ch, &do_help, "motd");
    if(d->character->max_pblood < 100) d->character->max_pblood = 100;
    d->connected = CON_READ_MOTD;
    break;
    }
    write_to_buffer(d,"That is not a valid choice. Please type either Y or N.\n\rDo you wish to start the game as a Vampire? [Y/N]? ",0);
    d->connected = CON_CHOICE_VAMP;
    break;

    case CON_PICK_CLAN:
    argument = one_argument(argument,buf);

    if(!str_prefix(buf,"help"))
    {
        do_function(ch,&do_help,argument);


        write_to_buffer(d,"Please choose a Clan (Assamite, Brujah, Gangrel, Malkavian, Nosferatu, Ravnos,\n\r", 0);
	   write_to_buffer(d,"Toreador, Tremere, Ventrue or help <clan>) ",0);
        d->connected = CON_PICK_CLAN;
        break;
    }

    if(str_prefix(buf,"assamite") &&
    str_prefix(buf, "brujah" ) &&
    str_prefix(buf, "gangrel") &&
    str_prefix(buf, "malkavian") &&
    str_prefix(buf, "nosferatu") &&
    str_prefix(buf, "ravnos") &&
    str_prefix(buf, "toreador" ) &&
	str_prefix(buf, "tremere") &&
	str_prefix(buf, "ventrue") )

    {
        write_to_buffer(d,"[  Invalid clan choice.  ]\n\r",0);
        write_to_buffer(d,"Please choose a Clan (Assamite, Brujah, Gangrel, Malkavian, Nosferatu, Ravnos,\n\r", 0);
        write_to_buffer(d,"Toreador, Tremere, Ventrue or help <clan>) ",0);
        d->connected = CON_PICK_CLAN;
        break;
    }
    if(!make_vamp(ch,buf))
        write_to_buffer(d,"ERROR! make_vamp() returning FALSE\n\r",0);

    ch->freebie = 15;
    ch->class = 0;
    update_csstats(ch);
    write_to_buffer(d,"\n\r",2);
    SET_BIT( ch->act, PLR_AUTOEXIT );
    SET_BIT( ch->act, PLR_AUTOGOLD );
    SET_BIT( ch->act, PLR_AUTOLOOT );
    do_function(ch, &do_help, "motd");
    if(d->character->max_pblood < 100) d->character->max_pblood = 100;
    d->connected = CON_READ_MOTD;
    break;

    case CON_READ_IMOTD:
    write_to_buffer(d,"\n\r",2);
        do_function(ch, &do_help, "motd");
        d->connected = CON_READ_MOTD;
    break;

    case CON_READ_MOTD:
                    if ( ch->pcdata == NULL || ch->pcdata->pwd[0] == '\0')
        {
            write_to_buffer( d, "Warning! Null password!\n\r",0 );
            write_to_buffer( d, "Please report old password with bug.\n\r",0);
            write_to_buffer( d,
                "Type 'password null <new password>' to fix.\n\r",0);
        }

    write_to_buffer( d,
    "\n\rWelcome to Haven of the Embraced. May your stay be... Interesting.\n\r",
        0 );
    ch->next    = char_list;
    char_list   = ch;
    pcdata      = ch->pcdata;

    d->connected    = CON_PLAYING;
    reset_char(ch);
    stop_fighting( ch, TRUE );
    if ( ch->level == 0 )
    {


        ch->level   = 1;
        ch->exp = exp_per_level(ch,ch->pcdata->points);
        ch->hit = ch->max_hit;
        ch->mana    = ch->max_mana;
        ch->move    = ch->max_move;
        ch->train    = 3;
        ch->practice = 5;
        sprintf( buf, "the Newbie");
        set_title( ch, buf );

        do_function (ch, &do_outfit,"");
        obj_to_char(create_object(get_obj_index(OBJ_VNUM_MAP),0),ch);
        obj_to_char(create_object(get_obj_index(OBJ_VNUM_GUIDE),0),ch);

        char_to_room( ch, get_room_index( ROOM_VNUM_SCHOOL ) );
        send_to_char("\n\r",ch);
        do_function(ch, &do_help, "info");
        send_to_char("\n\r",ch);
/*      if (newch) announce(ch, NULL, WIZ_NEWBIE); */
        announce(ch, NULL, WIZ_NEWBIE);
    }
    else if ( ch->in_room != NULL)
    {
        if (is_affected(ch, gsn_astralprojection)) {
            ch->in_room = get_room_index(ROOM_VNUM_TEMPLE);
            pass_gauntlet(ch, FALSE);
            sendch("{RYou were astrally projected and you body is now lost.\n\rYou've been returned to Recall.{x\n\r", ch);
            affect_strip(ch, gsn_astralprojection);
            affect_strip(ch, gsn_astrallylost);
        }
        char_to_room( ch, ch->in_room );
    }
    else if ( IS_IMMORTAL(ch) )
    {
        char_to_room( ch, get_room_index( ROOM_VNUM_LIMBO ) );
    }
    else
    {
        char_to_room( ch, get_room_index( ROOM_VNUM_TEMPLE ) );
    }

    act( "$n has entered the game.", ch, NULL, NULL, TO_ROOM );
    do_function(ch, &do_look, "auto" );

    wiznet("$N has left real life behind.",ch,NULL,
        WIZ_LOGINS,WIZ_SITES,get_trust(ch));

    if (ch->pet != NULL)
    {
        char_to_room(ch->pet,ch->in_room);
        act("$n has entered the game.",ch->pet,NULL,NULL,TO_ROOM);
    }

        if (ch->mount != NULL)
    {
        char_to_room(ch->mount,ch->in_room);
            act("$n has entered the game.",ch->mount,NULL,NULL,TO_ROOM);
            add_follower(ch->mount, ch);
            do_mount(ch, ch->mount->name);
    }
//  if(d->character->max_pblood < 100) d->character->max_pblood = 100;
    if(d->character->race == race_lookup("vampire") || d->character->race == race_lookup("methuselah")) d->character->pcdata->sect = sect_lookup("vampire");
    if(d->character->race == race_lookup("garou")) d->character->pcdata->sect = sect_lookup("garou");
    if(d->character->clan == clan_lookup("mage")) d->character->pcdata->sect = sect_lookup("mage");
    if(d->character->level > 100) d->character->pcdata->sect = sect_lookup("immortal");
    do_function(ch, &do_unread, "");
   // if (!IS_SET(ch->comm, COMM_AUTOOOC))
   //     SET_BIT(ch->comm, COMM_NOOOC);

// Setting first value for QP-total.
if (d->character->totalqpoints < d->character->qpoints)
    d->character->totalqpoints = d->character->qpoints;

//Fixing tribe and tradition channels.
    if (ch->race == race_lookup("garou") && ch->pcdata->tribe != 0) {
        switch (ch->pcdata->tribe)
        {
        case 0: break;
        case BLACK_FURY: ch->clan = clan_lookup("blackfury");break;
        case CHILDREN_OF_GAIA: ch->clan = clan_lookup("childrenofgaia");break;
        case FIANNA: ch->clan = clan_lookup("fianna");break;
        case FENRIR: ch->clan = clan_lookup("fenrir");break;
        case SHADOW_LORD: ch->clan = clan_lookup("shadowlord");break;
        case BONE_GNAWER: ch->clan = clan_lookup("bonegnawer");break;
        case SILVER_FANG: ch->clan = clan_lookup("silverfang");break;
        case WARDERSOFMEN: ch->clan = clan_lookup("wardersofmen");break;
        case RED_TALON: ch->clan = clan_lookup("redtalon");break;
        case SILENT_STRIDER: ch->clan = clan_lookup("silentstrider");break;
        }
    } else if (ch->race == race_lookup("human") && ch->clan == clan_lookup("mage")) {
        if (ch->tradition == tradition_lookup("Akashic Brotherhood"))
            ch->clan == clan_lookup("akashicbrotherhood");
        else if (ch->tradition == tradition_lookup("Dreamspeakers"))
            ch->clan == clan_lookup("dreamspeakers");
        else if (ch->tradition == tradition_lookup("Euthanatos"))
            ch->clan == clan_lookup("euthanatos");
        else if (ch->tradition == tradition_lookup("Order of Hermes"))
            ch->clan == clan_lookup("orderofhermes");
        else if (ch->tradition == tradition_lookup("Verbena"))
            ch->clan == clan_lookup("verbena");
        else
            ch->clan == clan_lookup("mage");
        }

/* disabled til i debug everything else */
    if(!IS_NPC(ch) && ch->pcdata->rank > 0 && ch->pcdata->progress == 8)
    {
        int remort_cnt, renown=0;
        bool done;


        for(remort_cnt = ch->remorts;remort_cnt-3 > 0;remort_cnt--)
        {
            done = FALSE;
            if(ch->pcdata->renown[TEMP_GLORY] == 10 && ch->pcdata->renown[TEMP_HONOR] == 10 && ch->pcdata->renown[TEMP_WISDOM] == 10)
            {
                switch(number_range(1,3))
                {
                    case 1: ch->pcdata->renown[TEMP_GLORY] = 0;
                            ch->pcdata->renown[GLORY]++;
                            break;
                    case 2: ch->pcdata->renown[TEMP_HONOR] = 0;
                            ch->pcdata->renown[HONOR]++;
                            break;
                    case 3: ch->pcdata->renown[TEMP_WISDOM] = 0;
                            ch->pcdata->renown[WISDOM]++;
                            break;
                }
            }
            else while(!done)
            {
                switch(number_range(1,3))
                {
                    case 1: renown = TEMP_GLORY; break;
                    case 2: renown = TEMP_HONOR; break;
                    case 3: renown = TEMP_WISDOM; break;
                }
                if(ch->pcdata->renown[renown] < 10)
                {
                    ch->pcdata->renown[renown]++;
                    done = TRUE;
                }
            }
        }
        ch->pcdata->progress = 9;
    }

    if(ch->clan == clan_lookup("mage") && ch->avatar == 0)
    {
        if(ch->race == race_lookup("human"))
        {
            send_to_char("You are an oldstyle mage. You must convert to the new mage system. THIS IS A REQUIREMENT.\n\r",ch);
            send_to_char("You may type HELP MAGE, HELP TRADITIONS for more information. Type gomage mage <tradition>\n\r",ch);
        }
        else
        {
            send_to_char("You can no longer be a Dhampire Mage in this game. You must either remain a dhampire and give\n\r",ch);
            send_to_char("up your mage abilities or you may convert to the new mage system. THIS IS A REQUIREMENT!\n\r",ch);
            send_to_char("Type HELP MAGE and HELP TRADITIONS for more information. To remain a Dhampire type: gomage dhampire\n\r",ch);
            send_to_char("To become a New Mage type: gomage mage <tradition>\n\r",ch);
        }
    }
    if(ch->pcdata->progress < 1)
        send_to_char("{WYou need to type {Mcreate{W to complete your creation process. A lot of features are disabled until you do so.{x\n\r",ch);
    extern bool slaughter;
    extern bool doubleexp;
    extern bool manualxp;
    extern bool spookums;
    extern bool carnival;
    if (slaughter)
        send_to_char("Time to get killing, {RSlaughterfest{x is running!\n\r", ch);
    if (doubleexp || manualxp)
    {
        if (xpawardmult == 2)
            send_to_char("{CDoubleexp{x is turned on!{x\n\r", ch);
        if (xpawardmult == 3)
            send_to_char("{CTripleexp{x is turned on!{x\n\r", ch);
    }
    if (spookums)
        send_to_char("A mystic spookiness lies across the land!\n\r", ch);
    if (carnival)
        send_to_char("Laissez les bons temp rouler!\n\r", ch);
    /*if (IS_VAMP(ch) && ch->gen < 10)
    {
        switch (ch->gen)
        {
        case 1: case 2: case 3: case 4:
        case 5: ch->pcdata->csbackgrounds[CSBACK_GENERATION] = 5; break;
        case 6: ch->pcdata->csbackgrounds[CSBACK_GENERATION] = 4; break;
        case 7: ch->pcdata->csbackgrounds[CSBACK_GENERATION] = 3; break;
        case 8: ch->pcdata->csbackgrounds[CSBACK_GENERATION] = 2; break;
        case 9: ch->pcdata->csbackgrounds[CSBACK_GENERATION] = 1; break;
        }
    }*/

    if (is_clan(ch) && !IS_SET(ch->act2, PLR2_LEADER) && !str_cmp(clan_table[ch->clan].leader, ch->name))
    {
        printf_to_char(ch, "You have been set as the leader of the %s clan while you were offline!\n\r", clan_table[ch->clan].name);
        SET_BIT(ch->act2, PLR2_LEADER);
    }
    if (is_clan(ch) && IS_SET(ch->act2, PLR2_LEADER) && str_cmp(clan_table[ch->clan].leader, ch->name))
    {
        printf_to_char(ch, "You have been removed as the leader of the %s clan and replaced by %s.\n\r", clan_table[ch->clan].name, clan_table[ch->clan].leader);
        REMOVE_BIT(ch->act2, PLR2_LEADER);
    }

//    if(!ch->pcdata->registered)
//        send_to_char("{WYou need to register a {Rvalid email address{W to be able to remort. Do this with the 'register' command.{x\n\r",ch);

    break;
    }

    return;
}



/*
 * Parse a name for acceptability.
 */
bool check_parse_name( char *name )
{
    int clan;

    /*
     * Reserved words.
     */
    if (is_exact_name(name,
        "all auto immortal self someone something the you loner none"))
    return FALSE;


    /* check clans */
 /*   for (clan = 0; clan < MAX_CLAN; clan++)
    {
    if (LOWER(name[0]) == LOWER(clan_table[clan].name[0])
    &&  !str_cmp(name,clan_table[clan].name))
       return FALSE;
    }
*/
    /*
     * Length restrictions.
     */

    if ( strlen(name) <  2 )
    return FALSE;

#if defined(MSDOS)
    if ( strlen(name) >  8 )
    return FALSE;
#endif

#if defined(macintosh) || defined(unix)
    if ( strlen(name) > 12 )
    return FALSE;
#endif

    /*
     * Alphanumerics only.
     * Lock out IllIll twits.
     */
    {
    char *pc;
    bool fIll,adjcaps = FALSE,cleancaps = FALSE;
    int total_caps = 0;
    fIll = TRUE;
    for ( pc = name; *pc != '\0'; pc++ )
    {
        if ( !isalpha(*pc) )
        return FALSE;

        if ( isupper(*pc)) /* ugly anti-caps hack */
        {
        if (adjcaps)
            cleancaps = TRUE;
        total_caps++;
        adjcaps = TRUE;
        }
        else
        adjcaps = FALSE;

        if ( LOWER(*pc) != 'i' && LOWER(*pc) != 'l' )
        fIll = FALSE;
    }

    if ( fIll )
        return FALSE;

    if (cleancaps || (total_caps > (strlen(name)) / 2 && strlen(name) < 3))
        return FALSE;
    }

    /*
     * Prevent players from naming themselves after mobs.
     */
    {
    extern MOB_INDEX_DATA *mob_index_hash[MAX_KEY_HASH];
    MOB_INDEX_DATA *pMobIndex;
    int iHash;
    for ( iHash = 0; iHash < MAX_KEY_HASH; iHash++ )
    {
        for ( pMobIndex  = mob_index_hash[iHash];
          pMobIndex != NULL;
          pMobIndex  = pMobIndex->next )
        {
        if ( is_name( name, pMobIndex->player_name ) )
            return FALSE;
        }
    }
    }

    return TRUE;
}



/*
 * Look for link-dead player to reconnect.
 */
bool check_reconnect( DESCRIPTOR_DATA *d, char *name, bool fConn )
{
    CHAR_DATA *ch;

    for ( ch = char_list; ch != NULL; ch = ch->next )
    {
    if ( !IS_NPC(ch)
    &&   (!fConn || ch->desc == NULL)
    &&   !str_cmp( d->character->name, ch->name ) )
    {
        if ( fConn == FALSE )
        {
        free_string( d->character->pcdata->pwd );
        d->character->pcdata->pwd = str_dup( ch->pcdata->pwd );
        }
        else
        {
        free_char( d->character );
        d->character = ch;
        ch->desc     = d;
        ch->timer    = 0;
        send_to_char(
            "Reconnecting. Type replay to see missed tells.\n\r", ch );
        act( "$n has reconnected.", ch, NULL, NULL, TO_ROOM );

        sprintf( log_buf, "%s@%s reconnected.", ch->name, d->host );
        log_string( log_buf );
        wiznet("$N groks the fullness of their link.",
            ch,NULL,WIZ_LINKS,0,0);
        d->connected = CON_PLAYING;
        }
        return TRUE;
    }
    }

    return FALSE;
}



/*
 * Check if already playing.
 */
bool check_playing( DESCRIPTOR_DATA *d, char *name )
{
    DESCRIPTOR_DATA *dold;

    for ( dold = descriptor_list; dold; dold = dold->next )
    {
    if ( dold != d
    &&   dold->character != NULL
    &&   dold->connected != CON_GET_NAME
    &&   dold->connected != CON_GET_OLD_PASSWORD
    &&   !str_cmp( name, dold->original
             ? dold->original->name : dold->character->name ) )
    {
        write_to_buffer( d, "That character is already playing.\n\r",0);
        write_to_buffer( d, "Do you wish to connect anyway (Y/N)?",0);
        d->connected = CON_BREAK_CONNECT;
        return TRUE;
    }
    }

    return FALSE;
}



void stop_idling( CHAR_DATA *ch )
{
    if ( ch == NULL
    ||   ch->desc == NULL
    ||   ch->desc->connected != CON_PLAYING
    ||   ch->was_in_room == NULL
    ||   ch->in_room != get_room_index(ROOM_VNUM_LIMBO))
    return;

    ch->timer = 0;
    char_from_room( ch );
    char_to_room( ch, ch->was_in_room );
    ch->was_in_room = NULL;
    act( "$n has returned from the void.", ch, NULL, NULL, TO_ROOM );
    wiznet("$N has returned from the void.",ch,NULL,WIZ_LINKS,0,0);
    return;
}



/*
 * Write to one char.
 */
void send_to_char_bw( const char *txt, CHAR_DATA *ch )
{
    if ( txt != NULL && ch->desc != NULL )
        write_to_buffer( ch->desc, txt, strlen(txt) );
    return;
}
char temp_HSL_workspace[HSL];
//prepend string with \n\r, wordwrap.
void wrapln(CHAR_DATA *ch, const char *buf)
{
    char *wrapped=str_dup(buf);
    wrapped=wrapstring(wrapped);
    cprintf(ch, "%s", wrapped);
    free_string(wrapped);
}
//prepend string with \n\r, wordwrap.
void wraplnf(CHAR_DATA *ch, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vsprintf(temp_HSL_workspace, fmt, args);
    va_end(args);

    char *wrapped=str_dup(temp_HSL_workspace);
    wrapped=wrapstring(wrapped);
    cprintf(ch, "%s", wrapped);
    free_string(wrapped);
    free_string(temp_HSL_workspace);
}
//Channel wrapper, so it records to replayroom.
void channel_to_char(const char *txt, CHAR_DATA*ch)
{
    if (!ch || IS_NPC(ch))
    return;

    sendch(txt, ch);
    record_replaychannels_event(ch, txt);
    return;

}


/*
 * Write to one char, new colour version, by Lope.
 */
void send_to_char( const char *txt, CHAR_DATA *ch )
{
    const   char    *point;
            char    *point2;
            char    buf[ MAX_STRING_LENGTH*4 ];
        int skip = 0;

    buf[0] = '\0';
    point2 = buf;
    if( txt && ch->desc )
    {
        if( IS_SET( ch->act, PLR_COLOUR ) )
        {
        for( point = txt ; *point ; point++ )
            {
            if( *point == '{' )
            {
            point++;
            if(*point == '<')
            {
                skip = colour2(*++point,ch,point2);
            }
            else if (*point == '?')
                skip = colour( randomcolors[number_range(0, MAX_RANDOM)], ch, point2 );
            else
                skip = colour( *point, ch, point2 );
            while( skip-- > 0 )
                ++point2;
            continue;
            }
            *point2 = *point;
            *++point2 = '\0';
        }
        *point2 = '\0';
            write_to_buffer( ch->desc, buf, point2 - buf );
        }
        else
        {
        for( point = txt ; *point ; point++ )
            {
            if( *point == '{' )
            {
            point++;
            continue;
            }
            *point2 = *point;
            *++point2 = '\0';
        }
        *point2 = '\0';
            write_to_buffer( ch->desc, buf, point2 - buf );
        }
    }
    return;
}

/*
 * Send a page to one char.
 */
void page_to_char_bw( const char *txt, CHAR_DATA *ch )
{
    if ( txt == NULL || ch->desc == NULL)
    return;

    if (ch->lines == 0 )
    {
    send_to_char(txt,ch);
    return;
    }

#if defined(macintosh)
    send_to_char(txt,ch);
#else
    ch->desc->showstr_head = alloc_mem(strlen(txt) + 1);
    strcpy(ch->desc->showstr_head,txt);
    ch->desc->showstr_point = ch->desc->showstr_head;
    show_string(ch->desc,"");
#endif
}

/*
 * Page to one char, new colour version, by Lope.
 */
void page_to_char( const char *txt, CHAR_DATA *ch )
{
    const char *point;
    char *point2;
    char buf[ MAX_STRING_LENGTH * 4 ];
    char text[ MAX_STRING_LENGTH * 4];
    int skip = 0;

    buf[0] = '\0';
    point2 = buf;


         strcpy(text, txt);

    if(ch->desc)
    {
     if( IS_SET( ch->act, PLR_COLOUR ) )
     {
         for( point = text ; *point ; point++ )
         {
              if( *point == '{' )
              {
                 point++;

                if(*point == '<')
                {
                    skip = colour2(*++point,ch,point2);
                } else if (*point == '?')
                skip = colour( randomcolors[number_range(0, MAX_RANDOM)], ch, point2 );
                else
                    skip = colour( *point, ch, point2 );
                 while( skip-- > 0 )
                    ++point2;
                 continue;
              }
              *point2 = *point;
              *++point2 = '\0';
         }
         *point2 = '\0';

         ch->desc->showstr_head  = alloc_mem( strlen( buf ) + 1 );
         strcpy( ch->desc->showstr_head, buf );
         ch->desc->showstr_point = ch->desc->showstr_head;
         show_string( ch->desc, "" );
     }
     else
     {
         for( point = txt ; *point ; point++ )
         {
              if( *point == '{' )
              {
                  point++;
                  continue;
              }
              *point2 = *point;
              *++point2 = '\0';
         }
         *point2 = '\0';
         ch->desc->showstr_head  = alloc_mem( strlen( buf ) + 1 );
         strcpy( ch->desc->showstr_head, buf );
         ch->desc->showstr_point = ch->desc->showstr_head;
         show_string( ch->desc, "" );
     }
    }
    return;
}


/* string pager */
void show_string(struct descriptor_data *d, char *input)
{
    char buffer[4*MAX_STRING_LENGTH];
    char buf[MAX_INPUT_LENGTH];
    register char *scan, *chk;
    int lines = 0, toggle = 1;
    int show_lines;

    one_argument(input,buf);
    if (buf[0] != '\0')
    {
    if (d->showstr_head)
    {
        free_mem(d->showstr_head,strlen(d->showstr_head));
        d->showstr_head = 0;
    }
        d->showstr_point  = 0;
    return;
    }

    if (d->character)
    show_lines = d->character->lines;
    else
    show_lines = 0;

    for (scan = buffer; ; scan++, d->showstr_point++)
    {
    if (((*scan = *d->showstr_point) == '\n' || *scan == '\r')
        && (toggle = -toggle) < 0)
        lines++;

    else if (!*scan || (show_lines > 0 && lines >= show_lines))
    {
        *scan = '\0';
        write_to_buffer(d,buffer,strlen(buffer));
        for (chk = d->showstr_point; isspace(*chk); chk++);
        {
        if (!*chk)
        {
            if (d->showstr_head)
                {
                    free_mem(d->showstr_head,strlen(d->showstr_head));
                    d->showstr_head = 0;
                }
                d->showstr_point  = 0;
            }
        }
        return;
    }
    }
    return;
}

/* Channel replay buffer storage.*/

// record says and emotes etc for display with the replayroom command
void record_replayroom_event(CHAR_DATA *ch, const char *txt)
{

    if(!ch || !ch->pcdata){
        // only record for players
        return;
    }

    char recordbuf[MSL];
    if(ch->pcdata->replayroom_lastevent_roomvnum!=ch->in_room->vnum){
        // insert a blank line with a - for new rooms
        sprintf(recordbuf, "-\r\n%s> %s", shorttime(NULL), txt);
        ch->pcdata->replayroom_lastevent_roomvnum=ch->in_room->vnum;
    }else{
        sprintf(recordbuf, "%s> %s", shorttime(NULL), txt);
    }

    replace_string( // record it in their replayroom buffer
        ch->pcdata->replayroom_text[ch->pcdata->next_replayroom],
        recordbuf);
        ch->pcdata->next_replayroom++;
    ch->pcdata->next_replayroom = ch->pcdata->next_replayroom %MAX_REPLAYROOM;
}

// record channels for display with the replaychan command
void record_replaychannels_event(CHAR_DATA *ch, const char *txt)
{

    if(!ch || !ch->pcdata){
        // only record for players
        return;
    }

    char recordbuf[MSL];
    sprintf(recordbuf, "%s> %s", shorttime(NULL), txt);


    replace_string( // record it in their replayroom buffer
        ch->pcdata->replaychannels_text[ch->pcdata->next_replaychannels],
        recordbuf);
        ch->pcdata->next_replaychannels++;
    ch->pcdata->next_replaychannels = ch->pcdata->next_replaychannels %MAX_REPLAYCHANNELS;
}
//Records tells for display with the replay command
void record_replay_event(CHAR_DATA *ch, const char *txt)
{

    if(!ch || !ch->pcdata){
        // only record for players
        return;
    }

    char recordbuf[MSL];
    sprintf(recordbuf, "%s> %s", shorttime(NULL), txt);


    replace_string( // record it in their replayroom buffer
        ch->pcdata->replaytell_text[ch->pcdata->next_replaytell],
        recordbuf);
        ch->pcdata->next_replaytell++;
    ch->pcdata->next_replaytell = ch->pcdata->next_replaytell %MAX_REPLAYTELL;
}
char *shorttime(time_t *tm) // kalahn - nov 97
{
    static int i;
    static char result[3][30];
    // rotate buffers
    i++;
    i = i % 3;
    result[i][0] = '\0';

    if(!tm){
        tm=&current_time;
    }

    char *tbuf = ctime( tm);
    tbuf[str_len(tbuf)-6] = '\0';

    strcpy(result[i], (char *)&tbuf[11]);

    return(result[i]);
}

/* quick sex fixer */
void fix_sex(CHAR_DATA *ch)
{
    if (ch->sex < 0 || ch->sex > 2)
        ch->sex = IS_NPC(ch) ? 0 : ch->pcdata->true_sex;
}

/*
 * The colour version of the act_new( ) function, -Lope
 */
void act_new( const char *format, CHAR_DATA *ch, const void *arg1,
              const void *arg2, int type, int min_pos, bool channel )
{
    static char * const he_she  [] = { "they",  "he",  "she" };
    static char * const him_her [] = { "them",  "him", "her" };
    static char * const his_her [] = { "their", "his", "her" };

    CHAR_DATA           *to;
    CHAR_DATA           *vch = ( CHAR_DATA * ) arg2;
    OBJ_DATA            *obj1 = ( OBJ_DATA  * ) arg1;
    OBJ_DATA            *obj2 = ( OBJ_DATA  * ) arg2;
    const       char    *str;
    char                *i = NULL;
    char                *point;
    char                *pbuff;
    char                buffer[ MAX_STRING_LENGTH*2 ];
    char                buf[ MAX_STRING_LENGTH   ];
    char                fname[ MAX_INPUT_LENGTH  ];
    bool                fColour = FALSE;

    /*
     * Discard null and zero-length messages.
     */
    if( !format || !*format )
        return;

    /* discard null rooms and chars */
    if( !ch || !ch->in_room )
        return;

    to = ch->in_room->people;
    if( type == TO_VICT )
    {
        if( !vch )
        {
            bug( "Act: null vch with TO_VICT.", 0 );
            return;
        }

        if( !vch->in_room )
            return;

        to = vch->in_room->people;
    }

    for( ; to ; to = to->next_in_room )
    {
        if ( (!IS_NPC(to) && to->desc == NULL )
                ||   ( IS_NPC(to) && !HAS_TRIGGER_MOB(to, TRIG_ACT) )
                ||    to->position < min_pos )
            continue;

        if( ( type == TO_CHAR ) && to != ch )
            continue;
        if( type == TO_VICT && ( to != vch || to == ch ) )
            continue;
        if( type == TO_ROOM && to == ch )
            continue;
        if( type == TO_NOTVICT && (to == ch || to == vch) )
            continue;
        if( type == TO_NEWBIECH && (to != ch || !IS_NEWBIE(ch)))
            continue;
        if (type == TO_NEWBIEALL && !IS_NEWBIE(to))
            continue;
        if(!channel && ((IS_AFFECTED2(ch, AFF2_UMBRA) && !IS_AFFECTED2(to, AFF2_UMBRA)) || (!IS_AFFECTED2(ch, AFF2_UMBRA) && IS_AFFECTED2(to, AFF2_UMBRA))))
            continue;

/*
        if(!IS_IMMORTAL(to) && IS_AFFECTED2(ch, AFF2_UMBRA) && !IS_AFFECTED2(to, AFF2_UMBRA))
            continue;
        if((type == TO_NOTVICT || type == TO_VICT) && !IS_IMMORTAL(to) && IS_AFFECTED2(vch, AFF2_UMBRA) && !IS_AFFECTED2(to, AFF2_UMBRA))
            continue;
*/


        point   = buf;
        str     = format;
        while( *str != '\0' )
        {
            if( *str != '$' )
            {
                *point++ = *str++;
                continue;
            }

            fColour = TRUE;
            ++str;
            i = " <@@@> ";
            if( !arg2 && *str >= 'A' && *str <= 'Z' )
            {
                bug( "Act: missing arg2 for code %d.", *str );
                i = " <@@@> ";
            }
            else
            {
                switch ( *str )
                {
                default:  bug( "Act: bad code %d.", *str );
                          i = " <@@@> ";                                break;
                /* Thx alex for 't' idea */
                case 't': i = (char *) arg1;                            break;
                case 'T': i = (char *) arg2;                            break;
                case 'n': i = PERS( ch,  to, channel  );                break;
                case 'N': i = PERS( vch, to, channel  );                break;
                case 'e': i = he_she  [URANGE(0, ch  ->sex, 2)];        break;
                case 'E': i = he_she  [URANGE(0, vch ->sex, 2)];        break;
                case 'm': i = him_her [URANGE(0, ch  ->sex, 2)];        break;
                case 'M': i = him_her [URANGE(0, vch ->sex, 2)];        break;
                case 's': i = his_her [URANGE(0, ch  ->sex, 2)];        break;
                case 'S': i = his_her [URANGE(0, vch ->sex, 2)];        break;

                case 'p':
                    i = can_see_obj( to, obj1 )
                            ? obj1->short_descr
                            : "something";
                    break;

                case 'P':
                    i = can_see_obj( to, obj2 )
                            ? obj2->short_descr
                            : "something";
                    break;

                case 'd':
                    if ( arg2 == NULL || ((char *) arg2)[0] == '\0' )
                    {
                        i = "door";
                    }
                    else
                    {
                        one_argument( (char *) arg2, fname );
                        i = fname;
                    }
                    break;
                }
            }

            ++str;
            while ( ( *point = *i ) != '\0' )
                ++point, ++i;
        }

        *point++ = '\n';
        *point++ = '\r';
            *point       = '\0';
        buf[0]   = UPPER(buf[0]);
        pbuff    = buffer;

        colourconv( pbuff, buf, to );


        // display it to the player/mob with a moblog/act trigger


                    if ( to->desc != NULL )
                    {
                    sendch(buf, to);
                if(RECORD_TO_REPLAYROOM){
                    record_replayroom_event(to, buf);
                } else if (channel) {
                    record_replaychannels_event(to, buf);
                }
            }


                if ( IS_NPC(to) && MOBtrigger )
                    p_act_trigger( buf, to, NULL, NULL, ch, arg1, arg2, TRIG_ACT );

                    if ( type == TO_ROOM || type == TO_NOTVICT )
                    {
                    	OBJ_DATA *obj, *obj_next;
                    	CHAR_DATA *tch, *tch_next;

                    	 point   = buf;
                    	 str     = format;
                    	 while( *str != '\0' )
                    	 {
                    	     *point++ = *str++;
                    	 }
                    	 *point   = '\0';

                    	for( obj = ch->in_room->contents; obj; obj = obj_next )
                    	{
                    	    obj_next = obj->next_content;
                    	    if ( HAS_TRIGGER_OBJ( obj, TRIG_ACT ) )
                    		p_act_trigger( buf, NULL, obj, NULL, ch, NULL, NULL, TRIG_ACT );
                    	}

                    	for( tch = ch; tch; tch = tch_next )
                    	{
                    	    tch_next = tch->next_in_room;

                    	    for ( obj = tch->carrying; obj; obj = obj_next )
                    	    {
                    		obj_next = obj->next_content;
                    		if ( HAS_TRIGGER_OBJ( obj, TRIG_ACT ) )
                    		    p_act_trigger( buf, NULL, obj, NULL, ch, NULL, NULL, TRIG_ACT );
                	    }
                	}

                	 if ( HAS_TRIGGER_ROOM( ch->in_room, TRIG_ACT ) )
                	     p_act_trigger( buf, NULL, NULL, ch->in_room, ch, NULL, NULL, TRIG_ACT );
                    }

        }
    return;
}


void act_new2( const char *format, CHAR_DATA *ch, const void *arg1,
              const void *arg2, int type, int min_pos )
{
    static char * const he_she  [] = { "they",  "he",  "she" };
    static char * const him_her [] = { "them",  "him", "her" };
    static char * const his_her [] = { "their", "his", "her" };

    CHAR_DATA           *to;
    CHAR_DATA           *vch = ( CHAR_DATA * ) arg2;
    OBJ_DATA            *obj1 = ( OBJ_DATA  * ) arg1;
    OBJ_DATA            *obj2 = ( OBJ_DATA  * ) arg2;
    const       char    *str;
    char                *i = NULL;
    char                *point;
    char                *pbuff;
    char                buffer[ MAX_STRING_LENGTH*2 ];
    char                buf[ MAX_STRING_LENGTH   ];
    char                fname[ MAX_INPUT_LENGTH  ];
    bool                fColour = FALSE;


    /*
     * Discard null and zero-length messages.
     */
    if( !format || !*format )
        return;

    /* discard null rooms and chars */
    if( !ch || !ch->in_room )
        return;

    to = ch->in_room->people;
    if( type == TO_VICT )
    {
        if( !vch )
        {
            bug( "Act: null vch with TO_VICT.", 0 );
            return;
        }

        if( !vch->in_room )
            return;

        to = vch->in_room->people;
    }

    for( ; to ; to = to->next_in_room )
    {
        if ( (!IS_NPC(to) && to->desc == NULL )
                ||   ( IS_NPC(to) && !HAS_TRIGGER_MOB(to, TRIG_ACT) )
                ||    to->position < min_pos )
            continue;

        if( ( type == TO_CHAR ) && to != ch )
            continue;
        if( type == TO_VICT && ( to != vch || to == ch ) )
            continue;
        if( type == TO_ROOM && to == ch )
            continue;
        if( type == TO_NOTVICT && (to == ch || to == vch) )
            continue;
        if(IS_SET(to->comm, COMM_COMBAT_BRIEF))
            continue;
        if((IS_AFFECTED2(ch, AFF2_UMBRA) && !IS_AFFECTED2(to, AFF2_UMBRA)) || (!IS_AFFECTED2(ch, AFF2_UMBRA) && IS_AFFECTED2(to, AFF2_UMBRA)))
            continue;
/*
        if(IS_AFFECTED2(ch, AFF2_UMBRA) && !IS_AFFECTED2(to, AFF2_UMBRA))
            continue;
        if(IS_AFFECTED2(vch, AFF2_UMBRA) && !IS_AFFECTED2(to, AFF2_UMBRA))
            continue;
*/
        point   = buf;
        str     = format;
        while( *str != '\0' )
        {
            if( *str != '$' )
            {
                *point++ = *str++;
                continue;
            }

            fColour = TRUE;
            ++str;
            i = " <@@@> ";
            if( !arg2 && *str >= 'A' && *str <= 'Z' )
            {
                bug( "Act: missing arg2 for code %d.", *str );
                i = " <@@@> ";
            }
            else
            {
                switch ( *str )
                {
                default:  bug( "Act: bad code %d.", *str );
                          i = " <@@@> ";                                break;
                /* Thx alex for 't' idea */
                case 't': i = (char *) arg1;                            break;
                case 'T': i = (char *) arg2;                            break;
                case 'n': i = PERS( ch,  to, FALSE);                    break;
                case 'N': i = PERS( vch, to, FALSE);                    break;
                case 'e': i = he_she  [URANGE(0, ch  ->sex, 2)];        break;
                case 'E': i = he_she  [URANGE(0, vch ->sex, 2)];        break;
                case 'm': i = him_her [URANGE(0, ch  ->sex, 2)];        break;
                case 'M': i = him_her [URANGE(0, vch ->sex, 2)];        break;
                case 's': i = his_her [URANGE(0, ch  ->sex, 2)];        break;
                case 'S': i = his_her [URANGE(0, vch ->sex, 2)];        break;

                case 'p':
                    i = can_see_obj( to, obj1 )
                            ? obj1->short_descr
                            : "something";
                    break;

                case 'P':
                    i = can_see_obj( to, obj2 )
                            ? obj2->short_descr
                            : "something";
                    break;

                case 'd':
                    if ( arg2 == NULL || ((char *) arg2)[0] == '\0' )
                    {
                        i = "door";
                    }
                    else
                    {
                        one_argument( (char *) arg2, fname );
                        i = fname;
                    }
                    break;
                }
            }

            ++str;
            while ( ( *point = *i ) != '\0' )
                ++point, ++i;
        }

        *point++ = '\n';
        *point++ = '\r';
            *point       = '\0';
        buf[0]   = UPPER(buf[0]);
        pbuff    = buffer;

        colourconv( pbuff, buf, to );
        if ( to->desc != NULL )
        write_to_buffer( to->desc, buffer, 0 );
        else
                if ( MOBtrigger )
                    p_act_trigger( buf, to, NULL, NULL, ch, arg1, arg2, TRIG_ACT );
    }

    return;
}




/*
 * Macintosh support functions.
 */
#if defined(macintosh)
int gettimeofday( struct timeval *tp, void *tzp )
{
    tp->tv_sec  = time( NULL );
    tp->tv_usec = 0;
}
#endif

int colour( char type, CHAR_DATA *ch, char *string )
{
    char    code[ 20 ];
    char    *p = NULL;

    if( IS_NPC( ch ) )
    return( 0 );


        switch( type )
        {
        default:
            sprintf(code, "{%c",type);
//          sprintf( code, CLEAR );
            break;
        case 'x':
            sprintf( code, CLEAR );
            break;
        case 'b':
            sprintf( code, C_BLUE );
            break;
        case 'c':
            sprintf( code, C_CYAN );
            break;
        case 'g':
            sprintf( code, C_GREEN );
            break;
        case 'm':
            sprintf( code, C_MAGENTA );
            break;
        case 'r':
            sprintf( code, C_RED );
            break;
        case 'w':
            sprintf( code, C_WHITE );
            break;
        case 'y':
            sprintf( code, C_YELLOW );
            break;
        case 'B':
            sprintf( code, C_B_BLUE );
            break;
        case 'C':
            sprintf( code, C_B_CYAN );
            break;
        case 'G':
            sprintf( code, C_B_GREEN );
            break;
        case 'M':
            sprintf( code, C_B_MAGENTA );
            break;
        case 'R':
            sprintf( code, C_B_RED );
            break;
        case 'W':
            sprintf( code, C_B_WHITE );
            break;
        case 'Y':
            sprintf( code, C_B_YELLOW );
            break;
        case 'D':
            sprintf( code, C_D_GREY );
            break;
        case '/':
                sprintf( code, "%c", 012 );
            break;
        case '{':
            sprintf( code, "%c", '{' );
            break;

        }
    p = code;
    while( *p != '\0' )
    {
    *string = *p++;
    *++string = '\0';
    }

    return( strlen( code ) );
}

char colour2(char type, CHAR_DATA *ch, char *string)
{
    char    code[ 20 ];
    char    *p = NULL;

    if( IS_NPC( ch ) )
    return( 0 );

    switch( type )
    {
        default:
            sprintf( code, CLEAR );
            break;
        case 'D':
            sprintf( code, "[40m" );
            break;
        case 'R':
            sprintf( code, "[41m" );
            break;
        case 'G':
            sprintf( code, "[42m" );
            break;
        case 'Y':
            sprintf( code, "[43m" );
            break;
        case 'B':
            sprintf( code, "[44m" );
            break;
        case 'M':
            sprintf( code, "[45m" );
            break;
        case 'C':
            sprintf( code, "[46m" );
            break;
        case 'W':
            sprintf( code, "[47m" );
            break;
        case '^':
            sprintf( code, "[7m" );
            break;
        case '*':
            sprintf( code, "%c", 007 );
            break;
        case '/':
            sprintf( code, "%c", 012 );
            break;
    }

        p = code;
        while( *p != '\0' )
        {
        *string = *p++;
        *++string = '\0';
        }

    return( strlen( code ) );

}

void colourconv( char *buffer, const char *txt, CHAR_DATA *ch )
{
    const   char    *point;
        int skip = 0;

    if( ch->desc && txt )
    {
    if( IS_SET( ch->act, PLR_COLOUR ))
    {
        for( point = txt ; *point ; point++ )
        {
        if( *point == '{' )
        {
            point++;
            if(*point == '<')
            {
                skip = colour2(*++point,ch,buffer);
            } else if (*point == '?')
                skip = colour( randomcolors[number_range(0, MAX_RANDOM)], ch, buffer );
            else
                skip = colour( *point, ch, buffer );
            while( skip-- > 0 )
            ++buffer;
            continue;
        }
        *buffer = *point;
        *++buffer = '\0';
        }
        *buffer = '\0';
    }
    else
    {
        for( point = txt ; *point ; point++ )
        {
        if( *point == '{' )
        {
            point++;
            continue;
        }
        *buffer = *point;
        *++buffer = '\0';
        }
        *buffer = '\0';
    }
    }
    return;
}


/* source: EOD, by John Booth <???> */
void cprintf (CHAR_DATA *ch, char *fmt, ...)
{
    char buf [MAX_STRING_LENGTH];
    va_list args;
    va_start (args, fmt);
    vsprintf (buf, fmt, args);
    va_end (args);

    send_to_char (buf, ch);
}


void printf_to_char (CHAR_DATA *ch, char *fmt, ...)
{
    char buf [MAX_STRING_LENGTH];
    va_list args;
    va_start (args, fmt);
    vsprintf (buf, fmt, args);
    va_end (args);

    send_to_char (buf, ch);
}

void bugf (char * fmt, ...)
{
    char buf [2*MSL];
    va_list args;
    va_start (args, fmt);
    vsprintf (buf, fmt, args);
    va_end (args);

    bug (buf, 0);
}

void init_signals()
{
  signal(SIGBUS,sig_handler);
  signal(SIGTERM,sig_handler);
  signal(SIGABRT,sig_handler);
  signal(SIGSEGV,sig_handler);
}

void sig_handler(int sig)
{
  switch(sig)
  {
    case SIGBUS:
      bug("Sig handler SIGBUS.",0);
      do_auto_shutdown();
      break;
    case SIGTERM:
      bug("Sig handler SIGTERM.",0);
      do_auto_shutdown();
      break;
    case SIGABRT:
      bug("Sig handler SIGABRT",0);
      do_auto_shutdown();
     case SIGSEGV:
      bug("Sig handler SIGSEGV",0);
      do_auto_shutdown();
      break;
  }
}
/**************************************************************************/
// redirect to str_len(), check the result to make sure we don't do a
// signed overflow
int str_len(const char *s)
{
    size_t l=strlen(s);
    int r=(int)l;
    assert(r>=0);
    return r;
}
void titlebar(CHAR_DATA *ch)
{
        sendch("{D-===========================================================================-{x\n\r", ch);
        return;
}


void save_config( void )
{
    FILE *fp;

    fclose( fpReserve );
    if ( !( fp = fopen( CONFIG_FILE, "w" ) ) )
    {
    bug( "save_config: fopen", 0 );
    perror( CONFIG_FILE );
    }

    fprintf( fp, "Slaughter %d\n",      slaughter);
    fprintf( fp, "Doubledam %d\n",      doubledam);
    fprintf( fp, "Doubleexp %d\n",      doubleexp);
    fprintf( fp, "Nosun %d\n",          nosun);
    fprintf( fp, "Spookums %d\n",       spookums);
    fprintf( fp, "Arena %d\n",          arena);
    fprintf( fp, "Resolver %d\n",       resolver);
    fprintf (fp, "Wizlock %d\n",        wizlock);
    fprintf (fp, "Newlock %d\n",        newlock);
    fprintf (fp, "Gxp %d\n",            global_xp);
    fprintf (fp, "Gqp %d\n",            global_qp);
    fprintf (fp, "Xpmult %d\n",         xpawardmult);
    fprintf (fp, "Manualxp %d\n",       manualxp);
    fprintf (fp, "Maxon %d\n",          max_players);

    fprintf( fp, "End\n\n\n\n" );
    fclose( fp );
    fpReserve = fopen( NULL_FILE, "r" );
    return;
}



#if defined(KEY)
#undef KEY
#endif

#define KEY( literal, field, value )                \
                if ( !str_cmp( word, literal ) )    \
                {                                   \
                    field  = value;                 \
                    fMatch = TRUE;                  \
                    break;                          \
                                }

#define SKEY( string, field )                       \
                if ( !str_cmp( word, string ) )     \
                {                                   \
                    free_string( field );           \
                    field = fread_string( fp );     \
                    fMatch = TRUE;                  \
                    break;                          \
                                }


void load_config( void )
{
    char      *word;
    bool      fMatch;
    FILE *fp;

    fclose( fpReserve );
    if ( !( fp = fopen( CONFIG_FILE, "r" ) ) )
    {
    bug( "load_config: fopen", 0 );
    perror( CONFIG_FILE );
    }


    for ( ; ; )
    {
       word   = feof( fp ) ? "End" : fread_word( fp );
       fMatch = FALSE;

       switch ( UPPER(word[0]) )
       {
          case 'A':
               KEY( "Arena", arena, fread_number(fp) );
               break;
           case 'R':
               KEY("Resolver", resolver, fread_number(fp) );
               break;
           case 'N':
               KEY("Nosun", nosun, fread_number(fp) );
               KEY("Newlock", newlock, fread_number(fp));
               break;
           case 'M':
               KEY( "Manualxp", manualxp, fread_number(fp));
               KEY( "Maxon", max_players, fread_number(fp));
               break;
          case 'D':
                KEY( "Doubleexp", doubleexp, fread_number(fp) );
                KEY( "Doubledam", doubledam, fread_number(fp) );
                break;
           case 'G':
               KEY("Gxp", global_xp, fread_number(fp) );
               KEY("Gqp", global_qp, fread_number(fp) );
               break;
           case 'X':
               KEY("Xpmult", xpawardmult, fread_number(fp));
               break;
           case 'S':
               KEY( "Slaughter", slaughter, fread_number(fp) );
               KEY( "Spookums", spookums, fread_number(fp) );
               break;
           case 'C':
               KEY("Carnival", carnival, fread_number(fp));

           case 'E':
                if ( !str_cmp( word, "End" ) )
                {
                    fMatch = TRUE;
                    fclose( fp );
                    fpReserve = fopen( NULL_FILE, "r" );
                    return;
                }
           case 'W':
               KEY("Wizlock", wizlock, fread_number(fp));
               break;

        }
    }
}
