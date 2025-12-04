#ifndef CODEGEN_X86_H
#define CODEGEN_X86_H

#include <stdio.h>
#include "ast.h"

// AST(리스트)를 받아 x86 어셈블리를 파일로 출력하는 함수
void gen_x86_program(AST* root, FILE* out);

#endif // CODEGEN_X86_H