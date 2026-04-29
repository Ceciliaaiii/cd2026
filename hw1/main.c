#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ========== 輔助函式：從 hw0 的 is_ascii 演變而來 ========== */

int is_alpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c == '_');
}

int is_digit(char c) {
    return (c >= '0' && c <= '9');
}

int is_space(char c) {
    return (c == ' ' || c == '\t' || (int)c == 10 || (int)c == 13);
}

/* ========== 核心辨識邏輯 ========== */

void check_token(char *str) {
    // 檢查是否為關鍵字，否則就是 ID
    if (strcmp(str, "int") == 0)      printf("int: TYPE_TOKEN\n");
    else if (strcmp(str, "main") == 0)  printf("main: MAIN_TOKEN\n");
    else if (strcmp(str, "if") == 0)    printf("if: IF_TOKEN\n");
    else if (strcmp(str, "else") == 0)  printf("else: ELSE_TOKEN\n");
    else if (strcmp(str, "while") == 0) printf("while: WHILE_TOKEN\n");
    else                               printf("%s: ID_TOKEN\n", str);
}

/* ========== 執行 Scanner：由原本的 run_method1 修改而來 ========== */

void run_scanner(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        // 如果檔案打不開，嘗試讀取 stdin (為了方便測試)
        fp = stdin;
    }

    int c;
    while ((c = fgetc(fp)) != EOF) {
        // 1. 跳過空白 (基於原本 hw0 跳過非統計字元的邏輯)
        if (is_space(c)) continue;

        // 2. 處理 ID 或 關鍵字 (DFA 狀態：字母開頭)
        if (is_alpha(c)) {
            char buffer[256];
            int i = 0;
            buffer[i++] = c;
            while ((c = fgetc(fp)) != EOF && (is_alpha(c) || is_digit(c))) {
                buffer[i++] = c;
            }
            buffer[i] = '\0';
            ungetc(c, fp);
            check_token(buffer);
        }
        // 3. 處理數字 (DFA 狀態：數字開頭)
        else if (is_digit(c)) {
            char buffer[256];
            int i = 0;
            buffer[i++] = c;
            while ((c = fgetc(fp)) != EOF && is_digit(c)) {
                buffer[i++] = c;
            }
            buffer[i] = '\0';
            ungetc(c, fp);
            printf("%s: LITERAL_TOKEN\n", buffer);
        }
        // 4. 處理符號 (DFA 狀態：特殊符號)
        else {
            switch (c) {
                case '(': printf("(: LEFTPAREN_TOKEN\n"); break;
                case ')': printf("): REFTPAREN_TOKEN\n"); break;
                case '{': printf("{: LEFTBRACE_TOKEN\n"); break;
                case '}': printf("}: REFTBRACE_TOKEN\n"); break;
                case ';': printf(";: SEMICOLON_TOKEN\n"); break;
                case '+': printf("+: PLUS_TOKEN\n"); break;
                case '-': printf("-: MINUS_TOKEN\n"); break;
                case '=':
                    c = fgetc(fp);
                    if (c == '=') printf("==: EQUAL_TOKEN\n");
                    else { ungetc(c, fp); printf("=: ASSIGN_TOKEN\n"); }
                    break;
                case '>':
                    c = fgetc(fp);
                    if (c == '=') printf(">=: GREATEREQUAL_TOKEN\n");
                    else { ungetc(c, fp); printf(">: GREATER_TOKEN\n"); }
                    break;
                case '<':
                    c = fgetc(fp);
                    if (c == '=') printf("<=: LESSEQUAL_TOKEN\n");
                    else { ungetc(c, fp); printf("<: LESS_TOKEN\n"); }
                    break;
            }
        }
    }
    
    if (fp != stdin) fclose(fp);
}

/* ========== 主程式：保持 hw0 的簡潔結構 ========== */

int main(int argc, char *argv[]) {
    // 你可以選擇讀取自己 (跟 hw0 一樣) 或讀取測試檔
    // 這裡建議預設讀取標準輸入，這樣你才能貼上測試資料
    run_scanner("test.c"); 

    return 0;
}
