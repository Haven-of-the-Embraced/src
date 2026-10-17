int get_addict_bonus args( (CHAR_DATA *ch, int xp) );
double get_addict_mult args ( (CHAR_DATA *ch) );
#define ADDICTED(ch)       ((int) ((current_time - (ch)->logon) / 3600))
