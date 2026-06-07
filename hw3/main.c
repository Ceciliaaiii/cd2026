#include <stdio.h>
#include <string.h>

/* ========== 1. 定義 Token 型態與全域變數 ========== */
typedef enum {
    TOKEN_EOF = 0,
    TYPE_TOKEN, MAIN_TOKEN, IF_TOKEN, ELSE_TOKEN, WHILE_TOKEN,
    ID_TOKEN, LITERAL_TOKEN,
    LEFTPAREN_TOKEN, RIGHTPAREN_TOKEN, LEFTBRACE_TOKEN, RIGHTBRACE_TOKEN,
    SEMICOLON_TOKEN, PLUS_TOKEN, MINUS_TOKEN,
    ASSIGN_TOKEN, EQUAL_TOKEN, GREATEREQUAL_TOKEN, GREATER_TOKEN, LESSEQUAL_TOKEN, LESS_TOKEN
} TokenType;

TokenType current_token;  // 當前的 Token 型態
char token_text[256];     // 當前 Token 的字串內容
FILE *fp = NULL;          // 檔案指標
int has_error = 0;        // 語法錯誤旗標

/* ========== 2. 繼承自 HW2 的 Scanner 基礎邏輯 ========== */
int is_alpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c == '_');
}

int is_digit(char c) {
    return (c >= '0' && c <= '9');
}

int is_space(char c) {
    return (c == ' ' || c == '\t' || (int)c == 10 || (int)c == 13);
}

/* 每次呼叫此函式，會自動讀取下一個 Token 並更新 current_token */
void next_token() {
    if (has_error) return;
    
    int c;
    while ((c = fgetc(fp)) != EOF) {
        if (is_space(c)) continue;

        // 處理 ID 或 關鍵字
        if (is_alpha(c)) {
            int i = 0;
            token_text[i++] = c;
            while ((c = fgetc(fp)) != EOF && (is_alpha(c) || is_digit(c))) {
                if (i < 255) token_text[i++] = c;
            }
            token_text[i] = '\0';
            ungetc(c, fp);

            if (strcmp(token_text, "int") == 0) { current_token = TYPE_TOKEN; return; }
            if (strcmp(token_text, "main") == 0) { current_token = MAIN_TOKEN; return; }
            if (strcmp(token_text, "if") == 0) { current_token = IF_TOKEN; return; }
            if (strcmp(token_text, "else") == 0) { current_token = ELSE_TOKEN; return; }
            if (strcmp(token_text, "while") == 0) { current_token = WHILE_TOKEN; return; }
            
            current_token = ID_TOKEN;
            return;
        }
        // 處理數字
        else if (is_digit(c)) {
            int i = 0;
            token_text[i++] = c;
            while ((c = fgetc(fp)) != EOF && is_digit(c)) {
                if (i < 255) token_text[i++] = c;
            }
            token_text[i] = '\0';
            ungetc(c, fp);
            
            current_token = LITERAL_TOKEN;
            return;
        }
        // 處理符號
        else {
            token_text[0] = c;
            token_text[1] = '\0';
            switch (c) {
                case '(': current_token = LEFTPAREN_TOKEN; return;
                case ')': current_token = RIGHTPAREN_TOKEN; return;
                case '{': current_token = LEFTBRACE_TOKEN; return;
                case '}': current_token = RIGHTBRACE_TOKEN; return;
                case ';': current_token = SEMICOLON_TOKEN; return;
                case '+': current_token = PLUS_TOKEN; return;
                case '-': current_token = MINUS_TOKEN; return;
                case '=':
                    c = fgetc(fp);
                    if (c == '=') { strcpy(token_text, "=="); current_token = EQUAL_TOKEN; }
                    else { ungetc(c, fp); current_token = ASSIGN_TOKEN; }
                    return;
                case '>':
                    c = fgetc(fp);
                    if (c == '=') { strcpy(token_text, ">="); current_token = GREATEREQUAL_TOKEN; }
                    else { ungetc(c, fp); current_token = GREATER_TOKEN; }
                    return;
                case '<':
                    c = fgetc(fp);
                    if (c == '=') { strcpy(token_text, "<="); current_token = LESSEQUAL_TOKEN; }
                    else { ungetc(c, fp); current_token = LESS_TOKEN; }
                    return;
            }
        }
    }
    current_token = TOKEN_EOF;
    strcpy(token_text, "EOF");
}

/* ========== 3. Recursive-Descent Parser 核心實作 ========== */

