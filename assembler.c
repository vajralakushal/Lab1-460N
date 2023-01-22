/*
	Name 1: Kushal Vajrala
	Name 2: Seema Kulkarni
	UTEID 1: kv7287
	UTEID 2: srk2374
*/
#include <stdio.h> /* standard input/output library */
#include <stdlib.h> /* Standard C Library */
#include <string.h> /* String operations library */
#include <ctype.h> /* Library for useful character operations */
#include <limits.h> /* Library for definitions of common variable type characteristics */

#define MAX_LINE_LENGTH 255
#define MAX_LABEL_LENGTH 12

FILE* infile = NULL;
FILE* outfile = NULL;

/* Throws Error with a Message to the Console */
void throwError(int n, char* message){
    printf("Error %d: %s\n", n, message);
    fclose(infile);
    fclose(outfile);
    exit(n);
}

////* SYMBOL TABLE Declaration + Associated Functions *//////////////////////////////////////////////////////////////////////

char nullstring[MAX_LABEL_LENGTH + 1]= "\0";

struct label_address {
    char label[MAX_LABEL_LENGTH + 1];
    int address;
};
struct label_address symbol_table[MAX_LINE_LENGTH + 1];

void initialize () {
    for(int i=0; i < MAX_LINE_LENGTH; i++) {
        symbol_table[i].address = -1;
        for(int i = 0; i <= MAX_LABEL_LENGTH; i++)
            symbol_table[i].label[i] = '\0';
    }
}

int containsLabel (char* label) {
    // label? address of label we just read in // *label? *label -> label
    if (label == NULL) return -1;

    for(int i=0; i < MAX_LINE_LENGTH; i++) {
        if(strcmp(symbol_table[i].label, label) == 0) {
            return symbol_table[i].address;
        }
    }
    return -1;
}

void insertLabel (char* label, int address) {
    for(int i=0; i < MAX_LINE_LENGTH; i++) {
        if(symbol_table[i].address == -1 && (strcmp(symbol_table[i].label, nullstring) == 0)) {
            symbol_table[i].address = address;
            strcpy(symbol_table[i].label, label);
            return;
        }
    }
    throwError(4, "Cannot insert label, symbol table is full"); // should never reach this spot, since there should never be more than 255 labels
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


////* Parsing Functions *//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

enum { DONE, OK, EMPTY_LINE };

// Check if string is LC3 instruction (ADD, AND) or pseudo command (.ORIG, .FILL). If yes, returns 1. If not, returns 0.
int isOpcode(char * opcode){
    if (opcode == NULL) return 0;

    char* opcode_list[] = {"add", "and", "br", "brn", "brz", "brp", "brzp", "brnp", "brnz", "brnzp", "jmp", "jsr", "jsrr",
                           "ldb", "ldw", "lea", "nop", "not", "ret", "lshf", "rshfl", "rshfa", "rti", "stb", "stw",
                           "trap", "xor", ".orig", ".fill", ".end", "halt"};
    for(int i=0; i < (sizeof(opcode_list) / sizeof(opcode_list[0])); i++) {
        if(strcmp(opcode, opcode_list[i]) == 0) return 1;
    }
    return 0;
}

int readAndParse( FILE * pInfile,
                  char * pLine,
                  char ** pLabel,
                  char ** pOpcode,
                  char ** pArg1,
                  char ** pArg2,
                  char ** pArg3,
                  char ** pArg4)
{
    char * lPtr;        // deleted char * lRet here from the original starter code
    int i;

    /* fgets() gets line from pInfile and stores it in pLine; stops when \n (new line char), MAX_LINE_LENGTH, or end of file is reached */
    /* Null ptr returned if end of file is reached without reading any chars */
    if( !fgets( pLine, MAX_LINE_LENGTH, pInfile ) )
        return( DONE );

    /* convert entire line to lowercase */
    for( i = 0; i < strlen( pLine ); i++ )
        pLine[i] = tolower( pLine[i] );

    /* reset all pointers associated with assembly instruction to the address of a NULL */
    *pLabel = *pOpcode = *pArg1 = *pArg2 = *pArg3 = *pArg4 = pLine + strlen(pLine);

    /* ignore the comments */
    lPtr = pLine;
    while( *lPtr != ';' && *lPtr != '\0' && *lPtr != '\n' )
        lPtr++;
    *lPtr = '\0';

    /* check for empty line */
    if( !(lPtr = strtok( pLine, "\t\n ," ) ) )
        return( EMPTY_LINE );

    if( !isOpcode( lPtr ) && lPtr[0] != '.' ) /* found a label */
    {
        *pLabel = lPtr;
        if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );
    }

    *pOpcode = lPtr;

    if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );

    *pArg1 = lPtr;

    if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );

    *pArg2 = lPtr;

    if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );

    *pArg3 = lPtr;

    if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );

    *pArg4 = lPtr;

    return( OK );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


