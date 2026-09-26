#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "merc.h"
#include "interp.h"

#define MAP_RADIUS 3
#define MAP_SIZE (MAP_RADIUS * 2 + 1)
#define GRID_SIZE (MAP_SIZE * 2 - 1)

typedef struct map_grid_data
{
    ROOM_INDEX_DATA *room;
} MAP_GRID_DATA;

static void map_build_grid(ROOM_INDEX_DATA *room, MAP_GRID_DATA grid[GRID_SIZE][GRID_SIZE], int x, int y, int depth)
{
    EXIT_DATA *pexit;
    ROOM_INDEX_DATA *to_room;

    if (x < 0 || y < 0 || x >= GRID_SIZE || y >= GRID_SIZE)
        return;

    if (grid[y][x].room != NULL)
        return;

    grid[y][x].room = room;

    if (depth == 0)
        return;

    if ((pexit = room->exit[0]) != NULL && (to_room = pexit->u1.to_room) != NULL && !IS_SET(pexit->exit_info, EX_HIDDEN))
        map_build_grid(to_room, grid, x, y - 2, depth - 1);
    
    if ((pexit = room->exit[1]) != NULL && (to_room = pexit->u1.to_room) != NULL && !IS_SET(pexit->exit_info, EX_HIDDEN))
        map_build_grid(to_room, grid, x + 2, y, depth - 1);

    if ((pexit = room->exit[2]) != NULL && (to_room = pexit->u1.to_room) != NULL && !IS_SET(pexit->exit_info, EX_HIDDEN))
        map_build_grid(to_room, grid, x, y + 2, depth - 1);

    if ((pexit = room->exit[3]) != NULL && (to_room = pexit->u1.to_room) != NULL && !IS_SET(pexit->exit_info, EX_HIDDEN))
        map_build_grid(to_room, grid, x - 2, y, depth - 1);
}

static char *get_sector_color(int sector)
{
    switch (sector)
    {
        case SECT_INSIDE:       return "{W"; // White
        case SECT_CITY:         return "{D"; // Dark Grey
        case SECT_FIELD:        return "{G"; // Green
        case SECT_FOREST:       return "{g"; // Dark Green
        case SECT_HILLS:        return "{y"; // Brown
        case SECT_MOUNTAIN:     return "{w"; // Light Grey
        case SECT_WATER_SWIM:   return "{C"; // Light Blue
        case SECT_WATER_NOSWIM: return "{B"; // Dark Blue
        case SECT_UNUSED:       return "{D"; // Dark Grey
        case SECT_AIR:          return "{c"; // Cyan
        case SECT_DESERT:       return "{Y"; // Yellow
        case SECT_WATER_DROWN:  return "{B"; // Dark Blue
        case SECT_HOT:          return "{R"; // Red
        case SECT_COLD:         return "{W"; // White
        case SECT_NODE:         return "{M"; // Magenta
        default:                return "{x"; // Default
    }
}

