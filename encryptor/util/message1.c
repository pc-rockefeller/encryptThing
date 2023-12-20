#include "message.h"
#include "util.h"
#include "detect.h"

#include <string.h>

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

static unsigned char ec1(unsigned char c, unsigned char k) {
    return c ^ k;
}

typedef struct {
    unsigned char key;
    unsigned char step;
} crypt_params;

static const crypt_params
mc1_crypt_params[] = {
    {  0x1b, 0x37 },
    {  0x51, 0xe9 },
    {  0xc1, 0x51 },
    {  0x03, 0x19 },
    {  0xab, 0xcd },
    {  0x12, 0x34 },
    {  0x35, 0x97 },
    {  0x99, 0x37 },
};

static int mc1_encrypt(Message* message) {
    unsigned char k = randLength(MIN_CHAR, MAX_CHAR);
    unsigned char s = randLength(1, MIN_CHAR);
    unsigned char kk = randLength(MIN_CHAR, MAX_CHAR);
    unsigned char ks = randLength(1, MIN_CHAR);

    message->message[message->messageLength++] = k;
    message->message[message->messageLength++] = s;
    message->key[message->keyLength++] = kk;
    message->key[message->keyLength++] = ks;
    kk *= k;
    ks *= s;
    k -= s;
    kk -= ks;
    message->message[4] = ec1(76, k + s * 3); // 'L'
    message->key[4] = ec1(76, kk + ks * 3);
    message->message[message->messageLength++] = ec1(77, k += s); // 'M'
    message->key[message->keyLength++] = ec1(77, kk += ks);
    message->message[message->messageLength++] = ec1(78, k += s); k += s; message->messageLength++; // 'N'
    message->key[message->keyLength++] = ec1(78, kk += ks); kk += ks; message->keyLength++;
    message->message[message->messageLength++] = ec1('1', k += s);
    message->key[message->keyLength++] = ec1('1', kk += ks); 

    for (int i = 0; i < message->entryCount; i++) {
        int entryLength = strlen(message->entries[i]);
        message->message[message->messageLength++] = (char)entryLength;
        unsigned char key = mc1_crypt_params[i%8].key;
        unsigned char step = mc1_crypt_params[i%8].step;
        if (message->entries[i][0] >= 32) {
            message->key[message->keyLength++] = (char)i; // special
        }
        for (int j = 0; j < entryLength; j++) {
            message->message[message->messageLength++] = message->entries[i][j] ^ key;
            key += step;
        }
    }

    int addKeyLength = randLength(3, 8);
    generateString(message->key + message->keyLength, addKeyLength);
    message->keyLength += addKeyLength;    

    return 0;
}

static int mc1_decrypt(Message* message) {
    // entries[0] is encrypted message, entriesCount = length of entries[0]
    printf("decripting shijth\n"); //TODO: udalit

    // TODO add safety on Length

    char* ent = message->entries[0];
    char* key = message->key;
    int entInd = 0;
    int keyInd = 0;

    message->key[keyInd] = ent[entInd++] * message->key[keyInd]; keyInd++;
    message->key[keyInd] = ent[entInd++] * message->key[keyInd]; keyInd++;

    if (detectType(message->key) != 1)
        return -1;
    keyInd += 4;
    entInd += 4;
    
    int spec = message->key[keyInd++];

    for (int i = 0; i < spec; i++) {

    }

    int encr = 0;
    while (encr != spec) {
        entInd += spec;
    }
    
    message->messageLength = 0;
    unsigned char key = mc1_crypt_params[spec%8].key;
    unsigned char step = mc1_crypt_params[spec%8].step;
    spec = ent[entInd++];
    for (int j = 0; j < spec; j++) {
        message->message[message->messageLength++] = ent[entInd++] ^ key;
        key += step;
    }

    return 0;
}


const MessageController messageController1 = {
    mc1_encrypt,
    mc1_decrypt
};

#ifdef __cplusplus
}
#endif