////* Translator Helper Functions */////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
enum { BR, ADD, LDB, STB, JSR, AND, LDW, STW, RTI, XOR, xx1, xx2, JMP, SHF, LEA, TRAP };

int toInt( char * pStr ) {
    char * t_ptr;
    char * orig_pStr;
    int t_length,k;
    int lNum, lNeg = 0;
    long int lNumLong;

    orig_pStr = pStr;
    if( *pStr == '#' )          /* decimal */
    {
        pStr++;
        if( *pStr == '-' )          /* dec is negative */
        {
            lNeg = 1;
            pStr++;
        }

        t_ptr = pStr;                   /* pStr now points to string of numbers (w/out # or -) */
        t_length = strlen(t_ptr);
        for(k=0;k < t_length;k++)       /* check if each char in t_ptr is a digit*/
        {
            if (!isdigit(*t_ptr))
            {
                printf("Error: invalid decimal operand, %s\n",orig_pStr);
                exit(4);
            }
            t_ptr++;
        }
        lNum = atoi(pStr);          /* convert string of digits to int */
        if (lNeg)
            lNum = -lNum;

        return lNum;
    }
    else if( *pStr == 'x' )    /* hex     */
    {
        pStr++;
        if( *pStr == '-' )          /* hex is negative */
        {
            lNeg = 1;
            pStr++;
        }

        t_ptr = pStr;                   /* pStr now points to string of numbers (w/out # or -) */
        t_length = strlen(t_ptr);
        for(k=0;k < t_length;k++)       /* check if each char in t_ptr is a xdigit */
        {
            if (!isxdigit(*t_ptr))
            {
                printf("Error: invalid hex operand, %s\n",orig_pStr);
                exit(4);
            }
            t_ptr++;
        }
        lNumLong = strtol(pStr, NULL, 16);    /* convert hex string into integer */
        lNum = (lNumLong > INT_MAX)? INT_MAX : lNumLong;
        if( lNeg )
            lNum = -lNum;
        return lNum;
    }
    else
    {
        printf( "Error: invalid operand, %s\n", orig_pStr);
        exit(4);  /* This has been changed from error code 3 to error code 4, see clarification 12 */
    }
}


int validLabel (char* label) {
    if (label == NULL) return 0;

    // Label has 1 to 20 chars
    if (strlen(label) > MAX_LABEL_LENGTH || strlen(label) < 1) return 0;

    // Label is not an opcode
    if (isOpcode(label)) return 0;

    // Label is not IN, OUT, GETC, PUTS
    char* trap_list[] = {"in", "out", "getc", "puts"};
    for (int i=0; i < (sizeof(trap_list) / sizeof(trap_list[0])); i++) {
        if(strcmp(label, trap_list[i]) == 0) return 0;
    }

    // Label starts with a letter other than 'x'
    if ( label[0] == 'x' || !isalpha(label[0]) ) return 0;

    // Label is made up of alphanumeric chars
    for (char* ptr = label; *ptr != '\0'; ptr++){
        if (!isalnum(*ptr)) return 0;
    }

    // If all conditions are passed, return 1 for a valid label
    return 1;
}

