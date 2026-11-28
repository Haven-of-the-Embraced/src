#ifndef MOON_H
#define MOON_H


/* Moon phase definitions */
#define MOON_NEW             0
#define MOON_WAXING_CRESCENT 1
#define MOON_FIRST_QUARTER   2
#define MOON_WAXING_GIBBOUS  3
#define MOON_FULL            4
#define MOON_WANING_GIBBOUS  5
#define MOON_LAST_QUARTER    6
#define MOON_WANING_CRESCENT 7
#define MAX_MOON_PHASE       8

/* Function declarations */
void update_moon_phase(void);
const char *get_moon_phase_name(int phase);
const char *get_moon_phase_desc(int phase);
int get_moon_illumination(int phase);
const char *get_moon_ascii_art(int phase);
bool is_auspice_moon(int auspice, int phase);

#endif /* MOON_H */
