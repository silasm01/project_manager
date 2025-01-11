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
    int index;
    scanf("%d", &index);

    INDEXER* indexer = (INDEXER*)getValue(subdirs, index);

    if (indexer == NULL) {
        printf("Invalid index.\n");
        return 1;
    }

    printf("Opening project: %s\n", indexer->name);

    // combine the path with the project name
    char* command = (char*)malloc(strlen("code ") + strlen(indexer->path) + 1);
    strcpy(command, "code ");
    strcat(command, indexer->path);

    // Open the project
    system(command);

    // Free the list
    freeList(subdirs);

    return 0;
}