void show_map_to_char(CHAR_DATA *ch)
{
    MAP_GRID_DATA grid[GRID_SIZE][GRID_SIZE];
    int x, y;
    char buf[MAX_STRING_LENGTH];
    char line[MAX_STRING_LENGTH];
    char symbol[10];

    memset(grid, 0, sizeof(grid));
    
    // Build map starting from the center (GRID_SIZE / 2 is the middle coordinate)
    int center = GRID_SIZE / 2;
    map_build_grid(ch->in_room, grid, center, center, MAP_RADIUS);

    send_to_char("\n\r", ch);
    
    for (y = 0; y < GRID_SIZE; y++)
    {
        line[0] = '\0';
        bool row_has_content = FALSE;

        for (x = 0; x < GRID_SIZE; x++)
        {
            if (x % 2 == 0 && y % 2 == 0) // Room cell
            {
                if (grid[y][x].room != NULL)
                {
                    row_has_content = TRUE;
                    if (y == center && x == center) // Player is here
                    {
                        sprintf(symbol, "{R*{x");
                    }
                    else
                    {
                        sprintf(symbol, "%s#{x", get_sector_color(grid[y][x].room->sector_type));
                    }
                    strcat(line, symbol);
                }
                else
                {
                    strcat(line, " ");
                }
            }
            else if (x % 2 != 0 && y % 2 == 0) // East/West exit cell
            {
                bool has_exit = FALSE;
                bool is_closed = FALSE;
                
                if (grid[y][x-1].room != NULL && grid[y][x+1].room != NULL)
                {
                    EXIT_DATA *pexit = grid[y][x-1].room->exit[1];
                    if (pexit != NULL && pexit->u1.to_room == grid[y][x+1].room && !IS_SET(pexit->exit_info, EX_HIDDEN))
                    {
                        has_exit = TRUE;
                        if (IS_SET(pexit->exit_info, EX_CLOSED)) is_closed = TRUE;
                    }
                    pexit = grid[y][x+1].room->exit[3];
                    if (pexit != NULL && pexit->u1.to_room == grid[y][x-1].room && !IS_SET(pexit->exit_info, EX_HIDDEN))
                    {
                        has_exit = TRUE;
                        if (IS_SET(pexit->exit_info, EX_CLOSED)) is_closed = TRUE;
                    }
                }
                
                if (has_exit)
                {
                    if (is_closed) strcat(line, "{D+{x");
                    else strcat(line, "{D-{x");
                    row_has_content = TRUE;
                }
                else
                {
                    strcat(line, " ");
                }
            }
            else if (x % 2 == 0 && y % 2 != 0) // North/South exit cell
            {
                bool has_exit = FALSE;
                bool is_closed = FALSE;
                
                if (grid[y-1][x].room != NULL && grid[y+1][x].room != NULL)
                {
                    EXIT_DATA *pexit = grid[y-1][x].room->exit[2];
                    if (pexit != NULL && pexit->u1.to_room == grid[y+1][x].room && !IS_SET(pexit->exit_info, EX_HIDDEN))
                    {
                        has_exit = TRUE;
                        if (IS_SET(pexit->exit_info, EX_CLOSED)) is_closed = TRUE;
                    }
                    pexit = grid[y+1][x].room->exit[0];
                    if (pexit != NULL && pexit->u1.to_room == grid[y-1][x].room && !IS_SET(pexit->exit_info, EX_HIDDEN))
                    {
                        has_exit = TRUE;
                        if (IS_SET(pexit->exit_info, EX_CLOSED)) is_closed = TRUE;
                    }
                }
                
                if (has_exit)
                {
                    if (is_closed) strcat(line, "{D+{x");
                    else strcat(line, "{D|{x");
                    row_has_content = TRUE;
                }
                else
                {
                    strcat(line, " ");
                }
            }
            else // Diagonal filler
            {
                strcat(line, " ");
            }
        }
        
        if (row_has_content)
        {
            // Optional: trim trailing spaces
            int len = strlen(line);
            while(len > 0 && line[len-1] == ' ')
            {
                line[len-1] = '\0';
                len--;
            }
            sprintf(buf, "    %s\n\r", line); // indent slightly
            send_to_char(buf, ch);
        }
    }
}

void do_map(CHAR_DATA *ch, char *argument)
{
    if (ch->in_room == NULL)
        return;
    show_map_to_char(ch);
}

void do_automap(CHAR_DATA *ch, char *argument)
{
    if (IS_NPC(ch))
        return;

    if (IS_SET(ch->comm, COMM_AUTOMAP))
    {
        send_to_char("Auto-mapper removed.\n\r", ch);
        REMOVE_BIT(ch->comm, COMM_AUTOMAP);
    }
    else
    {
        send_to_char("Auto-mapper enabled.\n\r", ch);
        SET_BIT(ch->comm, COMM_AUTOMAP);
    }
}
