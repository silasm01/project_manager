#include "main.h"

void printData(void* data, int index) {
    INDEXER* indexer = (INDEXER*)data;
    SYSTEMTIME systemTime;
    FileTimeToSystemTime(&indexer->lastModified, &systemTime);
    printf("%d: %s (Last Modified: %02d/%02d/%d %02d:%02d:%02d)\n", index, indexer->name, 
        systemTime.wDay, systemTime.wMonth, systemTime.wYear, 
        systemTime.wHour, systemTime.wMinute, systemTime.wSecond);
}

LinkedList* get_subdirs(char* root) {
    LinkedList* subdirs = createList(free);

    if (subdirs == NULL) {
        return NULL;
    }

    WIN32_FIND_DATA findData;
    HANDLE hFind = FindFirstFile(root, &findData);

    if (hFind == INVALID_HANDLE_VALUE) {
        return NULL;
    }

    char* root_copy = strdup(root);
    root_copy[strlen(root_copy) - 1] = '\0';
    for (int i = 0; i < strlen(root_copy); i++) {
        if (root_copy[i] == '\\') {
            root_copy = (char*)realloc(root_copy, strlen(root_copy) + 2);
            memmove(root_copy + i + 1, root_copy + i, strlen(root_copy) - i + 1);
            root_copy[i] = '\\';
            i++;
        }
    }


    do {
        if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            if (strcmp(findData.cFileName, ".") == 0 || strcmp(findData.cFileName, "..") == 0) {
                continue;
            }

            char* subdir = strdup(findData.cFileName);
            char* path = strdup(root_copy);

            // Concatenate subdirectory name to the path
            path = (char*)realloc(path, strlen(path) + strlen(subdir) + 2);
            strcat(path, subdir);

            INDEXER* indexer = (INDEXER*)malloc(sizeof(INDEXER));
            indexer->name = subdir;
            indexer->path = path;
            indexer->lastModified = findData.ftLastWriteTime;

            push(subdirs, indexer);
        }
    } while (FindNextFile(hFind, &findData) != 0);

    FindClose(hFind);

    return subdirs;
}

int compareLastModified(void* a, void* b) {
    INDEXER* indexerA = (INDEXER*)a;
    INDEXER* indexerB = (INDEXER*)b;

    return CompareFileTime(&indexerB->lastModified, &indexerA->lastModified);
}

Command* parseInput(char* input) {
    // Tokenize the input
    char* token = strtok(input, " ");
    
    // Check if the first token is a number or character
    if (token == NULL) {
        return NULL;
    }

    Command* command = (Command*)malloc(sizeof(Command));
    command->index = atoi(token);

    LinkedList* args = createList(free);

    while (token != NULL) {
        token = strtok(NULL, " ");
        if (token != NULL) {
            char* arg = strdup(token);
            push(args, arg);
        }
    }

    command->args = args;

    return command;
}

int main() {
    char* root = "C:\\Users\\silas\\code\\*";

    LinkedList* subdirs = get_subdirs(root);

    if (subdirs == NULL) {
        printf("No subdirectories found.\n");
        return 1;
    }

    // Sort by last modified
    sortLinkedList(subdirs, compareLastModified);

    printf("Enter the index of the project to open: \n");

    // Print the sorted list
    printList(subdirs, printData);

    // Take input and open the project
    char* command = (char*)malloc(100);
    fgets(command, 100, stdin);

    Command* parsed = parseInput(command);

    INDEXER* indexer = (INDEXER*)getValue(subdirs, parsed->index);

    printf("Opening project: %s\n", indexer->name);

    if (parsed->args->size == 0) {
        char* command = (char*)malloc(strlen(indexer->path) + 1);
        strcpy(command, "code ");
        strcat(command, indexer->path);
        system(command);
    }

    while (parsed->args->size > 0) {
        char* arg = (char*)pop(parsed->args);

        char* command = (char*)malloc(strlen(indexer->path) + strlen(arg) + 1);

        strcpy(command, arg);
        
        if (command[strlen(command) - 1] == '\n') {
            command[strlen(command) - 1] = ' ';
        } else {
            strcat(command, " ");
        }

        if (strcmp(command, "cmd ") == 0) {
            char* command = (char*)malloc(strlen(indexer->path) + 100);
            strcpy(command, "cmd.exe /c start \"\" cmd.exe /K \"cd /d ");
            strcat(command, indexer->path);
            strcat(command, "\"");

            system(command);
            continue;
        }

        strcat(command, indexer->path);
        system(command);
    }

    freeList(subdirs);
    freeList(parsed->args);
    free(parsed);

    return 0;
}
