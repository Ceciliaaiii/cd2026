
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ========== 資料結構定義 ========== */

/**
 * Linked List 節點
 * ch    : 字元
 * next  : 指向下一個節點
 */
typedef struct Node {
    unsigned char ch;
    struct Node *next;
} Node;

/* ========== 全域變數 ========== */

Node *head = NULL;  // linked list 的頭節點
Node *tail = NULL;  // 尾節點（方便新增）

/* ========== 函式宣告 ========== */

void add_char(unsigned char c);
void print_scanner_output();
void free_memory();
int is_ascii(unsigned char c);

/* ========== 輔助函式 ========== */

/**
 * 檢查是否為可顯示的 ASCII 字元
 * 保留所有字元，但用來判斷顯示格式
 */
int is_ascii(unsigned char c) {
    // 保留換行、縮排、空格
    if (c == '\n' || c == '\t' || c == ' ') {
        return 1;
    }
    // 可顯示的 ASCII (32-126)
    if (c >= 32 && c <= 126) {
        return 1;
    }
    return 0;
}

/* ========== Linked List 操作 ========== */

/**
 * 新增字元到 linked list 末端
 * 保持輸入順序
 */
void add_char(unsigned char c) {
    // 建立新節點
    Node *new_node = (Node *)malloc(sizeof(Node));
    if (new_node == NULL) {
        fprintf(stderr, "記憶體分配失敗！\n");
        exit(1);
    }
    
    new_node->ch = c;
    new_node->next = NULL;
    
    // 如果是第一個節點
    if (head == NULL) {
        head = new_node;
        tail = new_node;
    } else {
        tail->next = new_node;
        tail = new_node;
    }
}

/* ========== 輸出函式 ========== */

/**
 * 取得字元的顯示格式（用單引號包起來）
 */
void print_char_format(unsigned char c) {
    switch (c) {
        case '\n':
            printf("'\\n' ");
            break;
        case '\t':
            printf("'\\t' ");
            break;
        case ' ':
            printf("' ' ");
            break;
        case '\\':
            printf("'\\\\' ");
            break;
        case '\'':
            printf("'\\'' ");
            break;
        default:
            if (c >= 32 && c <= 126) {
                printf("'%c' ", c);
            } else {
                // 非 ASCII 用十六進位顯示
                printf("'\\x%02X' ", c);
            }
            break;
    }
}

/**
 * 印出 scanner 輸出
 * 格式：每個字元用單引號包起來，用 → 連接
 */
void print_scanner_output() {
    Node *current = head;
    int count = 0;
    
    printf("\nScanner Output:\n");
    printf("========================================\n");
    
    // 第一種格式：用 → 連接
    while (current != NULL) {
        print_char_format(current->ch);
        
        // 如果不是最後一個，印出 →
        if (current->next != NULL) {
            printf("→ ");
        }
        
        current = current->next;
        
        // 每行印 5 個就換行（讓輸出好看）
        count++;
        if (count % 5 == 0) {
            printf("\n");
        }
    }
    
    printf("\n========================================\n");
    
    // 第二種格式：依序列出（簡潔版）
    printf("\nSequential Output:\n");
    printf("========================================\n");
    current = head;
    while (current != NULL) {
        print_char_format(current->ch);
        current = current->next;
    }
    printf("\n========================================\n");
}

/**
 * 釋放 linked list 記憶體
 */
void free_memory() {
    Node *current = head;
    while (current != NULL) {
        Node *temp = current;
        current = current->next;
        free(temp);
    }
    head = NULL;
    tail = NULL;
}

/* ========== 主程式 ========== */

int main() {
    printf("\n");
    printf("╔════════════════════════════════════╗\n");
    printf("║     Lab1 - Scanner Implementation  ║\n");
    printf("║     讀取自己 + Linked List         ║\n");
    printf("╚════════════════════════════════════╝\n");
    
    // 讀取自己的原始碼
    FILE *fp = fopen(__FILE__, "r");
    if (fp == NULL) {
        fprintf(stderr, "錯誤：無法開啟檔案 %s\n", __FILE__);
        return 1;
    }
    
    // 逐字元讀取並加入 linked list
    int ch;
    while ((ch = fgetc(fp)) != EOF) {
        add_char((unsigned char)ch);
    }
    fclose(fp);
    
    // 輸出 scanner 結果
    print_scanner_output();
    
    // 釋放記憶體
    free_memory();
    
    printf("\n程式執行完畢！\n");
    
    return 0;
}
