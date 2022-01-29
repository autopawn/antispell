#include "spell_catalog.h"

static Color POWERCHAR_COLOR[256] = {
    ['I'] = ORANGE,
    ['C'] = BLUE,
    ['R'] = BROWN,
    ['E'] = GREEN,
    ['F'] = ORANGE,
    ['$'] = YELLOW,
};

static Spell SPELL_CATALOG[] = {
    {SPELLTYPE_NONE,  "NONE",  (Color){  0,   0,   0, 255}, (Color){  0,   0,   0, 255}},
    {SPELLTYPE_ICE,   "ICE",   (Color){102, 191, 255, 255}, (Color){122, 211, 255, 255}},
    {SPELLTYPE_FREE,  "FREE",  (Color){172, 166, 224, 255}, (Color){229,  25,  35, 255}},
    {SPELLTYPE_FIRE,  "FIRE",  (Color){242,  29,  29, 255}, (Color){242, 224,  29, 255}},
    {SPELLTYPE_IRE,   "IRE",   (Color){239,  12,  12, 255}, (Color){239,  12,  12, 255}},
    {SPELLTYPE_FEE,   "FEE",   (Color){252, 240, 106, 255}, (Color){252, 240, 106, 255}},
    {SPELLTYPE_REIF,  "REIF",  (Color){252, 240, 106, 255}, (Color){252, 240, 106, 255}},
    {SPELLTYPE_REFER, "REFER", (Color){130, 130, 130, 255}, (Color){214, 216, 216, 255}},
    {SPELLTYPE_RICE,  "RICE",  (Color){221, 220, 210, 255}, (Color){181, 180, 179, 255}},
    {SPELLTYPE_REEF,  "REEF",  (Color){  0, 117,  44, 255}, (Color){  0, 217, 144, 255}},
    {SPELLTYPE_SEE,   "SEE",   (Color){  0, 255, 255, 255}, (Color){255, 200, 200, 255}},
    {SPELLTYPE_LESS,  "LESS",  DARKGRAY, DARKBLUE},
    {SPELLTYPE_SELL,  "SELL",  GREEN, YELLOW},
    {SPELLTYPE_CELL,  "CELL",  MAGENTA, BLUE},
    {SPELLTYPE_ELSE,  "ELSE",  PINK, PURPLE},
    {SPELLTYPE_LOSS,  "LOSS",  YELLOW, LIGHTGRAY},
    {SPELLTYPE_COOL,  "COOL",  SKYBLUE, WHITE},
    {SPELLTYPE_LOSE,  "LOSE",  PURPLE, DARKBROWN},
    {SPELLTYPE_LOL,   "LOL",   YELLOW, ORANGE},
    {SPELLTYPE_CEO,   "CEO",   BLACK, YELLOW},
    {SPELLTYPE_SOLO,  "SOLO",  LIGHTGRAY, DARKBLUE},
    {SPELLTYPE_LOOSE,  "LOOSE", LIME, DARKPURPLE},
    {SPELLTYPE_LOCO,  "LOCO",  DARKBROWN, VIOLET},
    {SPELLTYPE_EEL,   "EEL",   YELLOW, BLACK},
    {SPELLTYPE_CLOSE, "CLOSE", BROWN, BLACK},
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
        {
            SPELL_CATALOG[i].used = 1; // Mark as used for the credits
            return SPELL_CATALOG[i];
        }
    }
    return (Spell){SPELLTYPE_NONE, NULL, (Color){0, 0, 0, 0}};
}

Spell GetSpellFromType(SpellType type)
{
    const int SpellN = sizeof(SPELL_CATALOG)/sizeof(SPELL_CATALOG[0]);
    for (int i = 0; i < SpellN; i++)
    {
        if (SPELL_CATALOG[i].type == type)
            return SPELL_CATALOG[i];
    }
    return SPELL_CATALOG[0];
}
