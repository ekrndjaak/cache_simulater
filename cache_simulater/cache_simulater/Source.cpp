#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define _CRT_SECURE_NO_WARNINGS
#define MEMSIZE 128
#define BLOCKSIZE 4
#define LINESIZE 4
#define MEMBLSIZE 32
#define CACHESIZE 32
#define CACHELINE 8


char CACHE[CACHESIZE][8];
int LRU[CACHESIZE] = { 0 };
int ex[CACHESIZE] = { 0 };

int totalLoads = 0;
int totalStores = 0;
int loadHits = 0;
int loadMisses = 0;
int storeHits = 0;
int storeMisses = 0;
int totalCycles = 0;
void cacheS(int oper, char* ad, int a) {
    printf("%d,%s\n", oper, ad);
    int exFlag = 0, temp = 0, cacheFlag = 0;

    // ��� ĳ�� ������ LRU ���� ������ŵ�ϴ�.
    for (temp = 0; temp < CACHESIZE; temp++) {
        LRU[temp]++;
    }

    // ĳ�ÿ� �ּҰ� �ִ��� Ȯ���մϴ�.
    for (temp = 0; temp < CACHESIZE; temp++) {
        if (strcmp(CACHE[temp], ad) == 0) {    // ĳ�ÿ� ������ ���
            printf("hit %d %s\n", oper, CACHE[temp]);
            LRU[temp] = 1;  // lru�� ��� �����ߴٰ� �˸�
            exFlag = 1;     // ���� flag 1�� ����
            break;
        }
    }

    if (exFlag == 0) {  // no exist > miss > ĳ�ÿ� �����ؾ���
        printf("miss %d %s\n", oper, ad);

        // �� ĳ�� ������ ã���ϴ�.
        for (temp = 0; temp < CACHESIZE; temp++) {
            if (ex[temp] == 0) {    // �� ĳ�� �߰�
                cacheFlag = 1;      // ĳ�� flag 1�� ����
                strncpy(CACHE[temp], ad, 8);  // ĳ�ÿ� �ּ� ����
                ex[temp] = 1;
                LRU[temp] = 1;  // lru �ʱ�ȭ
                printf("%s,%d\n", CACHE[temp], temp);
                break;
            }
        }

        if (cacheFlag == 0) {  // �� ĳ�ð� ���� ��� > LRU�� ĳ�ÿ� �ּ� ����
            int index = 0;
            int maxLRU = LRU[0];

            // LRU ��ġ�� ���� ĳ�� ������ ã���ϴ�.
            for (temp = 1; temp < CACHESIZE; temp++) {
                if (LRU[temp] > maxLRU) {
                    index = temp;
                    maxLRU = LRU[temp];
                }
            }

            // LRU ��ġ�� ���� ĳ�ÿ� �ּ� ����
            strncpy(CACHE[index], ad, 8);
            LRU[index] = 1;  // lru �ʱ�ȭ
            printf("%d,%d\n", index, maxLRU);
        }

        if (oper == 108) {  // oper == load
            loadMisses++;
        }
        else if (oper == 115) {  // oper == store
            storeMisses++;
        }
    }
    else {  // exist > hit
        if (oper == 108) {  // oper == load
            loadHits++;
        }
        else if (oper == 115) {  // oper == store
            storeHits++;
        }
    }

    totalCycles++;
}

char* split(char* str, int start, int length) {
    int strLength = strlen(str);
    if (start >= strLength || length <= 0) {
        return NULL;
    }
    int end = start + length;
    if (end > strLength) {
        end = strLength;
    }
    char* result = (char*)malloc(length + 1);
    strncpy(result, str + start, length);
    result[length] = '\0';
    return result;
}
int main() {
    int temp = 0;
    FILE* traceFile = fopen("read01.trace", "r");
    if (traceFile == NULL) {
        printf("Error: Failed to open trace file.\n");
        return 0;
    }

    char buffer[50];
    int oper;
    while (!feof(traceFile)) {
        oper = fgetc(traceFile);
        fgets(buffer, sizeof(buffer), traceFile);
        strtok(buffer, "\n");
        strtok(buffer, " ");
        char* cutStr = split(buffer, 3, strlen(buffer) - 3);
        if (cutStr != NULL) {
            if (oper == 108) {  // oper == load
                totalLoads++;
            }
            else if (oper == 115) {  // oper == store
                totalStores++;
            }
            cacheS(oper, cutStr, temp);
            free(cutStr);
        }
        temp++;
        printf("                                            count : %d\n", totalCycles);
    }

    printf("Total loads: %d\n", totalLoads);
    printf("Total stores: %d\n", totalStores);
    printf("Load hits: %d\n", loadHits);
    printf("Load misses: %d\n", loadMisses);
    printf("Store hits: %d\n", storeHits);
    printf("Store misses: %d\n", storeMisses);
    printf("Total cycles: %d\n", totalCycles);
    fclose(traceFile);

    return 0;
}