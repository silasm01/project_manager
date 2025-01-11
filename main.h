#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <string.h>
#include "linked.h"

typedef struct INDEXER {
    char* name;
    char* path;
    FILETIME lastModified;
} INDEXER;

typedef struct Command {
    int index;
    LinkedList* args;
} Command;

LinkedList* get_subdirs(char* root);
void printData(void* data, int index);
int compareLastModified(void* a, void* b);
Command* parseInput(char* input);

#endif