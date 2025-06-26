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
 * Accommodate old non-Ansi compilers.
 */
#if defined(TRADITIONAL)
#define const
#define args( list )            ( )
#define DECLARE_DO_FUN( fun )       void fun( )
#define DECLARE_SPEC_FUN( fun )     bool fun( )
#define DECLARE_SPELL_FUN( fun )    void fun( )
#define DECLARE_ROTE_FUN( fun )     void fun( )
#define DECLARE_OBJ_FUN( fun )		void fun( )
#define DECLARE_ROOM_FUN( fun )		void fun( )
#else
#define args( list )            list
#define DECLARE_DO_FUN( fun )       DO_FUN    fun
#define DECLARE_SPEC_FUN( fun )     SPEC_FUN  fun
#define DECLARE_SPELL_FUN( fun )    SPELL_FUN fun
#define DECLARE_ROTE_FUN( fun )     ROTE_FUN fun
#define DECLARE_OBJ_FUN( fun )		OBJ_FUN	  fun
#define DECLARE_ROOM_FUN( fun )		ROOM_FUN  fun
#endif

/* system calls */
int unlink();
int system();

#define DECLARE_REPEAT_FUN(fun, repeat) void fun (CHAR_DATA *ch, char *argument) \
        {do_function(ch, repeat, argument);}



// Special XP global and QP
extern int global_qp;
extern int global_xp;
extern int qpmult; //Sets rate that QP is added to global qp
extern int xpmult; //sets rate that XP is added to global qp
extern int xpawardmult; // Set double xp multiplier
extern int xpstat; //Sets the milestone status for firing double xp
/*
 * Short scalar types.
 * Diavolo reports AIX compiler has bugs with short types.
 */
#if !defined(FALSE)
#define FALSE    0
#endif

#if !defined(TRUE)
#define TRUE     1
#endif

#if defined(_AIX)
#if !defined(const)
#define const
#endif
typedef int             sh_int;
typedef int             bool;
#define unix
#else
typedef short   int         sh_int;
typedef unsigned char           bool;
#endif

/* ea */
#define MSL MAX_STRING_LENGTH
#define MIL MAX_INPUT_LENGTH


/*
 * Structure types.
 */
typedef struct  affect_data     AFFECT_DATA;
typedef struct  area_data       AREA_DATA;
typedef struct  ban_data        BAN_DATA;
typedef struct  buf_type        BUFFER;
typedef struct  char_data       CHAR_DATA;
typedef struct  descriptor_data     DESCRIPTOR_DATA;
typedef struct  exit_data       EXIT_DATA;
typedef struct  extra_descr_data    EXTRA_DESCR_DATA;
typedef struct  help_data       HELP_DATA;
typedef struct  help_area_data      HELP_AREA;
typedef struct  kill_data       KILL_DATA;
typedef struct  mem_data        MEM_DATA;
typedef struct  mob_index_data      MOB_INDEX_DATA;
typedef struct  note_data       NOTE_DATA;
typedef struct  obj_data        OBJ_DATA;
typedef struct  obj_index_data      OBJ_INDEX_DATA;
typedef struct  pc_data         PC_DATA;
typedef struct  sleep_data      SLEEP_DATA;
typedef struct  quest_data      QUEST_DATA;
typedef struct  gen_data        GEN_DATA;
typedef struct  reset_data      RESET_DATA;
typedef struct  room_index_data     ROOM_INDEX_DATA;
typedef struct  shop_data       SHOP_DATA;
typedef struct  time_info_data      TIME_INFO_DATA;
typedef struct  weather_data        WEATHER_DATA;
typedef struct  config_data         CONFIG_DATA;
// typedef struct   database_data       DATABASE_DATA;  non-functional database code
typedef struct  prog_list      PROG_LIST;
typedef struct  prog_code      PROG_CODE;
typedef struct    clan_type        CLAN_DATA;


/*
 * Function types.
 */
typedef void DO_FUN args( ( CHAR_DATA *ch, char *argument ) );
typedef bool SPEC_FUN   args( ( CHAR_DATA *ch ) );
typedef void SPELL_FUN  args( ( int sn, int level, CHAR_DATA *ch, void *vo, int target ) );
typedef void ROTE_FUN   args( ( CHAR_DATA *ch, int success, CHAR_DATA *victim, OBJ_DATA *obj ) );
typedef void VAMP_FUN   args( ( CHAR_DATA *ch ) );
typedef void OBJ_FUN	args( ( OBJ_DATA *obj, char *argument ) );
typedef void ROOM_FUN	args( ( ROOM_INDEX_DATA *room, char *argument ) );

/* I am lazy :) */
#define MSL MAX_STRING_LENGTH
#define MIL MAX_INPUT_LENGTH


/*
 * String and memory management parameters.
 */
#define MAX_KEY_HASH         1024
#define MAX_STRING_LENGTH    9216
#define HUGE_STRING_LENGTH  (MAX_STRING_LENGTH * 15)
#define HSL                 HUGE_STRING_LENGTH
#define MAX_INPUT_LENGTH     2048
#define PAGELEN            22


/*
 * Language stuff
 */

#define COMMON          0
#define HUMAN           1
#define DWARVISH        2
#define ELVISH          3
#define GNOMISH         4
#define GOBLIN          5
#define ORCISH          6
#define OGRE            7
#define DROW            8
#define KOBOLD          9
#define TROLLISH        10
#define HOBBIT          11

/* Umbral direction flags*/
#define UMBRA_EXIT      0
#define UMBRA_ENTER     1

/*
 * Game parameters.
 * Increase the max'es if you add more of something.
 * Adjust the pulse numbers to suit yourself.
 */
#define MAX_HELP        5
#define MAX_SOCIALS       256
#define MAX_SKILL         460 //5/31/25
#define MAX_GIFT          90
#define MAX_GIFTS_CODED   168
#define MAX_BREW        24
#define MAX_GROUP          45
#define MAX_IN_GROUP           30
#define MAX_ALIAS           10
#define MAX_CLASS           13
#define MAX_PC_RACE         1
#define MAX_CLAN            34
#define MAX_SECT            7
#define MAX_TRADITION       6
#define MAX_ROTE            55
#define MAX_DAMAGE_MESSAGE  42
#define MAX_TEST_LOOPS      3
#define MAX_LEVEL          110
#define MAX_QUEST           50
#define MAX_TOTEM           5
#define MAX_QPOINTS         30000
#define LEVEL_HERO         (MAX_LEVEL - 10)
#define LEVEL_IMMORTAL         (MAX_LEVEL - 9)
#define MAX_LANGUAGE               12 /* Number of languages */
#define MAX_HOMETOWN 		5

#define PULSE_PER_SECOND        4
#define PULSE_VIOLENCE          ( 3 * PULSE_PER_SECOND)
#define PULSE_MOBILE            ( 4 * PULSE_PER_SECOND)
#define PULSE_MUSIC             ( 6 * PULSE_PER_SECOND)
#define PULSE_TICK              (60 * PULSE_PER_SECOND)
#define PULSE_WEATHER           (3 * PULSE_TICK)
#define PULSE_BANK              (6 * PULSE_TICK)
#define PULSE_AREA              (3 * PULSE_TICK)
#define PULSE_INFLUENCECHANGE   (60* PULSE_TICK * 4 )
#define PULSE_INFLUENCEFADE     (PULSE_INFLUENCECHANGE * 12 )
#define PULSE_UNDERWATER        (20 * PULSE_PER_SECOND)

#define INFLUENCE_TIMER        60
#define INFLUENCE_KILL_CHANCE      5
#define INFLUENCE_RP_CHANCE         15
// Defines spell pulse
#define PULSE_AFFECTS   ( 3 * PULSE_PER_SECOND)
#define PULSE_REGEN     ( 3 * PULSE_PER_SECOND)

#define IMPLEMENTOR     MAX_LEVEL
#define CREATOR         (MAX_LEVEL - 1)
#define SUPREME         (MAX_LEVEL - 2)
#define DEITY           (MAX_LEVEL - 3)
#define GOD         (MAX_LEVEL - 4)
#define IMMORTAL        (MAX_LEVEL - 5)
#define DEMI            (MAX_LEVEL - 6)
#define ANGEL           (MAX_LEVEL - 7)
#define AVATAR          (MAX_LEVEL - 8)
#define HERO            LEVEL_HERO

// For that one exception you told yourself is OK.
#define MAGIC_NUMBER    42

// Channel replaybuffer maxs.
#define MAX_REPLAYTELL          (30)
#define MAX_REPLAYROOM          (20)
#define MAX_REPLAYCHANNELS      (40)
// number of lines of channel to remember for Imms.

/*
 * Colour stuff by Lope of Loping Through The MUD
 */
#define CLEAR       "[0m"      /* Resets Colour    */
#define C_RED       "[0;31m"   /* Normal Colours   */
#define C_GREEN     "[0;32m"
#define C_YELLOW    "[0;33m"
#define C_BLUE      "[0;34m"
#define C_MAGENTA   "[0;35m"
#define C_CYAN      "[0;36m"
#define C_WHITE     "[0;37m"
#define C_D_GREY    "[1;30m"   /* Light Colors     */
#define C_B_RED     "[1;31m"
#define C_B_GREEN   "[1;32m"
#define C_B_YELLOW  "[1;33m"
#define C_B_BLUE    "[1;34m"
#define C_B_MAGENTA "[1;35m"
#define C_B_CYAN    "[1;36m"
#define C_B_WHITE   "[1;37m"

#define MAX_RANDOM 14
extern char randomcolors[MAX_RANDOM + 2];



#define ALIGN_NONE          0
#define ALIGN_LEFT          1
#define ALIGN_RIGHT         2
#define ALIGN_CENTER        3



/*
 * Site ban structure.
 */

#define BAN_SUFFIX      A
#define BAN_PREFIX      B
#define BAN_NEWBIES     C
#define BAN_ALL         D
#define BAN_PERMIT      E
#define BAN_PERMANENT       F

struct  ban_data
{
    BAN_DATA *  next;
    bool    valid;
    sh_int  ban_flags;
    sh_int  level;
    char *  name;
};

extern SLEEP_DATA *first_sleep;
/*
 *  Sleeping prog data
 */
struct sleep_data
{
    SLEEP_DATA *next;
    SLEEP_DATA *prev;
    CHAR_DATA *ch;
    CHAR_DATA *mob;
    OBJ_DATA *obj;
    ROOM_INDEX_DATA *room;
    PROG_CODE *prog;
    int valid;
    int vnum;
    int line;
    int timer;
};


struct buf_type
{
    BUFFER *    next;
    bool        valid;
    sh_int      state;  /* error state of the buffer */
    sh_int      size;   /* size in k */
    char *      string; /* buffer's string */
};

/*
 * Clan flags
 */
#define CLAN_DELETED            A
#define CLAN_CHANGED            B
#define CLAN_INDEPENDENT        C /* a "loner" guild */
#define CLAN_IMMORTAL           D /* immortal only clan */

#define TYPE_NONE           0
#define TYPE_CLAN           1
#define TYPE_TRADITION      2
#define TYPE_TRIBE          3


#define SPHERE_CORRESPONDENCE   1
#define SPHERE_ENTROPY          2
#define SPHERE_FORCES           3
#define SPHERE_LIFE             4
#define SPHERE_MATTER           5
#define SPHERE_MIND             6
#define SPHERE_PRIME            7
#define SPHERE_SPIRIT           8
#define SPHERE_TIME             9
#define MAX_SPHERE              9


/* charsheet code */
#define MAX_ATTRIBUTES          9
#define MAX_ABILITIES           30
#define MAX_SEC_ABILITIES       30
#define MAX_DISC                18
/* attributes */
#define CSATTRIB_STRENGTH       0
#define CSATTRIB_DEXTERITY      1
#define CSATTRIB_STAMINA        2
#define CSATTRIB_CHARISMA       3
#define CSATTRIB_MANIPULATION   4
#define CSATTRIB_APPEARANCE     5
#define CSATTRIB_PERCEPTION     6
#define CSATTRIB_INTELLIGENCE   7
#define CSATTRIB_WITS           8

#define STRENGTH                0
#define DEXTERITY               1
#define STAMINA                 2
#define CHARISMA                3
#define MANIPULATION            4
#define APPEARANCE              5
#define PERCEPTION              6
#define INTELLIGENCE            7
#define WITS                    8

/* abilities */
#define CSABIL_NONE             -1
#define CSABIL_EXPRESSION       0
#define CSABIL_ALERTNESS        1
#define CSABIL_ATHLETICS        2
#define CSABIL_BRAWL            3
#define CSABIL_DODGE            4
#define CSABIL_EMPATHY          5
#define CSABIL_INTIMIDATION     6
#define CSABIL_LEADERSHIP       7
#define CSABIL_LEGERDEMAIN       8
#define CSABIL_SUBTERFUGE       9
#define CSABIL_ANIMAL_KEN       10
#define CSABIL_RIDING           11
#define CSABIL_ETIQUETTE        12
#define CSABIL_ARCHERY          13
#define CSABIL_MELEE            14
#define CSABIL_PERFORMANCE      15
#define CSABIL_CRAFTS           16
#define CSABIL_COMMERCE         17
#define CSABIL_STEALTH          18
#define CSABIL_SURVIVAL         19
#define CSABIL_ACADEMICS        20
#define CSABIL_THEOLOGY         21
#define CSABIL_SENESCHAL        22
#define CSABIL_INVESTIGATION    23
#define CSABIL_LAW              24
#define CSABIL_LINGUISTICS      25
#define CSABIL_MEDICINE         26
#define CSABIL_OCCULT           27
#define CSABIL_POLITICS         28
#define CSABIL_HEARTHWISDOM     29
/* disciplines */
#define CSDISC_ANIMALISM        0
#define CSDISC_AUSPEX           1
#define CSDISC_CELERITY         2
#define CSDISC_CHIMERSTRY       3
#define CSDISC_DEMENTATION      4
#define CSDISC_DOMINATE         5
#define CSDISC_FORTITUDE        6
#define CSDISC_NECROMANCY       7
#define CSDISC_OBFUSCATE        8
#define CSDISC_OBTENEBRATION    9
#define CSDISC_POTENCE          10
#define CSDISC_PRESENCE         11
#define CSDISC_PROTEAN          12
#define CSDISC_QUIETUS          13
#define CSDISC_SERPENTIS        14
#define CSDISC_THAUMATURGY      15
#define CSDISC_VISCERATIKA      16
#define CSDISC_VICISSITUDE      17
#define CSDISC_MORTIS           18
#define CSDISC_DAIMOINON        19
/* backgrounds */
#define CSBACK_ALLIES           0
#define CSBACK_CONTACTS         1
#define CSBACK_FAME             2
#define CSBACK_INFLUENCE        5
#define CSBACK_MENTOR           6
#define CSBACK_RESOURCES        7
#define CSBACK_RETAINERS        8
#define CSBACK_GENERATION       3
#define CSBACK_HERD             4
#define CSBACK_STATUS           9
#define CSBACK_RITUALS          10
#define CSBACK_LIBRARY          11
#define CSBACK_FOUNT            12
#define CSBACK_FAMILIAR         13
#define CSBACK_WONDERS          14
#define CSBACK_ANCESTOR         15
#define CSBACK_PUREBREED        16
#define CSBACK_FETISH           17
#define CSBACK_RITES            18
#define CSBACK_GHOULEDAGE       19
#define CSBACK_NONE2            20
#define CSBACK_NONE3            21
#define CSBACK_NONE4            22
#define CSBACK_NONE5            23
#define CSBACK_NONE6            24
#define CSBACK_NONE7            25
#define CSBACK_NONE8            26
#define CSBACK_NONE9            27
#define CSBACK_NONE10           28
#define CSBACK_NONE11           29
#define MAX_CSBACK              30

#define CSBACK_NORACE           0
#define CSBACK_VAMPIRE          1
#define CSBACK_WEREWOLF         2
#define CSBACK_MAGE             3
#define CSBACK_GHOUL            4
/* virtues */
#define CSVIRT_CONSCIENCE       0
#define CSVIRT_SELF_CONTROL     1
#define CSVIRT_COURAGE          2


/* secondary abilities */
#define CSABIL_MINING           1
#define CSABIL_LOGGING          2
#define CSABIL_CARVING          3
#define CSABIL_PROSPECTING      4
#define CSABIL_MINERAL          5
#define CSABIL_METAL            6
#define CSABIL_PRECIOUS         7
#define CSABIL_WOOD             8
#define CSABIL_BONE             9
#define CSABIL_STONE            10
#define CSABIL_ENIGMAS          11
#define CSABIL_RITUALS          12
#define CSABIL_MEDITATION       13
#define CSABIL_BODYCRAFTS       14
#define CSABIL_LIGHT            15
#define CSABIL_BLUNT            16
#define CSABIL_HEAVY            17
#define CSABIL_POLEARM          18
#define CSABIL_BREWING          19

/*
 * Time and weather stuff.
 */
#define SUN_DARK            0
#define SUN_RISE            1
#define SUN_LIGHT           2
#define SUN_SET             3
#define HIGH_NOON       4

#define SKY_CLOUDLESS           0
#define SKY_CLOUDY          1
#define SKY_RAINING         2
#define SKY_LIGHTNING           3

struct  time_info_data
{
    int     hour;
    int     day;
    int     month;
    int     year;
    int     phase;
    int     moon_count;
};

struct  weather_data
{
    int     mmhg;
    int     change;
    int     sky;
    int     sunlight;
};

/*
 * Connected state for a channel.
 */
#define CON_PLAYING          0
#define CON_GET_NAME             1
#define CON_GET_OLD_PASSWORD         2
#define CON_CONFIRM_NEW_NAME         3
#define CON_GET_NEW_PASSWORD         4
#define CON_CONFIRM_NEW_PASSWORD     5
#define CON_GET_COLOUR           6
#define CON_GET_NEW_SEX          7
#define CON_GET_NEW_GROUP        8
#define CON_GET_NEW_CLASS        9
#define CON_GET_ALIGNMENT       10
#define CON_DEFAULT_CHOICE      11
#define CON_GEN_GROUPS          12
#define CON_PICK_WEAPON         13
#define CON_READ_IMOTD          14
#define CON_READ_MOTD           15
#define CON_BREAK_CONNECT       16
#define CON_PICK_CHILDHOOD      17
#define CON_PICK_ADULT          18
#define CON_GET_RACE            19
#define CON_CHOICE_VAMP         20
#define CON_PICK_CLAN           21
#define CON_COPYOVER_RECOVER    22

/* New stat assignment */
#define PHYSICAL                1
#define SOCIAL                  2
#define MENTAL                  3

#define SKILLS                  4
#define TALENTS                 5
#define KNOWLEDGES              6

#define PRIMARY                 1
#define SECONDARY               2
#define TERTIARY                3

#define CONSCIENCE              0
#define SELF_CONTROL                1
#define COURAGE                 2


#define MAX_CREATE              7

/*
 * Descriptor (channel) structure.
 */
struct  descriptor_data
{
    DESCRIPTOR_DATA *   next;
    DESCRIPTOR_DATA *   prev;
    DESCRIPTOR_DATA *   snoop_by;
    CHAR_DATA *     character;
    CHAR_DATA *     original;
    bool        valid;
    char *      host;
    sh_int      descriptor;
    sh_int      connected;
    bool        fcommand;
    char        inbuf       [4 * MAX_INPUT_LENGTH];
    char        incomm      [MAX_INPUT_LENGTH];
    char        inlast      [MAX_INPUT_LENGTH];
    int         repeat;
    char *      outbuf;
    int         outsize;
    int         outtop;
    char *      showstr_head;
    char *      showstr_point;
    void *      pEdit;      /* OLC */
    char **     pString;    /* OLC */
    int         editor;     /* OLC */
};



/*
 * Attribute bonus structures.
 */
struct  str_app_type
{
    sh_int  tohit;
    sh_int  todam;
    sh_int  carry;
    sh_int  wield;
};

struct  int_app_type
{
    sh_int  learn;
};

struct  wis_app_type
{
    sh_int  practice;
};

struct  dex_app_type
{
    sh_int  defensive;
};

struct  con_app_type
{
    sh_int  hitp;
    sh_int  shock;
};



/*
 * TO types for act.
 */
#define TO_ROOM         0
#define TO_NOTVICT      1
#define TO_VICT         2
#define TO_CHAR         3
#define TO_ALL          4
#define TO_NEWBIECH     5
#define TO_NEWBIEALL    6

