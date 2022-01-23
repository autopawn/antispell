#ifndef SPELL_CATALOG_H
#define SPELL_CATALOG_H

#include <string.h>

typedef struct {
    unsigned char r, g, b, a;
} SpellColor; // I don't want to include raylib in this small header.

typedef enum {
    SPELLTYPE_NONE = 0,
    SPELLTYPE_ICE = 1,
} SpellType;

typedef struct {
    SpellType type;
    const char *name;
    SpellColor color;
} Spell;

static Spell SPELL_CATALOG[] = {
    {SPELLTYPE_ICE, "ICE", (SpellColor){102, 191, 255, 255}},
};

static inline Spell GetSpell(const char *spell)
{
    const int SpellN = sizeof(SPELL_CATALOG)/sizeof(SPELL_CATALOG[0]);
    for (int i = 0; i < SpellN; i++)
    {
        if (strcmp(spell, SPELL_CATALOG[i].name) == 0)
            return SPELL_CATALOG[i];
    }
    return (Spell){SPELLTYPE_NONE, NULL, (SpellColor){0, 0, 0, 0}};
}

#endif
