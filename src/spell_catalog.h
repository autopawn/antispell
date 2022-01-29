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
    SPELLTYPE_SEE,
    SPELLTYPE_LESS,
    SPELLTYPE_SELL,
    SPELLTYPE_CELL,
    SPELLTYPE_ELSE,
    SPELLTYPE_LOSS,
    SPELLTYPE_COOL,
    SPELLTYPE_LOSE,
    SPELLTYPE_LOL,
    SPELLTYPE_CEO,
    SPELLTYPE_SOLO,
    SPELLTYPE_LOOSE,
    SPELLTYPE_LOCO,
    SPELLTYPE_EEL,
    SPELLTYPE_CLOSE,
    SPELLTYPE_LAST = SPELLTYPE_CLOSE,
} SpellType;

typedef struct {
    SpellType type;
    const char *name;
    Color color;
    Color color2;
    int used; // Only for the inner catalog
} Spell;

Color GetPowerCharColor(char c);

Spell GetSpell(const char *spell);

Spell GetSpellFromType(SpellType type);

#endif