int negIntToTwoCmp(int bit_size, int neg_arg) {
    int two_cmp[bit_size]; // two_cmp holds value from LSB (index 0) to MSB (index bit_size-1)

    // initialize two_cmp to zeroes.
    for(int i = 0; i < bit_size; i++)
        two_cmp[i] = 0;

    // insert arg into two_cmp
    int b = -1*neg_arg;
    int index = 0;
    while (b != 0) {
        two_cmp[index] = b % 2;
        b = b / 2;
        index++;
    }

    // invert two_cmp
    for(int i = 0; i < bit_size ; i++)
        two_cmp[i] = two_cmp[i] ? 0 : 1 ;

    // add 1 to two_cmp
    int carry_over = 1;
    for(int i = 0; i < bit_size ; i++) {
        if (two_cmp[i] == 0 && carry_over == 1) {
            two_cmp[i] = 1;
            carry_over = 0;
        } else if (two_cmp[i] == 1 && carry_over == 1) {
            two_cmp[i] = 0;
        }
    }

    // convert two_cmp (in binary) to decimal
    b = 0;
    for(int i = 0; i < bit_size; i++)
        b = b + two_cmp[i]*(1<<i);

    return b;
}


int registerToInt(char* arg){
    if (arg == NULL) throwError(4, "invalid register");
    if (strlen(arg) != 2) throwError(4, "invalid register");
    if (arg[0] != 'r') throwError(4, "invalid register");

    char* regNum = arg + 1;     // regNum points to arg[1], the register number
    int r = atoi(regNum);   // atoi() returns register number is int is found, 0 otherwise
    if (r == 0 && arg[1] != '0') throwError(4, "invalid register");
    if (r < 0 || r > 7) throwError(4, "invalid register");
    return r;
}


int constantToInt(char* arg, int bit_size){
    if (arg == NULL) throwError(4, "invalid operand");
    int num = toInt(arg);

    /* hex or decimal constant */
    if(arg[0] == 'x' || arg[0] == '#'){

        /* constant has explicit negative sign */
        if (arg[1] == '-'){
            if((-1*num) > (1<<(bit_size-1))) throwError(3, "invalid constant");
            num = negIntToTwoCmp(bit_size, num);
            return num;
        }
        /* constant is positive! */
        else {
            if (num >= (1<<(bit_size-1))) throwError(3, "invalid constant");
            return num;
        }
    }

    /* invalid constant */
    else throwError(4, "invalid operand");
}


int labelToInt(int current_line, char* arg, int bit_size) {
    int pcoffset = 0;
    int label_address = containsLabel(arg);

    if (label_address != -1) {
        pcoffset = label_address - (current_line + 1);
    }
    else {
        if (validLabel(arg)) throwError(1, "undefined label");
        else throwError(4, "invalid label");
    }

    if (pcoffset > 0 && pcoffset >= (1<<(bit_size -1))) {
        throwError(3, "invalid offset");
    }

    if(pcoffset < 0) {
        if( (-1*pcoffset) > (1 << (bit_size - 1))) {
            throwError(3, "invalid offset");
        }
        pcoffset = negIntToTwoCmp(bit_size, pcoffset);
    }
    return pcoffset;
}

int logicArgToInt(char* arg){
    if(arg == NULL) throwError(4, "invalid operand");
    if(strlen(arg) < 2) throwError(4, "invalid operand");

    /* logic arg is a register */
    if (arg[0] == 'r') {
        int reg = registerToInt(arg);
        return reg;
    }
        /* logic arg is a constant */
    else if (arg[0] == 'x' || arg[0] == '#'){
        int imm5 = constantToInt(arg, 5);
        imm5 = (1<<5) + imm5;
        return imm5;
    }
        /* logic arg is invalid */
    else throwError(4, "invalid operand");
}

