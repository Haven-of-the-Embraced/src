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
#include "interp.h"
#include "magic.h"
#include "recycle.h"
#include "tables.h"
#include "lookup.h"
struct rank_type
{
    int ragabash[3];
    int theurge[3];
    int philodox[3];
    int galliard[3];
    int ahroun[3];
};

struct rank_type rank_table [] =
{
{{3,0,0}, {0,0,3},  {0,3,0},  {2,0,1},  {2,1,0}},
{{7,0,0}, {1,0,5},  {1,4,1},  {4,0,2},  {4,1,1}},
{{13,0,0}, {2,1,7},  {2,6,2},  {4,2,4},  {6,3,1}},
{{19,0,0}, {4,2,9},  {3,8,4},  {7,2,6},  {9,4,2}},
{{25,0,0}, {4,9,10}, {4,10,9}, {9,5,9}, {10,9,4}}
};

#define HONO 1
#define GLOR 0
#define WISD 2

void do_remort(CHAR_DATA *ch, char *argument)
{
    char arg [MAX_INPUT_LENGTH];
    char arg2 [MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    OBJ_DATA *weapon;
    bool done=FALSE;
    int i;

    argument = one_argument( argument, arg );
    argument = one_argument( argument, arg2 );

    if(IS_NPC(ch))
        return;
/*
    if(!ch->pcdata->registered)
    {
        send_to_char("You cannot remort until you register a valid email address. Do so with the 'register' command.\n\r",ch);
        return;
    }
*/
        if(ch->level < 100)
    {
        send_to_char( "You must be level 100 to remort your character.\n\r", ch );
        return;
    }

    if(is_affected(ch,gsn_bloodofpotency))
        affect_strip( ch, gsn_bloodofpotency );

    if(IS_IMMORTAL(ch))
    {
        send_to_char("I don't think so buddy!",ch);
        return;
    }

    if(!str_cmp(arg, "garou"))
    {

        if(ch->race == race_lookup("mage") || clan_table[ch->clan].clan_type == TYPE_TRADITION)
        {
            send_to_char("If you were to do that, you'd lose your control over reality!\n\r",ch);
            return;
        }
        if(IS_VAMP(ch))
        {
            send_to_char( "Your vampiric nature is too strong to change into a Garou.\n\r", ch );
            return;
        }

        if(ch->embraced >= 1)
        {
            send_to_char( "Only True Blood humans may become Garou.\n\r", ch );
            return;
        }
        if(ch->level < 1)
        {
            send_to_char( "Something is seriously wrong, contact an immortal..\n\r", ch );
            return;
        }

        if(ch->race == race_lookup("garou"))
        {
            send_to_char( "You are already a Garou.\n\r", ch );
            return;
        }

                while ( ch->affected )
            affect_remove( ch, ch->affected );

        ch->race = race_lookup("garou");
        for ( obj = ch->carrying; obj != NULL; obj = obj_next )
        {
            obj_next = obj->next_content;
            if(obj->wear_loc != WEAR_NONE)
                unequip_char( ch, obj );
            if(obj->pIndexData->vnum == OBJ_VNUM_FETISH)
                extract_obj( obj );
        }
        while ( ch->affected )
            affect_remove( ch, ch->affected );
        ch->affected_by = race_table[ch->race].aff;
        ch->imm_flags   = ch->imm_flags|race_table[ch->race].imm;
        ch->res_flags   = ch->res_flags|race_table[ch->race].res;
        ch->vuln_flags  = ch->vuln_flags|race_table[ch->race].vuln;
        ch->practice = 4;
        ch->train = 3;
        ch->level    = 1;
        ch->clan = 0;
        ch->tradition = 0;
        ch->size = pc_race_table[ch->race].size;
        if(ch->pcdata->points < 30) ch->pcdata->points = 30;
        ch->max_hit  = 100;
        ch->max_mana = 100;
        ch->max_move = 100;
        ch->pcdata->perm_hit = ch->max_hit;
        ch->pcdata->perm_mana = ch->max_mana;
        ch->pcdata->perm_move = ch->max_move;
        ch->hit      = ch->max_hit;
        ch->mana     = ch->max_mana;
        ch->move     = ch->max_move;
        ch->position = POS_STANDING;
        ch->gen = 0;
        ch->bonded = 0;
        ch->embraced = 0;
            for(i = 0; i < MAX_DISC; i++)
            ch->pcdata->discipline[i] = 0;
            for (i = 0; i < MAX_CSBACK; i++)
            ch->pcdata->csbackgrounds[i] = 0;
        ch->pcdata->shiftform = HOMID;
        ch->pcdata->rage[0] = 0;
        ch->pcdata->rage[1] = 0;
        ch->pcdata->gnosis[0] = 0;
        ch->pcdata->gnosis[1] = 0;
        ch->pcdata->shift_count = 0;
        ch->pcdata->shift_to = 0;
        ch->pcdata->garou_fur = str_dup("");
        ch->pcdata->rank = 0;
        ch->pcdata->tribe = 0;
        ch->pcdata->breed = 0;
        ch->pcdata->auspice = 0;
        ch->pcdata->primal_urge = 0;
        ch->dpoints = 5;
        ch->short_descr = str_dup("");
        ch->long_descr = str_dup("");
        ch->pblood = 100;
        ch->embraced = 0;
        ch->childer = 0;
        ch->arete = 0;
        ch->avatar = 0;
        ch->paradox = 0;
        ch->quintessence = 0;
        ch->foci = 0;
        ch->breed = 0;
        ch->auspice = 0;
        ch->agg_dam = 0;
        ch->freebie = 0;
        for (i = 1;i < 10;i++)
            ch->sphere[i] = 0;
        for(i = 0; i < 30; i++)
            ch->pcdata->csabilities[i] = 0;
            for (i = 0; i < MAX_CSBACK; i++)
            ch->pcdata->csbackgrounds[i] = 0;
        for(i = 0; i < 9; i++)
            ch->pcdata->csattributes[i] = 1;
        for(i = 0; i < 30; i++)
            ch->pcdata->cssec_abil[i] = 0;
        ch->pcdata->csvirtues[CSVIRT_CONSCIENCE] = 1;
        ch->pcdata->csvirtues[CSVIRT_SELF_CONTROL] = 1;
        ch->pcdata->csvirtues[CSVIRT_COURAGE] = 1;
        ch->pcdata->csmax_willpower = 0;
        ch->pcdata->cswillpower = 0;
        ch->pcdata->cshumanity = 0;
        ch->pcdata->progress = 0;
        ch->pcdata->sect = 0;
        ch->pcdata->condition[COND_THIRST] = 10;
        ch->pcdata->condition[COND_HUNGER] = 10;
        ch->exp   = exp_per_level(ch,ch->pcdata->points) * UMAX( 1, ch->level );
        ch->freebie = 15 + (ch->remorts*3);

        send_to_char( "You are now reborn a Garou! Please logout and back in.\n\r", ch );
        send_to_char( "Then type 'gogarou' to set your breed/auspice/tribe and fur color.\n\r", ch);
        ch->freebie += 3;
        ch->remorts++;
        nuke_pets(ch);
        save_char_obj(ch);
        return;
    }
    if(!str_cmp(arg, "human"))
    {
        int i;

        if(ch->level < 100)
        {
            send_to_char( "You must be level 100 to return to humanity.\n\r", ch );
            return;
        }

        if(ch->race == race_lookup("human") && ch->clan == 0)
        {
            send_to_char( "You are already Human.\n\r", ch );
            return;
        }
        if ( IS_AFFECTED(ch, AFF_SHIFT))
        {
            if ( !IS_AFFECTED(ch, AFF_FANGS))
            SET_BIT(ch->affected_by, AFF_FANGS);
            do_function(ch, &do_shift, "");
        }

        while ( ch->affected )
            affect_remove( ch, ch->affected );

        ch->race = race_lookup("human");
        for ( obj = ch->carrying; obj != NULL; obj = obj_next )
        {
            obj_next = obj->next_content;
            if(obj->wear_loc != WEAR_NONE)
                unequip_char( ch, obj );
            if(obj->pIndexData->vnum == OBJ_VNUM_FETISH)
                extract_obj( obj );
        }
        while ( ch->affected )
            affect_remove( ch, ch->affected );
        ch->affected_by = race_table[ch->race].aff;
        ch->imm_flags   = ch->imm_flags|race_table[ch->race].imm;
        ch->res_flags   = ch->res_flags|race_table[ch->race].res;
        ch->vuln_flags  = ch->vuln_flags|race_table[ch->race].vuln;
        ch->practice = 4;
        ch->train = 3;
        ch->level    = 1;
        ch->clan = 0;
        ch->tradition = 0;
        ch->size = pc_race_table[ch->race].size;
        if(ch->pcdata->points < 30) ch->pcdata->points = 30;
        ch->max_hit  = 100;
        ch->max_mana = 100;
        ch->max_move = 100;
        ch->pcdata->perm_hit = ch->max_hit;
        ch->pcdata->perm_mana = ch->max_mana;
        ch->pcdata->perm_move = ch->max_move;
        ch->hit      = ch->max_hit;
        ch->mana     = ch->max_mana;
        ch->move     = ch->max_move;
        ch->position = POS_STANDING;
        ch->gen = 0;
        ch->bonded = 0;
        ch->embraced = 0;
	ch->aget = 17;
        ch->unlocksspent = 0;
            for(i = 0; i < MAX_DISC; i++)
            ch->pcdata->discipline[i] = 0;
            for (i = 0; i < MAX_CSBACK; i++)
            ch->pcdata->csbackgrounds[i] = 0;
        ch->pcdata->shiftform = HOMID;
        ch->pcdata->rage[0] = 0;
        ch->pcdata->rage[1] = 0;
        ch->pcdata->gnosis[0] = 0;
        ch->pcdata->gnosis[1] = 0;
        ch->pcdata->shift_count = 0;
        ch->pcdata->shift_to = 0;
        ch->pcdata->garou_fur = str_dup("");
        ch->pcdata->rank = 0;
        ch->pcdata->tribe = 0;
        ch->pcdata->breed = 0;
        ch->pcdata->auspice = 0;
        ch->pcdata->primal_urge = 0;
        ch->dpoints = 5;
        ch->short_descr = str_dup("");
        ch->long_descr = str_dup("");
        ch->pblood = 100;
        ch->embraced = 0;
        ch->childer = 0;
        ch->arete = 0;
        ch->avatar = 0;
        ch->paradox = 0;
        ch->quintessence = 0;
        ch->foci = 0;
        ch->breed = 0;
        ch->auspice = 0;
        ch->agg_dam = 0;
        ch->freebie = 0;
        for (i = 1;i < 10;i++)
            ch->sphere[i] = 0;
        for(i = 0; i < 30; i++)
            ch->pcdata->csabilities[i] = 0;
        for(i = 0; i < 30; i++)
            ch->pcdata->cssec_abil[i] = 0;

        for(i = 0; i < 9; i++)
            ch->pcdata->csattributes[i] = 1;
        ch->pcdata->csvirtues[CSVIRT_CONSCIENCE] = 1;
        ch->pcdata->csvirtues[CSVIRT_SELF_CONTROL] = 1;
        ch->pcdata->csvirtues[CSVIRT_COURAGE] = 1;
        ch->pcdata->csmax_willpower = 0;
        ch->pcdata->cswillpower = 0;
        ch->pcdata->cshumanity = 0;
        ch->pcdata->progress = 0;
        ch->pcdata->sect = 0;
        ch->pcdata->condition[COND_THIRST] = 10;
        ch->pcdata->condition[COND_HUNGER] = 10;
        ch->exp   = exp_per_level(ch,ch->pcdata->points) * UMAX( 1, ch->level );
        ch->freebie = 15 + (ch->remorts*3);
        send_to_char( "You regain your Humanity!\n\r", ch );
        nuke_pets(ch);
        save_char_obj(ch);
        return;
    }

 if (!str_cmp(arg, "rank"))
    {
        bool canrank = FALSE;
        if(ch->level < 100)
        {
            send_to_char( "You must be level 100 to remort.\n\r", ch );
            return;
        }

        if (ch->race != race_lookup("garou"))
        {
        sendch("Only Garou would benefit from remorting for Garou rank.\n\r", ch);
        return;
        }
        if (ch->pcdata->rank > 4)
        {
        sendch("You cannot raise your rank any higher!\n\r", ch);
        return;
        }


        if (ch->pcdata->auspice == RAGABASH)
        {
            if ((ch->pcdata->renown[GLORY] + ch->pcdata->renown[HONOR] + ch->pcdata->renown[WISDOM]) >= rank_table[ch->pcdata->rank].ragabash[0])
            canrank = TRUE;
        } else {
            switch (ch->pcdata->auspice)
            {
            case THEURGE:
                if (ch->pcdata->renown[GLORY] >= rank_table[ch->pcdata->rank].theurge[GLOR] &&
                    ch->pcdata->renown[HONOR] >= rank_table[ch->pcdata->rank].theurge[HONO] &&
                    ch->pcdata->renown[WISDOM] >= rank_table[ch->pcdata->rank].theurge[WISD] )
                        canrank = TRUE;
                break;
            case PHILODOX:
                if (ch->pcdata->renown[GLORY] >= rank_table[ch->pcdata->rank].philodox[GLOR] &&
                    ch->pcdata->renown[HONOR] >= rank_table[ch->pcdata->rank].philodox[HONO] &&
                    ch->pcdata->renown[WISDOM] >= rank_table[ch->pcdata->rank].philodox[WISD] )
                        canrank = TRUE;
                break;
            case GALLIARD:
                if (ch->pcdata->renown[GLORY] >= rank_table[ch->pcdata->rank].galliard[GLOR] &&
                    ch->pcdata->renown[HONOR] >= rank_table[ch->pcdata->rank].galliard[HONO] &&
                    ch->pcdata->renown[WISDOM] >= rank_table[ch->pcdata->rank].galliard[WISD] )
                        canrank = TRUE;
                break;
            case AHROUN:
                if (ch->pcdata->renown[GLORY] >= rank_table[ch->pcdata->rank].ahroun[GLOR] &&
                    ch->pcdata->renown[HONOR] >= rank_table[ch->pcdata->rank].ahroun[HONO] &&
                    ch->pcdata->renown[WISDOM] >= rank_table[ch->pcdata->rank].ahroun[WISD] )
                        canrank = TRUE;
                break;
             default: canrank = FALSE; break;
             }

                }
            if (!canrank)
            {
                sendch("You are not well known enough to Gaia and the Garou to attain this rank.\n\r", ch);
                return;
            }

        ch->pcdata->rank++;

        for ( obj = ch->carrying; obj != NULL; obj = obj_next )
        {
            obj_next = obj->next_content;
            if(obj->wear_loc != WEAR_NONE)
            unequip_char( ch, obj );
        }
        while ( ch->affected )
            affect_remove( ch, ch->affected );
        ch->affected_by = race_table[ch->race].aff;

        ch->practice = 4;
        ch->train = 3;
        ch->level    = 1;
        if(ch->pcdata->points < 30) ch->pcdata->points = 30;
        ch->max_hit  = 100;
        ch->max_mana = 100;
        ch->max_move = 100;
        ch->pcdata->perm_hit = ch->max_hit;
        ch->pcdata->perm_mana = ch->max_mana;
        ch->pcdata->perm_move = ch->max_move;
        ch->hit      = ch->max_hit;
        ch->mana     = ch->max_mana;
        ch->move     = ch->max_move;
        ch->position = POS_STANDING;
        ch->childer = 0;
        ch->pblood = ch->max_pblood;
        ch->remorts += 1;
        ch->agg_dam = 0;
        ch->exp   = exp_per_level(ch,ch->pcdata->points) * UMAX( 1, ch->level );
        sendch("Your deeds have earned you a higher rank in garou society.\n\r", ch);


        nuke_pets(ch);
        save_char_obj(ch);
        return;
    }

    if ( arg[0] == '\0')
    {
        if(ch->level < 100)
        {
            send_to_char( "You must be level 100 to remort.\n\r", ch );
            return;
        }

        if(ch->pcdata->rank > 0 && ch->pcdata->renown[TEMP_GLORY] == 10 &&
        ch->pcdata->renown[TEMP_HONOR] == 10 &&  ch->pcdata->renown[TEMP_WISDOM] == 10)
        {
            send_to_char("You cannot remort again until you've spent some of your Temporary Renown.\n\r",ch);
            return;
        }

            weapon = get_eq_char(ch,WEAR_WIELD);
    if (weapon != NULL)
    {
        if ( IS_VAMP(ch) && weapon->pIndexData->vnum == OBJ_VNUM_CLAWS)
        {
            affect_strip(ch,gsn_claws);
            if(weapon != NULL)
            extract_obj(weapon);
            send_to_char("Your claws slide back under your nails.\n\r", ch);
            act("$n's claws slide back under their fingernails.",ch,NULL,NULL,TO_NOTVICT);
        }
    }

        for ( obj = ch->carrying; obj != NULL; obj = obj_next )
        {
            obj_next = obj->next_content;
            if(obj->wear_loc != WEAR_NONE)
            unequip_char( ch, obj );
        }
        if (ch->race == race_lookup("garou") &&
            ch->pcdata->shiftform != HOMID)
            {
                act( "You assume the form of Man.", ch, NULL, NULL, TO_CHAR );
                act( "$n's form shrinks to that of a normal human.", ch, NULL, NULL, TO_NOTVICT );
                ch->pcdata->shiftform = HOMID;
                }
        while ( ch->affected )
            affect_remove( ch, ch->affected );
        ch->affected_by = race_table[ch->race].aff;

        ch->practice = 4;
        ch->train = 3;
        ch->level    = 1;
        if(ch->pcdata->points < 30) ch->pcdata->points = 30;
        ch->max_hit  = 100;
        ch->max_mana = 100;
        ch->max_move = 100;
        ch->pcdata->perm_hit = ch->max_hit;
        ch->pcdata->perm_mana = ch->max_mana;
        ch->pcdata->perm_move = ch->max_move;
        ch->hit      = ch->max_hit;
        ch->mana     = ch->max_mana;
        ch->move     = ch->max_move;
        ch->position = POS_STANDING;
        ch->childer = 0;
        ch->pblood = ch->max_pblood;
        ch->remorts += 1;
        ch->agg_dam = 0;
        ch->exp   = exp_per_level(ch,ch->pcdata->points) * UMAX( 1, ch->level );
        send_to_char( "You are reborn more powerful!\n\r", ch );
        ch->freebie += 3;

        if(ch->pcdata->rank > 0)
            renown_gain(ch);

        nuke_pets(ch);
        save_char_obj(ch);
        return;
    }

    send_to_char( "That is not a valid option!\n\r", ch );
    return;
}
