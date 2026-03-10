#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ========== 資料結構定義 ========== */

/**
 * Linked List 節點
 * ch    : 字元（使用 unsigned char 確保 0-255 範圍）
 * count : 出現次數
 * next  : 指向下一個節點
 */
typedef struct Node {
    unsigned char ch;
    int count;
    struct Node *next;
} Node;

/* ========== 全域變數 ========== */

Node *head = NULL;              // linked list 的頭節點
Node *char_map[256] = {NULL};   // 加速查詢用的陣列 (加分題)

/* ========== 函式宣告 ========== */

void print_header(const char *title);
void print_statistics();
void free_memory();
void run_method1(const char *filename);
void run_method2(const char *filename);
int is_ascii(unsigned char c);

/* ========== 輔助函式 ========== */

/**
 * 檢查是否為可統計的 ASCII 字元
 * 只統計：可顯示字元(32-126) + 空格(32) + 換行(10) + 縮排(9)
 */
int is_ascii(unsigned char c) {
    // 保留換行、縮排、空格
    if (c == '\n' || c == '\t' || c == ' ') {
        return 1;
    }
    // 只統計可顯示的 ASCII (32-126)
    if (c >= 32 && c <= 126) {
        return 1;
    }
    // 其他都跳過（包括中文字、控制字元）
    return 0;
}

/* ========== 方法一：線性搜尋 O(n) ========== */

/**
 * 線性搜尋：遍歷整個 linked list 尋找指定字元
 * 時間複雜度：O(n)
 */
Node *find_char_linear(unsigned char c) {
    Node *current = head;
    while (current != NULL) {
        if (current->ch == c) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

/**
 * 新增或更新字元（線性搜尋版）
 */
void add_or_increment_linear(unsigned char c) {
    // 跳過非 ASCII 字元
    if (!is_ascii(c)) {
        return;
    }
    
    Node *found = find_char_linear(c);
    
    if (found != NULL) {
        // 字元已存在，次數加一
        found->count++;
    } else {
        // 字元不存在，建立新節點
        Node *new_node = (Node *)malloc(sizeof(Node));
        if (new_node == NULL) {
            fprintf(stderr, "記憶體分配失敗！\n");
            exit(1);
        }
        
        new_node->ch = c;
        new_node->count = 1;
        new_node->next = head;
        head = new_node;
    }
}

/* ========== 方法二：陣列加速 O(1) ========== */

/**
 * 新增或更新字元（陣列加速版）
 * 時間複雜度：O(1)
 */
void add_or_increment_fast(unsigned char c) {
    // 跳過非 ASCII 字元
    if (!is_ascii(c)) {
        return;
    }
    
    if (char_map[c] != NULL) {
        // 直接從陣列找到節點，次數加一
        char_map[c]->count++;
    } else {
        // 建立新節點
        Node *new_node = (Node *)malloc(sizeof(Node));
        if (new_node == NULL) {
            fprintf(stderr, "記憶體分配失敗！\n");
            exit(1);
        }
        
        new_node->ch = c;
        new_node->count = 1;
        new_node->next = head;
        head = new_node;
        
        // 在陣列中記錄這個節點的位置
        char_map[c] = new_node;
    }
}

/* ========== 共用工具函式 ========== */

/**
 * 印出分隔線和標題
 */
void print_header(const char *title) {
    printf("\n%s\n", title);
    printf("========================================\n");
}

/**
 * 取得字元的顯示格式
 * 處理特殊字元如 \n, \t, 空格等
 */
const char *format_char(unsigned char c) {
    static char buffer[10];
    
    switch (c) {
        case '\n': return "\\n";
        case '\t': return "\\t";
        case '\r': return "\\r";
        case ' ':  return "space";
        case '\\': return "\\\\";
        case '\'': return "\\'";
        case '\"': return "\\\"";
        default:
            buffer[0] = c;
            buffer[1] = '\0';
            return buffer;
    }
}

/**
 * 印出統計結果（排序後輸出）
 */
void print_statistics() {
    Node *current = head;
    int total_chars = 0;
    int unique_chars = 0;
    
    // 第一次遍歷：計算總字元數
    while (current != NULL) {
        total_chars += current->count;
        unique_chars++;
        current = current->next;
    }
    
    printf("\n統計結果：\n");
    printf("----------------------------------------\n");
    printf("總字元數：%d\n", total_chars);
    printf("不同字元數：%d\n", unique_chars);
    printf("----------------------------------------\n");
    
    // 因為沒有排序，直接印出
    current = head;
    int count = 0;
    while (current != NULL) {
        printf("%-6s : %3d  ", 
               format_char(current->ch), 
               current->count);
        
        count++;
        if (count % 5 == 0) {
            printf("\n");
        }
        
        current = current->next;
    }
    
    if (count % 5 != 0) {
        printf("\n");
    }
    printf("========================================\n");
}

/**
 * 釋放 linked list 的記憶體
 */
void free_memory() {
    Node *current = head;
    while (current != NULL) {
        Node *temp = current;
        current = current->next;
        free(temp);
    }
    head = NULL;
    
    // 清空加速陣列
    for (int i = 0; i < 256; i++) {
        char_map[i] = NULL;
    }
}

/**
 * 執行方法一：線性搜尋
 */
void run_method1(const char *filename) {
    print_header("方法一：線性搜尋 O(n)");
    
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        fprintf(stderr, "錯誤：無法開啟檔案 %s\n", filename);
        return;
    }
    
    head = NULL;
    int ch;
    while ((ch = fgetc(fp)) != EOF) {
        add_or_increment_linear((unsigned char)ch);
    }
    fclose(fp);
    
    print_statistics();
    free_memory();
}

/**
 * 執行方法二：陣列加速
 */
void run_method2(const char *filename) {
    print_header("方法二：陣列加速 O(1)  ★加分題★");
    
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        fprintf(stderr, "錯誤：無法開啟檔案 %s\n", filename);
        return;
    }
    
    head = NULL;
    int ch;
    while ((ch = fgetc(fp)) != EOF) {
        add_or_increment_fast((unsigned char)ch);
    }
    fclose(fp);
    
    print_statistics();
    free_memory();
}

/* ========== 主程式 ========== */

int main() {
    printf("\n");
    printf("╔════════════════════════════════════╗\n");
    printf("║     HW0 - 字元統計程式             ║\n");
    printf("║     讀取自己 + Linked List         ║\n");
    printf("╚════════════════════════════════════╝\n");
    
    // 執行兩種方法做比較
    run_method1(__FILE__);
    run_method2(__FILE__);
    
    printf("\n程式執行完畢！\n");
    
    return 0;
}