#define MSL MAX_STRING_LENGTH
#define MIL MAX_INPUT_LENGTH
#define HELPS_SEC 4
/*
 * Help table types.
 */
struct  help_data
{
    HELP_DATA * next;
    sh_int  level;
    char *  keyword;
    char *  text;
    bool delete;
};

struct help_area_data
{
    HELP_AREA * next;
    HELP_DATA * first;
    HELP_DATA * last;
    AREA_DATA * area;
    char *      filename;
    bool        changed;
};


/*
 * Shop types.
 */
#define MAX_TRADE    5

struct  shop_data
{
    SHOP_DATA * next;           /* Next shop in list        */
    sh_int  keeper;         /* Vnum of shop keeper mob  */
    sh_int  buy_type [MAX_TRADE];   /* Item types shop will buy */
    sh_int  profit_buy;     /* Cost multiplier for buying   */
    sh_int  profit_sell;        /* Cost multiplier for selling  */
    sh_int  open_hour;      /* First opening hour       */
    sh_int  close_hour;     /* First closing hour       */
};


/*
 * Per-class stuff.
 */

#define MAX_GUILD   2
#define MAX_STATS   5
#define STAT_STR    0
#define STAT_INT    1
#define STAT_WIS    2
#define STAT_DEX    3
#define STAT_CON    4

struct  class_type
{
    char *  name;           /* the full name of the class */
    char    who_name    [4];    /* Three-letter name for 'who'  */
    sh_int  attr_prime;     /* Prime attribute      */
    sh_int  weapon;         /* First weapon         */
    sh_int  guild[MAX_GUILD];   /* Vnum of guild rooms      */
    sh_int  skill_adept;        /* Maximum skill level      */
    sh_int  thac0_00;       /* Thac0 for level  0       */
    sh_int  thac0_32;       /* Thac0 for level 32       */
    sh_int  hp_min;         /* Min hp gained on leveling    */
    sh_int  hp_max;         /* Max hp gained on leveling    */
    bool    fMana;          /* Class gains mana on level    */
    char *  base_group;     /* base skills gained       */
    char *  default_group;      /* default skills gained    */
};

struct item_type
{
    int     type;
    char *  name;
};

struct weapon_type
{
    char *  name;
    sh_int  vnum;
    sh_int  type;
    sh_int  *gsn;
};

struct wiznet_type
{
    char *  name;
    long    flag;
    int     level;
};

struct attack_type
{
    char *  name;           /* name */
    char *  noun;           /* message */
    int     damage;         /* damage class */
};

struct race_type
{
    char *  name;           /* call name of the race */
    bool    pc_race;        /* can be chosen by pcs */
    long    act;            /* act bits for the race */
    long    aff;            /* aff bits for the race */
    long    off;            /* off bits for the race */
    long    imm;            /* imm bits for the race */
    long        res;            /* res bits for the race */
    long    vuln;           /* vuln bits for the race */
    long    form;           /* default form flag for the race */
    long    parts;          /* default parts for the race */
    int                 race_lang;      /* race language */
};


struct pc_race_type  /* additional data for pc races */
{
    char *  name;           /* MUST be in race_type */
    char    who_name[6];
    sh_int  points;         /* cost in points of the race */
    sh_int  class_mult[MAX_CLASS];  /* exp multiplier for class, * 100 */
    char *  skills[5];      /* bonus skills for the race */
    sh_int  stats[MAX_STATS];   /* starting stats */
    sh_int  max_stats[MAX_STATS];   /* maximum stats */
    sh_int  size;           /* aff bits for the race */
    int                 race_lang;      /* race language */
};


struct spec_type
{
    char *  name;           /* special function name */
    SPEC_FUN *  function;       /* the function */
};

/* config file structure */
struct config_data
{
    int     totem_power[MAX_TOTEM];
};

/*
 * Data structure for notes.
 */

#define NOTE_NOTE   0
#define NOTE_IDEA   1
#define NOTE_PENALTY    2
#define NOTE_NEWS   3
#define NOTE_CHANGES    4
#define NOTE_RPNOTE    5
#define NOTE_SYSTEM 6
struct  note_data
{
    NOTE_DATA * next;
    bool    valid;
    sh_int  type;
    char *  sender;
    char *  date;
    char *  to_list;
    char *  subject;
    char *  text;
    time_t      date_stamp;
};



/*
 * An affect.
 */
struct  affect_data
{
    AFFECT_DATA *   next;
    bool        valid;
    sh_int      where;
    sh_int      type;
    sh_int      level;
    sh_int      duration;
    sh_int      location;
    sh_int      modifier;
    int         bitvector;
};

/* where definitions */
#define TO_AFFECTS  0
#define TO_OBJECT   1
#define TO_IMMUNE   2
#define TO_RESIST   3
#define TO_VULN     4
#define TO_WEAPON   5
#define TO_AFFECTS2     6

/*
 * A kill structure (indexed by level).
 */
struct  kill_data
{
    sh_int      number;
    sh_int      killed;
};



/***************************************************************************
 *                                                                         *
 *                   VALUES OF INTEREST TO AREA BUILDERS                   *
 *                   (Start of section ... start here)                     *
 *                                                                         *
 ***************************************************************************/

/*
 * Well known mob virtual numbers.
 * Defined in #MOBILES.
 */
#define MOB_VNUM_RAT                    1
#define MOB_VNUM_VICISSITUDE_SZLACHTA   4
#define MOB_VNUM_VICISSITUDE_VOZHD      5
#define MOB_VNUM_VICISSITUDE_ZULO       6
#define MOB_VNUM_ALLIES                 22
#define MOB_VNUM_DOPPLEGANGER           23
#define MOB_VNUM_HORDE_RATS             27
#define MOB_VNUM_MORTIS_SPIRIT          28
#define MOB_VNUM_MORTIS_ZOMBIE          29
#define MOB_VNUM_OBTEN_SERVANT          30
#define MOB_VNUM_HOMUNCULUS             32
#define MOB_VNUM_ATHANATOS              33
#define MOB_VNUM_FIDO                   711
#define MOB_VNUM_CITYGUARD              708
//#define MOB_VNUM_VAMPIRE              3404
#define MOB_VNUM_PATROLMAN              2106
#define GROUP_VNUM_TROLLS               2100
#define GROUP_VNUM_OGRES                2101


/*
 * Groups for mob->group
 */
#define MOB_GROUP_NONE                  0
#define MOB_GROUP_ASSAMITE              1
#define MOB_GROUP_BRUJAH                2
#define MOB_GROUP_CAPPADOCIAN           3
#define MOB_GROUP_FOLLOWEROFSET         4
#define MOB_GROUP_GANGREL               5
#define MOB_GROUP_LASOMBRA              6
#define MOB_GROUP_MALKAVIAN             7
#define MOB_GROUP_NOSFERATU             8
#define MOB_GROUP_RAVNOS                9
#define MOB_GROUP_TOREADOR              10
#define MOB_GROUP_TREMERE               11
#define MOB_GROUP_TZIMISCE              12
#define MOB_GROUP_VENTRUE               13
#define MOB_GROUP_BAALI                 14
#define MOB_GROUP_GARGOYLES             15
#define MOB_GROUP_SALUBRI               16
#define MOB_GROUP_BLACKFURIES           17
#define MOB_GROUP_BONEGNAWERS           18
#define MOB_GROUP_CHILDRENOFGAIA        19
#define MOB_GROUP_FIANNA                20
#define MOB_GROUP_GETOFFENRIS           21
#define MOB_GROUP_REDTALONS             22
#define MOB_GROUP_SHADOWLORDS           23
#define MOB_GROUP_SILENTSTRIDERS        24
#define MOB_GROUP_SILVERFANGS           25
#define MOB_GROUP_WARDERSOFMEN          26
#define MOB_GROUP_BLACKSPIRALDANCERS    27
#define MOB_GROUP_AHLIBATIN             28
#define MOB_GROUP_MESSIANICVOICES       29
#define MOB_GROUP_OLDFAITH              30
#define MOB_GROUP_ORDEROFHERMES         31
#define MOB_GROUP_SPIRITTALKER          32
#define MOB_GROUP_VALDAERMEN            33
#define MOB_GROUP_CRAFTMASONS           34
#define MOB_GROUP_EARTHSPIRIT           35
#define MOB_GROUP_FIRESPIRIT            36
#define MOB_GROUP_WINDSPIRIT            37
#define MOB_GROUP_WATERSPIRIT           38
#define MOB_GROUP_ELECTRICSPIRIT        39
#define MOB_GROUP_ICESPIRIT             40
#define MOB_GROUP_SILVERSPIRIT          41
#define MOB_GROUP_GENERALSPIRIT         42

#define MAX_MOBGROUP                    43


/* RT ASCII conversions -- used so we can have letters in this file */

#define A           1
#define B           2
#define C           4
#define D           8
#define E           16
#define F           32
#define G           64
#define H           128

#define I           256
#define J           512
#define K           1024
#define L           2048
#define M           4096
#define N           8192
#define O           16384
#define P           32768

#define Q           65536
#define R           131072
#define S           262144
#define T           524288
#define U           1048576
#define V           2097152
#define W           4194304
#define X           8388608

#define Y           16777216
#define Z           33554432
#define aa          67108864    /* doubled due to conflicts */
#define bb          134217728
#define cc          268435456
#define dd          536870912
#define ee          1073741824
#define ff      2147483648

/*
 * ACT bits for mobs.
 * Used in #MOBILES.
 */
#define ACT_IS_NPC      (A)     /* Auto set for mobs    */
#define ACT_SENTINEL            (B)     /* Stays in one room    */
#define ACT_SCAVENGER           (C)     /* Picks up objects */
#define ACT_QUEST           (D)
#define ACT_MOUNT           (E)
#define ACT_AGGRESSIVE      (F)         /* Attacks PC's     */
#define ACT_STAY_AREA       (G)     /* Won't leave area */
#define ACT_WIMPY           (H)
#define ACT_PET         (I)     /* Auto set for pets    */
#define ACT_TRAIN           (J)     /* Can train PC's   */
#define ACT_PRACTICE        (K)     /* Can practice PC's    */
#define ACT_UNDEAD      (O)
#define ACT_CLERIC      (Q)
#define ACT_MAGE            (R)
#define ACT_THIEF           (S)
#define ACT_WARRIOR     (T)
#define ACT_NOALIGN     (U)
#define ACT_NOPURGE     (V)
#define ACT_OUTDOORS        (W)
#define ACT_INDOORS     (Y)
#define ACT_IS_HEALER       (aa)
#define ACT_GAIN            (bb)
#define ACT_UPDATE_ALWAYS   (cc)
#define ACT_IS_CHANGER      (dd)
#define ACT_QUESTMOB        (ee)

// act2 bits.
#define ACT2_INFLUENCE         (A)
#define ACT2_BLOOD_DOLL        (B)
#define ACT2_RETAINER          (C)
#define ACT2_ULTRA_MOB         (D)
#define ACT2_IS_NPC            (E)
#define ACT2_MOLIATE           (F)
#define ACT2_BLACKSMITH        (G)
#define ACT2_SOCIALITE         (H)

/*Mobile Templates*/
#define TEMPLATE_DEFAULT    0
#define TEMPLATE_HUNTER     1
#define TEMPLATE_NEONATE    2
#define TEMPLATE_ANCIENT    3
#define TEMPLATE_CUB        4
#define TEMPLATE_ELDER      5
#define TEMPLATE_RODENT     6
#define TEMPLATE_WOLF       7

/*Mob template modifier flag bits*/
/*attr_flags*/
#define ATTR_STRONG         (A)
#define ATTR_WEAK           (B)
#define ATTR_BRILLIANT      (C)
#define ATTR_DULL           (D)
#define ATTR_SOCIAL         (E)
#define ATTR_AWKWARD        (F)
#define ATTR_ALLPLUSTWO     (G)
#define ATTR_ALLMINTWO      (H)
#define ATTR_ALLPLUSFOUR    (I)
#define ATTR_ALLMINFOUR     (J)
#define ATTR_CRINOS         (K)

/*Abil_flags*/
#define ABIL_WARRIOR        (A)
#define ABIL_OCCULTIST      (B)
#define ABIL_MAGE           (C)
#define ABIL_THIEF          (D)
#define ABIL_HEALER         (E)
#define ABIL_NOBLE          (F)
#define ABIL_CLERGY         (G)
#define ABIL_SCHOLAR        (H)
#define ABIL_MERCHANT       (I)
#define ABIL_SURVIVALIST    (J)
#define ABIL_HERMIT         (K)
#define ABIL_ENTERTAINER    (L)
#define ABIL_BEASTMASTER    (M)

/* damage classes */
#define DAM_NONE                0
#define DAM_BASH                1
#define DAM_PIERCE              2
#define DAM_SLASH               3
#define DAM_FIRE                4
#define DAM_COLD                5
#define DAM_LIGHTNING           6
#define DAM_ACID                7
#define DAM_POISON              8
#define DAM_NEGATIVE            9
#define DAM_HOLY                10
#define DAM_ENERGY              11
#define DAM_MENTAL              12
#define DAM_DISEASE             13
#define DAM_DROWNING            14
#define DAM_LIGHT       15
#define DAM_OTHER               16
#define DAM_HARM        17
#define DAM_CHARM       18
#define DAM_SOUND       19
#define DAM_SUNLIGHT    20
#define DAM_AGGREVATED  21

/* Trap damage classes */
#define TRAP_BASH       (A)
#define TRAP_PIERCE (B)
#define TRAP_SLASH  (C)
#define TRAP_FIRE       (D)
#define TRAP_COLD       (E)
#define TRAP_LIGHTNING  (F)
#define TRAP_ACID       (G)
#define TRAP_POISON (H)
#define TRAP_HOLY       (I)
#define TRAP_ENERGY (J)
#define TRAP_MENTAL (K)
#define TRAP_DROWNING   (L)
#define TRAP_LIGHT  (M)
#define TRAP_HARM       (N)
#define TRAP_SOUND  (O)
#define TRAP_SUNLIGHT   (P)
#define TRAP_BLIND  (Q)
#define TRAP_FAERIE_FIRE (R)
#define TRAP_CURSE  (S)
#define TRAP_SLEEP  (T)
#define TRAP_CALM       (U)
#define TRAP_WEAKEN (V)
#define TRAP_SLOW       (W)
#define TRAP_DEATH  (X)

/* Trap triggers */
#define TRIGGER_NORTH   (A)
#define TRIGGER_EAST    (B)
#define TRIGGER_WEST    (C)
#define TRIGGER_SOUTH   (D)
#define TRIGGER_UP  (E)
#define TRIGGER_DOWN    (F)
#define TRIGGER_ALL (G)
#define TRIGGER_ENTER   (H)
#define TRIGGER_GET (I)
#define TRIGGER_PUT (J)
#define TRIGGER_DROP    (K)
#define TRIGGER_OPEN    (L)

/* Trap focus */
#define FOCUS_SINGLE    (A)
#define FOCUS_GROUP (B)
#define FOCUS_ROOM  (C)

/* OFF bits for mobiles */
#define OFF_AREA_ATTACK         (A)
#define OFF_BACKSTAB            (B)
#define OFF_BASH                (C)
#define OFF_BERSERK             (D)
#define OFF_DISARM              (E)
#define OFF_DODGE               (F)

#define OFF_FAST                (H)
#define OFF_KICK                (I)
#define OFF_KICK_DIRT           (J)
#define OFF_PARRY               (K)
#define OFF_RESCUE              (L)
#define OFF_TAIL                (M)
#define OFF_TRIP                (N)
#define OFF_CRUSH       (O)
#define ASSIST_ALL          (P)
#define ASSIST_RACE             (R)
#define ASSIST_PLAYERS          (S)
#define ASSIST_GUARD            (T)
#define ASSIST_VNUM     (U)
#define OFF_SECOND_ATTACK   (V)
#define OFF_THIRD_ATTACK    (W)
#define OFF_FOURTH_ATTACK   (X)
#define OFF_SPHERE_ENTROPY  (Z)
#define OFF_SPHERE_FORCES   (aa)
#define OFF_SPHERE_LIFE     (bb)
#define OFF_SPHERE_MIND     (cc)
#define OFF_SPHERE_TIME     (dd)

/* return values for check_imm */
#define IS_NORMAL       0
#define IS_IMMUNE       1
#define IS_RESISTANT        2
#define IS_VULNERABLE       3

/* IMM bits for mobs */
#define IMM_SUMMON              (A)
#define IMM_CHARM               (B)
#define IMM_MAGIC               (C)
#define IMM_WEAPON              (D)
#define IMM_BASH                (E)
#define IMM_PIERCE              (F)
#define IMM_SLASH               (G)
#define IMM_FIRE                (H)
#define IMM_COLD                (I)
#define IMM_LIGHTNING           (J)
#define IMM_ACID                (K)
#define IMM_POISON              (L)
#define IMM_NEGATIVE            (M)
#define IMM_HOLY                (N)
#define IMM_ENERGY              (O)
#define IMM_MENTAL              (P)
#define IMM_DISEASE             (Q)
#define IMM_DROWNING            (R)
#define IMM_LIGHT       (S)
#define IMM_SOUND       (T)
#define IMM_WOOD                (X)
#define IMM_SILVER              (Y)
#define IMM_IRON                (Z)
#define IMM_SUNLIGHT            (aa)
#define IMM_DISARM              (bb)
/* RES bits for mobs */
#define RES_SUMMON      (A)
#define RES_CHARM       (B)
#define RES_MAGIC               (C)
#define RES_WEAPON              (D)
#define RES_BASH                (E)
#define RES_PIERCE              (F)
#define RES_SLASH               (G)
#define RES_FIRE                (H)
#define RES_COLD                (I)
#define RES_LIGHTNING           (J)
#define RES_ACID                (K)
#define RES_POISON              (L)
#define RES_NEGATIVE            (M)
#define RES_HOLY                (N)
#define RES_ENERGY              (O)
#define RES_MENTAL              (P)
#define RES_DISEASE             (Q)
#define RES_DROWNING            (R)
#define RES_LIGHT       (S)
#define RES_SOUND       (T)
#define RES_WOOD                (X)
#define RES_SILVER              (Y)
#define RES_IRON                (Z)
#define RES_SUNLIGHT            (aa)
/* VULN bits for mobs */
#define VULN_SUMMON     (A)
#define VULN_CHARM      (B)
#define VULN_MAGIC              (C)
#define VULN_WEAPON             (D)
#define VULN_BASH               (E)
#define VULN_PIERCE             (F)
#define VULN_SLASH              (G)
#define VULN_FIRE               (H)
#define VULN_COLD               (I)
#define VULN_LIGHTNING          (J)
#define VULN_ACID               (K)
#define VULN_POISON             (L)
#define VULN_NEGATIVE           (M)
#define VULN_HOLY               (N)
#define VULN_ENERGY             (O)
#define VULN_MENTAL             (P)
#define VULN_DISEASE            (Q)
#define VULN_DROWNING           (R)
#define VULN_LIGHT      (S)
#define VULN_SOUND      (T)
#define VULN_WOOD               (X)
#define VULN_SILVER             (Y)
#define VULN_IRON       (Z)
#define VULN_SUNLIGHT           (aa)
/* body form */
#define FORM_EDIBLE             (A)
#define FORM_POISON             (B)
#define FORM_MAGICAL            (C)
#define FORM_INSTANT_DECAY      (D)
#define FORM_OTHER              (E)  /* defined by material bit */

/* actual form */
#define FORM_ANIMAL             (G)
#define FORM_SENTIENT           (H)
#define FORM_UNDEAD             (I)
#define FORM_CONSTRUCT          (J)
#define FORM_MIST               (K)
#define FORM_INTANGIBLE         (L)

#define FORM_BIPED              (M)
#define FORM_CENTAUR            (N)
#define FORM_INSECT             (O)
#define FORM_SPIDER             (P)
#define FORM_CRUSTACEAN         (Q)
#define FORM_WORM               (R)
#define FORM_BLOB       (S)

#define FORM_MAMMAL             (V)
#define FORM_BIRD               (W)
#define FORM_REPTILE            (X)
#define FORM_SNAKE              (Y)
#define FORM_DRAGON             (Z)
#define FORM_AMPHIBIAN          (aa)
#define FORM_FISH               (bb)
#define FORM_COLD_BLOOD     (cc)

