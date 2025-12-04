#include <stdio.h>
#include <stdlib.h>
#include "ast.h"
#include "codegen_x86.h"

// Flex/Bison 관련 외부 변수 및 함수
extern int yyparse(void);
extern FILE* yyin;

// parser.y 등에서 접근할 수 있도록 전역으로 선언
FILE* asm_out = NULL;

int main(int argc, char** argv) {
    if (argc >= 2) {
        yyin = fopen(argv[1], "r");
        if (!yyin) {
            perror("Input file open error");
            return EXIT_FAILURE;
        }
    } else {
        yyin = stdin;
    }

    asm_out = fopen("out.s", "w");
    if (!asm_out) {
        perror("Output file open error");
        if (yyin != stdin) fclose(yyin);
        return EXIT_FAILURE;
    }

    // 파싱 시작 -> 내부에서 gen_x86_program 호출 -> out.s 작성
    if (yyparse() == 0) {
        printf("Successfully generated 'out.s'.\n");
    } else {
        fprintf(stderr, "Parsing failed.\n");
    }

    fclose(asm_out);
    if (yyin != stdin) fclose(yyin);
    
    return 0;
}