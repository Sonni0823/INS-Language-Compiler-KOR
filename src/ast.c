#include <stdlib.h>
#include "ast.h"

// 전역 변수로 리스트 관리 (Parser가 쉽게 접근하기 위해)
static AST* head = NULL;
static AST* tail = NULL;
static int size = 0;

void ast_add_value(long value) {
    AST* node = (AST*)malloc(sizeof(AST));
    node->value = value;
    node->next = NULL;

    if (!head) {
        head = tail = node;
    } else {
        tail->next = node;
        tail = node;
    }
    size++;
}

AST* ast_get_root() {
    return head;
}

int ast_get_size() {
    return size;
}

void ast_free_all() {
    AST* cur = head;
    while (cur) {
        AST* temp = cur;
        cur = cur->next;
        free(temp);
    }
    head = tail = NULL;
    size = 0;
}