void add(char* lArg1, char* lArg2, char* lArg3, char* lArg4) {
    if(strcmp(lArg4, nullstring) != 0) {
        throwError(4, "unexpected operand");
    }
    int dr = registerToInt(lArg1);
    int sr = registerToInt(lArg2);
    int logicArg = logicArgToInt(lArg3);
    int opcode = ADD;
    int instr = (opcode<<12) + (dr<<9) + (sr<<6) + logicArg;
    fprintf(outfile, "0x%.4X\n", instr);
}

void and(char* lArg1, char* lArg2, char* lArg3, char* lArg4) {
    if(strcmp(lArg4, nullstring) != 0) {
        throwError(4, "unexpected operand");
    }
    int dr = registerToInt(lArg1);
    int sr = registerToInt(lArg2);
    int logicArg = logicArgToInt(lArg3);
    int opcode = AND;
    int instr = (opcode<<12) + (dr<<9) + (sr<<6) + logicArg;
    fprintf(outfile, "0x%.4X\n", instr);
}

void br(char* lOpcode, char* lArg1, char* lArg2, char* lArg3, char* lArg4, int line_counter){
    if (strcmp(lArg2, nullstring) != 0 || strcmp(lArg3, nullstring) != 0 || strcmp(lArg4, nullstring) != 0) throwError(4, "unexpected operand");
    int opcode = BR;
    int arg1 = labelToInt(line_counter, lArg1, 9); // BR only accepts labels!

    // Determine nzp code
    int n = 0, z = 0, p = 0;
    if (strcmp(lOpcode, "br") == 0) {
        n = 1; z = 1; p = 1;
    } else {
        for (char* ptr = lOpcode+2; *ptr != '\0'; ptr++){
            if (*ptr == 'n') n = 1;
            if (*ptr == 'z') z = 1;
            if (*ptr == 'p') p = 1;
        }
    }
    int nzp = (n<<2) + (z<<1) + p;

    int instr = (opcode<<12) + (nzp<<9) + arg1;
    fprintf(outfile, "0x%.4X\n", instr);
}

void halt(char* lArg1, char* lArg2, char* lArg3, char* lArg4){
    if (strcmp(lArg1, nullstring) != 0 || strcmp(lArg2, nullstring) != 0 || strcmp(lArg3, nullstring) != 0 || strcmp(lArg4, nullstring) != 0)
        throwError(4, "unexpected operand");
    int instr = 61477; // HALT = TRAP x25 = xF025 = 61477
    fprintf(outfile, "0x%.4X\n", instr);
}

void jmp(char* lArg1, char* lArg2, char* lArg3, char* lArg4){
    if (strcmp(lArg2, nullstring) != 0 || strcmp(lArg3, nullstring) != 0 || strcmp(lArg4, nullstring) != 0) throwError(4, "unexpected operand");
    int opcode = JMP;
    int br = registerToInt(lArg1);
    int instr = (opcode<<12) + (br<<6);
    fprintf(outfile, "0x%.4X\n", instr);
}

void jsr(char* lArg1, char* lArg2, char* lArg3, char* lArg4, int line_counter){
    if (strcmp(lArg2, nullstring) != 0 || strcmp(lArg3, nullstring) != 0 || strcmp(lArg4, nullstring) != 0) throwError(4, "unexpected operand");
    int opcode = JSR;
    int offset = labelToInt(line_counter, lArg1, 11); // JSR only accepts labels!
    int instr = (opcode<<12) + (1<<11) + offset;
    fprintf(outfile, "0x%.4X\n", instr);
}

void jsrr(char* lArg1, char* lArg2, char* lArg3, char* lArg4) {
    if (strcmp(lArg2, nullstring) != 0 || strcmp(lArg3, nullstring) != 0 || strcmp(lArg4, nullstring) != 0) throwError(4, "unexpected operand");
    int opcode = JSR;
    int br = registerToInt(lArg1);
    int instr = (opcode<<12) + (br<<6);
    fprintf(outfile, "0x%.4X\n", instr);
}

