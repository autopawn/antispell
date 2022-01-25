#include "spell_catalog.h"

static Color POWERCHAR_COLOR[256] = {
    ['I'] = ORANGE,
    ['C'] = BLUE,
    ['E'] = GREEN,
};

static Spell SPELL_CATALOG[] = {
    {SPELLTYPE_ICE, "ICE", (Color){102, 191, 255, 255}},
};

Color GetPowerCharColor(char c)
{
    if (c == 0) return WHITE;
    return POWERCHAR_COLOR[(int)c];
}

Spell GetSpell(const char *spell)
{
    const int SpellN = sizeof(SPELL_CATALOG)/sizeof(SPELL_CATALOG[0]);
    for (int i = 0; i < SpellN; i++)
    {
        if (strcmp(spell, SPELL_CATALOG[i].name) == 0)
            return SPELL_CATALOG[i];
    }
    return (Spell){SPELLTYPE_NONE, NULL, (Color){0, 0, 0, 0}};
}


