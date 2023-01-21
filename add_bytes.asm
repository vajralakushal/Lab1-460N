	.ORIG x3000
	LEA R0, nib
	LDB R1, R0, #0
	LSHF R2, R1, #12
	RSHFA R3, R2, #12
	RSHFA R2, R1, #4
	ADD R4, R3, R2
	STB R4, R0, x1
	HALT
nib .FILL x3050
    .END
