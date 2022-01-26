#ifndef SPELL_CATALOG_H
#define SPELL_CATALOG_H

#include <raylib.h> // Just for Color, it is not so necessary
#include <string.h>

typedef enum {
    SPELLTYPE_NONE = 0,
    SPELLTYPE_ICE,
    SPELLTYPE_FREE,
    SPELLTYPE_FIRE,
    SPELLTYPE_FEE,
    SPELLTYPE_IRE,
    SPELLTYPE_REIF,
    SPELLTYPE_REFER,
    SPELLTYPE_RICE,
    SPELLTYPE_REEF,
} SpellType;

typedef struct {
    SpellType type;
    const char *name;
    Color color;
    Color color2;
} Spell;

Color GetPowerCharColor(char c);

Spell GetSpell(const char *spell);

#endif
