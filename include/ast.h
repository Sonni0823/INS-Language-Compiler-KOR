#ifndef AST_H
#define AST_H

// ABC-Lang은 단순히 메모리에 들어갈 숫자들의 나열입니다.
typedef struct AST {
    long value;         // 메모리에 기록될 값
    struct AST* next;   // 다음 값
} AST;

// 함수 선언
void ast_add_value(long value); // 리스트에 값 추가
AST* ast_get_root();            // 리스트의 시작점 반환
void ast_free_all();            // 메모리 해제
int ast_get_size();             // 현재 데이터 개수 확인

#endif // AST_H