import sys

# High Address Jump Test
# 300번지까지 더미로 채우고, 거기서 'H'를 출력하게 합니다.

code = "B B B " # 0,1,2 Reserved

# 3번지부터 299번지까지 아무것도 안 하는 명령(0 0 next)으로 채움
# (실제로는 그냥 건너뛰기 위해 바로 점프 명령을 씀)

# 3번지 명령: 0 0 300 (300번지로 점프)
code += "B "      # A=0
code += "B "      # B=0
code += ("A " * 300) + "B " # C=300

# 4번지 ~ 99번지 (대충 채움, 실행 안 됨)
for i in range(4, 100):
    code += "B B B "

# 300번지 데이터 위치 (데이터는 코드 뒤에 붙음)
# 간단하게 하기 위해 300번째 '명령어' 위치에 도달하도록 패딩
# 명령어 하나당 A, B, C 3개 요소.
# 현재 요소 개수 세기 귀찮으니, 그냥 무식하게 A를 많이 찍어서 주소를 늘림.

# 전략 변경: 파서가 A 개수를 제대로 세는지 확인하는 가장 쉬운 코드
# A가 300개 있는 명령어를 만듭니다.
# 300번지(0번지=Output)에 값을 씁니다.

long_jump_code = ""
long_jump_code += "B B B " # 0,1,2

# [명령 1] A=Data(72='H'), B=Output(0), C=Exit(2)
# Data는 주소 300번에 둠.

# A part: 300번 A를 씀.
long_jump_code += ("A " * 300) + "B " 
# B part: 0
long_jump_code += "B "
# C part: 2
long_jump_code += ("A " * 2) + "B "

# 데이터 채우기 (300번지가 될 때까지)
# 현재까지 요소: 3(sys) + 3(cmd) = 6개.
# 300 - 6 = 294개의 더미 데이터 필요
long_jump_code += ("B " * 294)

# 300번지에 'H'(72) 데이터
long_jump_code += ("A " * 72) + "B "

# 트리거
long_jump_code += "C"

with open("test_high.abc", "w") as f:
    f.write(long_jump_code)

print(long_jump_code)