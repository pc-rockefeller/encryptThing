#include "util/util.h"
#include "util/crypt_type.h"
#include "util/crypt.h"
#include "util/message.h"
#include "util/detect.h"

#include <malloc.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ENCRYPTED_ELEMENTS_COUNT 3
#define ERROR_SIZE 64
#define ENCRYPTED_SIZE 2048
#define DECRYPTED_SIZE 2048
#define KEY_SIZE 128

void init() {
    initRand();
}

int encrypt(Entry textEntry, char* outputBuffer, const long int outputBufferSize) {
    // [0:64] err [64:128] key [66:2048] [2048] \0
    // 
    printf("bufZise = %ld\n", outputBufferSize); // TODO: REMOVE
    printf("text = %s\n", textEntry.data);

    char error[ERROR_SIZE] = {};
    char encrypted[ENCRYPTED_SIZE] = {};
    char key[KEY_SIZE] = {};
    // char entries[20][256] = {};

    int type = randLength(1, MAX_CRYPT_TYPE + 1);

    Message* message = createMessage(type);
    message->messageLength = 0;
    message->message = encrypted;
    message->keyLength = 0;
    message->key = key;

    int precursorCount = randLength(3, 8);
    int postcursorCount = randLength(3, 8);

    int entryCount = precursorCount + 1 + postcursorCount;
    char** entries = calloc(entryCount, sizeof(char*));

    char **precursor; // TODO:

    int i;
    for (i = 0; i < precursorCount; i++) {
        int length = randLength(3, 8);
        entries[i] = malloc(length);
        generateString(entries[i], length);
        entries[i][0] = entries[i][1];
        length = compressString(entries[i], length);
    }

    entries[i] = malloc(textEntry.length + 1);
    memcpy(entries[i++], textEntry.data, textEntry.length);

    // char postcursor[8][8] = {};
    for (int j = i; j < postcursorCount + i; j++) {
        int length = randLength(3, 8);
        entries[j] = malloc(length);
        generateString(entries[j], length);
        entries[j][0] = entries[j][1];
        length = compressString(entries[j], length);
        // message->entries[j] = precursor[j];
    }

    message->entryCount = entryCount;
    message->entries = entries;

    message->controller->encode(message);

    RecollectElem elements[] = {
        { error, strlen(error) },
        { message->message, message->messageLength },
        { message->key, message->keyLength }
    };
    
    if (recollect(outputBuffer, outputBufferSize, elements, 3) != 0) {
        printf("Encrypting error\n");
        deleteMessage(message);
        for (i = 0; i < entryCount; i++)
            free(entries[i]);
        free(entries);
        return -1;
    }

    deleteMessage(message);
    for (i = 0; i < entryCount; i++)
        free(entries[i]);
    free(entries);

    return 0;
};

int decrypt(Entry encryptedTextEntry, Entry keyEntry, char* outputBuffer, const long int outputBufferSize) {
    // [0:64] err [64:128] key [66:2048] [2048] \0
    // 
    char error[ERROR_SIZE] = {};
    char decrypted[DECRYPTED_SIZE] = {};

    int type = detectType(encryptedTextEntry.data);

    Message* message = createMessage(type);

    char** entries = calloc(1, sizeof(char*));
    entries[0] = malloc(encryptedTextEntry.length + 1);
    memcpy(entries[0], encryptedTextEntry.data, encryptedTextEntry.length);

    char* messageKey = malloc(keyEntry.length + 1);
    memcpy(messageKey, keyEntry.data, keyEntry.length);

    message->entries = entries;
    message->key = messageKey;

    if (message->controller->decode(message) != 0) {
        strcpy(error, "Wrong key");
    }

    RecollectElem elements[] = {
        { error, strlen(error) },
        { message->message, message->messageLength }
    };

    printf("in decr outsizebuf = %ld\n", outputBufferSize);
    
    int result;
    #pragma omp critical
    {
    result = recollect(outputBuffer, outputBufferSize, &elements, 2); 
    }

    if (result != 0) {
        // deleteMessage(message);
        // free(entries[0]);
        // free(entries);
        // free(messageKey);
        return -1;
    }

    // deleteMessage(message);
    // free(entries[0]);
    // free(entries);
    // free(messageKey);

    return 0;
};


int testFlask(Entry textEntry) {
    printf("Flask call, text = %s\n", textEntry.data);
}

#ifdef __cplusplus
}
#endif
