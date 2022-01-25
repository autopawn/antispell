#ifndef SPELL_CATALOG_H
#define SPELL_CATALOG_H

#include <raylib.h> // Just for Color, it is not so necessary
#include <string.h>

typedef enum {
    SPELLTYPE_NONE = 0,
    SPELLTYPE_ICE = 1,
} SpellType;

typedef struct {
    SpellType type;
    const char *name;
    Color color;
} Spell;

Color GetPowerCharColor(char c);

Spell GetSpell(const char *spell);

#endif
