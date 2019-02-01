#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "merc.h"

void spells_update (void) {
    CHAR_DATA *ch;
    CHAR_DATA *ch_next;
    AFFECT_DATA af;


    /* Examine all mobs. */
    for ( ch = char_list; ch != NULL; ch = ch_next )
    {
    ch_next = ch->next;

    //EMERGOD POISON ACTUALLY SUCKS NOW CODE
    if ( IS_AFFECTED (ch, AFF_POISON) && ch->hit > (ch->max_hit / 20) )
        damage(ch,ch,
            (ch->hit/(50*get_attribute(ch, STAMINA) > 0 ? get_attribute(ch, STAMINA) : 1 )),
            gsn_poison,DAM_POISON,FALSE);

    if (IS_NPC(ch))
        continue;
        
//Making people being IT annoying. /*
/* Disabling random code for now
int r = arc4random_uniform(500);
if ((r > 495) && (ch->tagged == 1) && (!IS_IMMORTAL(ch)))
{
ch->move = 1;
send_to_char("Pesky being tagged as it isn't it?\n\r", ch);
send_to_char("You probably should {Ytag{x someone else!\n\r",ch);
} */
/*int r2 = arc4random_uniform(500);
if ((r2 > 499) && (ch->tagged == 1) && (!IS_IMMORTAL(ch)))
{

    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    ROOM_INDEX_DATA *location;
    int vnum;

    if (IS_NPC(ch) )
    {
    send_to_char("Only players can recall.\n\r",ch);
    return;
    }

    act( "$n calls out to the heavens for safe transportation!", ch, 0, 0, TO_ROOM );

    if (!IS_NPC(ch) && ch->pcdata->hometown && ch->pcdata->hometown < MAX_HOMETOWN)
	    vnum = get_hometown(ch->pcdata->hometown);
    else
	    vnum = ROOM_VNUM_TEMPLE;

    if ( ( location = get_room_index( vnum ) ) == NULL )
    {
    send_to_char( "You are completely lost.\n\r", ch );
    return;
    }

    if ( ch->in_room == location )
    return;

    stop_fighting( ch, TRUE );


        if(is_affected(ch, gsn_unseen) && ch->pcdata->discipline[OBFUSCATE] < 2)
    affect_strip( ch, gsn_unseen );
    act( "$n vanishes mysteriously and swiftly.", ch, NULL, NULL, TO_ROOM );
    REMOVE_BIT(ch->act,PLR_ARENA);
    REMOVE_BIT(ch->act,PLR_SPEC);
    char_from_room( ch );
    char_to_room( ch, location );
    act( "$n mysteriously appears in the room, looking briefly dazed.", ch, NULL, NULL, TO_ROOM );
    do_function(ch, &do_look, "auto" );

    if (ch->pet != NULL)
    {
        char_from_room(ch->pet);
	char_to_room(ch->pet, location);
    }
    if (ch->mount != NULL)
    {
        char_from_room(ch->mount);
	char_to_room(ch->mount, location);
    }

    return;
}

 */
// Line makes immortals always IT for tag game
if (IS_IMMORTAL(ch))
ch->tagged = 1;
//More tag fun

if ((ch->tagged == 1) && (!IS_IMMORTAL(ch)) && ( !IS_AFFECTED(ch, AFF_FAERIE_FIRE) ))
{


    af.where     = TO_AFFECTS;
    af.type      = 0;
    af.level     = 1;
    af.duration  = 5;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_FAERIE_FIRE;
    affect_to_char( ch, &af );

}


}




};