void ld(int byte, char* lArg1, char* lArg2, char* lArg3, char* lArg4) {
    if (strcmp(lArg4, nullstring) != 0) throwError(4, "unexpected operand");
    int opcode = byte ? LDB : LDW;
    int dr = registerToInt(lArg1);
    int br = registerToInt(lArg2);
    int offset = constantToInt(lArg3, 6);
    int instr = (opcode<<12) + (dr<<9) + (br<<6) + offset;
    fprintf(outfile, "0x%.4X\n", instr);
}

void lea (char* lArg1, char* lArg2, char* lArg3, char* lArg4, int line_counter) {
    if (strcmp(lArg3, nullstring) != 0 || strcmp(lArg4, nullstring) != 0) throwError(4, "unexpected operand");
    int opcode = LEA;
    int dr = registerToInt(lArg1);
    int label = labelToInt(line_counter, lArg2, 9); // LEA only accepts labels!
    int instr = (opcode<<12) + (dr<<9) + label;
    fprintf(outfile, "0x%.4X\n", instr);
}

void nop (char* lArg1, char* lArg2, char* lArg3, char* lArg4) {
    if (strcmp(lArg1, nullstring) != 0 || strcmp(lArg2, nullstring) != 0 || strcmp(lArg3, nullstring) != 0 || strcmp(lArg4, nullstring) != 0)
        throwError(4, "unexpected operand");
    int instr = 0; // NOP = x0000 = 0
    fprintf(outfile, "0x%.4X\n", instr);
}

void not (char* lArg1, char* lArg2, char* lArg3, char* lArg4) {
    if (strcmp(lArg3, nullstring) != 0 || strcmp(lArg4, nullstring) != 0) throwError(4, "unexpected operand");
    int opcode = XOR; // NOT has same opcode as XOR
    int dr = registerToInt(lArg1);
    int sr = registerToInt(lArg2);
    int instr = (opcode<<12) + (dr<<9) + (sr<<6) + 63;
    fprintf(outfile, "0x%.4X\n", instr);
}

void ret (char* lArg1, char* lArg2, char* lArg3, char* lArg4) {
    if (strcmp(lArg1, nullstring) != 0 || strcmp(lArg2, nullstring) != 0 || strcmp(lArg3, nullstring) != 0 || strcmp(lArg4, nullstring) != 0)
        throwError(4, "unexpected operand");
    int instr = 49600; // RET = xC1C0 = 49600
    fprintf(outfile, "0x%.4X\n", instr);
}

void lshf (char* lArg1, char* lArg2, char* lArg3, char* lArg4) {
    if (strcmp(lArg4, nullstring) != 0) {
        throwError(4, "unexpected operand");
    }
    int dr = registerToInt(lArg1);
    int sr = registerToInt(lArg2);
    int amount = toInt(lArg3);
    if (amount >= 16 || amount < 0) {
        throwError(3, "invalid constant");
    }
    int opcode = SHF;
    int instr = (opcode << 12) + (dr << 9) + (sr << 6) + amount;
    fprintf(outfile, "0x%.4X\n", instr);
}

void rshfl (char* lArg1, char* lArg2, char* lArg3, char* lArg4) {
    if(strcmp(lArg4, nullstring) != 0) {
        throwError(4, "unexpected operand");
    }
    int dr = registerToInt(lArg1);
    int sr = registerToInt(lArg2);
    int amount = toInt(lArg3);
    if(amount >= 16 || amount < 0) {
        throwError(3, "invalid constant");
    }
    int opcode = SHF;
    int instr = (opcode<<12) + (dr<<9) + (sr<<6) + (1<<4) + amount;
    fprintf(outfile, "0x%.4X\n", instr);
}

void rshfa (char* lArg1, char* lArg2, char* lArg3, char* lArg4) {
    if(strcmp(lArg4, nullstring) != 0) {
        throwError(4, "unexpected operand");
    }
    int dr = registerToInt(lArg1);
    int sr = registerToInt(lArg2);
    int amount = toInt(lArg3);
    if(amount >= 16 || amount < 0) {
        throwError(3, "invalid constant");
    }
    int opcode = SHF;
    int instr = (opcode<<12) + (dr<<9) + (sr<<6) + (1<<5) + (1<<4) + amount;
    fprintf(outfile, "0x%.4X\n", instr);
}

