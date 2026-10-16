#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>

#include "merc.h"
#include "moon.h"

struct moon_phase_type
{
    const char *name;
    const char *desc;
    int illumination;
    const char *ascii_art;
};

static const struct moon_phase_type moon_phases[MAX_MOON_PHASE] =
{
    {
        "new",
        "The Moon is New.\n\r",
        0,
        "{D         _..._\n\r"
        "{D       .::::::$.\n\r"
        "{D      :::::::::::\n\r"
        "{D      :::::::::::\n\r"
        "{D      `:::::::::'\n\r"
        "{D        `':::''{x\n\r"
    },
    {
        "waxing crescent",
        "The Crescent Moon shimmers mysteriously.\n\r",
        25,
        "{D        {W_..._\n\r"
        "{D      .::::{W. `.\n\r"
        "{D     :::::::{W.  :\n\r"
        "{D     :::::::{W:  :\n\r"
        "{D     `::::::{W' .'\n\r"
        "{D      `'::{W'-'{x\n\r"
    },
    {
        "waxing half",
        "The Half Moon glimmers overhead.\n\r",
        50,
        "{x        _.{W.._\n\r"
        "{x      .:::{W:  `.\n\r"
        "{x     :::::{W:    :\n\r"
        "{x     :::::{W:    :\n\r"
        "{x     `::::{W:   .'\n\r"
        "{x       `':{W:.-'{x\n\r"
    },
    {
        "waxing gibbous",
        "The Gibbous Moon winks from the shadows.\n\r",
        75,
        "{x        _{W..._\n\r"
        "{x      .::{W'   `.\n\r"
        "{x     ::{W:       :\n\r"
        "{x     ::{W:       :\n\r"
        "{x     `::{W.     .'\n\r"
        "{x      `':{W..-'{x\n\r"
    },
    {
        "full",
        "The Full Moon fills the heavens with it's brilliance.\n\r",
        100,
        "{W        _..._\n\r"
        "{W      .'     `.\n\r"
        "{W     :         :\n\r"
        "{W     :         :\n\r"
        "{W     `.       .'\n\r"
        "{W       `-...-' {x\n\r"
    },
    {
        "waning gibbous",
        "The Gibbous Moon winks from the shadows.\n\r",
        75,
        "{W        _{D..._\n\r"
        "{W      .'   {D`::.\n\r"
        "{W     :       {D:::\n\r"
        "{W     :       {D:::\n\r"
        "{W     `.     {D.::'\n\r"
        "{W       `-{D..:''{x\n\r"
    },
    {
        "waning half",
        "The Half Moon beams down from overhead.\n\r",
        50,
        "{W        _.{D.._\n\r"
        "{W      .'  {D::::.\n\r"
        "{W     :    {D::::::\n\r"
        "{W     :    {D::::::\n\r"
        "{W     `.   {D:::::'\n\r"
        "{W       `-.{D::''{x\n\r"
    },
    {
        "waning crescent",
        "The Crescent Moon hangs in the sky like a beautiful ornament.\n\r",
        25,
        "{W        _..._{D\n\r"
        "{W      .' .{D::::.\n\r"
        "{W     :  :{D:::::::\n\r"
        "{W     :  :{D:::::::\n\r"
        "{W     `. '{D::::::'\n\r"
        "{W       `-.::{D''{x\n\r"
    }
};

/* Advance moon phase by one step (called at midnight) */
void update_moon_phase(void)
{
    time_info.moon_count++;
    if (time_info.moon_count >= 3)
    {
        time_info.moon_count = 0;
        time_info.phase = (time_info.phase + 1) % MAX_MOON_PHASE;
    }
}

/* Returns the descriptive name of the phase */
const char *get_moon_phase_name(int phase)
{
    if (phase < 0 || phase >= MAX_MOON_PHASE)
        return "unknown";
    return moon_phases[phase].name;
}

/* Returns atmospheric phase descriptions */
const char *get_moon_phase_desc(int phase)
{
    if (phase < 0 || phase >= MAX_MOON_PHASE)
        return "The moon is in a strange alignment.\n\r";
    return moon_phases[phase].desc;
}

/* Returns 0-100 illumination percentage */
int get_moon_illumination(int phase)
{
    if (phase < 0 || phase >= MAX_MOON_PHASE)
        return 0;
    return moon_phases[phase].illumination;
}

/* Returns ASCII art block for this phase */
const char *get_moon_ascii_art(int phase)
{
    if (phase < 0 || phase >= MAX_MOON_PHASE)
        return "Error displaying moon art.\n\r";
    return moon_phases[phase].ascii_art;
}

/* Checks if a Garou's auspice matches the given moon phase (waxing or waning) */
bool is_auspice_moon(int auspice, int phase)
{
    switch (auspice)
    {
        case RAGABASH: return (phase == MOON_NEW);
        case THEURGE:  return (phase == MOON_WAXING_CRESCENT || phase == MOON_WANING_CRESCENT);
        case PHILODOX: return (phase == MOON_FIRST_QUARTER || phase == MOON_LAST_QUARTER);
        case GALLIARD: return (phase == MOON_WAXING_GIBBOUS || phase == MOON_WANING_GIBBOUS);
        case AHROUN:   return (phase == MOON_FULL);
        default:       return FALSE;
    }
}
