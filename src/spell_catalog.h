#ifndef SPELL_CATALOG_H
#define SPELL_CATALOG_H

#include <string.h>

static const char *SPELL_NAMES[] = {
    "ICE",
};

static inline int SpellIndex(const char *spell)
{
    const int SpellN = sizeof(SPELL_NAMES)/sizeof(SPELL_NAMES[0]);
    for (int i = 0; i < SpellN; i++)
    {
        if (strcmp(spell, SPELL_NAMES[i]) == 0)
            return i;
    }
    return -1;
}

#endif
