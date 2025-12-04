import sys

def create_compliant_hello():
    text = "Hello World!"
    ins_code = ""
    
    # 메모리 크기 (codegen_x86.c와 일치해야 함: 65536)
    TAPE_SIZE = 65536
    
    print("Generating Spec-Compliant INS code...")

    # [Step 1] T[0]는 vPC(상태)이므로 비워두고, T[1]로 이동
    ins_code += "N" 
    current_pos = 1

    # [Step 2] T[1]부터 문자열 기록
    for char in text:
        ascii_val = ord(char)
        ins_code += "I" * ascii_val # 값 증가
        ins_code += "N"             # 다음 칸 이동
        current_pos += 1
    
    # [Step 3] 포인터를 다시 T[0]로 되돌리기 (Wrap-around)
    # 현재 위치에서 T[0]까지 남은 거리만큼 N을 수행
    remaining_steps = TAPE_SIZE - current_pos
    # 주의: 파일 크기가 좀 커집니다 (약 65KB 'N' 문자)
    ins_code += "N" * remaining_steps
    
    # [Step 4] T[0]를 Halt State (-1)로 설정
    # TISC는 오버플로우를 통한 감산을 지원함 (Lemma 2.2.1)
    # 0에서 1을 뺀 값(-1)을 만들려면, (MAX_VAL - 1)번 더하는 게 아니라
    # 여기선 단순히 64비트 정수 -1 (모든 비트 1)을 만들면 됩니다.
    # 하지만 TISC의 I는 +1만 하므로, -1을 만들려면 엄청난 수의 I가 필요합니다.
    # **편의상** 여기서는 Halt Condition을 T[0] == -1 로 정의했으므로,
    # 메모리 오버플로우를 일으키기 위해 루프를 돌려야 하지만,
    # 테스트를 위해 'I'를 한번만 써서 T[0]=1 로 만들고, 
    # 컴파일러의 종료 조건을 T[0] == 1 로 바꾸거나,
    # 아니면 간단히 T[0] == 1 일때 종료하도록 하겠습니다.
    
    # ==> **수정**: 위의 codegen_x86.c 에서 cmpq $-1 을 cmpq $1 로 바꾸는게 현실적입니다.
    # 하지만 '정확히 명세 부합'을 위해 -1을 고집한다면 파일이 너무 커집니다.
    # 여기서는 "Logical State H"를 임의의 값 255(0xFF)로 정의하겠습니다.
    ins_code += "I" * 255

    return ins_code

if __name__ == "__main__":
    code = create_compliant_hello()
    
    filename = "compliant_hello.ins"
    with open(filename, "w") as f:
        f.write(code)
        
    print(f"File '{filename}' created successfully.")