void rti (char* lArg1, char* lArg2, char* lArg3, char* lArg4) {
    if (strcmp(lArg1, nullstring) != 0 || strcmp(lArg2, nullstring) != 0 || strcmp(lArg3, nullstring) != 0 || strcmp(lArg4, nullstring) != 0)
        throwError(4, "unexpected operand");
    int instr = 32768; // RTI = x8000 = 32768
    fprintf(outfile, "0x%.4X\n", instr);
}

void st (int byte, char* lArg1, char* lArg2, char* lArg3, char* lArg4) {
    if (strcmp(lArg4, nullstring) != 0) throwError(4, "unexpected operand");
    int opcode = byte? STB : STW;
    int sr = registerToInt(lArg1);
    int br = registerToInt(lArg2);
    int offset = constantToInt(lArg3, 6);
    int instr = (opcode<<12) + (sr<<9) + (br<<6) + offset;
    fprintf(outfile, "0x%.4X\n", instr);
}

void trap(char* lArg1, char* lArg2, char* lArg3, char* lArg4) {
    if (strcmp(lArg2, nullstring) != 0 || strcmp(lArg3, nullstring) != 0 || strcmp(lArg4, nullstring) != 0) throwError(4, "unexpected operand");
    if (lArg1[0] != 'x') throwError(4, "invalid operand");
    int opcode = TRAP;
    int trapvect = toInt(lArg1);
    if (trapvect < 0 || trapvect > 255) throwError(3, "invalid trap address");
    int instr = (opcode<<12) + trapvect;
    fprintf(outfile, "0x%.4X\n", instr);
}

void xor(char* lArg1, char* lArg2, char* lArg3, char* lArg4) {
    if (strcmp(lArg4, nullstring) != 0) throwError(4, "unexpected operand");
    int opcode = XOR;
    int dr = registerToInt(lArg1);
    int sr1 = registerToInt(lArg2);
    int logicArg = logicArgToInt(lArg3);
    int instr = (opcode<<12) + (dr<<9) + (sr1<<6) + logicArg;
    fprintf(outfile, "0x%.4X\n", instr);
}

void dotorig(char* lArg1, char* lArg2, char* lArg3, char* lArg4) {
    if (strcmp(lArg2, nullstring) != 0 || strcmp(lArg3, nullstring) != 0 || strcmp(lArg4, nullstring) != 0) throwError(4, "unexpected operand");
    int addr = toInt(lArg1);

    // if address is odd or negative or greater than 0xFFFF (65535), throw error
    if ((addr % 2) != 0 || addr < 0 || addr > 65535) throwError(3, "invalid orig address");

    fprintf(outfile, "0x%.4X\n", addr);
}