/* body parts */
#define PART_HEAD               (A)
#define PART_ARMS               (B)
#define PART_LEGS               (C)
#define PART_HEART              (D)
#define PART_BRAINS             (E)
#define PART_GUTS               (F)
#define PART_HANDS              (G)
#define PART_FEET               (H)
#define PART_FINGERS            (I)
#define PART_EAR                (J)
#define PART_EYE        (K)
#define PART_LONG_TONGUE        (L)
#define PART_EYESTALKS          (M)
#define PART_TENTACLES          (N)
#define PART_FINS               (O)
#define PART_WINGS              (P)
#define PART_TAIL               (Q)
/* for combat */
#define PART_CLAWS              (U)
#define PART_FANGS              (V)
#define PART_HORNS              (W)
#define PART_SCALES             (X)
#define PART_TUSKS      (Y)


/*
 * Bits for 'affected_by'.
 * Used in #MOBILES.
 */
#define AFF_BLIND       (A)
#define AFF_INVISIBLE       (B)
#define AFF_DETECT_EVIL     (C)
#define AFF_DETECT_INVIS    (D)
#define AFF_DETECT_MAGIC    (E)
#define AFF_DETECT_HIDDEN   (F)
#define AFF_DETECT_GOOD     (G)
#define AFF_SANCTUARY       (H)
#define AFF_FAERIE_FIRE     (I)
#define AFF_INFRARED        (J)
#define AFF_CURSE       (K)
#define AFF_SHIFT       (L) /* was unused */
#define AFF_POISON      (M)
#define AFF_PROTECT_EVIL    (N)
#define AFF_PROTECT_GOOD    (O)
#define AFF_SNEAK       (P)
#define AFF_HIDE        (Q)
#define AFF_SLEEP       (R)
#define AFF_CHARM       (S)
#define AFF_FLYING      (T)
#define AFF_PASS_DOOR       (U)
#define AFF_HASTE       (V)
#define AFF_CALM        (W)
#define AFF_PLAGUE      (X)
#define AFF_WEAKEN      (Y)
#define AFF_DARK_VISION     (Z)
#define AFF_BERSERK     (aa)
#define AFF_SWIM        (bb)
#define AFF_REGENERATION        (cc)
#define AFF_SLOW        (dd)
#define AFF_FANGS       (ee)
#define AFF_XP_BOOST    (ff)
/*
 * Bits for 'affected2_by'.
 * Used in #MOBILES.
 */

#define AFF2_MIST               (A)
#define AFF2_VEIL               (B)
#define AFF2_VAMP_FRENZY        (C)
#define AFF2_MAJESTY            (D)
#define AFF2_QUIETUS_BLOODCURSE (E)
#define AFF2_HOLYSHIELD         (F)
#define AFF2_FIRESHIELD         (G)
#define AFF2_DOUBLE_EXP         (H)
#define AFF2_DARKSHIELD         (I)
#define AFF2_UMBRA              (J)
#define AFF2_UNSEEN             (K)
#define AFF2_DETECT_UNSEEN      (L)
// These are defined out of order and it bugs me - Matthew
#define AFF2_IGNIS_FATUUS       (X)

#define AFF2_VAMPIRE_REGEN      (Z)
/*
 * Sex.
 * Used in #MOBILES.
 */
#define SEX_NEUTRAL           0
#define SEX_MALE              1
#define SEX_FEMALE            2


/* AC types */
#define AC_PIERCE           0
#define AC_BASH             1
#define AC_SLASH            2
#define AC_EXOTIC           3

/* dice */
#define DICE_NUMBER         0
#define DICE_TYPE           1
#define DICE_BONUS          2

/* size */
#define SIZE_TINY           0
#define SIZE_SMALL          1
#define SIZE_MEDIUM         2
#define SIZE_LARGE          3
#define SIZE_HUGE           4
#define SIZE_GIANT          5

/* diceroll modifier for damage */
#define MODTYPE_VERYWEAK                0
#define MODTYPE_WEAK                    1
#define MODTYPE_WEAKMELEE               2
#define MODTYPE_MELEE                   3
#define MODTYPE_STRONGMELEE             4
#define MODTYPE_STRONG                  5
#define MODTYPE_VERYSTRONG              6
#define MODTYPE_GODLY                   7



/*
 * Well known object virtual numbers.
 * Defined in #OBJECTS.
 */
#define OBJ_VNUM_SILVER_ONE       1
#define OBJ_VNUM_GOLD_ONE         2
#define OBJ_VNUM_GOLD_SOME        3
#define OBJ_VNUM_SILVER_SOME          4
#define OBJ_VNUM_COINS            5
#define OBJ_VNUM_BREW_POTION          6
#define OBJ_VNUM_GOLDEN_BERRY         8
#define OBJ_VNUM_SKIN           9

#define OBJ_VNUM_CORPSE_NPC      10
#define OBJ_VNUM_CORPSE_PC       11
#define OBJ_VNUM_SEVERED_HEAD        12
#define OBJ_VNUM_TORN_HEART      13
#define OBJ_VNUM_SLICED_ARM      14
#define OBJ_VNUM_SLICED_LEG      15
#define OBJ_VNUM_GUTS            16
#define OBJ_VNUM_BRAINS          17
#define OBJ_VNUM_PLASM           18
#define OBJ_VNUM_TROPHY          19
#define OBJ_VNUM_MUSHROOM        20
#define OBJ_VNUM_LIGHT_BALL      21
#define OBJ_VNUM_SPRING          22
#define OBJ_VNUM_DISC            23
#define OBJ_VNUM_GIFT_TOKEN      24
#define OBJ_VNUM_PORTAL          25
#define OBJ_VNUM_ROSE            26
#define OBJ_VNUM_RIFT            50

#define OBJ_VNUM_PIT           705

#define OBJ_VNUM_SCHOOL_MACE       802
#define OBJ_VNUM_SCHOOL_DAGGER     827
#define OBJ_VNUM_SCHOOL_SWORD      826
#define OBJ_VNUM_SCHOOL_SPEAR      809
#define OBJ_VNUM_SCHOOL_STAFF      808
#define OBJ_VNUM_SCHOOL_AXE        807
#define OBJ_VNUM_SCHOOL_FLAIL      806
#define OBJ_VNUM_SCHOOL_WHIP       805
#define OBJ_VNUM_SCHOOL_POLEARM    804
#define OBJ_VNUM_SCHOOL_LANCE      836

#define OBJ_VNUM_SCHOOL_VEST       825
#define OBJ_VNUM_SCHOOL_SHIELD     824
#define OBJ_VNUM_SCHOOL_BANNER     812
#define OBJ_VNUM_MAP           715
#define OBJ_VNUM_GUIDE         3733

/* vamp stuff */
#define OBJ_VNUM_EMPTY_VIAL    29
#define OBJ_VNUM_BROKEN_MIRROR      32

#define OBJ_VNUM_TOME      3310

/* crafting stuff */
#define OBJ_VNUM_TREE       26500
#define OBJ_VNUM_VEIN       26510
#define OBJ_VNUM_PLANK      28850
#define OBJ_VNUM_BLOCK      28855
#define OBJ_VNUM_BANDAGE    3735

/*Mage stuff*/
#define OBJ_VNUM_TASS_MUSHROOM  30500

#define QUEST_OBJ 40


#define YEW                 0
#define MAPLE               1
#define ASH                 2
#define WALNUT              3
#define OAK                 4

#define IRON                0
#define COPPER              1
#define TIN                 2
#define NICKEL              3
#define SILVER              4
#define GOLD                5
#define LEAD                6
#define COAL                7
#define ZINC                8


/*
 * Item types.
 * Used in #OBJECTS.
 */
#define ITEM_LIGHT            1
#define ITEM_SCROLL           2
#define ITEM_WAND             3
#define ITEM_STAFF            4
#define ITEM_WEAPON           5
#define ITEM_RESOURCE         6
#define ITEM_TOOL             7
#define ITEM_TREASURE             8
#define ITEM_ARMOR            9
#define ITEM_POTION          10
#define ITEM_CLOTHING            11
#define ITEM_FURNITURE           12
#define ITEM_TRASH           13
#define ITEM_CRAFTED             14
#define ITEM_CONTAINER           15
#define ITEM_DRINK_CON           17
#define ITEM_KEY             18
#define ITEM_FOOD            19
#define ITEM_MONEY           20
#define ITEM_BOAT            22
#define ITEM_CORPSE_NPC          23
#define ITEM_CORPSE_PC           24
#define ITEM_FOUNTAIN            25
#define ITEM_PILL            26
#define ITEM_PROTECT             27
#define ITEM_MAP             28
#define ITEM_PORTAL          29
#define ITEM_WARP_STONE          30
#define ITEM_ROOM_KEY            31
#define ITEM_GEM             32
#define ITEM_JEWELRY             33
#define ITEM_JUKEBOX             34
#define ITEM_TRAP            35
#define ITEM_NOTEPAPER       36
#define ITEM_PLASM            37
#define ITEM_LOCKPICK         38
#define ITEM_BLASTPOWDER      39



/*
 * Extra flags.
 * Used in #OBJECTS.
 */
#define ITEM_GLOW       (A)
#define ITEM_HUM        (B)
#define ITEM_DARK       (C)
#define ITEM_HIDDEN     (D)
#define ITEM_ILLUMINATE (E)
#define ITEM_INVIS      (F)
#define ITEM_MAGIC      (G)
#define ITEM_NODROP     (H)
#define ITEM_BLESS      (I)
#define ITEM_PAUSE_TIMER    (J)
#define ITEM_REFLECTIVE     (K)
#define ITEM_SCALING        (L)
#define ITEM_NOREMOVE       (M)
#define ITEM_INVENTORY      (N)
#define ITEM_NOPURGE        (O)
#define ITEM_ROT_DEATH      (P)
#define ITEM_VIS_DEATH      (Q)
#define ITEM_RARE           (R)
#define ITEM_NONMETAL       (S)
#define ITEM_NOLOCATE       (T)
#define ITEM_MELT_DROP      (U)
#define ITEM_HAD_TIMER      (V)
#define ITEM_SELL_EXTRACT   (W)
#define ITEM_SILVER         (X)
#define ITEM_BURN_PROOF     (Y)
#define ITEM_NOUNCURSE      (Z)
#define ITEM_IS_STAKE       (aa)
#define ITEM_IS_PRESERVE    (bb)
#define ITEM_IS_ENHANCED    (cc)
#define ITEM_BREW_POT       (dd)
#define ITEM_UMBRA          (ee)



/*
 * Wear flags.
 * Used in #OBJECTS.
 */
#define ITEM_TAKE       (A)
#define ITEM_WEAR_FINGER    (B)
#define ITEM_WEAR_NECK      (C)
#define ITEM_WEAR_TORSO      (D)
#define ITEM_WEAR_HEAD      (E)
#define ITEM_WEAR_LEGS      (F)
#define ITEM_WEAR_FEET      (G)
#define ITEM_WEAR_HANDS     (H)
#define ITEM_WEAR_ARMS      (I)
#define ITEM_WEAR_SHIELD    (J)
#define ITEM_WEAR_BODY     (K)
#define ITEM_WEAR_WAIST     (L)
#define ITEM_WEAR_WRIST     (M)
#define ITEM_WIELD      (N)
#define ITEM_HOLD       (O)
#define ITEM_NO_SAC     (P)
#define ITEM_WEAR_FLOAT     (Q)
#define ITEM_WEAR_EAR       (R)
#define ITEM_WEAR_FACE      (S)
#define ITEM_WEAR_BACK      (T)

/* weapon class */
#define WEAPON_EXOTIC       0
#define WEAPON_SWORD        1
#define WEAPON_DAGGER       2
#define WEAPON_SPEAR        3
#define WEAPON_MACE     4
#define WEAPON_AXE      5
#define WEAPON_FLAIL        6
#define WEAPON_WHIP     7
#define WEAPON_POLEARM      8
#define WEAPON_LANCE        9

/* weapon types */
#define WEAPON_FLAMING      (A)
#define WEAPON_FROST        (B)
#define WEAPON_VAMPIRIC     (C)
#define WEAPON_SHARP        (D)
#define WEAPON_VORPAL       (E)
#define WEAPON_TWO_HANDS    (F)
#define WEAPON_SHOCKING     (G)
#define WEAPON_POISON       (H)
#define WEAPON_SLOW     (J)
#define WEAPON_AGG_DAMAGE   (K)

/* gate flags */
#define GATE_NORMAL_EXIT    (A)
#define GATE_NOCURSE        (B)
#define GATE_GOWITH     (C)
#define GATE_BUGGY      (D)
#define GATE_RANDOM     (E)
#define GATE_UMBRA      (F)

/* furniture flags */
#define STAND_AT        (A)
#define STAND_ON        (B)
#define STAND_IN        (C)
#define SIT_AT          (D)
#define SIT_ON          (E)
#define SIT_IN          (F)
#define REST_AT         (G)
#define REST_ON         (H)
#define REST_IN         (I)
#define SLEEP_AT        (J)
#define SLEEP_ON        (K)
#define SLEEP_IN        (L)
#define PUT_AT          (M)
#define PUT_ON          (N)
#define PUT_IN          (O)
#define PUT_INSIDE      (P)

/* light flags */
#define LIGHT_FLAMES        (A)
#define LIGHT_MYSTICAL      (B)
#define LIGHT_INTENSE       (C)
#define LIGHT_WEAK          (D)
#define LIGHT_INTERMITTENT  (E)
#define LIGHT_PULSING       (F)
#define LIGHT_RECHARGE      (G)

/*
 * Apply types (for affects).
 * Used in #OBJECTS.
 */
#define APPLY_NONE            0
#define APPLY_STR             1
#define APPLY_DEX             2
#define APPLY_INT             3
#define APPLY_WIS             4
#define APPLY_CON             5
#define APPLY_SEX             6
#define APPLY_CLASS           7
#define APPLY_LEVEL           8
#define APPLY_AGE             9
#define APPLY_HEIGHT             10
#define APPLY_WEIGHT             11
#define APPLY_MANA           12
#define APPLY_HIT            13
#define APPLY_MOVE           14
#define APPLY_GOLD           15
#define APPLY_EXP            16
#define APPLY_AC             17
#define APPLY_HITROLL            18
#define APPLY_DAMROLL            19
#define APPLY_SAVES          20
#define APPLY_SAVING_PARA        20
#define APPLY_SAVING_ROD         21
#define APPLY_SAVING_PETRI       22
#define APPLY_SAVING_BREATH      23
#define APPLY_SAVING_SPELL       24
#define APPLY_SPELL_AFFECT       25
#define APPLY_ALIGN          26
#define APPLY_GENERATION     27
#define APPLY_CS_STR        28
#define APPLY_CS_DEX        29
#define APPLY_CS_STA        30
#define APPLY_CS_CHA        31
#define APPLY_CS_MAN        32
#define APPLY_CS_APP        33
#define APPLY_CS_PER        34
#define APPLY_CS_INT        35
#define APPLY_CS_WIT        36
#define APPLY_MAX_BLOOD     37

/*
 * Values for containers (value[1]).
 * Used in #OBJECTS.
 */
#define CONT_CLOSEABLE            1
#define CONT_PICKPROOF            2
#define CONT_CLOSED           4
#define CONT_LOCKED           8
#define CONT_PUT_ON          16
#define CONT_CAULDRON             32

#define sendch(buf, ch) send_to_char(buf, ch)

#define ROLEPLAY      107
/*
 * Well known room virtual numbers.
 * Defined in #ROOMS.
 */
#define ROOM_VNUM_LIMBO           2
//#define ROOM_VNUM_CHAT           1200
#define ROOM_VNUM_TEMPLE       701
#define ROOM_VNUM_ALTAR        703
#define ROOM_VNUM_DONATE       705
#define ROOM_VNUM_SCHOOL       3700
#define ROOM_VNUM_BALANCE      4500
#define ROOM_VNUM_CIRCLE       4400
#define ROOM_VNUM_DEMISE       4201
#define ROOM_VNUM_HONOR        4300
#define ROOM_VNUM_PARADOX      30790



/*
 * Room flags.
 * Used in #ROOMS.
 */
#define ROOM_DARK       (A)
#define ROOM_NO_QUIT    (B)
#define ROOM_NO_MOB     (C)
#define ROOM_INDOORS        (D)
#define ROOM_HUMAN_ONLY     (E)
#define ROOM_GHOUL_ONLY     (F)
#define ROOM_VAMPIRE_ONLY   (G)
#define ROOM_GAROU_ONLY     (H)
#define ROOM_MAGE_ONLY      (I)
#define ROOM_PRIVATE        (J)
#define ROOM_SAFE       (K)
#define ROOM_SOLITARY       (L)
#define ROOM_PET_SHOP       (M)
#define ROOM_NO_RECALL      (N)
#define ROOM_IMP_ONLY       (O)
#define ROOM_GODS_ONLY      (P)
#define ROOM_HEROES_ONLY    (Q)
#define ROOM_NEWBIES_ONLY   (R)
#define ROOM_LAW        (S)
#define ROOM_NOWHERE        (T)
#define ROOM_FORGE      (U)
#define ROOM_MOUNT_SHOP     (V)
#define ROOM_UMBRA      (W)
#define ROOM_BANK       (X)
#define ROOM_UNDERWATER (Y)
#define ROOM_NOTELE		(Z)

/*
 * Directions.
 * Used in #ROOMS.
 */
#define DIR_NORTH             0
#define DIR_EAST              1
#define DIR_SOUTH             2
#define DIR_WEST              3
#define DIR_UP                4
#define DIR_DOWN              5



/*
 * Exit flags.
 * Used in #ROOMS.
 */
#define EX_ISDOOR             (A)
#define EX_CLOSED             (B)
#define EX_LOCKED             (C)
#define EX_PICKPROOF              (F)
#define EX_NOPASS             (G)
#define EX_EASY               (H)
#define EX_HARD               (I)
#define EX_INFURIATING            (J)
#define EX_NOCLOSE            (K)
#define EX_NOLOCK             (L)
#define EX_LIFT           (M)
#define EX_HIDDEN             (N)



/*
 * Sector types.
 * Used in #ROOMS.
 */
#define SECT_INSIDE             0
#define SECT_CITY               1
#define SECT_FIELD              2
#define SECT_FOREST             3
#define SECT_HILLS              4
#define SECT_MOUNTAIN           5
#define SECT_WATER_SWIM         6
#define SECT_WATER_NOSWIM       7
#define SECT_UNUSED             8
#define SECT_AIR                9
#define SECT_DESERT             10
#define SECT_WATER_DROWN        11
#define SECT_HOT                12
#define SECT_COLD               13
#define SECT_NODE               14
#define SECT_MAX                15

#define RECALL_COST             100 // gold.

/*
 * Equpiment wear locations.
 * Used in #RESETS.
 */
#define WEAR_NONE            -1
#define WEAR_LIGHT            0
#define WEAR_FINGER_L             1
#define WEAR_FINGER_R             2
#define WEAR_NECK_1           3
#define WEAR_NECK_2           4
#define WEAR_TORSO             5
#define WEAR_HEAD             6
#define WEAR_LEGS             7
#define WEAR_FEET             8
#define WEAR_HANDS            9
#define WEAR_ARMS            10
#define WEAR_SHIELD          11
#define WEAR_BODY           12
#define WEAR_WAIST           13
#define WEAR_WRIST_L             14
#define WEAR_WRIST_R             15
#define WEAR_WIELD           16
#define WEAR_HOLD            17
#define WEAR_FLOAT           18
#define WEAR_EAR_L           19
#define WEAR_EAR_R           20
#define WEAR_FACE            21
#define WEAR_BACK            22
#define MAX_WEAR             23


/***************************************************************************
 *                                                                         *
 *                   VALUES OF INTEREST TO AREA BUILDERS                   *
 *                   (End of this section ... stop here)                   *
 *                                                                         *
 ***************************************************************************/

/*
 * Conditions.
 */
#define COND_DRUNK            0
#define COND_FULL             1
#define COND_THIRST           2
#define COND_HUNGER           3



/*
 * Positions.
 */
#define POS_DEAD              0
#define POS_TORPOR            1
#define POS_MORTAL            2
#define POS_INCAP             3
#define POS_STUNNED           4
#define POS_SLEEPING              5
#define POS_RESTING           6
#define POS_SITTING           7
#define POS_FIGHTING              8
#define POS_STANDING              9