// 比對當前 Token 是否符合預期，符合就吃掉（讀取下一個），不符合報語法錯誤
void match(TokenType expected) {
    if (has_error) return;
    if (current_token == expected) {
        next_token();
    } else {
        printf("Syntax Error: Unexpected token '%s'\n", token_text);
        has_error = 1;
    }
}

// 宣告各個語法規則函式
void program();
void block();
void statement();
void declaration_stmt();
void assign_stmt();
void if_stmt();
void while_stmt();
void condition();
void expression();
void term();

/* Program -> int main() Block */
void program() {
    match(TYPE_TOKEN);
    match(MAIN_TOKEN);
    match(LEFTPAREN_TOKEN);
    match(RIGHTPAREN_TOKEN);
    block();
}

/* Block -> { StatementList } */
void block() {
    match(LEFTBRACE_TOKEN);
    while (current_token != RIGHTBRACE_TOKEN && current_token != TOKEN_EOF && !has_error) {
        statement();
    }
    match(RIGHTBRACE_TOKEN);
}

/* Statement -> DeclarationStmt | AssignStmt | IfStmt | WhileStmt | Block */
void statement() {
    if (has_error) return;
    
    if (current_token == TYPE_TOKEN) {
        declaration_stmt();
    } else if (current_token == ID_TOKEN) {
        assign_stmt();
    } else if (current_token == IF_TOKEN) {
        if_stmt();
    } else if (current_token == WHILE_TOKEN) {
        while_stmt();
    } else if (current_token == LEFTBRACE_TOKEN) {
        block();
    } else {
        printf("Syntax Error: Invalid statement starting with '%s'\n", token_text);
        has_error = 1;
    }
}

/* DeclarationStmt -> int ID; */
void declaration_stmt() {
    match(TYPE_TOKEN);
    match(ID_TOKEN);
    match(SEMICOLON_TOKEN);
}

/* AssignStmt -> ID = Expression; */
void assign_stmt() {
    match(ID_TOKEN);
    match(ASSIGN_TOKEN);
    expression();
    match(SEMICOLON_TOKEN);
}

/* IfStmt -> if ( Condition ) Statement [ else Statement ] */
void if_stmt() {
    match(IF_TOKEN);
    match(LEFTPAREN_TOKEN);
    condition();
    match(RIGHTPAREN_TOKEN);
    statement();
    if (current_token == ELSE_TOKEN) {
        match(ELSE_TOKEN);
        statement();
    }
}

/* WhileStmt -> while ( Condition ) Statement */
void while_stmt() {
    match(WHILE_TOKEN);
    match(LEFTPAREN_TOKEN);
    condition();
    match(RIGHTPAREN_TOKEN);
    statement();
}

/* Condition -> Expression RelOp Expression */
void condition() {
    expression();
    if (current_token == EQUAL_TOKEN || current_token == GREATER_TOKEN ||
        current_token == GREATEREQUAL_TOKEN || current_token == LESS_TOKEN ||
        current_token == LESSEQUAL_TOKEN) {
        next_token(); // 吃掉比較運算子
        expression();
    } else {
        printf("Syntax Error: Expected relational operator, got '%s'\n", token_text);
        has_error = 1;
    }
}

/* Expression -> Term { (+|-) Term } */
void expression() {
    term();
    while (current_token == PLUS_TOKEN || current_token == MINUS_TOKEN) {
        next_token();
        term();
    }
}

/* Term -> ID | LITERAL */
void term() {
    if (current_token == ID_TOKEN) {
        match(ID_TOKEN);
    } else if (current_token == LITERAL_TOKEN) {
        match(LITERAL_TOKEN);
    } else {
        printf("Syntax Error: Expected Identifier or Literal, got '%s'\n", token_text);
        has_error = 1;
    }
}

/* ========== 4. 主程式 ========== */
int main() {
    // 優先讀取 test.c，若沒有則讀取標準輸入 (stdin) 方便線上測資貼上測試
    fp = fopen("test.c", "r");
    if (fp == NULL) {
        fp = stdin;
    }

    // 預讀第一個 Token 進入串流
    next_token();
    
    // 開始進行語法剖析
    program();

    // 檢查檢查剖析完畢後是否剛好到達檔案結尾
    if (!has_error && current_token == TOKEN_EOF) {
        printf("Parsing Successful! No syntax errors.\n");
    } else if (!has_error && current_token != TOKEN_EOF) {
        printf("Syntax Error: Unparsed redundant tokens left at the end.\n");
    }

    if (fp != stdin && fp != NULL) {
        fclose(fp);
    }
    return 0;
}
