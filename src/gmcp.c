/* gmcp.c - Generic MUD Communication Protocol
 *
 * Implements GMCP for Mudlet mapper integration and WoD vitals monitoring.
 * Protocol format: IAC SB GMCP "Package.Name {json}" IAC SE
 */

#if defined(macintosh)
#include <types.h>
#include <time.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "merc.h"
#include "telnet.h"

/* Direction abbreviations for the Mudlet mapper */
static const char *gmcp_dir_name[] = {
    "n", "e", "s", "w", "u", "d"
};


/* Simple JSON string escaping to avoid breaking client parsers */
static void escape_json( const char *src, char *dst )
{
    if ( !src )
    {
        *dst = '\0';
        return;
    }
    while ( *src )
    {
        if ( *src == '"' || *src == '\\' )
        {
            *dst++ = '\\';
            *dst++ = *src++;
        }
        else if ( *src == '\n' )
        {
            *dst++ = '\\';
            *dst++ = 'n';
            src++;
        }
        else if ( *src == '\r' )
        {
            *dst++ = '\\';
            *dst++ = 'r';
            src++;
        }
        else if ( *src == '\t' )
        {
            *dst++ = '\\';
            *dst++ = 't';
            src++;
        }
        else
        {
            *dst++ = *src++;
        }
    }
    *dst = '\0';
}

/*
 * Send a GMCP package to a descriptor.
 * Format: IAC SB GMCP "Package.Name {json_data}" IAC SE
 */
void gmcp_send( DESCRIPTOR_DATA *d, const char *package, const char *data )
{
    char buf[MAX_STRING_LENGTH * 2];
    int len;

    if ( !d || !d->gmcp_enabled )
        return;

    /* Build: IAC SB GMCP <package> <space> <data> IAC SE */
    buf[0] = (char)IAC;
    buf[1] = (char)SB;
    buf[2] = (char)TELOPT_GMCP;

    if ( data && data[0] != '\0' )
        len = sprintf( buf + 3, "%s %s", package, data );
    else
        len = sprintf( buf + 3, "%s", package );

    buf[3 + len] = (char)IAC;
    buf[4 + len] = (char)SE;

    write_to_buffer( d, buf, 5 + len );
}

/*
 * Send Room.Info — the core mapper packet.
 * Called whenever a player enters a room (do_look, char_to_room, etc.)
 */
void gmcp_send_room( CHAR_DATA *ch )
{
    char buf[MAX_STRING_LENGTH * 2];
    char exits_buf[MAX_STRING_LENGTH];
    char escaped_name[MAX_STRING_LENGTH];
    char escaped_area[MAX_STRING_LENGTH];
    ROOM_INDEX_DATA *room;
    int i;
    bool first_exit = TRUE;

    if ( !ch || !ch->desc || !ch->desc->gmcp_enabled || !ch->in_room )
        return;

    room = ch->in_room;

    /* Build exits JSON object */
    strcpy( exits_buf, "{" );
    for ( i = 0; i < 6; i++ )
    {
        if ( room->exit[i] != NULL
        &&   room->exit[i]->u1.to_room != NULL
        &&   !IS_SET(room->exit[i]->exit_info, EX_CLOSED) )
        {
            char exit_entry[128];
            sprintf( exit_entry, "%s\"%s\":%d",
                first_exit ? "" : ",",
                gmcp_dir_name[i],
                room->exit[i]->u1.to_room->vnum );
            strcat( exits_buf, exit_entry );
            first_exit = FALSE;
        }
    }
    strcat( exits_buf, "}" );

    /* Escape string fields safely */
    escape_json( room->name ? room->name : "Unknown", escaped_name );
    escape_json( (room->area && room->area->name) ? room->area->name : "Unknown", escaped_area );

    /* Build the full Room.Info JSON */
    sprintf( buf,
        "{\"num\":%d,"
         "\"name\":\"%s\","
         "\"area\":\"%s\","
         "\"environment\":\"%s\","
         "\"exits\":%s}",
        room->vnum,
        escaped_name,
        escaped_area,
        sector_name( room->sector_type ),
        exits_buf );

    gmcp_send( ch->desc, "Room.Info", buf );
}

/*
 * Send Char.Vitals — health, mana, movement, and World of Darkness stats.
 * Called on prompt, after combat, regenerate tick, etc.
 */
void gmcp_send_char_vitals( CHAR_DATA *ch )
{
    char buf[MAX_STRING_LENGTH];
    int willpower = 0;
    int max_willpower = 0;
    int bloodpool = 0;
    int max_bloodpool = 0;
    int rage = 0;
    int max_rage = 0;
    int gnosis = 0;
    int max_gnosis = 0;
    int quintessence = 0;
    int max_quintessence = 0;
    int paradox = 0;

    if ( !ch || !ch->desc || !ch->desc->gmcp_enabled || IS_NPC(ch) )
        return;

    willpower = ch->cswillpower;
    max_willpower = ch->csmax_willpower;

    if ( ch->pcdata )
    {
        rage = ch->pcdata->rage[0];
        max_rage = ch->pcdata->rage[1];
        gnosis = ch->pcdata->gnosis[0];
        max_gnosis = ch->pcdata->gnosis[1];
    }

    bloodpool = ch->pblood/10;
    max_bloodpool = ch->max_pblood/10;
    quintessence = ch->quintessence;
    max_quintessence = ch->max_quintessence;
    paradox = ch->paradox;

    sprintf( buf,
        "{\"hp\":%d,\"maxhp\":%d,"
         "\"mana\":%d,\"maxmana\":%d,"
         "\"move\":%d,\"maxmove\":%d,"
         "\"willpower\":%d,\"maxwillpower\":%d,"
         "\"bloodpool\":%d,\"maxbloodpool\":%d,"
         "\"rage\":%d,\"maxrage\":%d,"
         "\"gnosis\":%d,\"maxgnosis\":%d,"
         "\"quintessence\":%d,\"maxquintessence\":%d,"
         "\"paradox\":%d}",
        ch->hit, ch->max_hit,
        ch->mana, ch->max_mana,
        ch->move, ch->max_move,
        willpower, max_willpower,
        bloodpool, max_bloodpool,
        rage, max_rage,
        gnosis, max_gnosis,
        quintessence, max_quintessence,
        paradox );

    gmcp_send( ch->desc, "Char.Vitals", buf );
}

/*
 * Helper: sector type to environment name string for mapper coloring.
 */
const char *sector_name( int sector )
{
    switch ( sector )
    {
        case SECT_INSIDE:       return "indoors";
        case SECT_CITY:         return "city";
        case SECT_FIELD:        return "field";
        case SECT_FOREST:       return "forest";
        case SECT_HILLS:        return "hills";
        case SECT_MOUNTAIN:     return "mountain";
        case SECT_WATER_SWIM:   return "shallowwater";
        case SECT_WATER_NOSWIM: return "water";
        case SECT_AIR:          return "air";
        case SECT_DESERT:       return "desert";
        case SECT_WATER_DROWN:  return "underwater";
        case SECT_HOT:          return "lava";
        case SECT_COLD:         return "tundra";
        case SECT_NODE:         return "node";
        default:                return "unknown";
    }
}