// Combat defense types.
#define DEFENSE_NONE        0
#define DEFENSE_SOAK        1
#define DEFENSE_ARMOR       2
#define DEFENSE_FULL        3


/*
 * ACT bits for players.
 */
#define PLR_IS_NPC      (A)     /* Don't EVER set.  */

/* part of my new vamp code - Ugha */
#define PLR_AUTOVAMP        (B)

/* RT auto flags */
#define PLR_AUTOASSIST      (C)
#define PLR_AUTOEXIT        (D)
#define PLR_AUTOLOOT        (E)
#define PLR_AUTOSAC             (F)
#define PLR_AUTOGOLD        (G)
#define PLR_AUTOSPLIT       (H)

/* Camarilla stuff */
#define PLR_CAMARILLA       (I)

/* for arena */
#define PLR_SPEC            (J)


/* RT personal flags */
#define PLR_IC          (M)
#define PLR_HOLYLIGHT       (N)
#define PLR_CANLOOT     (P)
#define PLR_NOSUMMON        (Q)
#define PLR_NOFOLLOW        (R)
/*
 * Colour stuff by Lope of Loping Through The MUD
 */
#define PLR_COLOUR      (T)    /* Colour Flag By Lope */
/* 1 bit reserved, S */

/* penalty flags */
#define PLR_PERMIT      (U)
#define PLR_PUEBLO      (V)    /* Pueblo Client Support */
#define PLR_LOG         (W)
#define PLR_DENY        (X)
#define PLR_FREEZE      (Y)
#define PLR_THIEF       (Z)
#define PLR_KILLER      (aa)
#define PLR_NEWBIEHELP  (bb)
#define PLR_IMMTALK     (cc)
#define PLR_ARENA       (ee)
#define PLR_PLAYTEST    (ff) // Flag for testing d10 combat on non-garou.

/*extra act bits for PLayer flags. Needed more flags! -Matthew*/
#define PLR2_NEWBIE     (A)
#define PLR2_PVP        (B)
#define PLR2_QUESTOR    (C)
#define PLR2_LEADER     (D)
#define PLR2_IS_NPC     (E) //Should NEVER be set.
#define PLR2_AUTODOORS  (F) //Automatically open/unlock doors.
#define PLR2_WHOLOCIC   (G) //Show area name on who when IC.
#define PLR2_WHOLOCQUIET (H) //Show area name on who when Quiet.
#define PLR2_WHOLOCALWAYS (I) // Always show location on who.
#define PLR2_WHOSHOWLEVEL (J) // Show level tags on 'who'.
#define PLR2_IMMSHOWLOC   (K) // Show player location regardless of player setting.
#define PLR2_NOCONDITION  (L) // don't show hungry/thirsty messages
#define PLR2_NOSHORTDESC  (M) // Do not show shifted descs for other players
#define PLR2_BATTLEPROMPT (N) // Show battle prompt in combat.
#define PLR2_DEBUG        (O) // Flag char a debugger
#define PLR2_DEBUGMSG     (P) // Show debug messages
#define PLR2_RP_AVAILABLE (Q) // Show player looking for RP


/* RT comm flags -- may be used on both mobs and chars */
#define COMM_QUIET              (A)
#define COMM_DEAF               (B)
#define COMM_NOWIZ              (C)
#define COMM_NOAUCTION          (D)
#define COMM_NOIC          (E)
#define COMM_NOQUESTION         (F)
#define COMM_NOMUSIC            (G)
#define COMM_NOCLAN     (H)
#define COMM_NOOOC      (I)
#define COMM_SHOUTSOFF      (J)
#define COMM_NOIMP      (K)
#define COMM_NOADMIN    (L)

/* display flags */
#define COMM_COMPACT        (L)
#define COMM_BRIEF      (M)
#define COMM_PROMPT     (N)
#define COMM_COMBINE        (O)
#define COMM_TELNET_GA      (P)
#define COMM_SHOW_AFFECTS   (Q)
#define COMM_NOGRATS        (R)

/* announcement code */
#define COMM_NOANNOUNCE     (S)

/* penalties */
#define COMM_NOEMOTE        (T)
#define COMM_NOSHOUT        (U)
#define COMM_NOTELL     (V)
#define COMM_NOCHANNELS     (W)

/* sect channel */
#define COMM_NOSECT     (X)

/* other comms */
#define COMM_SNOOP_PROOF    (Y)
#define COMM_AFK        (Z)
#define COMM_COMBAT_BRIEF   (aa)
#define COMM_NOCHAT         (bb)
#define COMM_AUTOOOC        (cc)

/*Immortals Special comm flags.*/
#define COMM_WIZINAME       (dd)
#define COMM_IMMTITLE       (ee)

/* WIZnet flags */
#define WIZ_ON          (A)
#define WIZ_TICKS       (B)
#define WIZ_LOGINS      (C)
#define WIZ_SITES       (D)
#define WIZ_LINKS       (E)
#define WIZ_DEATHS      (F)
#define WIZ_RESETS      (G)
#define WIZ_MOBDEATHS       (H)
#define WIZ_FLAGS       (I)
#define WIZ_PENALTIES       (J)
#define WIZ_SACCING     (K)
#define WIZ_LEVELS      (L)
#define WIZ_SECURE      (M)
#define WIZ_SWITCHES        (N)
#define WIZ_SNOOPS      (O)
#define WIZ_RESTORE     (P)
#define WIZ_LOAD        (Q)
#define WIZ_NEWBIE      (R)
#define WIZ_PREFIX      (S)
#define WIZ_SPAM        (T)
#define WIZ_PKILL       (U)
#define WIZ_FILE        (V)
#define WIZ_BUGS        (W)

/* Qitem Data */
#define     PLACE_ROOM      0
#define     PLACE_MOB       1
#define     PLACE_OBJ       2
typedef struct  qitem_data          QITEM_DATA;
extern  QITEM_DATA                  *qitem_list;
extern  QITEM_DATA                  *qitem_free;
QITEM_DATA *new_qitem  args ( (void) );
void    free_qitem args ( (QITEM_DATA *item) );
QITEM_DATA *qitem_lookup args ((const char *name ));
QITEM_DATA *qitem_obj_lookup args( (OBJ_INDEX_DATA *pObj) );
struct qitem_data
{
    QITEM_DATA      *next;
    char            *name;
    sh_int          qobjvnum;
    sh_int          place;
    sh_int          roomvnum;
    sh_int          mobvnum;
    sh_int          objvnum;
    bool            found;
    char            *foundby;
    bool            notify;
    char            *notified;
    bool            valid;
    bool            loaded;
    bool            delete;
};
/*Domains data*/
typedef struct  domain_data         DOMAIN_DATA;
extern  DOMAIN_DATA                 *domain_list;
extern  DOMAIN_DATA                 *domain_free;
extern  DOMAIN_DATA                 *domain_first;
extern  DOMAIN_DATA                 *domain_last;

void    free_domain args( (DOMAIN_DATA *dom) );
DOMAIN_DATA  *new_domain args ( (void) );
#define  RELATION_NONE    0
#define  RELATION_ALLIED  1
#define  RELATION_WARRING 2
struct domain_data
{
    DOMAIN_DATA     *next;
    sh_int          vnum;
    sh_int          clan;
    sh_int          influence[MAX_CLAN];
    sh_int          change[MAX_CLAN];
    sh_int          tax;
    sh_int          relation[MAX_CLAN];
    bool            valid;
};

/* Prototypes for clan data */


struct clan_type
{
    long flags;                   /* flags for clan */
    char *name;                 /* name of clan */
    char *who_name;             /* name sent for "who" command */
    char *leader;                       /* leader of clan */
    int     bank;
    char *banker;
    sh_int     bonus;
    sh_int  clan_type; /* TYPE_NONE, TYPE_CLAN, TYPE_TRADITION, TYPE_TRIBE*/
    sh_int  clanhall;
};

/*
 * Prototype for a mob.
 * This is the in-memory version of #MOBILES.
 */
struct  mob_index_data
{
    MOB_INDEX_DATA *    next;
    SPEC_FUN *      spec_fun;
    SHOP_DATA *     pShop;
    PROG_LIST *        mprogs;
    AREA_DATA *     area;       /* OLC */

    sh_int      vnum;
    sh_int      group;
    bool        new_format;
    bool        delete;
    sh_int      count;
    sh_int      killed;
    sh_int      maxload;
    char *      player_name;
    char *      short_descr;
    char *      long_descr;
    char *      description;
    long        act;
    long        act2;
    long        affected_by;
    long        affected2_by;
    sh_int      alignment;
    sh_int      level;
    sh_int      hitroll;
    sh_int      hit[3];
    sh_int      mana[3];
    sh_int      damage[3];
    sh_int      ac[4];
    sh_int      dam_type;
    long        off_flags;
    long        imm_flags;
    long        res_flags;
    long        vuln_flags;
    sh_int      start_pos;
    sh_int      default_pos;
    sh_int      sex;
    sh_int      race;
    long        wealth;
    long        form;
    long        parts;
    sh_int      size;
    char *      material;
    long        mprog_flags;
    long        attr_flags;
    long        abil_flags;
};



/* memory settings */
#define MEM_CUSTOMER    A
#define MEM_SELLER  B
#define MEM_HOSTILE C
#define MEM_AFRAID  D

/* memory for mobs */
struct mem_data
{
    MEM_DATA    *next;
    bool    valid;
    int     id;
    int     reaction;
    time_t  when;
};


/*
 * One character (PC or NPC).
 */
 // bm_char-data -- bookmark to char_data
struct  char_data
{

    CHAR_DATA *     next;
    CHAR_DATA *     next_in_room;
    CHAR_DATA *     master;
    CHAR_DATA *     leader;
    CHAR_DATA *     fighting;
    CHAR_DATA *     reply;
    CHAR_DATA *     pet;
    CHAR_DATA *     mprog_target;
    CHAR_DATA *     grudge;
    MEM_DATA *      memory;
    SPEC_FUN *      spec_fun;
    MOB_INDEX_DATA *    pIndexData;
    DESCRIPTOR_DATA *   desc;
    AFFECT_DATA *   affected;
    NOTE_DATA *     pnote;
    OBJ_DATA *      carrying;
    OBJ_DATA *      on;
    ROOM_INDEX_DATA *   in_room;
    ROOM_INDEX_DATA *   was_in_room;
    AREA_DATA *     zone;
    PC_DATA *       pcdata;
    GEN_DATA *      gen_data;
    bool        valid;
    char *      name;
    long        id;
    sh_int      version;
    char *      short_descr;
    char *      long_descr;
    char *      description;
    char *      prompt;
    char *      prefix;
    char *      leadername;
    sh_int      group;
    sh_int      clan;
    sh_int      petition;
    sh_int      sex;
    sh_int      class;
    sh_int      race;
    sh_int      level;
    sh_int      trust;
    int         played;
    bool        oldclan;
    int         lines;  /* for the pager */
    int       pblood;
    int       max_pblood;
    int         bloodtick;
    int         unlockpoints;
    int         unlocksspent;
    int		cslock;
    int		smited;
    int 	currentkills;
    int 	totalkills;
    int         cheater;
    int		maxdamage;
    int         tagged;
    int         airtick;
    int     qpoints;
    long    totalqpoints;
    int       gen;
    int       drains;
    int       embraced;
    int       bonded;
    int       roleplaying;
    int       childer;
    int       agg;
    int       protean;
    int       animalism;
    int       obtenebration;
    int       auspex;
    int       celerity;
    int       fortitude;
    int       potence;
    int       obfuscate;
    int       domination;
    int       presence;
    int       mortis;
    int       quietus;
    int       thaumaturgy;
    int       vicissitude;
    int       chimerstry;
    int       dpoints;
    int     changed;
    int     remorts;
    int     freebie;
    int     inhumanity;
    int     rage;
    int     breed;
    int     auspice;
    char *      gift1;
    char *      gift2;
    char *      gift3;
    char *      gift4;
    char *      gift5;
    int         renown;
    char *      sire;
    char *      apprentice;
    char *      vamp_master;
    char *      shift;
    int         rank;
    int         tradition;
    int         arete;
    int         avatar;
    int         paradox;
    int         quintessence;
    int         max_quintessence;
    int         foci;
    sh_int      sphere[10];
    time_t      logon;
    sh_int      timer;
    sh_int      wait;
    sh_int      daze;
    sh_int      hit;
    sh_int      max_hit;
    sh_int      agg_dam;
    sh_int      mana;
    sh_int      max_mana;
    sh_int      move;
    sh_int      max_move;
    long        gold;
    long        silver;
    int         exp;
    long        act;
    long        act2;
    long        comm;   /* RT added to pad the vector */
    long        wiznet; /* wiz stuff */
    long        imm_flags;
    long        res_flags;
    long        vuln_flags;
    sh_int      invis_level;
    sh_int      incog_level;
    long        affected_by;
    long        affected2_by;
    sh_int      position;
    sh_int      practice;
    sh_int      train;
    sh_int      carry_weight;
    sh_int      carry_number;
    sh_int      saving_throw;
    sh_int      alignment;
    sh_int      hitroll;
    sh_int      damroll;
    sh_int      armor[4];
    sh_int      wimpy;
    /* stats */
    sh_int      perm_stat[MAX_STATS];
    sh_int      mod_stat[MAX_STATS];
    sh_int      csattributes[MAX_ATTRIBUTES];
    sh_int      csattrib_mod[MAX_ATTRIBUTES];
    sh_int      csabilities[MAX_ABILITIES];
    sh_int      csmax_willpower;
    sh_int      cswillpower;
    long        attr_flags;
    long        abil_flags;
    /* parts stuff */
    long        form;
    long        parts;
    sh_int      size;
    char*       material;
    /* mobile stuff */
    long        off_flags;
    sh_int      damage[3];
    sh_int      dam_type;
    sh_int      start_pos;
    sh_int      default_pos;
    CHAR_DATA *     mount;
    bool        riding;
    sh_int      mprog_delay;
    sh_int      stopped;
    int         quest; /* quest identifier for mobs */
    char        *memory_name[100]; /* Mob memory of last player interacted with */
    sh_int      memory_like[100]; /* 1-100 scale of how much a mob likes memory_name[i] */
    CHAR_DATA *         questgiver; /* Vassago */
    sh_int              nextquest; /* Vassago */
    sh_int              quest_accum;
    sh_int              countdown; /* Vassago */
    sh_int              questobj; /* Vassago */
    sh_int              questmob; /* Vassago */

};



/*
 * Data which only PC's have.
 */
 // bm_pc_data -- bookmark to pc_data
struct  pc_data
{
    PC_DATA *       next;
    PC_DATA *       pc_next;
    PC_DATA *       pc_prev;
    CHAR_DATA *     character;
    BUFFER *        buffer;
    bool        valid;
    char *      pwd;
    char *      title;
    char *      pretitle;
    time_t      last_note;
    time_t      last_idea;
    time_t      last_penalty;
    time_t      last_news;
    time_t      last_changes;
    time_t      last_rpnote;
    time_t      last_sysnote;
    sh_int      perm_hit;
    sh_int      perm_mana;
    sh_int      perm_move;
    sh_int      true_sex;
    int         last_level;
    sh_int      condition   [4];
    sh_int      learned     [MAX_SKILL];
    bool        group_known [MAX_GROUP];
    sh_int      points;
    bool        confirm_delete;
    char *      alias[MAX_ALIAS];
    char *      alias_sub[MAX_ALIAS];
    sh_int      sect;
    int         bank;
    char *      kill_target;

/* New home/haven code */
    int         home;
    int         hometown;

/* Immortal Stuff */
    bool        admin;
    char *      wiziname;
    char *      immtitle;
    char *      bamfin;
    char *      bamfout;
    int         security;   /* OLC */ /* Builder security */
    int         immclass;

/* Imm-set player IC notes. */
    char *  playernotes;

/* Roleplay Code  */
    long        IC_total;
    long        IC_rank;
    long        IC_goal;

/* Domain influence timer*/
    int      influenced;

/*Registration */
    char *      email;
    bool        registered;
    int         reg_num;

/* Charsheet Stats */
    char *      csname;
    char *      cschronicle;
    char *      csclan;
    char *      csconcept;
    char *      cssect;
    char *      cssire;
    sh_int      csgeneration;
    char *      cshaven;
    int         csattributes[MAX_ATTRIBUTES];
    int         csattrib_mod[MAX_ATTRIBUTES];
    int         csabilities[MAX_ABILITIES];
    int         cssec_abil[MAX_SEC_ABILITIES];
    int         discipline[MAX_DISC];
    int         csbackgrounds[MAX_CSBACK];
    int         csvirtues[3];
    int         cshumanity;
    int         csmax_willpower;
    int         cswillpower;
    int         csmax_bloodpool;
    int         csbloodpool;
    int         cshealth;
    int         csexp;
    int         primary;
    int         secondary;
    int         tertiary;
    int         progress;
    int         stat[MAX_CREATE];

/* New Garou data */
    int         primal_urge;
    int         shiftform;
    int         rage[2];
    int         gnosis[2];
    int         shift_count;
    int         shift_to;
    char *      garou_fur;
    int         renown[6];
    int         rank;
    int         tribe;
    int         breed;
    int         auspice;
    int         favor[25];
    int         gift[MAX_GIFT];
    char *      csdemeanor;
    char *      csnature;
    int         ip;
/* Quest journal shit - ugha */
    char *      quest_name[MAX_QUEST];
    char *      quest_string[MAX_QUEST];
    int         quest_id[MAX_QUEST];
    int         quest_step[MAX_QUEST];

/* Matt's anti rp milking code. */
    int         last_pose;
    int         room_last_pose;

// replay buffers
    unsigned int        next_replaytell;
    char     *          replaytell_text[MAX_REPLAYTELL];
    unsigned int        next_replayroom;
    char     *          replayroom_text[MAX_REPLAYROOM];
    sh_int              replayroom_lastevent_roomvnum;
    unsigned int        next_replaychannels;
    char     *          replaychannels_text[MAX_REPLAYCHANNELS];

// Language stuff
    int                 language        [ MAX_LANGUAGE ];
    int                 speaking;
    int                 learn;
};

/* Data for generating characters -- only used during generation */
struct gen_data
{
    GEN_DATA    *next;
    bool    valid;
    bool    skill_chosen[MAX_SKILL];
    bool    group_chosen[MAX_GROUP];
    int     points_chosen;
};



/*
 * Liquids.
 */
#define LIQ_WATER        0
#define LIQ_BENEDICTINE  22
#define LIQ_MUSCUS       36

struct  liq_type
{
    char *  liq_name;
    char *  liq_color;
    sh_int  liq_affect[5];
};


struct  tool_type
{
    char *  tool_name;
};

struct  crafted_type
{
    char *  crafted_name;
};



/*
 * Extra description data for a room or object.
 */
struct  extra_descr_data
{
    EXTRA_DESCR_DATA *next; /* Next in list                     */
    bool valid;
    char *keyword;              /* Keyword in look/examine          */
    char *description;          /* What to see                      */
};



/*
 * Prototype for an object.
 */
struct  obj_index_data
{
    OBJ_INDEX_DATA *    next;
    EXTRA_DESCR_DATA *  extra_descr;
    AFFECT_DATA *   affected;
    AREA_DATA *     area;       /* OLC */
    PROG_LIST *     oprogs;
    long        oprog_flags;
    bool        new_format;
    bool        delete;
    char *      name;
    char *      short_descr;
    char *      description;
    sh_int      vnum;
    sh_int      reset_num;
    char *      material;
    sh_int      item_type;
    int         extra_flags;
    int         wear_flags;
    sh_int      level;
    sh_int      condition;
    sh_int      count;
    sh_int      weight;
    int         cost;
    int         value[5];
    sh_int      timer;
    sh_int      size;
};



/*
 * One object.
 */
