/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,    *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
 *                                     *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael      *
 *  Chastain, Michael Quan, and Mitchell Tse.                  *
 *                                     *
 *  In order to use any part of this Merc Diku Mud, you must comply with   *
 *  both the original Diku license in 'license.doc' as well the Merc       *
 *  license in 'license.txt'.  In particular, you may not remove either of *
 *  these copyright notices.                           *
 *                                     *
 *  Much time and thought has gone into this software and you are      *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                          *
 ***************************************************************************/

/***************************************************************************
*   ROM 2.4 is copyright 1993-1998 Russ Taylor             *
*   ROM has been brought to you by the ROM consortium          *
*       Russ Taylor (rtaylor@efn.org)                  *
*       Russ Taylor (rtaylor@hypercube.org)                *
*       Gabrielle Taylor (gtaylor@hypercube.org)               *
*   By using this code, you have agreed to follow the terms of the     *
*   ROM license, in the file Rom24/doc/rom.license             *
***************************************************************************/
struct cdisc_type
{
    int clan_disc[3];
};

struct hometown_type
{
	char *name;
	int  vnum;
};

struct cr_abil_type
{
    int abil[MAX_ABILITIES];
    int virtue[3];
};

struct cr_attribute_type
{
    int attribute[MAX_ATTRIBUTES];
};

struct template_type
{
    char *name;
    sh_int attribute[MAX_ATTRIBUTES];
    sh_int ability[MAX_ABILITIES];
    sh_int willpower;
};

struct flag_type
{
    char *name;
    int bit;
    bool settable;
};


struct sect_type
{
    char    *name;
    char    *cname;
    char    *who_name;
    sh_int  hall;
    bool    vamp;
};
struct  rote_type
{
    char *  name;           /* Name of the rote */
    ROTE_FUN *  rote_fun;       /* function containing the Rote */
    sh_int  correspondence;  /* Spheres */
    sh_int  entropy;
    sh_int  forces;
    sh_int  life;
    sh_int  matter;
    sh_int  mind;
    sh_int  prime;
    sh_int  spirit;
    sh_int  time;
    sh_int  target;
    sh_int  quintessence;   /* quintessence required */
    sh_int  diff;           /* diff of the rote */
    bool vulgar;
    bool passthrough;
};


struct tradition_type
{
    char    *name;
    sh_int  sphere;
    char    *foci;
};


struct position_type
{
    char *name;
    char *short_name;
};

struct name_type
{
    char *name;
};

struct  bit_type
{
    const   struct  flag_type * table;
    char *              help;
};

struct  brew_type
{
    char *name;
    sh_int sn;
    int level;
    int plevel;
    int diff;
    sh_int component[5];
    bool mage;
    bool immonly;
    char *color;
};

struct  gift_type
{
    char *name;
    int level;
    int lag;
    int fail;
    int breed;
    int auspice;
    int tribe;
    int rank;
    int rage;
};


// name, race specific?
struct csback_type
{
    char *name;
    int race;
};

struct cs_type2
{
    char *name;
    int req1;
    int req2;
    int req3;
    int req4;
    int req5;
    int depth;
};

struct  garou_form_type
{
    char *name;
    char *long_name;
    int strength;
    int dexterity;
    int stamina;
    int appearance;
    int manipulation;
    int perception;
    int diff;
};

struct  tribe_type
{
    char *name;
    char *whoname;
    int totem;
    char *totemname;
};

struct  crafted_item_type
{
    char *name;
    char *type;
    int vnum;
    int skill;
    int lvl;
    int resource_type;
    int comp[5];
    int socket[5];
    int best[10];
};

struct mob_group_type
{
    char *name;
    int groupnumber;
    bool raceonly;
    char *race1;
    char *race2;
    char *race3;
};

