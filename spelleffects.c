#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "interp.h"
#include "music.h"
#include "recycle.h"
#include "tables.h"
// Local definitions.
int pulse_hit_gain args( ( CHAR_DATA *ch ) );
int pulse_mana_gain args( ( CHAR_DATA *ch ) );
int pulse_move_gain args( ( CHAR_DATA *ch ) );

void spells_update (void) {
    CHAR_DATA *ch;
    CHAR_DATA *ch_next;



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
        
// Newbie tag bonus regen (Similiar but much weaker than vampire regen, always on)
    if (!IS_NPC(ch) && IS_SET(ch->act2, PLR2_NEWBIE)) {
        ch->hit += ch->level/20 + 1;
        ch->move += ch->level/20 + 1;
        ch->mana += ch->level/20 + 1;
        
        if (ch->hit > (ch->max_hit-ch->agg_dam)) 
            ch->hit = (ch->max_hit-ch->agg_dam);
        if (ch->move > ch->max_move) 
            ch->move = ch->max_move;
        if (ch->mana > ch->max_mana) 
            ch->mana = ch->max_mana; 
    }
    
	if (ch->race == race_lookup("garou"))
    {
        if (ch->hit < (ch->max_hit - ch->agg_dam))
            ch->hit += pulse_hit_gain(ch);
        else
            ch->hit = ch->max_hit;
        if (ch->mana < ch->max_mana)
            ch->mana += pulse_mana_gain(ch);
        else
            ch->mana = ch->max_mana;
        if (ch->move < ch->max_move)
            ch->move += pulse_move_gain(ch); 
        else
            ch->move = ch->max_move;
        }
	
// Zelan's new regen code
  if ( IS_AFFECTED2 (ch, AFF2_VAMPIRE_REGEN)) {
        ch->hit += ch->level/5 + 10*(20-ch->gen)/3;

        if (ch->agg_dam > 0) 
            ch->agg_dam -= UMIN( ((ch->level/20 + (20-ch->gen))/10), ch->agg_dam );

        ch->move += ch->level*3/3;
        ch->mana += ch->level/3;

        if ( ch->bloodtick > 0) ch->bloodtick -= 1;
        if ( ch->bloodtick == 0) {
            ch->pblood -= 1;
            ch->bloodtick = 2;
            }
            
        if (ch->agg_dam < 0) ch->agg_dam = 0;
        if (ch->hit > (ch->max_hit-ch->agg_dam)) ch->hit = (ch->max_hit-ch->agg_dam);
        if (ch->move > ch->max_move) ch->move = ch->max_move;
        if (ch->mana > ch->max_mana) ch->mana = ch->max_mana; 
     
        if ((ch->hit == (ch->max_hit-ch->agg_dam)) && 
        ch->agg_dam == 0 && (ch->move == ch->max_move) && 
        (ch->mana == ch->max_mana)){
            send_to_char( "You stop regenerating.\n\r", ch );
            act( "$n stops regenerating.", ch, NULL, NULL, TO_ROOM );
            affect_strip(ch, gsn_vampire_regen);
            }
        if (ch-> pblood < 10) {
            send_to_char( "You do not have the vitae to continue regenerating.\n\r", ch);
            affect_strip(ch, gsn_vampire_regen);
            }
    }

// End of new regen code
}




};

/*
 * Regeneration stuff.
 */
int pulse_hit_gain( CHAR_DATA *ch )
{
    int gain;
    int number;

    if (ch->in_room == NULL)
    return 0;
    
    if (ch->race == race_lookup("vampire") && !is_affected(ch, gsn_vampire_regen))
    return 0;
    
    // IF HUMAN/GHOUL/Garou
    if (ch->race != race_lookup("vampire")) {
    gain = UMAX(5, (get_attribute(ch, STAMINA) * 2) * ch->max_hit/200);
    gain += ch->level/5;

    }

    if (IS_VAMP(ch) && is_affected(ch, gsn_vampire_regen))
    gain += (ch->max_hit/400) * (20-ch->gen/2);


    //Garou have boosted regen in non-birth forms, except Metis who regen in all forms.
    if (ch->race == race_lookup("garou") &&
        (ch->pcdata->breed == METIS || 
        ch->pcdata->shiftform != ch->pcdata->breed) )
        gain += get_attribute(ch, STAMINA) * ch->max_hit/200;
        
    //IF NOTVAMP/NOTGAROU
    if ( ch->race != race_lookup("vampire") && 
    ch->race != race_lookup("garou") && IS_AFFECTED(ch, AFF_POISON) )
    gain /= 4;
    
    //IF NOTVAMP
    if ( ch->race != race_lookup("vampire"))
    {
        gain += (ch->pcdata->condition[COND_HUNGER] + ch->pcdata->condition[COND_THIRST])/2;

        number = number_percent();
        if (number < get_skill(ch,gsn_fast_healing))
        {
            gain += number * gain / 100;
            if (ch->hit < ch->max_hit)
            check_improve(ch,gsn_fast_healing,TRUE,8);
        }
        
        if (ch->on != NULL && ch->on->item_type == ITEM_FURNITURE)
        gain = gain * ch->on->value[3] / 100;

        if (IS_AFFECTED(ch,AFF_HASTE))
        gain /=2;

        if (IS_AFFECTED(ch,AFF_SLOW) || ch->in_room->sector_type == SECT_COLD)
        gain /=2;

        if( get_affect_level(ch, gsn_bandage) >= 2)
            gain += gain;
        if( get_affect_level(ch, gsn_bandage) >= 5 && 
            number_percent() > 85 && ch->agg_dam > 0)
            ch->agg_dam--;
    }     //ENDIF NOTVAMP

    gain = gain * ch->in_room->heal_rate / 100;
    
    if (ch->race == race_lookup("vampire"))
        gain /= 6;
    else
        switch ( ch->position )
        {
            default:        gain /= 8;          break;
            case POS_SLEEPING:                  break;
            case POS_RESTING: gain /= 4; break;
            case POS_FIGHTING:  gain = 0;          break;
        }

    return UMIN(gain, (ch->max_hit+ch->agg_dam) - ch->hit);
}