struct  obj_data
{
    OBJ_DATA *      next;
    OBJ_DATA *      next_content;
    OBJ_DATA *      contains;
    OBJ_DATA *      in_obj;
    OBJ_DATA *      on;
    CHAR_DATA *     carried_by;
    CHAR_DATA *	oprog_target;
    EXTRA_DESCR_DATA *  extra_descr;
    AFFECT_DATA *   affected;
    OBJ_INDEX_DATA *    pIndexData;
    ROOM_INDEX_DATA *   in_room;
    QITEM_DATA      *   qitem;
    bool        valid;
    bool        enchanted;
    char *          owner;
    char *      name;
    char *      short_descr;
    char *      description;
    sh_int      item_type;
    int         extra_flags;
    int         wear_flags;
    sh_int      wear_loc;
    sh_int      weight;
    int         cost;
    sh_int      level;
    sh_int      condition;
    char *      material;
    sh_int      timer;
    int         value   [5];
    sh_int			oprog_delay;
    sh_int      size;
};



/*
 * Exit data.
 */
struct  exit_data
{
    union
    {
    ROOM_INDEX_DATA *   to_room;
    sh_int          vnum;
    } u1;
    sh_int      exit_info;
    sh_int      key;
    char *      keyword;
    char *      description;
    EXIT_DATA *     next;       /* OLC */
    int         rs_flags;   /* OLC */
    int         orig_door;  /* OLC */
    PROG_LIST *		rprogs;
    CHAR_DATA *		rprog_target;
    long		rprog_flags;
    sh_int		rprog_delay;

};



/*
 * Reset commands:
 *   '*': comment
 *   'M': read a mobile
 *   'O': read an object
 *   'P': put object in object
 *   'G': give object to mobile
 *   'E': equip object to mobile
 *   'D': set state of door
 *   'R': randomize room exits
 *   'S': stop (end of list)
 */

/*
 * Area-reset definition.
 */
struct  reset_data
{
    RESET_DATA *    next;
    char        command;
    sh_int      arg1;
    sh_int      arg2;
    sh_int      arg3;
    sh_int      arg4;
};



/*
 * Area definition.
 */
struct  area_data
{
    AREA_DATA *     next;
    DOMAIN_DATA *       domain;
    HELP_AREA *     helps;
    char *        file_name;
    char *      name;
    char *      credits;
    sh_int      age;
    sh_int      nplayer;
    sh_int      low_range;
    sh_int      high_range;
    sh_int      min_vnum;
    sh_int      max_vnum;
    bool        empty;
    char *      builders;   /* OLC */ /* Listing of */
    int         vnum;       /* OLC */ /* Area vnum  */
    int         area_flags; /* OLC */
    int         security;   /* OLC */ /* Value 1-9  */

};



/*
 * Room type.
 */
struct  room_index_data
{
    ROOM_INDEX_DATA *   next;
    CHAR_DATA *     people;
    OBJ_DATA *      contents;
    EXTRA_DESCR_DATA *  extra_descr;
    AREA_DATA *     area;
    EXIT_DATA *     exit    [6];
    RESET_DATA *    reset_first;    /* OLC */
    RESET_DATA *    reset_last; /* OLC */
    char *      name;
    char *      description;
    char *      owner;
    sh_int      vnum;
    int         room_flags;
    sh_int      light;
    sh_int      sector_type;
    sh_int      heal_rate;
    sh_int      mana_rate;
    sh_int      clan;
    PROG_LIST *		rprogs;
    CHAR_DATA *		rprog_target;
    long		rprog_flags;
    sh_int		rprog_delay;
};


/*
 *  Disciplines. Used to check clan discs
 */
#define DISC_NONE           0
#define DISC_ANIMALISM          1
#define DISC_AUSPEX         2
#define DISC_PROTEAN            3
#define DISC_CELERITY           4
#define DISC_FORTITUDE          5
#define DISC_POTENCE            6
#define DISC_OBFUSCATE          7
#define DISC_OBTENEBRATION      8
#define DISC_DOMINATE           9
#define DISC_PRESENCE           10
#define DISC_MORTIS         11
#define DISC_THAUMATURGY        12
#define DISC_QUIETUS            13
#define DISC_VICISSITUDE        14
#define DISC_CHIMERSTRY         15
#define DISC_DEMENTATION        16
#define DISC_SERPENTIS          17
/* Shorter versions cause I'm lazy - Ugha */
#define NONE            0
#define ANIMALISM           1
#define AUSPEX          2
#define PROTEAN         3
#define CELERITY            4
#define FORTITUDE           5
#define POTENCE         6
#define OBFUSCATE           7
#define OBTENEBRATION       8
#define DOMINATE            9
#define PRESENCE            10
#define MORTIS          11
#define THAUMATURGY     12
#define QUIETUS         13
#define VICISSITUDE     14
#define CHIMERSTRY      15
#define DEMENTATION     16
#define SERPENTIS       17

/*
 * Protean4+ Shifted Forms
 * Refers to Affect Level of Shift
*/
#define SHIFT_SYRUP         0
#define SHIFT_RAT           1
#define SHIFT_BAT           2
#define SHIFT_RAVEN         3
#define SHIFT_WOLF          4
#define SHIFT_BEAR          5
#define SHIFT_MIST          6

/*
 * Life4 & Combo Mutate Forms
 * Refers to Affect level of Mutate Form
*/
#define MUTATE_SLOTH        0
#define MUTATE_OWL          1
#define MUTATE_SQUIRREL     2
#define MUTATE_PANTHER      3
#define MUTATE_BEAR         4
#define MUTATE_DRAGON       5

/*
 * Types of attacks.
 * Must be non-overlapping with spell/skill types,
 * but may be arbitrary beyond that.
 */
#define TYPE_UNDEFINED               -1
#define TYPE_HIT                     1000



/*
 *  Target types.
 */
#define TAR_IGNORE          0
#define TAR_CHAR_OFFENSIVE      1
#define TAR_CHAR_DEFENSIVE      2
#define TAR_CHAR_SELF           3
#define TAR_OBJ_INV         4
#define TAR_OBJ_CHAR_DEF        5
#define TAR_OBJ_CHAR_OFF        6

#define TARGET_CHAR         0
#define TARGET_OBJ          1
#define TARGET_ROOM         2
#define TARGET_NONE         3

/*
 * Command types.
 */
typedef struct  command_type        CMD_DATA;

struct  command_type
{
    CMD_DATA    *   prev;
    CMD_DATA    *   next;
    CMD_DATA    *   char_next;
    bool        valid;
    char *          name;
    DO_FUN *            do_fun;
    sh_int              position;
    sh_int              level;
    sh_int              log;
    sh_int              show;
    sh_int              cat;
};

struct cs_skill_type
 {
    char * name; // Name of skill.
    sh_int * gsn; // Pointer to gsn.
    sh_int level;
    sh_int primary[3]; // primary[stat] primary[dot], primary[type].
    sh_int secondary[3]; //secondary[stat], secondary[dot], secondary[type].
};





/*
 * Skills include spells as a particular case.
 */
struct  skill_type
{
    char *  name;           /* Name of skill        */
    sh_int  skill_level[MAX_CLASS]; /* Level needed by class    */
    sh_int  rating[MAX_CLASS];  /* How hard it is to learn  */
    SPELL_FUN * spell_fun;      /* Spell pointer (for spells)   */
    sh_int  target;         /* Legal targets        */
    sh_int  minimum_position;   /* Position for caster / user   */
    sh_int *    pgsn;           /* Pointer to associated gsn    */
    sh_int  slot;           /* Slot for #OBJECT loading */
    sh_int  min_mana;       /* Minimum mana used        */
    sh_int  beats;          /* Waiting time after use   */
    char *  noun_damage;        /* Damage message       */
    char *  msg_off;        /* Wear off message     */
    char *  msg_obj;        /* Wear off message for obects  */
};

struct  group_type
{
    char *  name;
    sh_int  rating[MAX_CLASS];
    char *  spells[MAX_IN_GROUP];
};

/*
 * MOBprog definitions
 */
#define TRIG_ACT    (A)
#define TRIG_BRIBE  (B)
#define TRIG_DEATH  (C)
#define TRIG_ENTRY  (D)
#define TRIG_FIGHT  (E)
#define TRIG_GIVE   (F)
#define TRIG_GREET  (G)
#define TRIG_GRALL  (H)
#define TRIG_KILL   (I)
#define TRIG_HPCNT  (J)
#define TRIG_RANDOM (K)
#define TRIG_SPEECH (L)
#define TRIG_EXIT   (M)
#define TRIG_EXALL  (N)
#define TRIG_DELAY  (O)
#define TRIG_SURR   (P)
#define TRIG_GET		(Q)
#define TRIG_DROP	(R)
#define TRIG_SIT	(S)
#define TRIG_SYSTIME   (T)
#define TRIG_MUDTIME (U)
#define TRIG_TALK   (V)

/*
 * Prog types
 */
#define PRG_MPROG	0
#define PRG_OPROG	1
#define PRG_RPROG	2

struct prog_list
{
    int                 trig_type;
    char *              trig_phrase;
    sh_int              vnum;
    char *              code;
    PROG_LIST *         next;
    bool                valid;
};

struct prog_code
{
    sh_int              vnum;
    char *              code;
    PROG_CODE *         next;
};

/*HUGE string, for wordwrap functions.*/
extern char temp_HSL_workspace[HSL];
extern bool RECORD_TO_REPLAYROOM;

extern bool ACT_LOOKING;

/*
 * These are skill_lookup return values for common skills and spells.
 */
extern  sh_int  gsn_backstab;
extern  sh_int  gsn_dodge;
extern  sh_int  gsn_envenom;
extern  sh_int  gsn_hide;
extern  sh_int  gsn_peek;
extern  sh_int  gsn_pick_lock;
extern  sh_int  gsn_sneak;
extern  sh_int  gsn_steal;
extern  sh_int  gsn_block;

extern  sh_int  gsn_disarm;
extern  sh_int  gsn_enhanced_damage;
extern  sh_int  gsn_critical_strike;
extern  sh_int  gsn_kick;
extern  sh_int  gsn_parry;
extern  sh_int  gsn_rescue;
extern  sh_int  gsn_second_attack;
extern  sh_int  gsn_third_attack;
extern  sh_int  gsn_fourth_attack;
extern  sh_int  gsn_knight_training;
/*Spells*/
extern  sh_int  gsn_armor;
extern  sh_int  gsn_blindness;
extern  sh_int  gsn_charm_person;
extern  sh_int  gsn_curse;
extern  sh_int  gsn_invis;
extern  sh_int  gsn_mass_invis;
extern  sh_int  gsn_plague;
extern  sh_int  gsn_poison;
extern  sh_int  gsn_bleeding;
extern  sh_int  gsn_detect_hidden;
extern  sh_int  gsn_detect_invis;
extern  sh_int  gsn_sleep;
extern  sh_int  gsn_fly;
extern  sh_int  gsn_sanctuary;
extern  sh_int  gsn_teleport;
extern  sh_int          gsn_call_lightning;
extern  sh_int          gsn_cause_light;
extern  sh_int  gsn_change_sex;
extern  sh_int          gsn_colour_spray;
extern  sh_int          gsn_create_food;
extern  sh_int          gsn_create_water;
extern  sh_int          gsn_create_spring;
extern  sh_int          gsn_cure_blindness;
extern  sh_int          gsn_cure_poison;
extern  sh_int          gsn_dispel;
extern  sh_int          gsn_earthquake;
extern  sh_int          gsn_heat_metal;
extern  sh_int          gsn_lightning_bolt;
extern  sh_int          gsn_ray_of_truth;
extern  sh_int          gsn_remove_curse;
extern  sh_int          gsn_zeal;
extern  sh_int          gsn_gouge;
/* new gsns */
extern sh_int  gsn_axe;
extern sh_int  gsn_dagger;
extern sh_int  gsn_flail;
extern sh_int  gsn_mace;
extern sh_int  gsn_polearm;
extern sh_int  gsn_lance;
extern sh_int  gsn_shield_block;
extern sh_int  gsn_spear;
extern sh_int  gsn_sword;
extern sh_int  gsn_whip;

extern sh_int  gsn_bash;
extern sh_int  gsn_berserk;
extern sh_int  gsn_bite;
extern sh_int  gsn_dirt;
extern sh_int  gsn_hand_to_hand;
extern sh_int  gsn_shred;
extern sh_int  gsn_trip;

extern sh_int  gsn_fast_healing;
extern sh_int  gsn_haggle;
extern sh_int  gsn_lore;
extern sh_int  gsn_study;
extern sh_int  gsn_classify;
extern sh_int  gsn_soothe;
extern sh_int  gsn_cue;
extern sh_int  gsn_rhythm;
extern sh_int  gsn_meditation;
extern sh_int  gsn_notoriety;

extern sh_int  gsn_scrolls;
extern sh_int  gsn_mass_healing;
extern sh_int  gsn_staves;
extern sh_int  gsn_wands;
extern sh_int  gsn_recall;
extern sh_int  gsn_riding;
extern sh_int  gsn_claws;
extern sh_int  gsn_feralspeech;
extern sh_int  gsn_songofserenity;
extern sh_int  gsn_celbuff;
extern sh_int  gsn_incubuspassion;
extern sh_int  gsn_hauntthesoul;
extern sh_int  gsn_silencethesanemind;
extern sh_int  gsn_howlinglunacy;
extern sh_int  gsn_shift;
extern sh_int  gsn_elemental;
extern sh_int  gsn_holyshield;
extern sh_int  gsn_fireshield;
extern sh_int  gsn_darkshield;
extern sh_int  gsn_shockshield;
extern sh_int  gsn_chant;
extern sh_int  gsn_vamp_frenzy;
extern sh_int  gsn_blight;
extern sh_int  gsn_garou_frenzy;
extern sh_int  gsn_thaumaturgy_frenzy;
extern sh_int  gsn_reveal;
extern sh_int  gsn_touch;
extern sh_int  gsn_telepathy;
extern sh_int  gsn_astralprojection;
extern sh_int  gsn_astrallylost;
extern sh_int  gsn_farsight;
extern sh_int  gsn_bloodofpotency;
extern sh_int  gsn_theft;
extern sh_int  gsn_cauldron;
extern sh_int  gsn_armsoftheabyss;
extern sh_int  gsn_awe;
extern sh_int  gsn_botched_presence;
extern sh_int  gsn_socialdisgrace;
extern sh_int  gsn_track;
extern sh_int  gsn_cloakshadow;
extern sh_int  gsn_nocturne;
extern sh_int  gsn_majesty;
extern sh_int  gsn_sparkofrage;
extern sh_int  gsn_silenceofdeath;
extern sh_int  gsn_quietus_weakness;
extern sh_int  gsn_quietus_bloodcurse;
extern sh_int  gsn_acidblood;
extern sh_int  gsn_blood_agony;
extern sh_int  gsn_eyesoftheserpent;
extern sh_int  gsn_tongueoftheasp;
extern sh_int  gsn_tonguelash;
extern sh_int  gsn_skinoftheadder;
extern sh_int  gsn_formofthecobra;
extern sh_int  gsn_darkheart;
extern sh_int  gsn_sanctus;
extern sh_int  gsn_bandage;
extern sh_int  gsn_ground;
extern sh_int  gsn_warcry;
extern sh_int  gsn_deafened;
extern sh_int  gsn_laryngitis;
extern sh_int  gsn_headbutt;
extern sh_int  gsn_blast;
extern sh_int  gsn_slip;
extern sh_int  gsn_enhanced_speed;
extern sh_int  gsn_renown;
extern sh_int  gsn_second_kick;
extern sh_int  gsn_slow;
extern sh_int  gsn_vigor;
extern sh_int  gsn_glower;
extern sh_int  gsn_waylay;
extern sh_int  gsn_fortress;
extern sh_int  gsn_scry;
extern sh_int  gsn_brew;
extern sh_int  gsn_skin;
extern sh_int  gsn_change;
extern sh_int  gsn_retreat;
extern sh_int  gsn_behead;
extern sh_int  gsn_hometown_change;

/* New disc stuff */
extern sh_int  gsn_regen;
extern sh_int  gsn_blood_buff;
extern sh_int  gsn_torpor;
extern sh_int  gsn_arise;
extern sh_int  gsn_gleam;
extern sh_int  gsn_unseen;
extern sh_int  gsn_veil;
extern sh_int  gsn_ignis_fatuus;
extern sh_int  gsn_chimaera;
extern sh_int  gsn_chimerstry;
extern sh_int  gsn_phantasm;
extern sh_int  gsn_horridreality;

/* for rituals */
extern sh_int  gsn_store_inferno;
extern sh_int  gsn_store_assamiteacid;
extern sh_int  gsn_blood_magic;
extern sh_int  gsn_arena;
extern sh_int  gsn_fetish;
extern sh_int   gsn_forget;
extern sh_int   gsn_mental_resilience;
extern sh_int   gsn_mask;
extern sh_int   gsn_cloak_the_gathering;
extern sh_int   gsn_earthmeld;
extern sh_int   gsn_shadowform;
extern sh_int   gsn_vicissitude_bonecraft;
extern sh_int   gsn_vicissitude_horrid;
extern sh_int   gsn_vicissitude_chiropteran;
extern sh_int   gsn_wingclaws;

/* Sengir Thaum Rituals */
extern sh_int   gsn_incorporeal_passage;

/* trap crap */
extern sh_int  gsn_bash_trap;
extern sh_int  gsn_pierce_trap;
extern sh_int  gsn_slash_trap;
extern sh_int  gsn_fire_trap;
extern sh_int  gsn_cold_trap;
extern sh_int  gsn_acid_trap;
extern sh_int  gsn_energy_trap;

/* for professions */
extern  sh_int  gsn_assassinate;
extern  sh_int  gsn_divine_strength;


/* mage stuff */

extern  sh_int  gsn_controlrandomness;
extern  sh_int  gsn_magick;
extern  sh_int  gsn_kineticshield;
extern  sh_int  gsn_betterbody;
extern  sh_int  gsn_mutateform;
extern  sh_int  gsn_spiritsight;
extern  sh_int  gsn_trappedingauntlet;
extern  sh_int  gsn_watertowine;
extern  sh_int  gsn_refinematter;
extern  sh_int  gsn_empower;
extern  sh_int  gsn_primesense;
extern  sh_int  gsn_imbueobject;
extern  sh_int  gsn_wellspring;
extern  sh_int  gsn_paradoxward;
extern  sh_int  gsn_timesense;
extern  sh_int  gsn_precognition;
extern  sh_int  gsn_timealteration;
extern  sh_int  gsn_stoptheclock;
extern  sh_int  gsn_sidesteptime;
extern  sh_int  gsn_camouflagediscernment;
extern  sh_int  gsn_negationfield;
extern  sh_int  gsn_touchofhelios;
extern  sh_int  gsn_sluggishspeed;
extern  sh_int  gsn_mythicform;
extern  sh_int  gsn_breathweapon;
/*extern  sh_int  gsn_quintessentialbond;*/
// Matthew's random GSNs.
extern  sh_int  gsn_botched_allies;
extern  sh_int  gsn_allies;
extern  sh_int  gsn_summoned_illusion;

/* new garou stuff */
extern  sh_int  gsn_rage;
extern  sh_int  gsn_spirit_totem;

//Matthew's spell gsns
extern sh_int gsn_weaken;
extern sh_int gsn_shadowplay;

//Zelan's XP boost & regen
extern sh_int gsn_xp_boost;
extern sh_int gsn_vampire_regen;

/* Matthew's Utility*/
#define HLEVELS(ch, num)    ((ch)->max_hit/(8-(num)))
#define IS_LEADER(ch)       ((IS_IMMORTAL((ch)) || !str_cmp(clan_table[(ch)->clan].leader, (ch)->name)))
#define IS_MAGE(ch)         (!IS_NPC((ch)) && (ch)->avatar > 0)
#define IS_BANKER(ch, Clan)    (!IS_NPC(ch) && !IS_SWITCHED(ch) &&  \
                ( strstr(clan_table[Clan].banker, ch->name ) \
               || strstr(clan_table[Clan].banker, "All" )   \
               || strstr(clan_table[Clan].banker, "None" )  \
               || !str_cmp(ch->name, clan_table[Clan].leader)))
#define PLAYTESTING(ch)     (!IS_NPC((ch)) && IS_SET((ch)->act, PLR_PLAYTEST))
#define IS_DEBUGGING(ch)    (!IS_NPC((ch)) && IS_SET((ch)->act2, PLR2_DEBUG) \
                            && IS_SET((ch)->act2, PLR2_DEBUGMSG))
