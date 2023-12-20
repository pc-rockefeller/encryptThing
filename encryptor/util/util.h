#pragma once

#ifdef __cplusplus
extern "C" {
#endif

static const char MIN_CHAR = 32;
static const char MAX_CHAR = 127;
static const char DIF_CHAR = 94; // 127 - 32 - 1 

void initRand();

int randLength(const int min, const int max);

typedef struct RecollectElem RecollectElem;

typedef struct RecollectElem {
    char* buffer;
    const long int length;
} RecollectElem;

char generateChar();

int generateString(char* buffer, const int length);

int compressString(char* buffer, const int length);

int recollect(char* buffer, const long int length, RecollectElem* elements, const long int elementsCount);

#ifdef __cplusplus
}
#endif