/* game tables */
extern  const   struct cdisc_type clandisc_table[MAX_CLAN];
extern  const   struct  sect_type   sect_table[MAX_SECT];
extern  const   struct  brew_type   brew_table[MAX_BREW];
extern  const   struct  position_type   position_table[];
extern  const   struct  name_type    sex_table[];
extern  const   struct  name_type   size_table[];
extern  const   struct  tradition_type  tradition_table[MAX_TRADITION];
extern  const   struct  tribe_type  tribe_table[];
extern  const   struct  rote_type   rote_table[MAX_ROTE];
extern  const   struct  name_type ability_table[30];
extern  const   struct  cs_type2    sec_abil_table[30];
extern  const   struct  name_type attribute_table[9];
extern  const   struct  garou_form_type garou_form_table[6];
extern  const   struct  crafted_item_type   crafted_item_table[];
extern  const   struct  gift_type   gift_table  [MAX_GIFTS_CODED];
extern  const   struct  name_type   disc_table  [MAX_DISC];
extern  const   struct  name_type   sphere_table [MAX_SPHERE+1];
extern  const   struct  csback_type csback_table    [MAX_CSBACK];
extern  const   struct  cr_abil_type    cr_abil_table   [3];
extern  const   struct  cr_attribute_type   cr_attribute_table  [3];
extern  const   struct  template_type   template_table[];
extern 	const 	struct 	hometown_type 		hometown_table  [MAX_HOMETOWN];
extern  const   struct  name_type    dir_table[];
extern  const   struct  mob_group_type  mob_group_table[];
/* flag tables */
extern  const   struct  flag_type   act_flags[];
extern  const   struct  flag_type   act2_flags[];
extern  const   struct  flag_type   attr_flags[];
extern  const   struct  flag_type   abil_flags[];
extern  const   struct  flag_type   plr_flags[];
extern  const   struct  flag_type   plr2_flags[];
extern  const   struct  flag_type   affect_flags[];
extern  const   struct  flag_type   affect2_flags[];
extern  const   struct  flag_type   off_flags[];
extern  const   struct  flag_type   imm_flags[];
extern  const   struct  flag_type   form_flags[];
extern  const   struct  flag_type   part_flags[];
extern  const   struct  flag_type   comm_flags[];
extern  const   struct  flag_type   extra_flags[];
extern  const   struct  flag_type   wear_flags[];
extern  const   struct  flag_type   weapon_flags[];
extern  const   struct  flag_type   container_flags[];
extern  const   struct  flag_type   portal_flags[];
extern  const   struct  flag_type   room_flags[];
extern  const   struct  flag_type   exit_flags[];
extern  const   struct  flag_type   mprog_flags[];
extern	const	struct	flag_type	oprog_flags[];
extern	const	struct	flag_type	rprog_flags[];
extern  const   struct  flag_type   area_flags[];
extern  const   struct  flag_type   sector_flags[];
extern  const   struct  flag_type   door_resets[];
extern  const   struct  flag_type   wear_loc_strings[];
extern  const   struct  flag_type   wear_loc_flags[];
extern  const   struct  flag_type   res_flags[];
extern  const   struct  flag_type   imm_flags[];
extern  const   struct  flag_type   vuln_flags[];
extern  const   struct  flag_type   type_flags[];
extern  const   struct  flag_type   clantype_flags[];
extern  const   struct  flag_type   apply_flags[];
extern  const   struct  flag_type   sex_flags[];
extern  const   struct  flag_type   furniture_flags[];
extern  const   struct  flag_type   light_flags[];
extern  const   struct  flag_type   trap_flags[];
extern  const   struct  flag_type   trigger_flags[];
extern  const   struct  flag_type   focus_flags[];
extern  const   struct  flag_type   weapon_class[];
extern  const   struct  flag_type   apply_types[];
extern  const   struct  flag_type   weapon_type2[];
extern  const   struct  flag_type   apply_types[];
extern  const   struct  flag_type   size_flags[];
extern  const   struct  flag_type   position_flags[];
extern  const   struct  flag_type   ac_type[];
extern  const   struct  bit_type    bitvector_type[];


extern  const   struct  flag_type       clan_flags[];