#define IS_UMBRA(ch)        (IS_AFFECTED2((ch), AFF2_UMBRA))
#define SAME_UMBRA(a, b)    ((IS_AFFECTED2((a), AFF2_UMBRA) && IS_AFFECTED2((b), AFF2_UMBRA)) \
                        || (!IS_AFFECTED2((a), AFF2_UMBRA) && !IS_AFFECTED2((b), AFF2_UMBRA)))
#define SAME_UMBRA_OBJ(a, b)    ((IS_AFFECTED2((a), AFF2_UMBRA) && IS_OBJ_STAT((b), ITEM_UMBRA)) \
                  || (!IS_AFFECTED2((a), AFF2_UMBRA) && !IS_OBJ_STAT((b), ITEM_UMBRA)))

#define SAME_ROOM(ch, victim)     (IS_VALID((ch)) && IS_VALID((victim)) && (ch)->in_room != NULL && (victim)->in_room != NULL && (ch)->in_room == (victim)->in_room)

// used by replay buffer system.
void record_replayroom_event args ((CHAR_DATA *ch, const char *txt));
void record_replay_event args ((CHAR_DATA *ch, const char *txt));
void record_replaychannels_event args ((CHAR_DATA *ch, const char *txt));
#define replace_string( pstr, nstr ) \
       { free_string( (pstr) ); \
        pstr=str_dup( (nstr) ); }
char    *FORMATF args ( (const char *formatbuf, ...) );
int     c_str_len args ( (const char *s) );
void    println args ((const char *buf, CHAR_DATA *ch));
void    titlebar args( (CHAR_DATA *ch));
void    channel_to_char args( (const char *txt, CHAR_DATA*ch) );


        /*
 * Utility macros.
 */
#define SAFE_DESC_CHARACTER(ch) (ch->desc->character? ch->desc->character: ch)
#define TRUE_CH(ch)             (ch->desc ? \
    (ch->desc->original ? ch->desc->original : SAFE_DESC_CHARACTER(ch)):ch)
#define IS_VALID(data)      ((data) != NULL && (data)->valid)
#define IS_ADMIN(ch)        ((ch) != NULL && (ch)->pcdata != NULL && ch->pcdata->admin)
#define VALIDATE(data)      ((data)->valid = TRUE)
#define INVALIDATE(data)    ((data)->valid = FALSE)
#define UMIN(a, b)      ((a) < (b) ? (a) : (b))
#define UMAX(a, b)      ((a) > (b) ? (a) : (b))
#define URANGE(a, b, c)     ((b) < (a) ? (a) : ((b) > (c) ? (c) : (b)))
#define LOWER(c)        ((c) >= 'A' && (c) <= 'Z' ? (c)+'a'-'A' : (c))
#define UPPER(c)        ((c) >= 'a' && (c) <= 'z' ? (c)+'A'-'a' : (c))
#define IS_SET(flag, bit)   ((flag) & (bit))
#define SET_BIT(var, bit)   ((var) |= (bit))
#define REMOVE_BIT(var, bit)    ((var) &= ~(bit))
#define TOGGLE_BIT(var, bit)    ((var) ^= (bit))

#define IS_NULLSTR(str)     ((str) == NULL || (str)[0] == '\0')
#define ENTRE(min,num,max)  ( ((min) < (num)) && ((num) < (max)) )
#define CHECK_POS(a, b, c)  {                           \
                    (a) = (b);                  \
                    if ( (a) < 0 )                  \
                    bug( "CHECK_POS : " c " == %d < 0", a );    \
                }

/*
 * Character macros.
 */
#define IS_NEWBIE(ch)      (!IS_NPC(ch) && IS_SET((ch)->act2, PLR2_NEWBIE))
#define IS_QUESTOR(ch)     (IS_SET((ch)->act2, PLR2_QUESTOR))
#define IS_NPC(ch)      (IS_SET((ch)->act, ACT_IS_NPC))
#define IS_IMMORTAL(ch)     ((ch)->pcdata != NULL && (ch)->level >= LEVEL_IMMORTAL) //(get_trust(ch) >= LEVEL_IMMORTAL)
#define IS_IMP(ch)      (get_trust(ch) >= IMPLEMENTOR - 1)
#define IS_SUPR(ch)     (get_trust(ch) >= SUPREME - 1)
#define IS_HERO(ch)     (get_trust(ch) >= LEVEL_HERO)
#define IS_TRUSTED(ch,level)    (get_trust((ch)) >= (level))
#define IS_AFFECTED(ch, sn) (IS_SET((ch)->affected_by, (sn)))
#define IS_AFFECTED2(ch, sn)    (IS_SET((ch)->affected2_by, (sn)))
#define IS_ATTRFLAGGED(ch, flag)    (IS_SET((ch)->attr_flags, (flag)))
#define IS_ABILFLAGGED(ch, flag)    (IS_SET((ch)->abil_flags, (flag)))

#define GET_AGE(ch)     ((int) (17 + ((ch)->played \
                    + current_time - (ch)->logon )/72000))

#define IS_GOOD(ch)     (ch->alignment >= 350)
#define IS_EVIL(ch)     (ch->alignment <= -350)
#define IS_NEUTRAL(ch)      (!IS_GOOD(ch) && !IS_EVIL(ch))

#define IS_AWAKE(ch)        (ch->position > POS_SLEEPING)
#define GET_AC(ch,type)     ((ch)->armor[type]              \
                + ( IS_AWAKE(ch)                \
            ? dex_app[get_curr_stat(ch,STAT_DEX)].defensive : 0 ))
#define GET_HITROLL(ch) \
        ((ch)->hitroll+str_app[get_curr_stat(ch,STAT_STR)].tohit)
#define GET_DAMROLL(ch) \
        ((ch)->damroll+str_app[get_curr_stat(ch,STAT_STR)].todam)

#define IS_OUTSIDE(ch)      (!IS_SET(                   \
                    (ch)->in_room->room_flags,          \
                    ROOM_INDOORS))
#define HAS_REFLECTION(ch)  ((ch)->clan != clan_lookup("lasombra"))

#define WAIT_STATE(ch, npulse)  ((ch)->wait = UMAX((ch)->wait, (npulse)))
#define DAZE_STATE(ch, npulse)  ((ch)->daze = UMAX((ch)->daze, (npulse)))
#define STOPPED(ch, npulse)     ((ch)->stopped = UMAX((ch)->stopped, (npulse)))
#define get_carry_weight(ch)    ((ch)->carry_weight + (ch)->silver/1000 +  \
                              (ch)->gold/100)
#define MOUNTED(ch) \
        ((!IS_NPC(ch) && ch->mount && ch->riding) ? ch->mount : NULL)
#define RIDDEN(ch) \
        ((IS_NPC(ch) && ch->mount && ch->riding) ? ch->mount : NULL)
#define IS_DRUNK(ch)        ((ch->pcdata->condition[COND_DRUNK] > 10))
#define act(format,ch,arg1,arg2,type)\
    act_new((format),(ch),(arg1),(arg2),(type),POS_RESTING, FALSE)

#define act2(format,ch,arg1,arg2,type)\
    act_new2((format),(ch),(arg1),(arg2),(type),POS_RESTING)

#define HAS_TRIGGER_MOB(ch,trig)    (IS_SET((ch)->pIndexData->mprog_flags,(trig)))
#define HAS_TRIGGER_OBJ(obj,trig) (IS_SET((obj)->pIndexData->oprog_flags,(trig)))
#define HAS_TRIGGER_ROOM(room,trig) ((room) != NULL && IS_SET((room)->rprog_flags,(trig)))
#define EDIT_CLAN(ch, Clan)     ( Clan = ch->desc->pEdit )
#define IS_SWITCHED( ch )       ( ch->desc && ch->desc->original )
#define IS_BUILDER(ch, Area)    ( !IS_NPC(ch) && !IS_SWITCHED( ch ) &&    \
                ( ch->pcdata->security >= Area->security  \
                || strstr( Area->builders, ch->name )     \
                || strstr( Area->builders, "All" ) ) )



/*
 * Object macros.
 */
#define CAN_WEAR(obj, part) (IS_SET((obj)->wear_flags,  (part)))
#define IS_OBJ_STAT(obj, stat)  (IS_SET((obj)->extra_flags, (stat)))
#define IS_WEAPON_STAT(obj,stat)(IS_SET((obj)->value[4],(stat)))
#define IS_LIGHT_FLAG(obj,flag)(IS_SET((obj)->value[4],(flag)))
#define WEIGHT_MULT(obj)    ((obj)->item_type == ITEM_CONTAINER ? \
    (obj)->value[4] : 100)
#define IS_REFLECTIVE(obj) (IS_SET((obj)->extra_flags, ITEM_REFLECTIVE ))


/*
 * Description macros.
 */
/*#define PERS(ch, looker)    ( can_see( looker, (ch) ) ? ((ch)->changed > 0 ? \
                (ch)->short_descr : IS_AFFECTED(ch, AFF_SHIFT) ? \
                (ch)->short_descr : is_affected(ch, gsn_mask) ? \
                (ch)->short_descr : is_affected(ch, gsn_shadowform) ? \
                (ch)->short_descr : is_affected(ch, gsn_vicissitude_horrid) ? \
                (ch)->short_descr : (!IS_NPC(ch) && ch->pcdata->shiftform >= CRINOS) ? \
                (ch)->short_descr : IS_NPC(ch) ? (ch)->short_descr : (ch)->name ) : "someone" )*/

//Description macro replaced with  more functional description function call.
char    *PERS   args( (CHAR_DATA *ch, CHAR_DATA *looker, bool channel) );


#define PERS2(ch, looker)   ( can_see( looker, (ch) ) ?     \
                ( IS_NPC(ch) ? (ch)->short_descr    \
                : (ch)->name ) : "Someone" )

#define PERS3(ch, looker)   ( can_see( looker, (ch) ) ?     \
                ( IS_NPC(ch) ? (ch)->short_descr    \
                : (ch)->shift ) : "someone" )


/*
 * Structure for a social in the socials table.
 */
struct  social_type
{
    char      name[20];
    char *    char_no_arg; // Shown to char, no argument.
    char *    others_no_arg; // Shown to room, no argument.
    char *    char_found; // Shown to char, target found.
    char *    others_found; // Shown to room, target found.
    char *    vict_found; // Show to target.
    char *    char_not_found; // Target not found
    char *      char_auto; // Show to char, (social) self
    char *      others_auto; // Show to room, (social) self
};



/*
 * Global constants.
 */
// Not working! extern  const   struct  lang_type       lang_table      [ MAX_LANGUAGE ];
extern  const   struct  str_app_type    str_app     [26];
extern  const   struct  int_app_type    int_app     [26];
extern  const   struct  wis_app_type    wis_app     [26];
extern  const   struct  dex_app_type    dex_app     [26];
extern  const   struct  con_app_type    con_app     [26];

extern  const   struct  class_type  class_table [MAX_CLASS];
extern  const   struct  weapon_type weapon_table    [];
extern  const   struct  item_type   item_table  [];
extern  const   struct  wiznet_type wiznet_table    [];
extern  const   struct  attack_type attack_table    [];
extern  const   struct  race_type   race_table  [];
extern  const   struct  spec_type   spec_table  [];
extern  const   struct  cs_skill_type csskill_table[];
extern  const   struct  liq_type    liq_table   [];
extern  const   struct  tool_type   tool_table  [];
extern  const   struct  crafted_type    crafted_table   [];
extern  const   struct  skill_type  skill_table [MAX_SKILL];
extern  const   struct  group_type      group_table [MAX_GROUP];
extern          struct social_type      social_table    [MAX_SOCIALS];



/*
 * Global variables.
 */
extern      HELP_DATA     * help_first;
extern      SHOP_DATA     * shop_first;

extern      CHAR_DATA     * char_list;
extern      DESCRIPTOR_DATA   * descriptor_list;
extern      DESCRIPTOR_DATA   * descriptor_tsil;
extern      OBJ_DATA      * object_list;
extern      PROG_CODE    * mprog_list;
extern          PROG_CODE         *     rprog_list;
extern          PROG_CODE         *     oprog_list;
extern      char            bug_buf     [];
extern      time_t          current_time;
extern      time_t          boot_time;
extern      time_t          agg_time;
extern      bool            fLogAll;
extern      bool            Debug;
extern      FILE *          fpReserve;
extern      KILL_DATA       kill_table  [];
extern      char            log_buf     [];
extern      TIME_INFO_DATA      time_info;
extern      WEATHER_DATA        weather_info;
// extern       DATABASE_DATA       database; non-functional database code
extern      bool            MOBtrigger;
extern      int         cards [2];
extern      int         bets [4];
extern      int         gamescore [2];
extern      int         delt1 [13];
extern      int         delt2 [13];
extern      CHAR_DATA   *   gamer1;
extern      CHAR_DATA   *   gamer2;
extern      bool            stay1;
extern      bool            stay2;
extern      bool            show1;
extern      bool            show2;
extern      CONFIG_DATA *   config;


/*
 * OS-dependent declarations.
 * These are all very standard library functions,
 *   but some systems have incomplete or non-ansi header files.
 */
#if defined(_AIX)
char *  crypt       args( ( const char *key, const char *salt ) );
#endif

#if defined(apollo)
int atoi        args( ( const char *string ) );
void *  calloc      args( ( unsigned nelem, size_t size ) );
char *  crypt       args( ( const char *key, const char *salt ) );
#endif

#if defined(hpux)
char *  crypt       args( ( const char *key, const char *salt ) );
#endif

#if defined(linux)
char *  crypt       args( ( const char *key, const char *salt ) );
#endif

#if defined(macintosh)
#define NOCRYPT
#if defined(unix)
#undef  unix
#endif
#endif

#if defined(MIPS_OS)
char *  crypt       args( ( const char *key, const char *salt ) );
#endif

#if defined(MSDOS)
#define NOCRYPT
#if defined(unix)
#undef  unix
#endif
#endif

#if defined(NeXT)
char *  crypt       args( ( const char *key, const char *salt ) );
#endif

#if defined(sequent)
char *  crypt       args( ( const char *key, const char *salt ) );
int fclose      args( ( FILE *stream ) );
int fprintf     args( ( FILE *stream, const char *format, ... ) );
int fread       args( ( void *ptr, int size, int n, FILE *stream ) );
int fseek       args( ( FILE *stream, long offset, int ptrname ) );
void    perror      args( ( const char *s ) );
int ungetc      args( ( int c, FILE *stream ) );
#endif

#if defined(sun)
char *  crypt       args( ( const char *key, const char *salt ) );
int fclose      args( ( FILE *stream ) );
int fprintf     args( ( FILE *stream, const char *format, ... ) );
#if defined(SYSV)
siz_t   fread       args( ( void *ptr, size_t size, size_t n,
                FILE *stream) );
#elif !defined(__SVR4)
int fread       args( ( void *ptr, int size, int n, FILE *stream ) );
#endif
int fseek       args( ( FILE *stream, long offset, int ptrname ) );
void    perror      args( ( const char *s ) );
int ungetc      args( ( int c, FILE *stream ) );
#endif

#if defined(ultrix)
char *  crypt       args( ( const char *key, const char *salt ) );
#endif



/*
 * The crypt(3) function is not available on some operating systems.
 * In particular, the U.S. Government prohibits its export from the
 *   United States to foreign countries.
 * Turn on NOCRYPT to keep passwords in plain text.
 */
#if defined(NOCRYPT)
#define crypt(s1, s2)   (s1)
#endif



/*
 * Data files used by the server.
 *
 * AREA_LIST contains a list of areas to boot.
 * All files are read in completely at bootup.
 * Most output files (bug, idea, typo, shutdown) are append-only.
 *
 * The NULL_FILE is held open so that we have a stream handle in reserve,
 *   so players can go ahead and telnet to all the other descriptors.
 * Then we close it whenever we need to open a file (e.g. a save file).
 */
#if defined(macintosh)
#define PLAYER_DIR  ""          /* Player files */
#define TEMP_FILE   "romtmp"
#define NULL_FILE   "proto.are"     /* To reserve one stream */
#endif

#if defined(MSDOS)
#define PLAYER_DIR  ""          /* Player files */
#define TEMP_FILE   "romtmp"
#define NULL_FILE   "nul"           /* To reserve one stream */
#endif

#if defined(unix)
#define AREA_DIR        "../area/"              /* area files */
#define PLAYER_DIR      "../player/"            /* Player files */
#define BK_PLAYER_DIR   "../player/bk/"         /* Backup player files */
#define GOD_DIR         "../gods/"          /* list of gods */
#define TEMP_FILE       "../player/romtmp"
#define NULL_FILE       "/dev/null"     /* To reserve one stream */
#define DATA_DIR        "../data/"
#define LOG_DIR         "../log/"       /* Location of log files. */
#endif
#define CLAN_FILE       "../data/guild.dat"
#define AREA_LIST       "../data/area.lst"  /* List of areas*/
#define DICT_FILE       "../data/dictionary.lst" /* For dict entries */
#define BUG_FILE        "../data/bugs.txt" /* For 'bug' and bug()*/
#define TYPO_FILE       "../data/typos.txt" /* For 'typo'*/
#define NOHELPS_FILE    "../data/nohelps.txt" /*For if help !found in 'help'*/
#define NOTE_FILE       "../data/notes.not"/* For 'notes'*/
#define IDEA_FILE       "../data/ideas.not"
#define PENALTY_FILE    "../data/penal.not"
#define NEWS_FILE       "../data/news.not"
#define CHANGES_FILE    "../data/chang.not"
#define IMMNOTE_FILE    "../data/immnote.not"
#define RPNOTE_FILE     "../data/rpnote.not"
#define SYSNOTE_FILE   "../data/sysnote.not"
#define SHUTDOWN_FILE   "../data/shutdown.txt"/* For 'shutdown'*/
#define BAN_FILE        "../data/ban.txt"
#define SYSTEM_LOG      "../log/current.log"
#define MUSIC_FILE      "../data/music.txt"
#define GOD_FILE        "../data/gods.txt"
#define HELP_FILE       "../data/help.dat"
#define SOCIAL_FILE     "../data/social.dat"
#define LAST_COMMAND_FILE   "../data/last.txt"
#define LS_FILE         "../player/ls.txt"
#define COMMAND_FILE     "../data/command.dat" /*Commands list*/
#define QUEST_FILE       "../data/quest.dat" /*qitems list */
#define CONFIG_FILE      "../data/config.cfg"
#define CONFIG_BACKUP    "../data/config.bk"
/* This file holds the copyover data */
#define COPYOVER_FILE   "../data/copyover.data"

/* This is the executable file */
#define EXE_FILE      "../src/rom"

/*
 * Our function prototypes.
 * One big lump ... this is every function in Merc.
 */
#define CD  CHAR_DATA
#define MID MOB_INDEX_DATA
#define OD  OBJ_DATA
#define OID OBJ_INDEX_DATA
#define RID ROOM_INDEX_DATA
#define SF  SPEC_FUN
#define AD  AFFECT_DATA
#define MPC PROG_CODE
/* act_comm.c */
void    check_sex   args( ( CHAR_DATA *ch) );
void    add_follower    args( ( CHAR_DATA *ch, CHAR_DATA *master ) );
void    stop_follower   args( ( CHAR_DATA *ch ) );
void    nuke_pets   args( ( CHAR_DATA *ch ) );
int cskill_lookup args( (int gsn) );
void    die_follower    args( ( CHAR_DATA *ch ) );
bool    is_same_group   args( ( CHAR_DATA *ach, CHAR_DATA *bch ) );
void  announce      args( ( CHAR_DATA *ch, CHAR_DATA *victim, long flag ) );


/* act_enter.c */
RID  *get_random_room   args ( (CHAR_DATA *ch) );

/* act_info.c */
void    set_title   args( ( CHAR_DATA *ch, char *title ) );

/* act_move.c */
void    move_char   args( ( CHAR_DATA *ch, int door, bool follow ) );

/* act_obj.c */
bool can_loot       args( (CHAR_DATA *ch, OBJ_DATA *obj) );
void    get_obj         args( ( CHAR_DATA *ch, OBJ_DATA *obj,OBJ_DATA *container ) );
void    wear_obj    args( (CHAR_DATA *ch, OBJ_DATA *obj, bool fReplace) );
bool can_wear_in_form   args((CHAR_DATA *ch, OBJ_DATA *obj));


/* act_wiz.c */
void wiznet     args( (char *string, CHAR_DATA *ch, OBJ_DATA *obj,
                   long flag, long flag_skip, int min_level ) );
