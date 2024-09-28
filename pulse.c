#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "merc.h"

void affects_update (void) {
    CHAR_DATA *ch;
    CHAR_DATA *ch_next;
    CHAR_DATA *rch;
    CHAR_DATA *rch_next;
    AFFECT_DATA af;
    int level;
    int mod;
    int success;

    /* Examine all mobs. */
    for ( ch = char_list; ch != NULL; ch = ch_next )
    {
    ch_next = ch->next;

    //EMERGOD POISON ACTUALLY SUCKS NOW CODE
    if ( IS_AFFECTED (ch, AFF_POISON) && ch->hit > (ch->max_hit / 20) &&
        godice(get_attribute(ch, STAMINA), 8) < 1)
        damage(ch, ch, (ch->max_hit / (50*get_attribute(ch, STAMINA))),
            gsn_poison,DAM_POISON,FALSE);

    if (is_affected(ch, gsn_bleeding) && has_blood(ch) && ch->hit > 10 * ch->level / 50)
    {
      level = get_affect_level(ch, gsn_bleeding);
      if (godice(get_attribute(ch, STAMINA), 8) < 1)
      {
        act("Your wounds continue to bleed out.", ch, NULL, NULL, TO_CHAR);
        act("$n continues to bleed from $s open wounds.", ch, NULL, NULL, TO_NOTVICT);
        damage(ch, ch, (level * ch->level / 50), gsn_bleeding, DAM_AGGREVATED, TRUE);
        if (!IS_NPC(ch) && number_percent() > 85)
        {
          ch->pblood--;
        }
      }
    }

    if (is_affected(ch, gsn_shadowplay) && get_affect_level(ch, gsn_shadowplay) != 0)
    {
        level = get_affect_level(ch, gsn_shadowplay);
        if ( godice(level, 8) > 0
        && ch->race != race_lookup("vampire") && ch->race != race_lookup("methuselah")
        && ch->race != race_lookup("construct") && ch->race != race_lookup("wraith")
        && ch->race != race_lookup("spirit") && ch->race != race_lookup("elemental")
        && ch->race != race_lookup("ooze") && ch->race != race_lookup("shadow")
        && ch->race != race_lookup("undead") && ch->race != race_lookup("vegetation")
        )
        {
            act( "$n chokes and gags as shadows try to suffocate $m.", ch, NULL, NULL, TO_ROOM );
            send_to_char( "You gag and struggle to breathe through the shadows in your throat.\n\r", ch );
            if(ch->position != POS_TORPOR)
                damage(ch,ch,ch->max_hit/(10*(15-level)),gsn_shadowplay,DAM_BASH,FALSE);
        }
    }

    if (is_affected(ch, gsn_armsoftheabyss) && ch->position != POS_TORPOR)
    {
        level = get_affect_level(ch, gsn_armsoftheabyss);
        mod = get_affect_modifier(ch, gsn_armsoftheabyss) * 2;
        success = godice(mod, 6);
        if ( success > 0 )
        {
            act( "$n gets pummelled by multiple shadowy tendrils assaulting $m.", ch, NULL, NULL, TO_ROOM );
            send_to_char( "You take a beating from the shadow tendrils surrounding you.\n\r", ch );
            d10_damage( ch, ch, success, level * 2, gsn_armsoftheabyss, DAM_BASH, DEFENSE_FULL, TRUE, FALSE);
 //           damage(ch,ch,ch->max_hit/(10*(15-level)),gsn_shadowplay,DAM_BASH,FALSE);
        }
    }

    if (is_affected(ch, gsn_ignis_fatuus) && IS_AWAKE(ch))
    {
        int level = get_affect_level(ch, gsn_ignis_fatuus);
        if (number_percent() < 4 * level)
        {
            char * chmsg;
            char * roommsg;

            switch (number_range(1, 4))
            {
                case 1:
                    chmsg = str_dup("You hear someone dashing up behind you and turn to find... nothing.\n\r");
                    roommsg = str_dup("$n suddenly spins around to face a threat... that isn't there.");
                    break;
                case 2:
                    chmsg = str_dup("You are momentarily deafened by the sound of bells!\n\r");
                    roommsg = str_dup("$n covers $s ears, distracted by some sound only $e can hear.");
                    break;
                case 3:
                    chmsg = str_dup("You feel spiders crawling all over your skin!\n\r");
                    roommsg = str_dup("$n starts dancing around, beating $s clothes to dislodge imaginary bugs.");
                    break;
                case 4:
                    chmsg = str_dup("You are dazzled by a bright light shining in your eyes!\n\r");
                    roommsg = str_dup("$n blinks and covers $s eyes, dazzled by some vision.");
                    break;
            }
            sendch(chmsg, ch);
            act(roommsg, ch, NULL, NULL, TO_ROOM);
            STOPPED(ch, 2*PULSE_VIOLENCE);
        }
    }

    if (is_affected(ch, gsn_blight) && ch->fighting != NULL &&
        (!IS_VAMP(ch) || ch->race == race_lookup("ghoul")) &&
        !IS_SET(ch->act2, ACT2_ULTRA_MOB) && (ch->level <= get_affect_level(ch, gsn_blight) + 5))
    {
      if (godice(get_attribute(ch, STAMINA), 6) < 1)
      {
        act( "$n clutches $s chest and collapses!", ch, NULL, NULL, TO_ROOM );
        send_to_char( "Your heart stops from the strain on your body!\n\r", ch );
        kill_em(ch->fighting,ch);
      }
    }

    // Player-Only affects below this point.
    if (IS_NPC(ch))
        continue;

    if (is_affected(ch, gsn_sparkofrage))
    {
      for ( rch = ch->in_room->people; rch != NULL; rch = rch_next )
      {
          rch_next = rch->next_in_room;

          if (!IS_NPC(rch) || is_aggsafe(rch, ch) || rch->fighting)
              continue;

              act("$n shrieks wildly and charges at you, overcome with rage!",rch,NULL,ch,TO_VICT);
              act("$n shrieks wildly and charges at $N... intent on ending $S life.",rch,NULL,ch,TO_NOTVICT);
              multi_hit( rch, ch, TYPE_UNDEFINED );
      }
    }


    } //for char_list

};
