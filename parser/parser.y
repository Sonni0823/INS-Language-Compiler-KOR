/* parser.y */
%{
#include <stdio.h>
#include <stdlib.h>
#include "ast.h"
#include "codegen_x86.h"

extern int yylex(void);
void yyerror(const char* s);
extern FILE* asm_out;

%}

/* * TISC 명령어 토큰 정의 
 * I: Increment, N: Next, S: Skip
 */
%token TOKEN_I TOKEN_N TOKEN_S

%%

program
    : instruction_list
      {
          /* * 파싱이 완료되면 생성된 AST(Program Tape)를 기반으로 
           * x86 어셈블리 코드를 생성합니다.
           * 생성 후 AST 메모리를 해제합니다.
           */
          gen_x86_program(ast_get_root(), asm_out);
          ast_free_all();
      }
    ;

/* * Instruction List Construction
 * TISC 프로그램은 명령어들의 단순 나열입니다. 
 * 순차 접근(Sequential Access) 방식을 따릅니다[cite: 118].
 */
instruction_list
    : /* empty */
    | instruction_list instruction
    ;

instruction
    : TOKEN_I
      {
          /* * Action: Val <- Val + 1 [cite: 36]
           * AST에 명령어 코드 1(I)을 기록합니다.
           */
          ast_add_value(1);
      }
    | TOKEN_N
      {
          /* * Action: P <- P + 1 [cite: 36]
           * AST에 명령어 코드 2(N)를 기록합니다.
           */
          ast_add_value(2);
      }
    | TOKEN_S
      {
          /* * Action: if Val == 0 then skip next [cite: 36]
           * AST에 명령어 코드 3(S)을 기록합니다.
           */
          ast_add_value(3);
      }
    ;

%%

void yyerror(const char* s) {
    fprintf(stderr, "Parser Error: %s\n", s);
}