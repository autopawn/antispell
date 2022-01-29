#include "spell_catalog.h"

static Color POWERCHAR_COLOR[256] = {
    ['I'] = ORANGE,
    ['C'] = BLUE,
    ['L'] = PURPLE,
    ['R'] = BROWN,
    ['E'] = GREEN,
    ['F'] = ORANGE,
    ['O'] = GRAY,
    ['S'] = PINK,
    ['$'] = YELLOW,
    ['0'] = (Color){ 55,  52,  53, 255},
    // Fairydust palette
    ['1'] = (Color){240, 218, 177, 255},
    ['2'] = (Color){227, 154, 172, 255},
    ['3'] = (Color){196,  93, 159, 255},
    ['4'] = (Color){ 99,  75, 125, 255},
    ['5'] = (Color){100,  97, 194, 255},
    ['6'] = (Color){ 43, 169, 180, 255},
    ['7'] = (Color){147, 212, 181, 255},
    ['8'] = (Color){240, 246, 232, 255},
    // --
    ['9'] = (Color){237,  50,  55, 255},
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
