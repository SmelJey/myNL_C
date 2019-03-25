#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

char* getStr(FILE *f){
    char* resultStr = malloc(256 * sizeof(char));
    int c;
    int i = 0;

    while ((c = fgetc(f)) != '\n' && c != EOF){
        resultStr[i++] = (char)c;
        if (!(i % 256)){
            resultStr = realloc(resultStr, i + 256);
        }
    }

    if (c == EOF && resultStr[0] == 0)
        return NULL;
    resultStr[i] == '\0';

    return resultStr;
}

bool isEmptyStr(char* checkStr){
    for(int i = 0; checkStr[i] != 0; i++){
        if (checkStr[i] != ' ' || checkStr[i] != '\t')
            return false;
    }

    return true;
}

//=============================================================================

int main(int argc, char* argv[]) {
    char currentDir[256];
    char optStr[] = "pb:f:h:i:n:s:v:d:";
    char key;

    int increment = 1;

    bool numSpaceLines = false;
    bool numLines = true;

    bool numSpaceLinesH = false;
    bool numLinesH = false;

    bool numSpaceLinesF = false;
    bool numLinesF = false;

    bool doRenum = true;

    char* numFormat = "\t%d%s%s\n";
    char* delimiter = "  ";

    char* separator = "\\:";

    int startIndx = 1;

    char mode = 1; // 0 - header, 1 - body, 2 - footer
    opterr = 0;

    while ((key = getopt(argc, argv, optStr)) != -1){
        switch(key){
            case 'b':
                if (!strcmp(optarg, "a")){
                    numSpaceLines = true;
                } else if (!strcmp(optarg, "n")){
                    numLines = false;
                } else if (strcmp(optarg, "t")) {
                    printf("nl: Неверный стиль нумерования тела: %s\n", optarg);
                    return 1;
                }
                break;

            case 'f':
                if (!strcmp(optarg, "a")){
                    numSpaceLinesF = true;
                    numLinesF = true;
                } else if (!strcmp(optarg, "t")){
                    numLinesF = true;
                } else if (strcmp(optarg, "n")) {
                    printf("nl: Неверный стиль нумерования хвоста: %s\n", optarg);
                    return 1;
                }
                break;

            case 'h':
                if (!strcmp(optarg, "a")){
                    numSpaceLinesH = true;
                    numLinesH = true;
                } else if (!strcmp(optarg, "t")){
                    numLinesH = true;
                } else if (strcmp(optarg, "n")) {
                    printf("nl: Неверный стиль нумерования заголовка: %s\n", optarg);
                    return 1;
                }
                break;

            case 'i':
                increment = atoi(optarg);
                if (increment <= 0){
                    printf("nl: Числовой результата вне допустимого диапазона: %d\n", increment);
                    return 1;
                }
                break;

            case 'n':
                if (!strcmp(optarg, "ln")){
                    numFormat = "%d\t%s%s\n";;
                } else if (!strcmp(optarg, "rz")){
                    numFormat = "%09d%s%s\n";;
                } else if (strcmp(optarg, "rn")){
                    printf("nl: Неверный стиль форматирования: %s\n", optarg);
                    return 1;
                }
                break;

            case 's':
                delimiter = optarg;
                break;

            case 'v':
                startIndx = atoi(optarg);
                if (startIndx == 0 && optarg[0] != '0'){
                    printf("nl: Неверный номер начальной строки: %d\n", optarg);
                    return 1;
                }
                break;

            case 'p':
                doRenum = false;
                break;

            case 'd':
                separator = optarg;
                break;

            default:
                printf("nl: Некорректная опция\n");
                return 1;
        }
    }

    char separator2[128];
    strcpy(separator2, separator);
    strcat(separator2, separator2);
    char separator3[128];
    strcpy(separator3, separator);
    strcat(separator3, separator2);

    int i = startIndx;

    for (int j = optind; j < argc; j++){
        strcat(getcwd(currentDir, 256), argv[j]);
        FILE *f = fopen(argv[j], "r");
        if (f == NULL){
            printf("nl: Указанный файл не найден\n");
            return 2;
        }

        char* readStr;
        while((readStr = getStr(f)) != NULL){
            if (!strcmp(readStr, separator)){
                printf("\n");
                mode = 2;
                if (doRenum)
                    i = startIndx;
                continue;
            }
            if (!strcmp(readStr, separator2)){
                printf("\n");
                mode = 1;
                if (doRenum)
                    i = startIndx;
                continue;
            }
            if (!strcmp(readStr, separator3)){
                printf("\n");
                mode = 0;
                if (doRenum)
                    i = startIndx;
                continue;
            }

            switch (mode){
                case 1:
                    if (numLines && !(isEmptyStr(readStr) && !numSpaceLines)) {
                        printf(numFormat, i, delimiter, readStr);
                        i += increment;
                    }
                    else
                        printf("\t%s\n", readStr);
                    break;

                case 0:
                    if (numLinesH && !(isEmptyStr(readStr) && !numSpaceLinesH)) {
                        printf(numFormat, i, delimiter, readStr);
                        i += increment;
                    }
                    else
                        printf("\t%s\n", readStr);
                    break;

                case 2:
                    if (numLinesF && !(isEmptyStr(readStr) && !numSpaceLinesF)) {
                        printf(numFormat, i, delimiter, readStr);
                        i += increment;
                    }
                    else
                        printf("\t%s\n", readStr);
                    break;
            }
        }
    }
    return 0;
}