void read_wizlist       args( (CHAR_DATA *ch));
void copyover_recover args((void));

/* alias.c */
void    substitute_alias args( (DESCRIPTOR_DATA *d, char *input) );

/* ban.c */
bool    check_ban   args( ( char *site, int type) );

/* clan.c */
extern  int             top_clan;
extern struct  clan_type       clan_table[MAX_CLAN];
char *  clan_bit_name   args( ( int clan_flags ) );
bool    is_clan         args( (CHAR_DATA *ch) );
bool    is_same_clan    args( (CHAR_DATA *ch, CHAR_DATA *victim) );
int     clan_lookup     args( (const char *name) );
void    do_promote      args( (CHAR_DATA *ch, char *argument) );
void    do_demote   args( (CHAR_DATA *ch, char *argument) );
char    *player_clan    args( (CHAR_DATA *ch) );

/* comm.c */
void    sig_handler args(  (int sig) );
void    show_string args( ( struct descriptor_data *d, char *input) );
void    close_socket    args( ( DESCRIPTOR_DATA *dclose ) );
void    write_to_buffer args( ( DESCRIPTOR_DATA *d, const char *txt,
                int length ) );
void    send_to_char    args( ( const char *txt, CHAR_DATA *ch ) );
void    page_to_char    args( ( const char *txt, CHAR_DATA *ch ) );
void    act     args( ( const char *format, CHAR_DATA *ch,
                const void *arg1, const void *arg2, int type ) );
void    act_new     args( ( const char *format, CHAR_DATA *ch,
                const void *arg1, const void *arg2, int type,
                int min_pos, bool channel) );
void    act_new2        args( ( const char *format, CHAR_DATA *ch,
                const void *arg1, const void *arg2, int type,
                int min_pos) );
void    printf_to_char  args( ( CHAR_DATA *ch, char *fmt, ... ) );
void wrapln         args ( (CHAR_DATA *ch, const char *buf) );
void     wraplnf            args ( (CHAR_DATA *ch, const char *fmt, ...) );


void    cprintf     args ( (CHAR_DATA *ch, char *fmt, ... ) );
void    bugf        args( ( char *, ... ) );

//cmdedit.c
void   save_cmd_table   args( ( ) );
void   load_cmd_list    args( ( ) );

//qiedit saves
void    save_qitem_table args( ( ) );
void    load_quest_list args( ( ) );
void    place_qitems args( ( ) );
/*
 * Colour stuff by Lope of Loping Through The MUD
 */
int colour      args( ( char type, CHAR_DATA *ch, char *string ) );
void    colourconv  args( ( char *buffer, const char *txt, CHAR_DATA *ch ) );
void    send_to_char_bw args( ( const char *txt, CHAR_DATA *ch ) );
void    page_to_char_bw args( ( const char *txt, CHAR_DATA *ch ) );

/* db.c */
void    reset_area      args( ( AREA_DATA * pArea ) );      /* OLC */
void    reset_room  args( ( ROOM_INDEX_DATA *pRoom ) ); /* OLC */
char *  print_flags args( ( int flag ));
void    boot_db     args( ( void ) );
void    area_update args( ( void ) );
CD *    create_mobile   args( ( MOB_INDEX_DATA *pMobIndex ) );
void    clone_mobile    args( ( CHAR_DATA *parent, CHAR_DATA *clone) );
OD *    create_object   args( ( OBJ_INDEX_DATA *pObjIndex, int level ) );
void    clone_object    args( ( OBJ_DATA *parent, OBJ_DATA *clone ) );
void    clear_char  args( ( CHAR_DATA *ch ) );
char *  get_extra_descr args( ( const char *name, EXTRA_DESCR_DATA *ed ) );
MID *   get_mob_index   args( ( int vnum ) );
OID *   get_obj_index   args( ( int vnum ) );
RID *   get_room_index  args( ( int vnum ) );
MPC *   get_prog_index args( ( int vnum, int type ) );
char    fread_letter    args( ( FILE *fp ) );
int fread_number    args( ( FILE *fp ) );
long    fread_flag  args( ( FILE *fp ) );
char *  fread_string    args( ( FILE *fp ) );
char *  fread_string_eol args(( FILE *fp ) );
void    fread_to_eol    args( ( FILE *fp ) );
char *  fread_word  args( ( FILE *fp ) );
long    flag_convert    args( ( char letter) );
void *  alloc_mem   args( ( int sMem ) );
void *  alloc_perm  args( ( int sMem ) );
void    free_mem    args( ( void *pMem, int sMem ) );
char *  str_dup     args( ( const char *str ) );
void    free_string args( ( char *pstr ) );
int number_fuzzy    args( ( int number ) );
int number_range    args( ( int from, int to ) );
int number_percent  args( ( void ) );
int number_door args( ( void ) );
int number_bits args( ( int width ) );
long     number_mm       args( ( void ) );
int dice        args( ( int number, int size ) );
int godice      args( ( int dice, int diff ) );
int interpolate args( ( int level, int value_00, int value_32 ) );
void    smash_tilde args( ( char *str ) );
char    *smash_colour   args( ( char *str ) );
bool    str_cmp     args( ( const char *astr, const char *bstr ) );
bool    str_prefix  args( ( const char *astr, const char *bstr ) );
bool    str_infix   args( ( const char *astr, const char *bstr ) );
bool    str_suffix  args( ( const char *astr, const char *bstr ) );
char *  capitalize  args( ( const char *str ) );
void    append_file args( ( CHAR_DATA *ch, char *file, char *str, bool add) );
void    bug     args( ( const char *str, int param ) );
void    log_string  args( ( const char *str ) );
void    tail_chain  args( ( void ) );
void    ansi_to_html    args( ( char *str, CHAR_DATA *ch, bool safe ) );
char    *squash_html    args( ( const char *txt, bool safe ) );
char * format_str_len(char * string, int length, int align);



/* dhampire.c */
void garou_frenzy   args( ( int sn, int level, CHAR_DATA *ch, void *vo, int target ) );

/* effect.c */
void    acid_effect args( (void *vo, int level, int dam, int target) );
void    cold_effect args( (void *vo, int level, int dam, int target) );
void    fire_effect args( (void *vo, int level, int dam, int target) );
void    poison_effect   args( (void *vo, int level, int dam, int target) );
void    shock_effect    args( (void *vo, int level, int dam, int target) );


/* fight.c */
bool    is_safe     args( (CHAR_DATA *ch, CHAR_DATA *victim ) );
bool    is_aggsafe  args( (CHAR_DATA *ch, CHAR_DATA *victim ) );
bool    is_safe_spell   args( (CHAR_DATA *ch, CHAR_DATA *victim, bool area ) );
void    violence_update args( ( void ) );
void    multi_hit   args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dt ) );
bool    damage      args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dam,
                    int dt, int class, bool show ) );

bool d10_damage args( (CHAR_DATA *ch, CHAR_DATA *victim, int damsuccess,
					int modifier, int dt, int dam_type, int defense, bool show, bool fatal) );

bool    damage_old      args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dam,
                                int dt, int class, bool show ) );
void    update_pos  args( ( CHAR_DATA *victim ) );
void    stop_fighting   args( ( CHAR_DATA *ch, bool fBoth ) );
void  kill_em           args( ( CHAR_DATA *ch, CHAR_DATA *victim) );
void  kill_triggers     args( ( CHAR_DATA *ch ));
void  raw_kill          args( ( CHAR_DATA *ch) );
bool    IN_LEVEL        args( (CHAR_DATA *ch, CHAR_DATA *victim ) );
int   get_modifier      args( (CHAR_DATA *ch, int type) );

/* note.c */
void sysnote          args ( (char *recipient, char *subject, char *body) );

//Garou stuff.

/* garou stuff */
/* breeds */
#define BREED_HOMID     1
#define BREED_METIS     2
#define BREED_LUPUS     3
/* auspices */
#define AUSPICE_RAGABASH    1
#define AUSPICE_THEURGE     2
#define AUSPICE_PHILODOX    3
#define AUSPICE_GALLIARD    4
#define AUSPICE_AHROUN      5
/* tribes */
#define TRIBE_SILVERFANG    1
#define TRIBE_WENDIGO       2
#define TRIBE_STARGAZER     3
#define TRIBE_GETOFFENRIS   4
#define TRIBE_SHADOWLORD    5

#define TYPE_NONE           0
#define TYPE_CLAN           1
#define TYPE_TRADITION      2
#define TYPE_TRIBE          3


/* garou change form forms */
#define CHANGED_HOMID           0
#define CHANGED_CRINOS          1
#define CHANGED_LUPUS           2

/* For new garou and others */
#define TEMP                    0
#define PERM                    1

/* new garou auspices */
#define RAGABASH                1
#define THEURGE                 2
#define PHILODOX                3
#define GALLIARD                4
#define AHROUN                  5

/* new garou forms + breeds*/
#define HOMID                   1
#define GLABRO                  2
#define CRINOS                  3
#define HISPO                   4
#define LUPUS                   5
#define METIS                   6

/* new garou renown types */
#define GLORY                   0
#define TEMP_GLORY              1
#define HONOR                   2
#define TEMP_HONOR              3
#define WISDOM                  4
#define TEMP_WISDOM             5

/* new garou tribes */
#define BLACK_FURY          1
#define CHILDREN_OF_GAIA    2
#define FIANNA              3
#define FENRIR       4
#define SHADOW_LORD         5
#define BONE_GNAWER      6
#define SILVER_FANG         7
#define WARDERSOFMEN        8
#define RED_TALON           9
#define SILENT_STRIDER      10

/* new garou totems */
#define SPIRIT_PEGASUS              0
#define SPIRIT_RAT                  1
#define SPIRIT_UNICORN              2
#define SPIRIT_STAG                 3
#define SPIRIT_FENRIS_WOLF          4
#define SPIRIT_COYOTE               5
#define SPIRIT_GRIFFIN              6
#define SPIRIT_GRANDFATHER_THUNDER  7
#define SPIRIT_OWL                  8
#define SPIRIT_FALCON               9
#define SPIRIT_CHIMERA              10
#define SPIRIT_LION                 11
#define SPIRIT_SPHINX               12
#define SPIRIT_BULL                 13
#define SPIRIT_WOLVERINE            14
#define SPIRIT_DOLPHIN              15
#define SPIRIT_WIND                 16
#define SPIRIT_FOX                  17
#define SPIRIT_RACCOON              18
#define SPIRIT_BEAR                 19
#define SPIRIT_COCKROACH            20
#define SPIRIT_SPARE2               21
#define SPIRIT_SPARE3               22
#define SPIRIT_SPARE4               23
#define SPIRIT_SPARE5               24


/* garou gifts */
extern  sh_int  gsn_gift_porcupine;
extern  sh_int  gsn_gift_blissfulignorance;
extern  sh_int  gsn_gift_pulseoftheinvisible;
extern  sh_int  gsn_gift_catfeet;
extern  sh_int  gsn_gift_resistpain;
extern  sh_int  gsn_gift_callofthewyld;
extern  sh_int  gsn_gift_razorclaws;
extern  sh_int  gsn_gift_silverclaws;
extern  sh_int  gsn_gift_sensewyrm;
extern  sh_int  gsn_gift_innerstrength;
extern  sh_int  gsn_gift_bloodyfeast;
extern  sh_int  gsn_gift_masteroffire;
extern sh_int  gsn_gift_persuasion;
extern sh_int  gsn_gift_speechoftheworld;
extern sh_int  gsn_gift_calmthesavagebeast;
extern sh_int  gsn_gift_reshapeobject;
extern sh_int  gsn_gift_burythewolf;
extern sh_int  gsn_gift_spiritward;
extern sh_int  gsn_gift_assimilation;
extern sh_int  gsn_gift_falsecomfort;
extern sh_int  gsn_gift_createelement;
extern sh_int  gsn_gift_shed;
extern sh_int  gsn_gift_curseofhatred;
extern sh_int  gsn_gift_sensesilver;
extern sh_int  gsn_gift_eyesofthecat;
extern sh_int  gsn_gift_mentalspeech;
extern sh_int  gsn_gift_witherlimb;
extern sh_int  gsn_gift_madness;
extern sh_int  gsn_gift_totemgift;
extern sh_int  gsn_gift_haresleap;
extern sh_int  gsn_gift_heightenedsenses;
extern sh_int  gsn_gift_leylines;
extern sh_int  gsn_gift_scentofsight;
extern sh_int  gsn_gift_sensetheunnatural;
extern sh_int  gsn_gift_devilschild;
extern sh_int  gsn_gift_namethespirit;
extern sh_int  gsn_gift_beastlife;
extern sh_int  gsn_gift_gnaw;
extern sh_int  gsn_gift_elementalgift;
extern sh_int  gsn_gift_songofthegreatbeast;
extern sh_int  gsn_gift_blurofthemilkyeye;
extern sh_int  gsn_gift_scentofrunningwater;
extern sh_int  gsn_gift_snowrunning;
extern sh_int  gsn_gift_senseofprey;
extern sh_int  gsn_gift_openmoonbridge;
extern sh_int  gsn_gift_reynardslie;
extern sh_int  gsn_gift_leperscurse;
extern sh_int  gsn_gift_lunasblessing;
extern sh_int  gsn_gift_thievingtalonsofthemagpie;
extern sh_int  gsn_gift_chimericalform;
extern sh_int  gsn_gift_motherstouch;
extern sh_int  gsn_gift_spiritspeech;
extern sh_int  gsn_gift_commandspirit;
extern sh_int  gsn_gift_sightfrombeyond;
extern sh_int  gsn_gift_expelspirit;
extern sh_int  gsn_gift_graspthebeyond;
extern sh_int  gsn_gift_spiritwassail;
extern sh_int  gsn_gift_nightmarepossession;
extern sh_int  gsn_gift_malleablespirit;
extern sh_int  gsn_gift_scentofthetrueform;
extern sh_int  gsn_gift_truthofgaia;
extern sh_int  gsn_gift_calltoduty;
extern sh_int  gsn_gift_strengthofpurpose;
extern sh_int  gsn_gift_spiritwitness;
extern sh_int  gsn_gift_warriorseye;
extern sh_int  gsn_gift_rollover;
extern sh_int  gsn_gift_scentofbeyond;
extern sh_int  gsn_gift_earthfriendspact;
extern sh_int  gsn_gift_geas;
extern sh_int  gsn_gift_beastspeech;
extern sh_int  gsn_gift_wakingdream;
extern sh_int  gsn_gift_callofthewyrm;
extern sh_int  gsn_gift_distractions;
extern sh_int  gsn_gift_eyeoftheasp;
extern sh_int  gsn_gift_songofrage;
extern sh_int  gsn_gift_shadowsbythefirelight;
extern sh_int  gsn_gift_songofmockery;
extern sh_int  gsn_gift_dreamgolems;
extern sh_int  gsn_gift_callforvengeance;
extern sh_int  gsn_gift_fallingtouch;
extern sh_int  gsn_gift_inspiration;
extern sh_int  gsn_gift_spiritofthefray;
extern sh_int  gsn_gift_truefear;
extern sh_int  gsn_gift_heartoffury;
extern sh_int  gsn_gift_fullmoonslight;
extern sh_int  gsn_gift_stokingfurysfurnace;
extern sh_int  gsn_gift_kissofhelios;
extern sh_int  gsn_gift_strengthofwill;
extern sh_int  gsn_gift_breathofthewyld;
extern sh_int  gsn_gift_mansskin;
extern sh_int  gsn_gift_curseoftheaeolus;
extern sh_int  gsn_gift_coupdegrace;
extern sh_int  gsn_gift_flamesofhestia;
extern sh_int  gsn_gift_bodywrack;
extern sh_int  gsn_gift_bowofpegasus;
extern sh_int  gsn_gift_gazeofthegorgon;
extern sh_int  gsn_gift_wyldwarp;
extern sh_int  gsn_gift_cookery;
extern sh_int  gsn_gift_resisttoxin;
extern sh_int  gsn_gift_scentofthehoneycomb;
extern sh_int  gsn_gift_corneredrat;
extern sh_int  gsn_gift_plaguevisage;
extern sh_int  gsn_gift_calltherust;
extern sh_int  gsn_gift_friendinneed;
extern sh_int  gsn_gift_blink;
extern sh_int  gsn_gift_clawsoftheglassblower;
extern sh_int  gsn_gift_mawofthewhale;
extern sh_int  gsn_gift_survivor;
extern sh_int  gsn_gift_evesblessing;
extern sh_int  gsn_gift_grandmotherstouch;
extern sh_int  gsn_gift_lunasarmor;
extern sh_int  gsn_gift_dazzle;
extern sh_int  gsn_gift_spiritfriend;
extern sh_int  gsn_gift_angelssemblance;
extern sh_int  gsn_gift_haloofthesun;
extern sh_int  gsn_gift_thelivingwood;
extern sh_int  gsn_gift_huntersharmony;
extern sh_int  gsn_gift_visageoffenris;
extern sh_int  gsn_gift_haltthecowardsflight;
extern sh_int  gsn_gift_senseguilt;
extern sh_int  gsn_gift_mightofthor;
extern sh_int  gsn_gift_berserkerssong;
extern sh_int  gsn_gift_ironcannotpierce;
extern sh_int  gsn_gift_blazeofglory;
extern sh_int  gsn_gift_hordeofvalhalla;
extern sh_int  gsn_gift_distractthefool;
extern sh_int  gsn_gift_faerielight;
extern sh_int  gsn_gift_howlofthebanshee;
extern sh_int  gsn_gift_warriorsdraught;
extern sh_int  gsn_gift_faerieaid;
extern sh_int  gsn_gift_woadling;
extern sh_int  gsn_gift_balorsgaze;
extern sh_int  gsn_gift_semblance;
extern sh_int  gsn_gift_fogonthemoor;
extern sh_int  gsn_gift_giftofthespriggan;
extern sh_int  gsn_gift_wolfatthedoor;
extern sh_int  gsn_gift_auraofconfidence;
extern sh_int  gsn_gift_fatalflaw;
extern sh_int  gsn_gift_seizingtheedge;
extern sh_int  gsn_gift_clapofthunder;
extern sh_int  gsn_gift_ravenscurse;
extern sh_int  gsn_gift_shadowcutting;
extern sh_int  gsn_gift_dolorousstroke;
extern sh_int  gsn_gift_strengthofthedominator;
extern sh_int  gsn_gift_shadowpack;
extern sh_int  gsn_gift_obedience;
extern sh_int  gsn_gift_speedofthought;
extern sh_int  gsn_gift_silence;
extern sh_int  gsn_gift_eyeofthefalcon;
extern sh_int  gsn_gift_lambentfire;
extern sh_int  gsn_gift_empathy;
extern sh_int  gsn_gift_exceptionalswordplay;
extern sh_int  gsn_gift_wrathofgaia;
extern sh_int  gsn_gift_lordlywill;
extern sh_int  gsn_gift_sidestepdeath;
extern sh_int  gsn_gift_lunasavenger;
extern sh_int  gsn_gift_pawsofthenewborncub;
extern sh_int  gsn_gift_artisanscommand;
extern sh_int  gsn_gift_giftofsalt;
extern sh_int  gsn_gift_maskthepredator;
extern sh_int  gsn_gift_tongues;
extern sh_int  gsn_gift_steelfur;
extern sh_int  gsn_gift_attunement;
extern sh_int  gsn_gift_doppelganger;
extern sh_int  gsn_gift_commandtheblaze;
extern sh_int  gsn_gift_redirectpain;


/* new garou stuff */
extern  sh_int  gsn_rage;
extern  sh_int  gsn_spirit_totem;

/* gift.c */
int gift_lookup args( ( const char *name ) );

/*garou.c*/
void renown_gain args ( (CHAR_DATA *ch ) );


/* pulse.c */
void affects_update args( (void) );

