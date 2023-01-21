    .ORIG x3000
    LEA R0, num1
    LDB R0, R0, #0 ;storing first num in R0
    BRp FirstPos
    BRn FirstNeg
    BRz SecondNum

SecondNum
    LEA R1, num2
    LDB R1, R0, #0; BOTH NUMBERS ARE STORED
    B NoOverflow

FirstPos
    LEA R1, num2
    LDB R1, R0, #0; BOTH NUMBERS ARE STORED
    BRzp BothPos ;unsure on condition code checking
    BRn NoOverflow


FirstNeg
    LEA R1, num2
    LDB R1, R0, #0; BOTH NUMBERS ARE STORED
    BRnz BothNeg ;unsure on condition code checking
    BRp NoOverflow


BothPos
    ADD R2, R1, R0
    BRn Overflow
    BRzp NoOverflow

BothNeg
    ADD R2, R1, R0
    BRp Overflow
    BRnz NoOverflow

NoOverflow
    ;LEA R1, num2
    ;LDB R1, R1, #0 ; SECOND NUMBER IN R1, FIRST IN R0
    ADD R2, R1, R0
    LEA R3, res
    STB R2, R3, #0; STORING THE RESULT IN X3102
    LEA R3, noflow
    LDB R3, R3, #0 ; STORING 0 IN R3
    LEA R4, oflowAdd
    LDB R4, R4, #0; X3103 IN R4
    STB R3, R4, #0; STORING 0 IN X3103
    B Fin


Overflow
    LEA R3, res
    STB R2, R3, #0; STORING THE RESULT IN X3102
    LEA R3, yoflow
    LDB R3, R3, #0 ; STORING 1 IN R3
    LEA R4, oflowAdd
    LDB R4, R4, #0; X3103 IN R4
    STB R3, R4, #0; STORING 1 IN X3103
    B Fin

Fin HALT
num1 .FILL x3100
num2 .FILL x3101
res .FILL x3102
oflowAdd .FILL x3103
yoflow .FILL x1
noflow .FILL x0

.END