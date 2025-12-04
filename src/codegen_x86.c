#include <stdio.h>
#include "ast.h"
#include "codegen_x86.h"

/* * [TISC Architecture - 8-bit Data Model]
 * * Data Tape (Td): 8-bit Integer (Z_256) Space
 * - Modulus M = 256
 * - Decrement Operation: Val - 1 == Val + 255 (mod 256)
 * - This makes the subtraction via overflow practically feasible.
 */

#define DATA_TAPE_SIZE 65536 // 64KB (Address space)

void gen_x86_program(AST* root, FILE* out) {
    // ---------------------------------------------------------
    // 1. Data Section
    // ---------------------------------------------------------
    fprintf(out, "    .section .data\n");
    
    // [Program Tape Tp] - 프로그램 명령어는 여전히 64bit 정수로 관리 (opcode)
    fprintf(out, "prog_tape:\n");
    AST* cur = root;
    int prog_len = 0;
    while (cur) {
        fprintf(out, "    .quad %ld\n", cur->value); 
        cur = cur->next;
        prog_len++;
    }
    if (prog_len == 0) {
        fprintf(out, "    .quad 0\n"); 
        prog_len = 1;
    }

    // [Data Tape Td] - **8-bit 변경됨**
    fprintf(out, "data_tape:\n");
    // Size: 65536 bytes (NOT quadwords)
    fprintf(out, "    .zero %d\n", DATA_TAPE_SIZE); 

    fprintf(out, "fmt_start: .string \"[TISC] System Started. Tape Length: %d (8-bit Mode)\\n\"\n", prog_len);
    // 종료 상태 H = 255 (0xFF, -1 in signed 8-bit)
    fprintf(out, "fmt_halt: .string \"[TISC] Logical Halt Detected (T[0] == 255). Result:\\n\"\n");

    // ---------------------------------------------------------
    // 2. Text Section
    // ---------------------------------------------------------
    fprintf(out, "\n    .section .text\n");
    fprintf(out, "    .globl main\n");
    fprintf(out, "main:\n");

    // [Prologue]
    fprintf(out, "    subq $40, %%rsp\n");

    // Start Log
    fprintf(out, "    leaq fmt_start(%%rip), %%rcx\n");
    fprintf(out, "    movq $%d, %%rdx\n", prog_len);
    fprintf(out, "    call printf\n");

    // [Machine Initialization]
    fprintf(out, "    leaq data_tape(%%rip), %%r15\n");
    fprintf(out, "    leaq prog_tape(%%rip), %%r14\n");
    fprintf(out, "    movq $0, %%r13\n"); // p (Data Pointer) - Index는 여전히 64bit
    fprintf(out, "    movq $0, %%r12\n"); // pc (Program Counter)
    fprintf(out, "    movq $0, %%r10\n"); // cycle counter
    fprintf(out, "    movq $%d, %%rbx\n", prog_len); // L

    // ---------------------------------------------------------
    // [Execution Loop]
    // ---------------------------------------------------------
    fprintf(out, ".Lloop:\n");

    // 1. Definition 2.2.5: Halting Condition Check
    // Check if T[0] == 255 (0xFF)
    // **수정**: movzbl 사용 (1바이트 읽어서 64비트로 확장)
    fprintf(out, "    movzbl (%%r15), %%eax\n"); 
    fprintf(out, "    cmpq $255, %%rax\n");    
    fprintf(out, "    je .Lhalt_logical\n");   

    // 2. Safety Limit (1억 사이클)
    fprintf(out, "    incq %%r10\n");
    fprintf(out, "    cmpq $100000000, %%r10\n");
    fprintf(out, "    jg .Lexit_error\n");

    // 3. Circular Execution (Wrap-around)
    fprintf(out, "    cmpq %%rbx, %%r12\n");
    fprintf(out, "    jl .Lfetch\n");
    fprintf(out, "    movq $0, %%r12\n");

    fprintf(out, ".Lfetch:\n");
    // Instruction Fetch (Opcode는 64bit)
    fprintf(out, "    movq (%%r14, %%r12, 8), %%rax\n");

    // Decoding
    fprintf(out, "    cmpq $1, %%rax\n");
    fprintf(out, "    je .L_exec_I\n");
    fprintf(out, "    cmpq $2, %%rax\n");
    fprintf(out, "    je .L_exec_N\n");
    fprintf(out, "    cmpq $3, %%rax\n");
    fprintf(out, "    je .L_exec_S\n");
    fprintf(out, "    jmp .L_next_pc\n");

    // Operations
    
    // [I] Increment: **incb (1 byte increment)**
    fprintf(out, ".L_exec_I:\n");
    // 주소 계산: Base + Index * 1 (Scale factor 1 for byte array)
    fprintf(out, "    incb (%%r15, %%r13, 1)\n");
    fprintf(out, "    jmp .L_next_pc\n");

    // [N] Next Pointer
    fprintf(out, ".L_exec_N:\n");
    fprintf(out, "    incq %%r13\n");
    // 순환 테이프 (Size 65536)
    fprintf(out, "    andq $0xFFFF, %%r13\n"); 
    fprintf(out, "    jmp .L_next_pc\n");

    // [S] Skip if Zero
    fprintf(out, ".L_exec_S:\n");
    // **수정**: movzbl로 1바이트 로드 후 비교
    fprintf(out, "    movzbl (%%r15, %%r13, 1), %%edx\n");
    fprintf(out, "    cmpq $0, %%rdx\n");
    fprintf(out, "    jne .L_next_pc\n");
    fprintf(out, "    addq $2, %%r12\n"); 
    fprintf(out, "    jmp .Lloop\n");

    fprintf(out, ".L_next_pc:\n");
    fprintf(out, "    incq %%r12\n");
    fprintf(out, "    jmp .Lloop\n");

    // ---------------------------------------------------------
    // [Logical Halt Handler]
    // ---------------------------------------------------------
    fprintf(out, ".Lhalt_logical:\n");
    fprintf(out, "    leaq fmt_halt(%%rip), %%rcx\n");
    fprintf(out, "    call printf\n");

    // 결과 출력 Loop
    fprintf(out, "    xorq %%r14, %%r14\n");
    fprintf(out, "    incq %%r14\n"); // index = 1

    fprintf(out, ".Lprint_loop:\n");
    // **수정**: movzbl 사용
    fprintf(out, "    movzbl (%%r15, %%r14, 1), %%eax\n");
    fprintf(out, "    cmpq $0, %%rax\n");
    fprintf(out, "    je .Lprint_done\n");

    fprintf(out, "    movq %%rax, %%rcx\n"); 
    fprintf(out, "    call putchar\n");

    fprintf(out, "    incq %%r14\n");
    fprintf(out, "    jmp .Lprint_loop\n");

    fprintf(out, ".Lprint_done:\n");
    fprintf(out, "    movq $10, %%rcx\n");
    fprintf(out, "    call putchar\n");
    
    fprintf(out, "    addq $40, %%rsp\n");
    fprintf(out, "    movq $0, %%rax\n");
    fprintf(out, "    ret\n");

    fprintf(out, ".Lexit_error:\n");
    fprintf(out, "    addq $40, %%rsp\n");
    fprintf(out, "    movq $1, %%rax\n");
    fprintf(out, "    ret\n");
}