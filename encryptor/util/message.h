#pragma once

#include <stddef.h>
#include <inttypes.h>
#include <errno.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef errno_t
#define errno_t int
#endif

typedef struct Entry Entry;

typedef struct Entry {
    char* data;
    // -1
    long int length;
} Entry;

typedef struct MessageController MessageController;

typedef struct Message Message;

typedef struct Message {
    unsigned int messageType;
    const MessageController* controller;
    unsigned int messageLength;
    char* message;
    unsigned int keyLength;
    char* key;
    size_t entryCount;
    char** entries;
} Message;

typedef struct MessageController {
    // unsigned long flags;

    int (*encode)(Message* message);
    int (*decode)(Message* message);
    // int (*open)(Message* message);
    // int (*create)(Message* message);
    // int (*close)(Message* message);

    // long int(*read)(Message* message, int entry, char* output);
    // long int(*write)(Message* message, int entry, char* input, size_t length);
} MessageController;

Message* createMessage(unsigned int messageType);

void deleteMessage(Message* message);

#ifdef __cplusplus
}
#endif