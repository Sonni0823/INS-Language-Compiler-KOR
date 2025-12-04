/* scanner.l */
%option noyywrap

%{
#include "ast.h"
#include "parser.tab.h" 
%}

%%

    /* * TISC Alphabet Definition 
     * Σ = {I, N, S}
     * 각 심볼은 단일 동작을 수행하는 명령어입니다.
     */

[iI]    { 
            /* Symbol I: Val <- Val + 1 (Increment) [cite: 36] */
            return TOKEN_I; 
        }

[nN]    { 
            /* Symbol N: P <- P + 1 (Move Next) [cite: 36] */
            return TOKEN_N; 
        }

[sS]    { 
            /* Symbol S: if Val == 0, then skip next (Conditional Skip) [cite: 36] */
            return TOKEN_S; 
        }

[ \t\n\r]+ { 
    /* 공백, 탭, 개행 문자는 무시합니다. 
     * TISC는 테이프 상의 심볼 순서가 중요하며, 포맷팅은 영향을 주지 않습니다. 
     */ 
}

.       { 
    /* 정의되지 않은 문자는 무시합니다(주석 처리). */ 
}

%%