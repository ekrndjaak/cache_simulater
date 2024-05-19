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

    // 모든 캐시 라인의 LRU 값을 증가시킵니다.
    for (temp = 0; temp < CACHESIZE; temp++) {
        LRU[temp]++;
    }

    // 캐시에 주소가 있는지 확인합니다.
    for (temp = 0; temp < CACHESIZE; temp++) {
        if (strcmp(CACHE[temp], ad) == 0) {    // 캐시에 존재할 경우
            printf("hit %d %s\n", oper, CACHE[temp]);
            LRU[temp] = 1;  // lru에 방금 접근했다고 알림
            exFlag = 1;     // 존재 flag 1로 변경
            break;
        }
    }

    if (exFlag == 0) {  // no exist > miss > 캐시에 적재해야함
        printf("miss %d %s\n", oper, ad);

        // 빈 캐시 라인을 찾습니다.
        for (temp = 0; temp < CACHESIZE; temp++) {
            if (ex[temp] == 0) {    // 빈 캐시 발견
                cacheFlag = 1;      // 캐시 flag 1로 변경
                strncpy(CACHE[temp], ad, 8);  // 캐시에 주소 적재
                ex[temp] = 1;
                LRU[temp] = 1;  // lru 초기화
                printf("%s,%d\n", CACHE[temp], temp);
                break;
            }
        }

        if (cacheFlag == 0) {  // 빈 캐시가 없을 경우 > LRU로 캐시에 주소 적재
            int index = 0;
            int maxLRU = LRU[0];

            // LRU 수치가 높은 캐시 라인을 찾습니다.
            for (temp = 1; temp < CACHESIZE; temp++) {
                if (LRU[temp] > maxLRU) {
                    index = temp;
                    maxLRU = LRU[temp];
                }
            }

            // LRU 수치가 높은 캐시에 주소 적재
            strncpy(CACHE[index], ad, 8);
            LRU[index] = 1;  // lru 초기화
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