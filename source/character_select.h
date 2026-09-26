#ifndef PIXELTOWN_CHARACTER_SELECT_H
#define PIXELTOWN_CHARACTER_SELECT_H

typedef struct
{
    int gender;
    int hairColor;
    int hairLength;
    int clothes;
} CharacterConfig;

extern CharacterConfig characterConfig;

void characterSelectRun(void);

#endif