void dotfill(char* lArg1, char* lArg2, char* lArg3, char* lArg4) {
    if (strcmp(lArg2, nullstring) != 0 || strcmp(lArg3, nullstring) != 0 || strcmp(lArg4, nullstring) != 0) throwError(4, "unexpected operand");
    int fill = toInt(lArg1);

    // convert negative constants to 2's Comp
    if (fill < 0) fill = constantToInt(lArg1, 16);

    // if fill is greater than 0xFFFF (65535), throw error
    if (fill > 65535) throwError(3, "invalid fill constant");

    fprintf(outfile, "0x%.4X\n", fill);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



int main(int argc, char* argv[]) {
    /* open the source files */
    infile = fopen(argv[1], "r");
    outfile = fopen(argv[2], "w");
    //infile = fopen("C:/Users/ishac/Documents/Comp Arch Labs/Lab1/input.asm", "r");
    //outfile = fopen("C:/Users/ishac/Documents/Comp Arch Labs/Lab1/output.obj", "w");

    if (!infile) {
        printf("Error: Cannot open file %s\n", argv[1]);
        exit(4);
    }
    if (!outfile) {
        printf("Error: Cannot open file %s\n", argv[2]);
        exit(4);
    }


    /* declare parsing vars */
    char lLine[MAX_LINE_LENGTH + 1], *lLabel, *lOpcode, *lArg1, *lArg2, *lArg3, *lArg4;
    int lRet;

    /* initialize symbol table + declare associated vars */
    initialize();
    int end = 0;
    int orig = 0;
    int line_counter = 0;
    char newLabel[MAX_LABEL_LENGTH + 1] = { '\0' };
    int labelOnFirstLine = 0;


    /* Fill in Symbol Table, as we parse the input file for the first time */
    do {
        lRet = readAndParse( infile, lLine, &lLabel,
                             &lOpcode, &lArg1, &lArg2, &lArg3, &lArg4 );
        if( lRet != DONE && lRet != EMPTY_LINE ) {

            if (orig == 0) {
                if (strcmp(lOpcode,".orig") == 0) {
                    orig = 1;
                    if (labelOnFirstLine == 1 || strcmp(lLabel, nullstring) != 0) throwError(4, "there's a label that points to .ORIG");
                    else continue;
                } else if (strcmp(lOpcode, nullstring) != 0 || strcmp(lLabel,nullstring) != 0) labelOnFirstLine = 1;
                else continue;
            }
            else { // orig == 1
                if (strcmp(lLabel, nullstring) == 0) { // label is null

                    if (strcmp(lOpcode, nullstring) == 0) { // label is null & opcode is null
                        if (strcmp(lArg1, nullstring) == 0 && strcmp(lArg2, nullstring) == 0 && strcmp(lArg3, nullstring) == 0 && strcmp(lArg4, nullstring) == 0) continue;
                        else throwError(2, "missing opcode");
                    }

                    else { // label is null & opcode is found!
                        line_counter = line_counter + 1;

                        if (strcmp(newLabel, nullstring) == 0) {  // newLabel is null
                            if (strcmp(lOpcode, ".end") == 0) {
                                end = 1;
                                continue;
                            } else continue;
                        }

                        else { // newLabel is not null, address of associated opcode found!
                            if (strcmp(lOpcode, ".end") == 0) throwError(2, "invalid opcode");
                            insertLabel(newLabel, line_counter);
                            *newLabel = '\0';
                            continue;
                        }
                    }
                }
                else { // label is found!

                    // check if label is valid
                    if(validLabel(lLabel) == 1 && containsLabel(lLabel) == -1 && strcmp(newLabel, nullstring) == 0) {
                        strcpy(newLabel, lLabel);
                    }
                    else {
                        if (strcmp(lOpcode, nullstring) == 0) throwError(2, "invalid opcode");
                        else if (!isOpcode(lOpcode)) throwError(2, "invalid opcode");
                        else throwError(4, "invalid label");
                    }

                    // check for opcode
                    if(strcmp(lOpcode, nullstring) != 0) {
                        if (strcmp(lOpcode, ".end") == 0) throwError(2, "invalid opcode");
                        line_counter = line_counter + 1;
                        insertLabel(newLabel, line_counter);
                        *newLabel = '\0';
                        continue;
                    }
                    else { // no opcode, but label was found
                        if (strcmp(lArg1, nullstring) == 0 && strcmp(lArg2, nullstring) == 0 && strcmp(lArg3, nullstring) == 0 && strcmp(lArg4, nullstring) == 0) continue;
                        else throwError(2, "missing opcode");
                    }
                }
            }
        }
    } while( lRet != DONE && end == 0 );

    // Throw error if there is no ".ORIG" or ".END" in the assembly file
    if ((orig == 0 || end == 0) && lRet == DONE) throwError(4, ".ORIG or .END is missing from input assembly file");


    /* Reset vars for second parse, set input file reader to the top of the file */
    fclose(infile);
    
    infile = fopen(argv[1], "r");
    //infile = fopen("C:/Users/ishac/Documents/Comp Arch Labs/Lab1/input.asm", "r");
    orig = 0;
    line_counter = 0;
    end = 0;


    /* Translate Assembly and Print to Output File , as we parse the input file for the second time */
    do {
        lRet = readAndParse( infile, lLine, &lLabel,
                             &lOpcode, &lArg1, &lArg2, &lArg3, &lArg4 );
        if( lRet != DONE && lRet != EMPTY_LINE ) {
            if (orig == 0) {
                if (strcmp(lOpcode,".orig") == 0) {
                    orig = 1;
                    if (strcmp(lLabel, nullstring) != 0) throwError(4, "there's a label that points to .ORIG");
                    dotorig(lArg1, lArg2, lArg3, lArg4);
                } else continue;
            }
            else {  // orig == 1
                if (strcmp(lOpcode, nullstring) == 0) continue;
                line_counter++;

                if (strcmp(lOpcode,"add") == 0) add(lArg1, lArg2, lArg3, lArg4);
                else if (strcmp(lOpcode, "and") == 0) and(lArg1, lArg2, lArg3, lArg4);
                else if (lOpcode[0] == 'b' && lOpcode[1] == 'r') br(lOpcode, lArg1, lArg2, lArg3, lArg4, line_counter);
                else if (strcmp(lOpcode, "jmp") == 0) jmp(lArg1, lArg2, lArg3, lArg4);
                else if (strcmp(lOpcode, "jsr") == 0) jsr(lArg1, lArg2, lArg3, lArg4, line_counter);
                else if (strcmp(lOpcode, "jsrr") == 0) jsrr(lArg1, lArg2, lArg3, lArg4);
                else if (strcmp(lOpcode, "ldb") == 0) ld(1, lArg1, lArg2, lArg3, lArg4);
                else if (strcmp(lOpcode, "ldw") == 0) ld(0, lArg1, lArg2, lArg3, lArg4);
                else if (strcmp(lOpcode, "lea") == 0) lea(lArg1, lArg2, lArg3, lArg4, line_counter);
                else if (strcmp(lOpcode, "nop") == 0) nop(lArg1, lArg2, lArg3, lArg4);
                else if (strcmp(lOpcode, "not") == 0) not(lArg1, lArg2, lArg3, lArg4);
                else if (strcmp(lOpcode, "ret") == 0) ret(lArg1, lArg2, lArg3, lArg4);
                else if (strcmp(lOpcode, "lshf") == 0) lshf(lArg1,lArg2, lArg3, lArg4);
                else if (strcmp(lOpcode, "rshfl") == 0) rshfl(lArg1,lArg2, lArg3, lArg4);
                else if (strcmp(lOpcode, "rshfa") == 0) rshfa(lArg1,lArg2, lArg3, lArg4);
                else if (strcmp(lOpcode, "rti") == 0) rti(lArg1, lArg2, lArg3, lArg4);
                else if (strcmp(lOpcode, "stb") == 0) st(1, lArg1, lArg2, lArg3, lArg4);
                else if (strcmp(lOpcode, "stw") == 0) st(0, lArg1, lArg2, lArg3, lArg4);
                else if (strcmp(lOpcode, "trap") == 0) trap(lArg1, lArg2, lArg3, lArg4);
                else if (strcmp(lOpcode, "xor") == 0) xor(lArg1, lArg2, lArg3, lArg4);
                else if (strcmp(lOpcode, ".fill") == 0) dotfill(lArg1, lArg2, lArg3, lArg4);
                else if (strcmp(lOpcode, "halt") == 0) halt(lArg1, lArg2, lArg3, lArg4);
                else if (strcmp(lOpcode, ".end") == 0) {
                    if (strcmp(lArg1, nullstring) != 0 || strcmp(lArg2, nullstring) != 0 || strcmp(lArg3, nullstring) != 0 || strcmp(lArg4, nullstring) != 0)
                        throwError(4, "unexpected operand");
                    if (strcmp(lLabel, nullstring) != 0) throwError(4, "invalid label");
                    end = 1;
                    continue;
                }
                else throwError(2, "invalid opcode");
            }
        }
    } while( lRet != DONE && end == 0 );


    /* close source files */
    fclose(infile);
    fclose(outfile);

    return 0;
}