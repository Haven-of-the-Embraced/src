/*
 * Domain Data. All things  Domain related.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#endif

#include "merc.h"
#include "db.h"
#include "recycle.h"
#include "music.h"
#include "tables.h"
#include "lookup.h"
#include "olc.h"
#define DEDIT( fun )        bool fun( CHAR_DATA *ch, char *argument )
 DOMAIN_DATA                 *domain_list;
 DOMAIN_DATA                 *domain_free;
 DOMAIN_DATA                 *domain_first;
 DOMAIN_DATA                 *domain_last;
 int                          top_domain;
 

DOMAIN_DATA *new_domain (void)
{

    static DOMAIN_DATA domain_zero;
    DOMAIN_DATA *dom;

    if (domain_free == NULL)
    dom = alloc_perm(sizeof(*dom));
    else
    {
    dom = domain_free;
    domain_free = domain_free->next;
    }

    *dom = domain_zero;

    VALIDATE(dom);
    return dom;
}

void free_domain (DOMAIN_DATA *dom)
{


    if (!IS_VALID(dom))
    return;

    dom->next = domain_free;
    domain_free  = dom;

    INVALIDATE(dom);
    return;
}