int pulse_mana_gain( CHAR_DATA *ch )
{
    int gain;
    int number;

    if (ch->in_room == NULL)
    return 0;
    
    if (ch->race == race_lookup("vampire") && !is_affected(ch, gsn_vampire_regen))
    return 0;
    
    // IF HUMAN/GHOUL/Garou
    if (ch->race != race_lookup("vampire")) {
    gain = UMAX(5, get_attribute(ch, INTELLIGENCE) * ch->max_mana/200);
    gain += ch->level/5;

    }

    if (IS_VAMP(ch) && is_affected(ch, gsn_vampire_regen))
		gain += (ch->max_mana/400) * (20-ch->gen/2) + ch->pcdata->discipline[THAUMATURGY];


        
    //IF NOTVAMP/NOTGAROU
    if ( ch->race != race_lookup("vampire") && 
    ch->race != race_lookup("garou") && IS_AFFECTED(ch, AFF_POISON) )
    gain /= 4;
    
    //IF NOTVAMP
    if ( ch->race != race_lookup("vampire"))
    {
        gain += (ch->pcdata->condition[COND_HUNGER] + ch->pcdata->condition[COND_THIRST])/2;
        
        if (ch->on != NULL && ch->on->item_type == ITEM_FURNITURE)
        gain = gain * ch->on->value[4] / 100;


        
    }     //ENDIF NOTVAMP

	        number = number_percent();
        if (number < get_skill(ch,gsn_meditation))
        {
            gain += number * gain / 200;
            if (ch->mana < ch->max_mana)
            check_improve(ch,gsn_meditation,TRUE,4);
        }
		
		if (is_affected(ch, gsn_meditation))
			gain += (ch->level/10) * get_attribute(ch, INTELLIGENCE);
		if (is_affected(ch, gsn_chant))
			gain += (ch->level/20) * get_attribute(ch, INTELLIGENCE);

    
    gain = gain * ch->in_room->mana_rate / 100;
    
    if (ch->race == race_lookup("vampire"))
        gain /= 4;
    else
        switch ( ch->position )
        {
            default:  gain /= 2;                break;
            case POS_SLEEPING:    gain += gain/4;              break;
            case POS_RESTING: gain += gain/8; break;
            case POS_FIGHTING:  gain = 0;          break;
        }
        
    return UMIN(gain, ch->max_mana - ch->mana);
}

int pulse_move_gain( CHAR_DATA *ch )
{
    int gain;
    int number;

    if (ch->in_room == NULL)
    return 0;
    
    if (ch->race == race_lookup("vampire") && !is_affected(ch, gsn_vampire_regen))
    return 0;
    
    // IF HUMAN/GHOUL/garou
    if (ch->race != race_lookup("vampire")) {
    gain = UMAX(5, get_attribute(ch, STAMINA) * ch->max_move/400);
    gain += ch->level/5;

    }

    if (IS_VAMP(ch) && is_affected(ch, gsn_vampire_regen))
    gain += (ch->max_move/400) * (20-ch->gen/2);


    //Garou have boosted regen in all forms.
    if (ch->race == race_lookup("garou"))
        gain += get_attribute(ch, STAMINA) * ch->max_move/400;
    // double movement regen in wolf form.
    if (ch->race == race_lookup("garou") && ch->pcdata->shiftform > CRINOS)
		gain += get_attribute(ch, STAMINA) * ch->max_move/400;
        
    //IF NOTVAMP/NOTGAROU
    if ( ch->race != race_lookup("vampire") && 
    ch->race != race_lookup("garou") && IS_AFFECTED(ch, AFF_POISON) )
    gain /= 4;
    
    //IF NOTVAMP
    if ( ch->race != race_lookup("vampire"))
    {
        gain += (ch->pcdata->condition[COND_HUNGER] + ch->pcdata->condition[COND_THIRST])/2;

        if (ch->on != NULL && ch->on->item_type == ITEM_FURNITURE)
        gain = gain * ch->on->value[3] / 100;

        if (IS_AFFECTED(ch,AFF_HASTE))
        gain /=2;

        if (IS_AFFECTED(ch,AFF_SLOW) || ch->in_room->sector_type == SECT_COLD)
        gain /=2;

    }     //ENDIF NOTVAMP

    gain = gain * ch->in_room->heal_rate / 100;
    
    if (ch->race == race_lookup("vampire"))
        gain /= 4;
    else
        switch ( ch->position )
        {
            default:   gain /= 2;               break;
            case POS_SLEEPING:    gain += gain/4;              break;
            case POS_RESTING: gain += gain/8; break;
            case POS_FIGHTING:  gain = 0;          break;
        }

    return UMIN(gain, ch->max_move - ch->move);
}

