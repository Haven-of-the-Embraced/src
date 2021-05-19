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

/* Local prototype for can_use_disc(). If we need it in other files, move it to merc.h */
bool can_use_disc(CHAR_DATA *ch, int disc, int dot, int blood, bool message);

void do_embrace(CHAR_DATA *ch, char *argument)
{
    char arg1 [MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    int calc, i;
    bool exghoul = FALSE;
    argument = one_argument( argument, arg1 );

    if(IS_NPC(ch)) return;

    if ( arg1[0] == '\0')
    {
        send_to_char( "Embrace whom?\n\r", ch );
        return;
    }
    if(is_affected( ch, gsn_bloodofpotency ))
    {
        send_to_char("You cannot embrace while your generation is altered.\n\r",ch);
        return;
    }

    if(ch->race != race_lookup("vampire"))
    {
        send_to_char("You are not a vampire!\n\r" ,ch);
        return;
    }
    if ( ( victim = get_char_room( ch, NULL, arg1 ) ) == NULL )
    {
        send_to_char( "Your future childe is not here.\n\r", ch );
        return;
    }
    if (IS_NPC(victim))
    {
        send_to_char( "You cannot embrace that creature.\n\r", ch );
        return;
    }
/*
    if(victim->pcdata->IC_total < 6*60)
    {
        send_to_char("You do not know them well enough to embrace them yet. (More IC time required)\n\r",ch);
        return;
    }
*/
    if ( IS_AFFECTED2(ch, AFF2_QUIETUS_BLOODCURSE))
    {
        send_to_char("Your blood curse prevents it!\n\r" ,ch);
        return;
    }

    if (victim->race == race_lookup("vampire"))
    {
        sprintf(buf, "%s is already kindred!\n\r", victim->name);
        send_to_char(buf, ch);
        return;
    }
    if((victim->race != race_lookup("human") && victim->race != race_lookup("ghoul")) &&
        clan_table[victim->clan].clan_type != TYPE_NONE && clan_table[victim->clan].clan_type != TYPE_CLAN)
    {
        send_to_char("Your dark curse only affects humanity.\n\r",ch);
        return;
    }
/*
    if(ch->gen < 10 && ch->childer >= 1)
    {
        send_to_char("You already have enough Childer\n\r" ,ch);
        return;
    }
*/
    if (!IS_AFFECTED(ch,AFF_FANGS))
    {
        send_to_char( "Your fangs aren't extended!\n\r", ch );
        return;
    }

    ch->childer += 1;
    make_vamp(victim,clan_table[ch->clan].name);
    victim->gen = 10;
    victim->pcdata->csgeneration = 10;
    victim->sire = str_dup(ch->name);
    if(victim->gen < 10)
        victim->max_pblood += (10 - victim->gen)*10;
    victim->pblood = 20;
    sprintf(buf, "You sink your fangs into %s...\n\r", victim->name);
    send_to_char(buf,ch);
    sprintf(buf, "%s is now a vampire!\n\r", victim->name);
    send_to_char(buf,ch);
    sprintf(buf, "%s sinks his fangs into you...\n\r", ch->name);
    send_to_char(buf,victim);
    send_to_char("Odd visions and images dance in your mind as pain echos through your body...\n\r\n\r", victim);
    send_to_char(" {wHe bore the curse of the seed of Caine,\n\r whereby God punished the grievous guilt\n\r of Abel's murderer. Nor ever had Caine\n\r caused to boast of the deed of blood. God\n\r banished him far from the fields of men,\n\r of his blood was begotten an evil brood.{x\n\r\n\r",victim);

    sprintf(buf,"You feel the power of %s blood surging in your veins!\n\r", capitalize(clan_table[victim->clan].name));
    send_to_char(buf,victim);
    if(victim->clan == clan_lookup("nosferatu")) ch->csattributes[CSATTRIB_APPEARANCE] = 0;
    victim->exp   = exp_per_level(victim,victim->pcdata->points) * UMAX( 1, victim->level );
    if(!IS_IMMORTAL(ch))
        ch->exp   = exp_per_level(ch,ch->pcdata->points) * UMAX( 1, ch->level-1 );
    send_to_char("You are now a vampire!\n\r",victim);
    act( "$n sinks $s fangs into $N!",  ch, NULL, victim, TO_NOTVICT );
    do_function(victim, &do_fangs, "" );
    return;
}

void do_vampire(CHAR_DATA *ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    if(ch->race != race_lookup("vampire") && ch->race != race_lookup("methuselah"))
    {
        send_to_char("You are not a vampire!\n\r" ,ch);
        return;
    }
    if(ch->pcdata->csgeneration == 1) send_to_char("You are a Vampire of the First Generation.\n\r" ,ch);
    if(ch->pcdata->csgeneration == 2) send_to_char("You are a Vampire of the Second Generation.\n\r" ,ch);
    if(ch->pcdata->csgeneration == 3) send_to_char("You are a Vampire of the Third Generation.\n\r" ,ch);
    if(ch->pcdata->csgeneration == 4) send_to_char("You are a Vampire of the Fourth Generation.\n\r" ,ch);
    if(ch->pcdata->csgeneration == 5) send_to_char("You are a Vampire of the Fifth Generation.\n\r" ,ch);
    if(ch->pcdata->csgeneration == 6) send_to_char("You are a Vampire of the Sixth Generation.\n\r" ,ch);
    if(ch->pcdata->csgeneration == 7) send_to_char("You are a Vampire of the Seventh Generation.\n\r" ,ch);
    if(ch->pcdata->csgeneration == 8) send_to_char("You are a Vampire of the Eighth Generation.\n\r" ,ch);
    if(ch->pcdata->csgeneration == 9) send_to_char("You are a Vampire of the Ninth Generation.\n\r" ,ch);
    if(ch->pcdata->csgeneration == 10) send_to_char("You are a Vampire of the Tenth Generation.\n\r" ,ch);
    if(ch->pcdata->csgeneration == 11) send_to_char("You are a Vampire of the Eleventh Generation.\n\r" ,ch);
    if(ch->pcdata->csgeneration == 12) send_to_char("You are a Vampire of the Twelfth Generation.\n\r" ,ch);
    if(ch->pcdata->csgeneration == 13) send_to_char("You are a Vampire of the Thirteenth Generation.\n\r" ,ch);
    if(ch->pcdata->csgeneration == 14) send_to_char("You are a Vampire of the Fourteenth Generation.\n\r" ,ch);
    if(ch->pcdata->csgeneration == 15) send_to_char("You are a Vampire of the Fifteenth Generation.\n\r" ,ch);
    if(ch->pcdata->csgeneration == 16) send_to_char("You are a Vampire of the Sixteenth Generation.\n\r" ,ch);
    if(ch->pcdata->csgeneration == 17) send_to_char("You are a Vampire of the Seventeenth Generation.\n\r" ,ch);
    if(ch->pcdata->csgeneration == 18) send_to_char("You are a Vampire of the Eightteenth Generation.\n\r" ,ch);
    if(ch->pcdata->csgeneration == 19) send_to_char("You are a Vampire of the Nineteenth Generation.\n\r" ,ch);
    if(ch->pcdata->csgeneration > 19)
    {
        sprintf(buf, "You are a Vampire of the %d Generation.\n\r", ch->pcdata->csgeneration);
        send_to_char(buf,ch);
    }
    sprintf(buf, "You are the Childe of %s.\n\r", ch->sire);
    send_to_char(buf,ch);
    sprintf(buf, "You have %d of %d Blood Points.\n\r", ch->pblood/10, ch->max_pblood/10);
    send_to_char(buf,ch);
    sprintf(buf, "You have %d Freebie points.\n\r", ch->freebie);
    send_to_char(buf,ch);
/*  sprintf(buf, "You have drained %d Kindred of their blood.\n\r", ch->drains);
    send_to_char(buf,ch);
*/
    sprintf(buf, "You have %d Childer.\n\r", ch->childer);
    send_to_char(buf,ch);
    if (IS_AFFECTED(ch,AFF_FANGS))
    {
        send_to_char("Your fangs are extended.\n\r" ,ch);
    }
// Ripped from the do_discipline code in order to eliminate the command - Ugha

    send_to_char("And have the following Disciplines:\n\r",ch);
    send_to_char("***********************************\n\r",ch);
    if(ch->pcdata->discipline[ANIMALISM])
    {
        sprintf(buf, "Animalism                  -    %d\n\r", ch->pcdata->discipline[ANIMALISM]);
        send_to_char(buf,ch);
    }
    if(ch->pcdata->discipline[AUSPEX])
    {
        sprintf(buf, "Auspex                     -    %d\n\r", ch->pcdata->discipline[AUSPEX]);
        send_to_char(buf,ch);
    }
    if(ch->pcdata->discipline[PROTEAN])
    {
        sprintf(buf, "Protean                    -    %d\n\r", ch->pcdata->discipline[PROTEAN]);
        send_to_char(buf,ch);
    }
    if(ch->pcdata->discipline[CELERITY])
    {
        sprintf(buf, "Celerity                   -    %d\n\r", ch->pcdata->discipline[CELERITY]);
        send_to_char(buf,ch);
    }
    if(ch->pcdata->discipline[CHIMERSTRY])
    {
        sprintf(buf, "Chimerstry                 -    %d\n\r", ch->pcdata->discipline[CHIMERSTRY]);
        send_to_char(buf,ch);
    }
    if(ch->pcdata->discipline[FORTITUDE])
    {
        sprintf(buf, "Fortitude                  -    %d\n\r", ch->pcdata->discipline[FORTITUDE]);
        send_to_char(buf,ch);
    }
    if(ch->pcdata->discipline[POTENCE])
    {
        sprintf(buf, "Potence                    -    %d\n\r", ch->pcdata->discipline[POTENCE]);
        send_to_char(buf,ch);
    }
    if(ch->pcdata->discipline[OBFUSCATE])
    {
        sprintf(buf, "Obfuscate                  -    %d\n\r", ch->pcdata->discipline[OBFUSCATE]);
        send_to_char(buf,ch);
    }
    if(ch->pcdata->discipline[OBTENEBRATION])
    {
        sprintf(buf, "Obtenebration              -    %d\n\r", ch->pcdata->discipline[OBTENEBRATION]);
        send_to_char(buf,ch);
    }
    if(ch->pcdata->discipline[DOMINATE])
    {
        sprintf(buf, "Dominate                   -    %d\n\r", ch->pcdata->discipline[DOMINATE]);
        send_to_char(buf,ch);
    }
    if(ch->pcdata->discipline[PRESENCE])
    {
        sprintf(buf, "Presence                   -    %d\n\r", ch->pcdata->discipline[PRESENCE]);
        send_to_char(buf,ch);
    }
    if(ch->pcdata->discipline[MORTIS])
    {
        sprintf(buf, "Mortis                     -    %d\n\r", ch->pcdata->discipline[MORTIS]);
        send_to_char(buf,ch);
    }
    if(ch->pcdata->discipline[THAUMATURGY])
    {
        sprintf(buf, "Thaumaturgy Path of Blood  -    %d\n\r", ch->pcdata->discipline[THAUMATURGY]);
        send_to_char(buf,ch);
    }
    if(ch->pcdata->discipline[QUIETUS])
    {
        sprintf(buf, "Quietus                    -    %d\n\r", ch->pcdata->discipline[QUIETUS]);
        send_to_char(buf,ch);
    }
    if(ch->pcdata->discipline[VICISSITUDE])
    {
        sprintf(buf, "Vicissitude                -    %d\n\r", ch->pcdata->discipline[VICISSITUDE]);
        send_to_char(buf,ch);
    }
    if(ch->pcdata->discipline[DEMENTATION])
    {
        sprintf(buf, "Dementation                -    %d\n\r", ch->pcdata->discipline[DEMENTATION]);
        send_to_char(buf,ch);
    }
    if(ch->pcdata->discipline[SERPENTIS])
    {
        sprintf(buf, "Serpentis                  -    %d\n\r", ch->pcdata->discipline[SERPENTIS]);
        send_to_char(buf,ch);
    }
    return;
}

void do_fangs(CHAR_DATA *ch, char *argument)
{
    if(IS_NPC(ch)) return;

    if(!IS_VAMP(ch) || ch->race == race_lookup("ghoul"))
    {
        send_to_char("You are not a vampire!\n\r" ,ch);
        return;
    }
    if (IS_AFFECTED(ch,AFF_FANGS))
    {
        /*if(ch->clan == 6)
        {
            send_to_char("You cannot retract your fangs!\n\r" ,ch);
            return;
        }*/
REMOVE_BIT(ch->affected_by, AFF_FANGS);
send_to_char("Your fangs slide back into your gums.\n\r" ,ch);
act("$n's fangs disappear into $s mouth.", ch,NULL,NULL,TO_NOTVICT);
return;
}

SET_BIT(ch->affected_by, AFF_FANGS);
send_to_char("You unsheath your fangs.\n\r" ,ch);
act("$n shows $s true form and unsheaths $s vampiric fangs!", ch,NULL,NULL,TO_NOTVICT);
return;
}

void do_feed(CHAR_DATA *ch, char *argument)
{
    OBJ_DATA *obj;
    CHAR_DATA *victim;
    char buf[MAX_STRING_LENGTH];
    char arg1[MAX_INPUT_LENGTH];
    char *name;
    int chance, bloodgain;
    int success, diff;

    if (IS_NPC(ch)) return;

    argument = one_argument( argument, arg1 );

    if(ch->race == race_lookup("dhampire") && !IS_NPC(ch))
    {
        if (ch->cswillpower <= 0)
        {
            send_to_char("You do not have enough strength of will to bring yourself to overcome your base human nature.\n\r", ch);
            return;
        }

        ch->cswillpower--;

        send_to_char("Overcoming your mortal aversion, you give in to your vampiric side, if only for a moment.\n\r", ch);

    }

    if(ch->race == race_lookup("ghoul"))
    {
        send_to_char("You must either be willingly nourished by a Kindred, or take sustenance from a beheaded vampire corpse.\n\r",ch);
        return;
    }

    if(ch->race == race_lookup("human") && clan_table[ch->clan].clan_type != TYPE_TRADITION && clan_table[ch->clan].clan_type != TYPE_TRIBE)
    {
        if(str_cmp(argument,"ghoul"))
        {
            send_to_char("You must type 'feed <target> ghoul' to enslave yourself to a vampiric master.\n\r",ch);
            return;
        }

        if(ch->level < 20)
        {
            send_to_char("You are too weak to become a Ghoul.\n\r",ch);
            return;
        }
/*
        if(ch->pcdata->IC_total < 300)
        {
            send_to_char("You have not spent enough time in the world to become a ghoul.\n\r", ch);
            return;
        }
*/
        if ( ( victim = get_char_room( ch, NULL, arg1 ) ) == NULL )
        {
            send_to_char( "They aren't here.\n\r", ch );
            return;
        }

        if(!IS_SET(ch->act, PLR_IC) || !IS_SET(victim->act, PLR_IC))
        {
            send_to_char("Both players must be Roleplaying to use the ghouling process.\n\r", ch);
            return;
        }

        if(is_affected( victim, gsn_bloodofpotency ))
        {
            send_to_char("You sense magical taint on their blood and will not feed from it.\n\r",ch);
            return;
        }

        if(victim->race != race_lookup("vampire") && victim->race != race_lookup("methuselah"))
        {
            send_to_char("You can't serve THAT!\n\r",ch);
            return;
        }
        if(IS_NPC(victim))
        {
            send_to_char("You can't serve THAT!\n\r",ch);
            return;
        }
        sprintf(buf,"Thrills fill your body as you gulp down the liquid heat of %s Vitae.\n\r",victim->name);
        send_to_char(buf,ch);
        sprintf(buf,"%s gasps in pleasure as they sample some of your vampiric Vitae.\n\r",ch->name);
        send_to_char(buf,victim);
        if(!str_cmp(ch->vamp_master,victim->name))
            ch->bonded++;
        else
        {
            ch->vamp_master = str_dup(victim->name);
            ch->bonded = 1;
        }
        if(ch->bonded == 1)
        {
            sprintf(buf,"You feel in awe of %s's greatness.\n\r",victim->name);
            send_to_char(buf,ch);
            act( "$n feeds deeply from $N's Vitae then gazes at $N in awe.",  ch, NULL, victim, TO_NOTVICT );
            return;
        }
        else if(ch->bonded == 2)
        {
            sprintf(buf,"You bow down in worship of %s's magnificence.\n\r",victim->name);
            send_to_char(buf,ch);
            act( "$n feeds deeply from $N's Vitae then bows down to worship $N!",  ch, NULL, victim, TO_NOTVICT );
            return;
        }
        else if(ch->bonded == 3)
        {
            sprintf(buf,"You commit your life and soul to the immortal service of %s!\n\r",victim->name);
            send_to_char(buf,ch);
                ch->race = race_lookup("ghoul");
                ch->gen = victim->pcdata->csgeneration;
                ch->pcdata->csgeneration = victim->pcdata->csgeneration;
                ch->clan = victim->clan;
                ch->dpoints = 0;
                ch->pblood = 100;
                ch->max_pblood = 100;
                ch->pblood = ch->max_pblood;
                ch->affected_by = race_table[ch->race].aff;
                sprintf(buf, "%s is now your ghoul servant!\n\r", ch->name);
                send_to_char(buf,victim);
                if (ch->embraced == 0)
                {
                    ch->pcdata->discipline[POTENCE]++;
                    ch->freebie += 1;
                    ch->embraced = 1;
                }

                ch->exp   = exp_per_level(ch,ch->pcdata->points) * UMAX( 1, ch->level );
                act( "$n feeds deeply from $N's Vitae and becomes $S immortal servant!",  ch, NULL, victim, TO_NOTVICT );
        }
        return;
    }

    if(!IS_VAMP(ch))
    {
        send_to_char("You are not a vampire!\n\r" ,ch);
        return;
    }

    if (!IS_AFFECTED(ch,AFF_FANGS) && ch->race != race_lookup("ghoul"))
    {
        send_to_char( "Your fangs aren't extended!\n\r", ch );
        return;
    }

    if ((obj = get_obj_here( ch, NULL, arg1 )) == NULL)
    {
        if ( ( victim = ch->fighting ) == NULL )
        {
            send_to_char( "You aren't fighting anyone.\n\r", ch );
            return;
        }
        if(victim->clan == 3)
        {
            send_to_char("They cannot be fed from!\n\r" ,ch);
            return;
        }
        if ( ch->position != POS_FIGHTING )
        {
            send_to_char( "You are not fighting!\n\r", ch );
            return;
        }
        if(IS_NPC(victim)) name = victim->short_descr;
        else name = victim->name;
        chance = number_range(1, 100);
        diff = 6;
        if (IS_AFFECTED(ch,AFF_SLOW) || IS_AFFECTED(victim, AFF_HASTE) || is_affected(victim, gsn_celbuff))
            diff += 1;
        if (IS_AFFECTED(victim,AFF_SLOW) || IS_AFFECTED(ch, AFF_HASTE) || is_affected(ch, gsn_celbuff))
            diff -= 1;
        if (victim->hit < victim->max_hit/2)
            diff -= 1;
        if (is_affected(victim, gsn_bleeding))
            diff--;

        success = godice(get_attribute(ch, DEXTERITY) + ch->csabilities[CSABIL_ATHLETICS] + ch->pcdata->csbackgrounds[CSBACK_HERD], diff);

        if (!has_blood(victim))
        {
            send_to_char("Your quarry has no blood to feed upon!\n\r",ch);
            return;
        }

        if (success > 0)
        {
            WAIT_STATE(ch, 12);
            affect_strip(ch, gsn_vamp_frenzy);
            affect_strip(ch, gsn_torpor);
            act("You sink your fangs deep into $N's neck,", ch, NULL, victim, TO_CHAR);
            if(!IS_NPC(victim))
            {
                sprintf(buf, "%s bites your neck and feeds off of you!\n\r", ch->name);
                send_to_char(buf,victim);
            }
            act("$n lunges forward and bites $N's neck!", ch,NULL,victim,TO_NOTVICT);
            bloodgain = godice(get_attribute(ch, STRENGTH) + success, 6);
            gain_exp(ch, success);
            if (victim->hit > victim->max_hit/2)
                bloodgain -= 2;

            if (bloodgain < 1) {
                act(" but fail to drain any blood from $M.\n\r", ch, NULL, victim, TO_CHAR);
                return;
            }
            send_to_char(" and are rewarded with sweet, warm blood to sate your hunger.\n\r", ch);
            victim->hit -= number_range(bloodgain*8,bloodgain*12);
            gain_condition( ch, COND_FULL, bloodgain);
            bloodgain *= 2;
            if (victim->size < SIZE_MEDIUM)
                bloodgain /= 2;
            if (victim->race == race_lookup("human"))
                bloodgain += 2;
            if (victim->race == race_lookup("vampire") || victim->race == race_lookup("ghoul"))
                bloodgain += 4;
            if (victim->race == race_lookup("dhampire") || victim->race == race_lookup("methuselah") || victim->race == race_lookup("garou"))
                bloodgain += 5;
            ch->pblood += number_range(2*bloodgain/3, 3*bloodgain/2);
            if (!IS_NPC(victim) && IS_VAMP(victim))
            {
                victim->pblood -= number_range(2*bloodgain/3, 3*bloodgain/2);
                if (victim->pblood < 0) victim->pblood = 0;
            }

            if(ch->pblood > ch->max_pblood) ch->pblood = ch->max_pblood;
            return;
        }
        else
        {
            WAIT_STATE( ch, 12 );
            act("$N evades your attempt to feed from $M!", ch, NULL, victim, TO_CHAR);
            if(!IS_NPC(victim))
            {
                sprintf(buf, "You barely evade %s's attempt to feed from you!\n\r", ch->name);
                send_to_char(buf,victim);
            }
            act("$n tries to feed from $N and fails!", ch,NULL,victim,TO_NOTVICT);
            return;
        }
    }
    else
    {
        if( obj->item_type == ITEM_CORPSE_PC || obj->item_type == ITEM_CORPSE_NPC)
        {
            if(obj->value[1] <= 0)
            {
                send_to_char( "This corpse has no blood remaining.\n\r", ch );
                return;
            }
            WAIT_STATE( ch, 12 );

            affect_strip(ch,gsn_vamp_frenzy);
            if( obj->item_type == ITEM_CORPSE_PC)
            {
                if(obj->timer < 20)
                {
                    act("$n feeds from the corpse but suddenly seems ill.", ch,NULL,NULL,TO_NOTVICT);
                    spell_poison( gsn_poison, ch->level, ch, ch,TARGET_CHAR);
                    send_to_char( "You gag and turn green.\n\r", ch );
                }
                else
                {
                    act("$n relieves $s hunger by feeding on a corpse.", ch,NULL,NULL,TO_NOTVICT);
                    send_to_char( "You feed on the corpse.\n\r", ch );
                }
                    ch->pblood += obj->value[1];
                    obj->value[1] = 0;
                    WAIT_STATE( ch, 12 );
                    gain_condition( ch, COND_FULL, 5);
                    if( ch->pblood > ch->max_pblood){
                        ch->pblood=ch->max_pblood;
                        gain_condition( ch, COND_FULL, 10);
                        }
/* disabled to reduce lag... maybe - Ugha
                    if(!IS_NPC(ch) && IS_SET(ch->act,PLR_PUEBLO))
                        send_html("<img xch_sound=play href=http://haven.wolfpaw.net/pueblo/sounds/feed2.wav>",ch,TO_ROOM);
*/
                    return;
            }
            if( obj->item_type == ITEM_CORPSE_NPC)
            {
                if(obj->timer < 2)
                {
                    act("$n feeds from the corpse but suddenly seems ill.", ch,NULL,NULL,TO_NOTVICT);
                    spell_poison( gsn_poison, ch->level, ch, ch,TARGET_CHAR);
                    send_to_char( "You gag and turn green.\n\r", ch );
                }
                else
                {
                    act("$n relieves $s hunger by feeding on a corpse.", ch,NULL,NULL,TO_NOTVICT);
                    send_to_char( "You feed on the corpse.\n\r", ch );
                }
/*Sengir tweaked feed corpse blood value to give some at lower level.*/
                    ch->pblood += obj->level/5 + 5;
                    obj->level = 0;
                    gain_condition( ch, COND_FULL, 5);
                    if( ch->pblood > ch->max_pblood)
                    {
                        ch->pblood=ch->max_pblood;
                        gain_condition( ch, COND_FULL, 10);
                        act("$n spits out the last of the blood from the corpse, having feasted $mself to excess.", ch, NULL, NULL, TO_NOTVICT);
                        send_to_char("Your head begins swirling as you finish gorging yourself on precious blood.\n\r", ch);
                        WAIT_STATE(ch, 2);
                    }
/* disabled to reduce lag... maybe - Ugha
                    if(!IS_NPC(ch) && IS_SET(ch->act,PLR_PUEBLO))
                        send_html("<img xch_sound=play href=http://haven.wolfpaw.net/pueblo/sounds/feed2.wav>",ch,TO_ROOM);
*/
                    return;
            }
        }
        send_to_char( "That is not a valid corpse!\n\r", ch );
        return;
    }
    send_to_char( "BUG! Something screwed up!\n\r", ch );
    return;
}

void do_smell(CHAR_DATA *ch, char *argument)
{
    OBJ_DATA *obj;

   if (argument[0] == '\0')
    {
    send_to_char( "Smell what?\n\r", ch );
    return;
    }
    obj = get_obj_here( ch, NULL, argument );
    if ( obj == NULL )

    {

        send_to_char( "Smell what?\n\r", ch );

        return;

    }
    act("$n sniffs a corpse.", ch,NULL,NULL,TO_NOTVICT);
    send_to_char( "You sniff the corpse...\n\r", ch );

    if( obj->item_type == ITEM_CORPSE_PC )
    {

    if ( obj->timer < 10)
        {
        send_to_char( "It's as rotten as week old fish!\n\r", ch );
        return;
        }
    if ( obj->timer < 20)
        {
        send_to_char( "It stinks pretty bad!\n\r", ch );
        return;
        }
    if ( obj->timer < 30)
        {
        send_to_char( "It smells fresh!\n\r", ch );
        return;
        }
    if ( obj->timer < 40)
        {
        send_to_char( "It must have died only seconds ago!\n\r", ch );
        return;
        }
    if ( obj->timer < 100)
        {
        send_to_char( "It must have died only seconds ago!\n\r", ch );
        return;
        }

    }
    if( obj->item_type == ITEM_CORPSE_NPC )
    {

    if ( obj->timer < 1)
        {
        send_to_char( "It's as rotten as week old fish!\n\r", ch );
        return;
        }
    if ( obj->timer < 2)
        {
        send_to_char( "It stinks pretty bad!\n\r", ch );
        return;
        }
    if ( obj->timer < 3)
        {
        send_to_char( "It smells fresh!\n\r", ch );
        return;
        }
    if ( obj->timer < 4)
        {
        send_to_char( "It must have died only seconds ago!\n\r", ch );
        return;
        }
    if ( obj->timer < 100)
        {
        send_to_char( "It must have died only seconds ago!\n\r", ch );
        return;
        }
    return;
    }
return;
}

void do_nourish(CHAR_DATA *ch, char *argument)
{
    char arg1 [MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    argument = one_argument( argument, arg1 );


    if(IS_NPC(ch))
    {
        if(arg1[0] == '\0')
            return;
        if((victim = get_char_room(ch,NULL, arg1)) == NULL)
            return;
        if(victim->race != race_lookup("vampire") && victim->race != race_lookup("methuselah"))
            return;
        if(ch->race != race_lookup("human"))
            return;

        victim->pblood += ((ch->level/2) - victim->gen);

        if(victim->pblood > victim->max_pblood)
            victim->pblood = victim->max_pblood;

        act( "$n submits to the will of $N and offers up $s's sweet vitae.",  ch, NULL, victim, TO_ROOM );
        kill_em(victim, ch);

        return;
    }

    if ( arg1[0] == '\0')
    {
        send_to_char( "Nourish whom?\n\r", ch );
        return;
    }
    if ( IS_AFFECTED2(ch, AFF2_QUIETUS_BLOODCURSE))
    {
        send_to_char("Your blood curse prevents it!\n\r" ,ch);
        return;
    }
    if ( ( victim = get_char_room( ch, NULL, arg1 ) ) == NULL )
    {
        send_to_char( "Nourish whom?\n\r", ch );
        return;
    }

    if (IS_NPC(victim))
    {
        send_to_char( "You cannot nourish that creature.\n\r", ch );
        return;
    }

    if(victim->race == race_lookup("human"))
    {
        send_to_char("That Kine must feed from you willingly.\n\r",ch);
        return;
    }

    if(!IS_VAMP(victim) || victim->race == race_lookup("dhampire"))
    {
        send_to_char( "Your blood would harm them!\n\r", ch );
        return;
    }
/* Changing so ghouls can be nourished by any vamp blood
    if(victim->race == race_lookup("ghoul") && str_cmp(victim->vamp_master,ch->name))
    {
        send_to_char("That is not your ghoul!\n\r",ch);
        return;
    }
*/
    if (victim->race == race_lookup("ghoul") && !IS_VAMP(ch))
    {
        send_to_char("Ghouls can only be nourished by vampiric blood.\n\r", ch);
        return;
    }

    if( victim == ch)
    {
        send_to_char( "You cannot nourish yourself!\n\r", ch );
        return;
    }
    if(ch->race == race_lookup("ghoul"))
    {

        if(ch->pblood < 11 || ch->hit < ch->max_hit/2)
        {
            send_to_char("You are too weak to nourish that kindred.\n\r",ch);
            return;
        }
        ch->hit -= ch->max_hit/2;
        ch->pblood -= 10;
        victim->pblood += 10;
    }
    else
    {
        if(IS_VAMP(ch))
        {
            if(ch->pblood < 35)
            {
                send_to_char( "You do not have enough blood to nourish them.\n\r", ch );
                return;
            }
            else
            {
                ch->pblood -= 20;
                if(victim->race == race_lookup("ghoul"))
                    victim->pblood += 30;
                else
                {
                    if (ch->gen > victim->gen)
                        victim->pblood += 10;
                    else
                        victim->pblood += 20;
                }
            }
        }
        else
        {
            ch->hit -= (ch->max_hit /10);
            victim->pblood += 20;
        }
    }
    if(victim->pblood > victim->max_pblood) victim->pblood = victim->max_pblood;
    sprintf(buf, "You open your wrist and feed %s some of your blood.\n\r", victim->name);
    send_to_char(buf,ch);
    sprintf(buf, "%s feeds you some of their blood...\n\r", ch->name);
    send_to_char(buf,victim);
    act( "$n opens their wrist and feeds $N some of their blood.",  ch, NULL, victim, TO_NOTVICT );
    if(victim->race == race_lookup("ghoul"))
    {
        send_to_char("You feel flushed as you consume sweet Vitae.\n\r",victim);
        send_to_char("You smile predatorily at the ghoul as it consumes your blood.\n\r",ch);
    }
    if(victim->position == POS_TORPOR)
    {
        victim->position=POS_RESTING;
        send_to_char( "{DThe taste of sweet {rblood {Dpasses your lips, rejuvenating your body and revitalizing your mind as you rise from Torpor!{x\n\r", victim );
        victim->hit = victim->max_hit/10;
        affect_strip(victim, gsn_torpor);
    }
    affect_strip(victim,gsn_vamp_frenzy);
    if(ch->clan == clan_lookup("tzimisce") && victim->inhumanity == 0) victim->inhumanity = 1;
    WAIT_STATE( victim, 24 );
    if(ch->hit < 0)
    {
        send_to_char("You feed them the last drops of your life blood and slowly slip away...\n\r", ch);
        kill_em(victim, ch);
    }
    return;
}

int get_disc_name(char *name)
{
    int i;

    for(i = 1;i == MAX_DISC;i++)
    {
        if(!str_prefix(name, disc_table[i].name))
            return i;
    }

    return 0;
}

void upgrade_disc(CHAR_DATA *ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    int disc, cost;

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if(!IS_VAMP(ch))
    {
        send_to_char("You are not a Vampire!\n\r",ch);
        return;
    }
    if(!(disc = get_disc_name(arg)))
    {
        send_to_char("That's not a valid Vampire Discipline.\n\r",ch);
        return;
    }
    if(ch->pcdata->discipline[disc] == 5)
    {
        send_to_char("Your already as skilled in that vampiric Discipline as you can be.\n\r",ch);
        return;
    }

    if(is_clan_disc(ch,disc))
        cost = ch->pcdata->discipline[disc]*5;
    else
        cost = ch->pcdata->discipline[disc]*7;

    if(ch->freebie < cost)
    {
        send_to_char("You don't have enough Freebies to upgrade this Discipline.\n\r",ch);
        return;
    }
    else
    {
        ch->freebie -= cost;
        ch->pcdata->discipline[disc]++;
        send_to_char("You've increased your knowledge in this vampiric Discipline.\n\r",ch);
        return;
    }
    send_to_char("Error. Something's wrong. Contact a staff member.\n\r",ch);
    return;
}

/* Used to test if a vampire can use X discipline. Can also be used to test for things
    like regeneration or the like. Blood is the bloodcost, if message is TRUE then it
    displays fail messages to the vamp.. if false it can be used silently. - Ugha

    Usage: if(!can_use_disc(ch,AUSPEX,3,10,TRUE)) send_to_char("You can't do that!\n\r",ch);
*/

bool can_use_disc(CHAR_DATA *ch, int disc, int dot, int blood, bool message)
{
    char buf[MAX_STRING_LENGTH];
    if(dot > 6 || dot < 0)
        return FALSE;

    if(!IS_VAMP(ch))
    {
        if(message) send_to_char("You are not a vampire!\n\r" ,ch);
        return FALSE;
    }
    if ( IS_AFFECTED2(ch, AFF2_QUIETUS_BLOODCURSE))
    {
        if(message) send_to_char("Your blood curse prevents it!\n\r" ,ch);
        return FALSE;
    }

    if(ch->pblood - 5 < blood)
    {
        if(message) send_to_char("You fear that doing that will reduce your blood too low.\n\r",ch);
        return FALSE;
    }

    if(disc != NONE && ch->pcdata->discipline[disc] < dot)
    {
        if(message)
        {
            sprintf(buf,"Your skill at %s is not great enough to do that.\n\r",disc_table[disc].name);
            send_to_char(buf,ch);
        }
        return FALSE;
    }

    return TRUE;
}

//New regen code
void do_regenerate(CHAR_DATA *ch, char *argument )
{
    AFFECT_DATA af;

    if((ch->race != race_lookup("vampire")) & (ch->race != race_lookup("ghoul")))
    {
        send_to_char( "You are not a vampire!\n\r", ch );
        return;
    }
    if ( IS_AFFECTED2( ch, AFF2_VAMPIRE_REGEN ))
    {
        send_to_char( "You stop regenerating your wounds.\n\r", ch );
        affect_strip(ch, gsn_vampire_regen);
        return;
    }
    if(ch->position == POS_FIGHTING)
    {
        send_to_char( "You cannot do this in combat!\n\r", ch );
        return;
    }

    act( "$n begins to regenerate their body.",  ch, NULL, NULL, TO_ROOM );
    send_to_char( "You begin to regenerate your body.\n\r", ch );

    af.where    = TO_AFFECTS2;
    af.type     = gsn_vampire_regen;
    af.level    = ch->level;
    af.duration = -1;
    af.modifier = 0;
    af.location = APPLY_NONE;
    af.bitvector    = AFF2_VAMPIRE_REGEN;
    affect_to_char( ch, &af );
}

/*Sengir coding Blood Buff to rais Phys CS stats for Vamps*/
void do_bloodbuff( CHAR_DATA *ch, char *argument )
{
    AFFECT_DATA af;

    if (IS_NPC(ch))
        return;

    if (!IS_VAMP(ch))
    {
        send_to_char("You do not have any vampiric vitae to enhance your attributes.\n\r", ch);
        return;
    }

    if (is_affected(ch, gsn_blood_buff))
    {
        send_to_char("You have already spent vitae to buff up your physical prowess!\n\r", ch);
        return;
    }

    if (ch->pblood < 10)
    {
        send_to_char("You do not have enough vitae in your system to spend.\n\r", ch);
        return;
    }

    ch->pblood -= 9;

    send_to_char("Spending precious {rVi{Rt{rae{x, you use the stolen blood running through your veins\n\rto augment your own physical capabilities.\n\r", ch);

    af.where    = TO_AFFECTS;
    af.type     = gsn_blood_buff;
    af.level    = 1;
    af.duration = 7;
    af.location = APPLY_CS_STR;
    af.modifier = 1;
    af.bitvector    = 0;
    affect_to_char( ch, &af );

    af.where    = TO_AFFECTS;
    af.type     = gsn_blood_buff;
    af.level    = 1;
    af.duration = 7;
    af.location = APPLY_CS_DEX;
    af.modifier = 1;
    af.bitvector    = 0;
    affect_to_char( ch, &af );

    af.where    = TO_AFFECTS;
    af.type     = gsn_blood_buff;
    af.level    = 1;
    af.duration = 7;
    af.location = APPLY_CS_STA;
    af.modifier = 1;
    af.bitvector    = 0;

    affect_to_char( ch, &af );

    return;
}

void vamp_frenzy( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
  AFFECT_DATA af;
  int newnum;

    if (IS_NPC(ch)) return;
  newnum = number_range(1, 100);

/*  sprintf(buf, "TEST: newnum: %d \n\r", newnum);
    send_to_char(buf,ch);
*/
  if ( !IS_AFFECTED2(ch, AFF2_VAMP_FRENZY) )
  {
    act( "$n snarls as $e enter a frenzy!",  ch, NULL, NULL, TO_NOTVICT );
    send_to_char( "You enter a Frenzy!\n\r", ch );
  }
/*
  if (newnum <= 10)
  {
   for (tch = ch->in_room->people; tch != NULL; tch = tch->next_in_room)
   {
    if (fdone)  fdone = TRUE, break out of the for, else keep going
        break;

    if (tch == ch)  stop icky victim == ch in dmg code
        continue;

    twonum = number_range(1, 100);
    if (twonum <= 10 && tch != NULL)
    {
        fdone = TRUE;
        act( "$n throws $Mself into a frenzy and attacks $N!",  ch, NULL, tch, TO_NOTVICT );
        sprintf(buf, "Frenzy overwelms you and you attack %s!\n\r", tch->name);
        send_to_char(buf,ch);
        sprintf(buf, "TEST: char in room is %s twonum: %d \n\r", tch->name, twonum);
        send_to_char(buf,ch);
        multi_hit( ch, tch, TYPE_UNDEFINED );
    }
   }
  }
*/

  if (!is_affected(ch, gsn_vamp_frenzy))
  {
    af.where     = TO_AFFECTS2;
    af.type      = gsn_vamp_frenzy;
    af.level     = level;
    af.duration  = -1;
    af.location  = APPLY_HITROLL;
    af.modifier  = -200;
    af.bitvector = AFF2_VAMP_FRENZY;
    affect_to_char( ch, &af );

    af.location  = APPLY_AC;
    af.modifier  = +200;
    affect_to_char( ch, &af );
  }
    damage( ch, ch, ch->level, gsn_vamp_frenzy, DAM_FIRE, TRUE);
    return;
}

void do_stake(CHAR_DATA *ch, char *argument)
{
   CHAR_DATA *victim;
   OBJ_DATA *stake;
   OBJ_DATA *obj;
   OBJ_DATA *obj_next;
   char buf[MAX_STRING_LENGTH];
   char arg[MAX_INPUT_LENGTH];
   int breaking;
   int chance;
   int awake;
   int i;

    if (IS_NPC(ch)) return;

    do_huh(ch, argument);
    return;
}

void do_diablerize(CHAR_DATA *ch, char *argument)
{
   CHAR_DATA *victim;
   char buf[MAX_STRING_LENGTH];
   char arg[MAX_INPUT_LENGTH];

    if (IS_NPC(ch)) return;

    argument = one_argument( argument, arg );

    if ( ( victim = get_char_room( ch, NULL, arg ) ) == NULL )
    {
        send_to_char( "You cannot diablerize someone who isn't here!\n\r", ch );
        return;
    }
    if(victim == ch)
    {
        send_to_char( "You cannot do this to yourself!\n\r", ch );
        return;
    }
    if(ch->race != race_lookup("vampire") && ch->race != race_lookup("methuselah"))
    {
        send_to_char("You are not a vampire!\n\r" ,ch);
        return;
    }

    if(!IS_VAMP(victim) || victim->race == race_lookup("dhampire"))
    {
        send_to_char("They are not a vampire!\n\r" ,ch);
        return;
    }
    if(victim->position != POS_TORPOR)
    {
        send_to_char("They are not in Torpor!\n\r" ,ch);
        return;
    }
    if ( IS_NPC(victim) )
    {
        send_to_char( "You cannot diablerize that creature!\n\r", ch );
        return;
    }

    if ( !IS_NPC(victim) && !IS_NPC(ch) && !IN_LEVEL(ch, victim))
    {
        send_to_char( "You cannot harm them, they are not in level to you!\n\r", ch );
        return;
    }

/*  if ( !IS_NPC(victim) && (!IS_SET(ch->act,PLR_IC) || !IS_SET(victim->act,PLR_IC)))
    {
        send_to_char( "You and your victim must both be in IC mode to fight.\n\r",ch);
        return;
    }
*/
    if ( ch->pblood > 5 )
    {
        send_to_char( "You cannot contain their blood!\n\r",ch);
        return;
    }
    send_to_char( "You commit the worst crime known...\n\r",ch);
    if(ch->race != race_lookup("methuselah"))   SET_BIT(ch->act, PLR_KILLER);
    if (victim->gen < ch->gen) ch->gen -= 1;
    if(ch->race == race_lookup("methuselah")) ch->drains += 1;
    sprintf(buf, "Slowly feel your unlife slip away as %s drains you of your remaining blood...\n\r", ch->name);
    send_to_char(buf,victim);
    sprintf(buf, "You drink %s's remaining blood and gain their power!\n\r", victim->name);
    send_to_char(buf,ch);
    ch->pblood = 100;
    ch->freebie += 1;
    victim->race = race_lookup("human");
    if(victim->pcdata->points < 30) victim->pcdata->points = 30;
    victim->exp   = exp_per_level(victim,victim->pcdata->points) * UMAX( 1, victim->level );
    victim->pblood = 0;
    victim->gen = 0;
    victim->dpoints = 0;
    victim->pcdata->discipline[ANIMALISM] = 0;

    victim->pcdata->discipline[AUSPEX] = 0;
    victim->pcdata->discipline[PROTEAN] = 0;
    victim->pcdata->discipline[CELERITY] = 0;
    victim->pcdata->discipline[CHIMERSTRY] = 0;
    victim->pcdata->discipline[FORTITUDE] = 0;
    victim->pcdata->discipline[POTENCE] = 0;
    victim->pcdata->discipline[OBFUSCATE] = 0;
    victim->pcdata->discipline[OBTENEBRATION] = 0;
    victim->pcdata->discipline[DOMINATE] = 0;
    victim->pcdata->discipline[PRESENCE] = 0;
    victim->pcdata->discipline[MORTIS] = 0;
    victim->pcdata->discipline[QUIETUS] = 0;
    victim->clan = 0;
    victim->pcdata->condition[COND_THIRST] = 20;
    victim->pcdata->condition[COND_HUNGER] = 20;
    while ( victim->affected )
        affect_remove( victim, victim->affected );
    victim->affected_by = race_table[victim->race].aff;
    kill_em(ch, victim);
    save_char_obj(victim);
    victim->max_hit -= 100;
    victim->pcdata->perm_hit = victim->max_hit;
    return;
}

void do_teach(CHAR_DATA *ch, char *argument)
{
    char arg1[MAX_INPUT_LENGTH], arg2[MIL];
    char buf[MSL];
    CHAR_DATA *victim;
    int i, disc, cost, num;
    disc = -1;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if (IS_NPC(ch)) return;


    if ( ( victim = get_char_world( ch, arg2 ) ) == NULL )
    {
        send_to_char( "Teach what to whom?\n\r", ch );
        return;
    }

    if (IS_NPC(victim))
    {
        send_to_char("They would not benefit from your knowledge.\n\r", ch);
        return;
    }
    if (!IS_SET(ch->act, PLR_IC) || !IS_SET(victim->act, PLR_IC))
    {
        send_to_char("You and your student must both be IC for this!\n\r", ch);
        return;
    }
    if(!IS_VAMP(victim))
    {
        send_to_char("They cannot learn Disciplines from you!\n\r",ch);
        return;
    }

    for (i = 0; i < MAX_DISC; i++)
    {
        if(!str_prefix(disc_table[i].name, arg1))
        {
            disc = i;
            break;
        }
    }

    if (disc == -1)
    {
        send_to_char("Teach what to whom?", ch);
        return;
    }

    if (victim->race != race_lookup("ghoul"))
        cost = 10;
    else
    {
        cost = 15;

        for (num = 0; num < 3; num ++)
        {
            if (i == clandisc_table[victim->clan].clan_disc[disc])
            {
                cost = 10;
                break;
            }
        }
        if (disc == CELERITY || disc == FORTITUDE || disc == POTENCE)
            cost = 8;
    } // if race == ghoul

    if (victim->freebie < cost)
    {
        send_to_char("They do not have enough freebies to learn this discipline.\n\r", ch);
        return;
    }
    if (victim->pcdata->discipline[disc] != 0)
    {
        send_to_char("They already know that discipline!\n\r", ch);
        return;
    }
    if (ch->pcdata->discipline[disc] < 3)
    {
        sprintf(buf, "You are not studied enough in %s to teach it to another.\n\r", capitalize(disc_table[disc].name));
        send_to_char(buf, ch);
        return;
    }

    victim->freebie -= cost;
    victim->pcdata->discipline[disc] = 1;
    act( "$n bestows $s wisdom upon you and teaches you the basics of a new discipline.",  ch, NULL, victim, TO_VICT );
    act( "You grant your knowledge to $N.",  ch, NULL, victim, TO_CHAR );
    return;
}

/* Temp home for Thaum ritual */

void do_incorporealpassage( CHAR_DATA *ch, char *argument )
{
    AFFECT_DATA af;
    OBJ_DATA *obj;

    int dicesuccess = 0;

    if(IS_NPC(ch))
        return;

    if(!IS_VAMP(ch))
    {
        send_to_char("You cannot enact a blood ritual.\n\r", ch);
        return;
    }

    if(ch->fighting != NULL)
    {
        send_to_char("You cannot do this while fighting!\n\r", ch);
        return;
    }

    if (ch->pcdata->discipline[THAUMATURGY] < 3)
    {
        send_to_char("You have not dabbled in the occult arts of blood magic long enough to enact this ritual.\n\r", ch);
        return;
    }

    if (is_affected(ch, gsn_incorporeal_passage))
    {
        send_to_char("You have already enacted this ritual upon yourself.\n\r", ch);
        return;
    }

    if (is_affected(ch, gsn_shadowform) || IS_AFFECTED2(ch, AFF2_MIST))
    {
        send_to_char("Your form already allows you to pass through objects with ease.  This ritual would serve no purpose.\n\r", ch);
        return;
    }

    if (IS_AFFECTED(ch, AFF_PASS_DOOR))
    {
        send_to_char("You are already able to pass through doors.  This ritual would be pointless.\n\r", ch);
        return;
    }

    if ( ( obj = get_carry_vnum( ch, OBJ_VNUM_BROKEN_MIRROR, WEAR_HOLD, TRUE) ) == NULL )
    {
        send_to_char("You must be holding a broken mirror for the ritual to work.\n\r", ch);
        return;
    }

    if (ch->pblood < 26)
    {
        send_to_char("You do not have enough blood to invest into the ritual.\n\r", ch);
        return;
    }

    ch->pblood -= 20;

    extract_obj(obj);

    act("You smash $p with your fist, shattering the glass into many shards.", ch, obj, NULL, TO_CHAR);
    act("$n smashes $p, shattering the glass.", ch, obj, NULL, TO_NOTVICT);

    damage(ch, ch, ch->level, gsn_incorporeal_passage, DAM_SLASH, TRUE);

    WAIT_STATE(ch, 7);

    dicesuccess = godice(get_attribute(ch, INTELLIGENCE)+ch->csabilities[CSABIL_OCCULT], 7);

    if (dicesuccess < 0)
    {
        act("You stare at your hand, bleeding upon the ground, as your body remains solid.", ch, NULL, NULL, TO_CHAR);
        act("$n bleeds onto the ground from a slash on $S hand.", ch, NULL, NULL, TO_NOTVICT);
        WAIT_STATE(ch, 8);
        ch->pblood -= 5;
        return;
    }

    if (dicesuccess == 0)
    {
        act("You look at your body quizzically, as it remains decidely solid.", ch, NULL, NULL, TO_CHAR);
        WAIT_STATE(ch, 5);
        return;
    }

    act("You smile as you watch your body fade and turn translucent.", ch, NULL, NULL, TO_CHAR);
    act("$n fades slowly, becoming a ghostly figure.", ch, NULL, NULL, TO_NOTVICT);

    dicesuccess = godice(get_attribute(ch, WITS)+ch->csabilities[CSABIL_SURVIVAL], 6);

    af.where    = TO_AFFECTS;
    af.type     = gsn_incorporeal_passage;
    af.level    = 3;
    af.duration = dicesuccess * 8;
    af.location = APPLY_NONE;
    af.modifier = 0;
    af.bitvector    = AFF_PASS_DOOR;
    affect_to_char( ch, &af );

    return;
}

/*Vampire Torpor code to rise.  Torp from damage, Humanity allows to wake up.  Blood loss, no*/
void do_arise( CHAR_DATA *ch, char *argument )
{
    AFFECT_DATA af;
    int diff = 0;
    int dicesuccess = 0;

    if (IS_NPC(ch))
    {
        ch->position = POS_STANDING;
        ch->hit = ch->max_hit / 12;
        return;
    }

    if(!IS_VAMP(ch))
    {
        send_to_char("You aren't a vampire!\n\r", ch);
        return;
    }

    if (ch->position != POS_TORPOR)
    {
        send_to_char("You aren't in {RT{ro{Drp{ro{Rr{x.\n\r", ch);
        return;
    }

    if (is_affected(ch, gsn_torpor))
    {
        send_to_char("Your body is still in too critical of a condition to try to arise.\n\r", ch);
        return;
    }

    if (ch->pblood < 10)
    {
        send_to_char("You do not have enough blood in your body to rise.\n\r", ch);
        return;
    }

    if (IS_NEWBIE(ch))
        diff = 3;
    else
        diff = 6;

    dicesuccess = godice(ch->pcdata->cshumanity, diff);
    ch->pblood -= 5;
    act_new("You gather up your reserves of determination, ", ch, NULL, NULL, TO_CHAR, POS_DEAD, FALSE);

    if (IS_NEWBIE(ch))
        dicesuccess++;

    if (dicesuccess < 0)
    {
        act_new("but fail miserably, falling into an even further pit of helplessness.\n\rOnly the taste of vitae can rouse you now.", ch, NULL, NULL, TO_CHAR, POS_DEAD, FALSE);

        af.where    = TO_AFFECTS;
        af.type     = gsn_torpor;
        af.level    = ch->level;
        af.duration = -1;
        af.location = APPLY_NONE;
        af.modifier = 0;
        af.bitvector    = 0;
        affect_to_char( ch, &af );

        return;
    }

    if (dicesuccess == 0)
    {
        act_new("ready to enter the world again, but your body refuses to move.\n\rWait a little longer, and then try again.", ch, NULL, NULL, TO_CHAR, POS_DEAD, FALSE);

        af.where    = TO_AFFECTS;
        af.type     = gsn_torpor;
        af.level    = ch->level;
        af.duration = 5;
        af.location = APPLY_NONE;
        af.modifier = 0;
        af.bitvector    = 0;
        affect_to_char( ch, &af );

        return;
    }

    act_new("and force your eyes wide awake.  Your body begins to respond, slowly.\n\rWith grim determination, you force yourself awake from {RT{ro{Drp{ro{Rr{x.", ch, NULL, NULL, TO_CHAR, POS_TORPOR, FALSE);
    act("$n's eyelids flutter open as $e begins to slowly rise from {RT{ro{Drp{ro{Rr{x.", ch, NULL, NULL, TO_ROOM);

    ch->position = POS_RESTING;
    ch->hit = ch->max_hit / 10;
    gain_exp(ch, dicesuccess * 3);

    return;


}

bool make_vamp(CHAR_DATA *ch, char *argument)
{
    char clan[MAX_INPUT_LENGTH];
    int i;

    one_argument(argument,clan);

    if(!clan_lookup(clan))
        return FALSE;
    if(ch->race == race_lookup("vampire"))
        return FALSE;
    if(IS_NPC(ch))
        return FALSE;

    ch->race = race_lookup("vampire");
    ch->embraced++;
    ch->gen = 10;
    ch->pcdata->csgeneration = 10;
    ch->clan = clan_lookup(clan);
    ch->sire = str_dup("Unknown");
    ch->max_pblood = 100;
    ch->pblood = ch->max_pblood;
    ch->pcdata->condition[COND_THIRST] = -1;
    ch->pcdata->condition[COND_HUNGER] = -1;
    ch->affected_by = race_table[ch->race].aff;
    REMOVE_BIT(ch->imm_flags, IMM_SUNLIGHT);
    for (i = 0; i < 3; i++)
    {
        if (ch->pcdata->discipline[clandisc_table[ch->clan].clan_disc[i]] == 0)
            ch->pcdata->discipline[clandisc_table[ch->clan].clan_disc[i]] = 1;
    }
    return TRUE;
}
