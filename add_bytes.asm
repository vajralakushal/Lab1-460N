.ORIG x3000
LEA R0, num1
LDB R0, R0, #0 ;storing first num in R0
BRp FirstPos
BRn FirstNeg
BRz SecondNum

SecondNum LEA R1, num2
LDB R1, R1, #0; BOTH NUMBERS ARE STORED
BR NoOverflow

FirstPos LEA R1, num2
LDB R1, R1, #0; BOTH NUMBERS ARE STORED
BRp BothPos 
BRnz NoOverflow


FirstNeg LEA R1, num2
LDB R1, R0, #0; BOTH NUMBERS ARE STORED
BRn BothNeg ;unsure on condition code checking
BRzp NoOverflow


BothPos ADD R2, R1, R0
BRn Overflow
BRzp NoOverflow

BothNeg ADD R2, R1, R0
BRp Overflow
BRnz NoOverflow

NoOverflow LEA R3, res; loads address of res in R3
LDB R3, R3, #0; loads R3 with x3102
STB R2, R3, #0; stores result of addition in x3102
LEA R3, noflow; loads address of noflow in R3
LDB R3, R3, #0 ; stores 0 in R3
LEA R4, oflowAdd
LDB R4, R4, #0; x3103 in R4
STB R3, R4, #0; stores 0 in X3103
BR Fin


Overflow LEA R3, res; loads address of res in R3
LDB R3, R3, #0; loads R3 with x3102
STB R2, R3, #0; stores result of addition in X3102
LEA R3, yoflow
LDB R3, R3, #0 ; stores a 1 in R3
LEA R4, oflowAdd
LDB R4, R4, #0; X3103 in R4
STB R3, R4, #0; stores 1 in X3103
BR Fin

Fin HALT
num1 .FILL x3100
num2 .FILL x3101
res .FILL x3102
oflowAdd .FILL x3103
yoflow .FILL x1
noflow .FILL x0
.END