/* handler.c */
int     get_aff_mod args((CHAR_DATA *ch, int sn, int loc));
AD      *affect_find args( (AFFECT_DATA *paf, int sn));
void    affect_check    args( (CHAR_DATA *ch, int where, int vector) );
int count_users args( (OBJ_DATA *obj) );
void    deduct_cost args( (CHAR_DATA *ch, int cost) );
int     check_immune    args( (CHAR_DATA *ch, int dam_type) );
int     material_lookup args( ( const char *name) );
int weapon_lookup   args( ( const char *name) );
int weapon_type args( ( const char *name) );
char    *weapon_name    args( ( int weapon_Type) );
char    *item_name  args( ( int item_type) );
int attack_lookup   args( ( const char *name) );
long    wiznet_lookup   args( ( const char *name) );
int class_lookup    args( ( const char *name) );
int classgroup_lookup   args( ( const char *name) );
int roll_stats  args( () );
bool    is_old_mob  args ( (CHAR_DATA *ch) );
int get_skill   args( ( CHAR_DATA *ch, int sn ) );
int get_weapon_sn   args( ( CHAR_DATA *ch ) );
int get_weapon_skill args(( CHAR_DATA *ch, int sn ) );
int     get_age         args( ( CHAR_DATA *ch ) );
void    reset_char  args( ( CHAR_DATA *ch )  );
int get_trust   args( ( CHAR_DATA *ch ) );
int get_curr_stat   args( ( CHAR_DATA *ch, int stat ) );
int get_attribute   args( ( CHAR_DATA *ch, int stat ) );
int get_ability     args( ( CHAR_DATA *ch, int stat ) );
int get_ability2    args( ( CHAR_DATA *ch, int stat ) );
int get_discipline  args( ( CHAR_DATA *ch, int disc) );
int get_gauntlet    args( ( CHAR_DATA *ch ) );
char *wordwrap args( (int desired_line, int length, char *argument, int alignment ) );
//char *wordwrap args( ( int line, int length, const char *argument ) );
char* wordwrap_str args( ( const char* src, short num_columns, const char* nstr ) );
char *wrapstr  args(( CHAR_DATA *ch, const char *str ));
int quest_status    args( ( CHAR_DATA *ch, int quest ) );
bool    add_quest       args( ( CHAR_DATA *ch, int quest ) );
int     get_max_train   args( ( CHAR_DATA *ch, int stat ) );
int can_carry_n args( ( CHAR_DATA *ch ) );
int can_carry_w args( ( CHAR_DATA *ch ) );
bool    is_name     args( ( char *str, char *namelist ) );
bool    is_exact_name   args( ( char *str, char *namelist ) );
int IS_VAMP     args( ( CHAR_DATA *ch ) );
bool    has_blood	args( ( CHAR_DATA *ch ) );
bool    breathes	args( ( CHAR_DATA *ch ) );
bool    is_natural  args( ( CHAR_DATA *ch) );
bool    higher_beast args( ( CHAR_DATA *ch) );
bool    beast_speech args( ( CHAR_DATA *ch) );
bool    human_variant args( ( CHAR_DATA *ch) );
bool    is_humanoid args( ( CHAR_DATA *ch));
bool    is_supernatural args( ( CHAR_DATA *ch));
bool    is_umbral_denizen args( ( CHAR_DATA *ch) );
char    *name_the_spirit args( (CHAR_DATA *ch) );
int     get_wound_penalty args( (CHAR_DATA * ch) );
bool    has_silver args( ( CHAR_DATA *ch ) );
bool    carries_silver_obj args( ( CHAR_DATA *ch) );
void    affect_to_char  args( ( CHAR_DATA *ch, AFFECT_DATA *paf ) );
void    affect_to_obj   args( ( OBJ_DATA *obj, AFFECT_DATA *paf ) );
void    affect_remove   args( ( CHAR_DATA *ch, AFFECT_DATA *paf ) );
void    affect_remove_obj args( (OBJ_DATA *obj, AFFECT_DATA *paf ) );
void    affect_strip    args( ( CHAR_DATA *ch, int sn ) );
bool    is_affected args( ( CHAR_DATA *ch, int sn ) );
int     get_affect_level    args( ( CHAR_DATA *ch, int sn ) );
int     get_affect_modifier args( ( CHAR_DATA *ch, int sn ) );
bool    has_affect_modifier args( ( CHAR_DATA *ch, int sn, int mod ) );
int     get_affect_duration args( ( CHAR_DATA *ch, int sn ) );
void    affect_join args( ( CHAR_DATA *ch, AFFECT_DATA *paf ) );
int     stealth_int_shadowplay   args( (CHAR_DATA *ch, int diff) );
void    char_from_room  args( ( CHAR_DATA *ch ) );
void    char_to_room    args( ( CHAR_DATA *ch, ROOM_INDEX_DATA *pRoomIndex ) );
void    obj_to_char args( ( OBJ_DATA *obj, CHAR_DATA *ch ) );
void    obj_from_char   args( ( OBJ_DATA *obj ) );
int apply_ac    args( ( OBJ_DATA *obj, int iWear, int type ) );
OD *    get_eq_char args( ( CHAR_DATA *ch, int iWear ) );
OD *    get_consumable args( ( CHAR_DATA *ch, int consumable ) );
void    *use_consumable args( ( CHAR_DATA *ch, OBJ_DATA *consumable, int change ) );
void    equip_char  args( ( CHAR_DATA *ch, OBJ_DATA *obj, int iWear ) );
void    unequip_char    args( ( CHAR_DATA *ch, OBJ_DATA *obj ) );
int count_obj_list  args( ( OBJ_INDEX_DATA *obj, OBJ_DATA *list ) );
void    obj_from_room   args( ( OBJ_DATA *obj ) );
void    obj_to_room args( ( OBJ_DATA *obj, ROOM_INDEX_DATA *pRoomIndex ) );
void    obj_to_obj  args( ( OBJ_DATA *obj, OBJ_DATA *obj_to ) );
void    obj_from_obj    args( ( OBJ_DATA *obj ) );
void    restore_affects args(   ( CHAR_DATA *ch ) );
void    extract_obj args( ( OBJ_DATA *obj ) );
void    extract_char    args( ( CHAR_DATA *ch, bool fPull ) );
CD *    get_char_room   args( ( CHAR_DATA *ch, ROOM_INDEX_DATA *room, char *argument ) );
CD *    get_char_world  args( ( CHAR_DATA *ch, char *argument ) );
CD *    get_char_area   args( ( CHAR_DATA *ch, char *argument ) );
OD *    get_obj_type    args( ( OBJ_INDEX_DATA *pObjIndexData ) );
OD *    get_obj_list    args( ( CHAR_DATA *ch, char *argument,
                OBJ_DATA *list ) );
OD *    get_obj_list2   args( ( CHAR_DATA *ch, char *argument,
                OBJ_DATA *list ) );
OD *    get_obj_carry   args( ( CHAR_DATA *ch, char *argument,
                CHAR_DATA *viewer ) );
OD *    get_carry_vnum  args( ( CHAR_DATA *ch, int vnum, int location, bool sight) );

OD *    get_obj_wear    args( ( CHAR_DATA *ch, char *argument, bool character ) );
OD *    get_obj_here    args( ( CHAR_DATA *ch, ROOM_INDEX_DATA *room, char *argument ) );
OD *    get_obj_hidden  args( ( CHAR_DATA *ch, char *argument ) );
OD *    get_obj_world   args( ( CHAR_DATA *ch, char *argument, bool unseen ) );
OD *    get_obj_anywhere   args( ( CHAR_DATA *ch, char *argument) );
OD *    create_money    args( ( int gold, int silver ) );
int get_obj_number  args( ( OBJ_DATA *obj ) );
int get_obj_weight  args( ( OBJ_DATA *obj ) );
int get_true_weight args( ( OBJ_DATA *obj ) );
bool    room_is_dark    args( ( ROOM_INDEX_DATA *pRoomIndex ) );
bool    room_is_silent  args( ( CHAR_DATA *ch, ROOM_INDEX_DATA *pRoomIndex ) );
bool    is_room_owner   args( ( CHAR_DATA *ch, ROOM_INDEX_DATA *room) );
bool    room_is_private args( ( ROOM_INDEX_DATA *pRoomIndex ) );
bool    can_see     args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool    can_see_obj args( ( CHAR_DATA *ch, OBJ_DATA *obj ) );
bool    can_see_room    args( ( CHAR_DATA *ch, ROOM_INDEX_DATA *pRoomIndex) );
bool    can_drop_obj    args( ( CHAR_DATA *ch, OBJ_DATA *obj ) );
char *  get_time_string args( ( int hours) );
bool    send_email      args( ( char *subject, char *address, char *body) );
char *  affect_loc_name args( ( int location ) );
char *  affect_bit_name args( ( int vector ) );
char *  affect2_bit_name    args( ( int vector ) );
char *  extra_bit_name  args( ( int extra_flags ) );
char *  wear_bit_name   args( ( int wear_flags ) );
char *  act_bit_name    args( ( int act_flags ) );
char *  act2_bit_name   args( ( int act_flags ) );
char *  off_bit_name    args( ( int off_flags ) );
char *  imm_bit_name    args( ( int imm_flags ) );
char *  form_bit_name   args( ( int form_flags ) );
char *  part_bit_name   args( ( int part_flags ) );
char *  weapon_bit_name args( ( int weapon_flags ) );
char *  comm_bit_name   args( ( int comm_flags ) );
char *  cont_bit_name   args( ( int cont_flags) );
char *  trap_bit_name   args( ( int trap_flags ) );
char *  light_bit_name  args( ( int light_flags ) );
char *  trigger_bit_name    args( ( int trigger_flags ) );
char *  focus_bit_name  args( ( int focus_flags ) );
char *  room_bit_name   args( ( int room_flags ) );
bool *  has_weapon_bit      args( ( int weapon_flags, char *weapon_bit ) );
bool *  has_room_bit        args( ( int room_flags, char *room_bit ) );
bool *  room_clan_only      args( ( int clan_room, char *clan_input) );
char *  match_length    args( ( char *text, int length ) );
char *  center      args( ( char *argument, int width, char *fill ) );
char *  dots        args( ( int num, bool plus ) );
char *  dots2       args( ( int num, bool space) );
char *  dots3       args( ( int num, bool space ) );
char *  dots6       args( ( int num, bool space ) );
char *  bloodpooldot    args( ( int num, bool space ) );
char *  bloodpooldot2   args( ( int num, bool space ) );
char *  quintessencedot    args( ( int num, bool space ) );
char *  quintessencedot2   args( ( int num, bool space ) );
bool    is_clan_disc    args( (CHAR_DATA *ch, int disc));
bool    update_csstats  args( (CHAR_DATA *ch));
char *  strip_color     args( (const char *string) );
int     strlen_color    args( (const char *string) );
bool    canwear_objaffect args( (CHAR_DATA *ch, OBJ_DATA *obj) );
bool    pass_gauntlet   args( (CHAR_DATA *ch, bool show) );
void    update_moston  args( (void) );
/* interp.c */
void    interpret   args( ( CHAR_DATA *ch, char *argument ) );
bool    is_number   args( ( char *arg ) );
int number_argument args( ( char *argument, char *arg ) );
int mult_argument   args( ( char *argument, char *arg) );
char *  one_argument    args( ( char *argument, char *arg_first ) );
char *  one_argument2   args( ( char *argument, char *arg_first ) );
extern CMD_DATA * cmd_first;
extern CMD_DATA * cmd_last;
struct cmdfun_type
{
    char *name;
    DO_FUN *do_fun;
};

/* magic.c */
int find_spell  args( ( CHAR_DATA *ch, const char *name) );
int     mana_cost   (CHAR_DATA *ch, int min_mana, int level);
int skill_lookup    args( ( const char *name ) );
int slot_lookup args( ( int slot ) );
bool    saves_spell args( ( int level, CHAR_DATA *victim, int dam_type ) );
void    say_spell   args( ( CHAR_DATA *ch, int sn ) );
void    obj_cast_spell  args( ( int sn, int level, CHAR_DATA *ch,
                    CHAR_DATA *victim, OBJ_DATA *obj ) );
/* mob_prog.c */
PROG_CODE *get_mprog_by_vnum args( (int vnum) );

/* mob_prog.c */
void	program_flow	args( ( sh_int vnum, char *source, CHAR_DATA *mob,
				OBJ_DATA *obj, ROOM_INDEX_DATA *room,
				CHAR_DATA *ch, const void *arg1,
				const void *arg2, int numlines ) );
void	p_act_trigger	args( ( char *argument, CHAR_DATA *mob,
				OBJ_DATA *obj, ROOM_INDEX_DATA *room,
				CHAR_DATA *ch, const void *arg1,
				const void *arg2, int type ) );
bool	p_percent_trigger args( ( CHAR_DATA *mob, OBJ_DATA *obj,
				ROOM_INDEX_DATA *room, CHAR_DATA *ch,
				const void *arg1, const void *arg2, int type ) );
bool	p_time_trigger args( ( CHAR_DATA *mob, OBJ_DATA *obj,
				ROOM_INDEX_DATA *room, CHAR_DATA *ch,
				const void *arg1, const void *arg2, int type ) );
void	p_bribe_trigger  args( ( CHAR_DATA *mob, CHAR_DATA *ch, int amount ) );
bool	p_exit_trigger   args( ( CHAR_DATA *ch, int dir, int type ) );
void	p_give_trigger   args( ( CHAR_DATA *mob, OBJ_DATA *obj,
				ROOM_INDEX_DATA *room, CHAR_DATA *ch,
				OBJ_DATA *dropped, int type ) );
void 	p_greet_trigger  args( ( CHAR_DATA *ch, int type ) );
void	p_hprct_trigger  args( ( CHAR_DATA *mob, CHAR_DATA *ch ) );
void    p_talk_trigger  args( ( CHAR_DATA *mob, CHAR_DATA *ch ) );

/* mob_cmds.c */
void    mob_interpret   args( ( CHAR_DATA *ch, char *argument ) );
void	obj_interpret	args( ( OBJ_DATA *obj, char *argument ) );
void room_interpret	args( ( ROOM_INDEX_DATA *room, char *argument ) );

/* save.c */
void    save_char_obj   args( ( CHAR_DATA *ch ) );
bool    load_char_obj   args( ( DESCRIPTOR_DATA *d, char *name ) );

/* skills.c */
bool    parse_gen_groups args( ( CHAR_DATA *ch,char *argument ) );
void    list_group_costs args( ( CHAR_DATA *ch ) );
void    list_group_known args( ( CHAR_DATA *ch ) );
int     exp_per_level   args( ( CHAR_DATA *ch, int points ) );
void    check_improve   args( ( CHAR_DATA *ch, int sn, bool success,
                    int multiplier ) );
int     group_lookup    args( (const char *name) );
void    gn_add      args( ( CHAR_DATA *ch, int gn) );
void    gn_remove   args( ( CHAR_DATA *ch, int gn) );
void    group_add   args( ( CHAR_DATA *ch, const char *name, bool deduct) );
void    group_remove    args( ( CHAR_DATA *ch, const char *name) );

/* special.c */
SF *    spec_lookup args( ( const char *name ) );
char *  spec_name   args( ( SPEC_FUN *function ) );

/* teleport.c */
RID *   room_by_name    args( ( char *target, int level, bool error) );

/* update.c */
void    advance_level   args( ( CHAR_DATA *ch, bool hide ) );
void    gain_exp    args( ( CHAR_DATA *ch, int gain ) );
void    gain_qp     args( (CHAR_DATA *ch, int gain));
void    gain_condition  args( ( CHAR_DATA *ch, int iCond, int value ) );
void    update_handler  args( ( void ) );
void    quest_update    args( ( void ) ); /* Vassago - quest.c */


/* mount.c */
int     mount_success   args( ( CHAR_DATA *ch, CHAR_DATA *mount, int canattack ) );
void    do_mount        args( ( CHAR_DATA *ch, char *argument ) );
void    do_dismount     args( ( CHAR_DATA *ch, char *argument ) );
void    do_buy_mount    args( ( CHAR_DATA *ch, char *argument ) );


/* string.c */
void    string_edit args( ( CHAR_DATA *ch, char **pString ) );
void    string_append   args( ( CHAR_DATA *ch, char **pString ) );
char *  string_replace  args( ( char * orig, char * old, char * new ) );
void    string_add      args( ( CHAR_DATA *ch, char *argument ) );
char *  format_string   args( ( char *oldstring /*, bool fSpace */ ) );
char *  wrapstring      args( ( char *oldstring ) );
char *  first_arg       args( ( char *argument, char *arg_first, bool fCase ) );
char *  string_unpad    args( ( char * argument ) );
char *  string_proper   args( ( char * argument ) );
bool grudge_check           args( ( CHAR_DATA *ch, CHAR_DATA *victim) );
bool grudge_update      args( ( CHAR_DATA *ch, CHAR_DATA *victim) );
bool paradox_check          args( ( CHAR_DATA *ch, bool vulgar) );


/* olc.c */
bool    run_olc_editor  args( ( DESCRIPTOR_DATA *d ) );
char    *olc_ed_name    args( ( CHAR_DATA *ch ) );
char    *olc_ed_vnum    args( ( CHAR_DATA *ch ) );
CLAN_DATA *get_clan_data     args( ( char *name ) );

/* olc_save.c */
void    save_clans      args( ( CHAR_DATA *ch, char *argument ) );
char    *fwrite_flag    args( ( long flags, char buf[] ) );

/* lookup.c */
int race_lookup args( ( const char *name) );
int item_lookup args( ( const char *name) );
int liq_lookup  args( ( const char *name) );
int clan_lookup args( ( const char *name) );
int tool_lookup args( ( const char *name) );
int crafted_lookup  args( ( const char *name) );

/* vampire.c */
void vamp_frenzy    args( ( int sn, int level, CHAR_DATA *ch, void *vo, int target ) );
void thaumaturgy_frenzy args( ( int sn, int level, CHAR_DATA *ch, void *vo, int target ) );
bool make_vamp      args( ( CHAR_DATA *ch, char *argument ) );
void check_unseen args ( (CHAR_DATA *ch, int type) );
#define UNSEEN_TALK 0
#define UNSEEN_MOVE 1



/* pipe.c */
char    *get_piperesult( char *cmd );

#undef  CD
#undef  MID
#undef  OD
#undef  OID
#undef  RID
#undef  SF
#undef AD


/*****************************************************************************
 *                                    OLC                                    *
 *****************************************************************************/

/*
 * Object defined in limbo.are
 * Used in save.c to load objects that don't exist.
 */
#define OBJ_VNUM_DUMMY  30

/*
 * Area flags.
 */
#define         AREA_NONE        0
#define         AREA_CHANGED    (A)   /* Area has been modified. */
#define         AREA_ADDED      (B)   /* Area has been added to. */
#define         AREA_LOADING    (C)   /* Used for counting in db.c */
#define         AREA_PKILL      (D)   /* Used for pure PK areas. */
#define         AREA_UNLINKED   (E)
#define			AREA_NOTELE		(F)   /* Used to limit teleport like effects */


#define MAX_DIR 6
#define NO_FLAG -99 /* Must not be used in flags or stats. */

/*
 * Global Constants
*/
extern  char *  const   dir_name        [];
extern  const   sh_int  rev_dir         [];          /* sh_int - ROM OLC */
extern  const   struct  spec_type   spec_table  [];

/*
 * Global variables
 */
extern      AREA_DATA *     area_first;
extern      AREA_DATA *     area_last;
extern      SHOP_DATA *     shop_last;
extern      CMD_DATA * cmd_first;
extern      CMD_DATA * cmd_last;
extern      PC_DATA *   pc_first;
extern      PC_DATA *   pc_last;

extern      int         top_affect;
extern      int         top_area;
extern      int         top_ed;
extern      int         top_exit;
extern      int         top_help;
extern      int         top_mob_index;
extern      int         top_obj_index;
extern      int         top_reset;
extern      int         top_room;
extern      int         top_shop;

extern      int         top_vnum_mob;
extern      int         top_vnum_obj;
extern      int         top_vnum_room;

extern  int     pulse_area;
extern  int     pulse_mobile;
extern  int     pulse_violence;
extern  int     pulse_point;
extern  int     pulse_music;
extern  int     pulse_influence;
extern  int     pulse_influence_fade;
extern  int     pulse_bank;
extern  int     pulse_weather;
extern  int     pulse_spells;
extern  int     pulse_regen;

extern      char            str_empty       [1];

extern      MOB_INDEX_DATA *    mob_index_hash  [MAX_KEY_HASH];
extern      OBJ_INDEX_DATA *    obj_index_hash  [MAX_KEY_HASH];
extern      ROOM_INDEX_DATA *   room_index_hash [MAX_KEY_HASH];

extern      char            *help_list  [MAX_HELP];

CMD_DATA *cmd_lookup(const char *name );
