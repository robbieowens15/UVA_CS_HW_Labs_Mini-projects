# Robert Owens (rao7utn)
# HW #6 seqhw
# 10/5/2021

#Default Register code block from tiny_pc.hcl
register pP {  
    # our own internal register. P_pc is its output, p_pc is its input.
	pc:64 = 0; # 64-bits wide; 0 is its default value.
	
	# we could add other registers to the P register bank
	# register bank should be a lower-case letter and an upper-case letter, in that order.
	
	# there are also two other signals we can optionally use:
	# "bubble_P = true" resets every register in P to its default value
	# "stall_P = true" causes P_pc not to change, ignoring p_pc's value
} 

# "pc" is a pre-defined input to the instruction memory and is the 
# address to fetch 6 bytes from (into pre-defined output "i10bytes").
pc = P_pc;

# op code is 8 bits and can be split into instruction code and modification flag
wire opcode:8, icode:4, ifun:4;
# two register parameters which may be used in an instruction
wire rA:4, rB:4;
# wire for 64 bit constant parameters like constants and addresses
wire valC:64, valP:64;

# the x[i..j] means "just the bits between i and j".  x[0..1] is the 
# low-order bit, similar to what the c code "x&1" does; "x&7" is x[0..3]
opcode = i10bytes[0..8];   # first byte read from instruction memory
ifun = i10bytes[0..4];
icode = opcode[4..8];      # top nibble of that byte
rB = i10bytes[8..12];
rA = i10bytes[12..16];

#ValC could always 64bits but could start at 8th bit for JXX instruction
#Or could start at 16 for all other instructions that use valC
valC = [
	icode in {JXX, CALL} : i10bytes[8..72];
	icode in {IRMOVQ, RMMOVQ, MRMOVQ} : i10bytes[16..80];
	1: 0xbad;
];

#Used by CALL
valP = P_pc + 9;

wire conditionsMet : 1;
conditionsMet = [
	((icode in {CMOVXX, JXX}) && ifun == 0) : 1;
	((icode in {CMOVXX, JXX}) && ifun == 1) : C_SF || C_ZF;
	((icode in {CMOVXX, JXX}) && ifun == 2) : C_SF;
	((icode in {CMOVXX, JXX}) && ifun == 3) : C_ZF;
	((icode in {CMOVXX, JXX}) && ifun == 4) : !C_ZF;
	((icode in {CMOVXX, JXX}) && ifun == 5) : !C_SF;
	((icode in {CMOVXX, JXX}) && ifun == 6) : !(C_SF || C_ZF);
	1: 0;
];

/* we could also have done i10bytes[4..8] directly, but I wanted to
 * demonstrate more bit slicing... and all 3 kinds of comments      */
// this is the third kind of comment

# named constants can help make code readable
const TOO_BIG = 0xC; # the first unused icode in Y86-64

# some named constants are built-in: the icodes, ifuns, STAT_??? and REG_???


# Stat is a built-in output; STAT_HLT means "stop", STAT_AOK means 
# "continue".  The following uses the mux syntax described in the 
# textbook
Stat = [
	icode == HALT : STAT_HLT;
	icode > 11 : STAT_INS;
	icode < 0 : STAT_INS;
	1 : STAT_AOK;
];

reg_srcA = rA;
reg_srcB = [
	icode in {PUSHQ, POPQ, CALL, RET} : 4; # register val of %rsp
	1 : rB;
];

wire ALU_OUT : 64, ALU_IN1 : 64, ALU_IN2 : 64;
ALU_IN1 = [
	icode == OPQ : reg_outputA;
	icode in {RMMOVQ, MRMOVQ} : valC;
	icode in {PUSHQ, CALL} : -8;
	icode in {POPQ, RET} : 8;
	1 : 0xbad;
];

ALU_IN2 = [
	icode in {OPQ, RMMOVQ, MRMOVQ, PUSHQ, POPQ, CALL, RET} : reg_outputB;
	1 : 0xbad;
];

ALU_OUT = [
	icode == OPQ && ifun == XORQ : ALU_IN1 ^ ALU_IN2;
	icode == OPQ && ifun == ANDQ : ALU_IN1 & ALU_IN2;
	icode == OPQ && ifun == ADDQ : ALU_IN1 + ALU_IN2;
	icode == OPQ && ifun == SUBQ : ALU_IN2 - ALU_IN1;
	icode in {RMMOVQ, MRMOVQ, PUSHQ, POPQ, CALL, RET} : ALU_IN1 + ALU_IN2;
	1 : 0xbad;
];

reg_dstE = [
	icode == CMOVXX && conditionsMet == 0 : REG_NONE;
	icode in {RRMOVQ, CMOVXX, IRMOVQ, OPQ} : rB;
	icode == POPQ : 4; # rsp
	icode == MRMOVQ : rA;
	icode in {PUSHQ, CALL, RET} : 4; # register val of %rsp
	1 : REG_NONE;
];

reg_dstM = [
	icode == POPQ : rA;
	1: REG_NONE;
];

# and a value to write.  Let's decide what to do based on the ifun
reg_inputE = [
	icode in {OPQ, PUSHQ, POPQ, CALL, RET} : ALU_OUT;
	icode == CMOVXX && conditionsMet == 1 : reg_outputA;
	icode == IRMOVQ : valC;
	icode == MRMOVQ : mem_output;
	1 : 0xbad;
];

reg_inputM = [
	icode == POPQ : mem_output;
	1: 0xbad;
];

register cC {
     SF:1 = 0;
     ZF:1 = 1;
}
#If instruction is not OPQ then do not accept new condition codes.
stall_C = (icode != OPQ);
c_ZF = (reg_inputE == 0);
c_SF = (reg_inputE >= 0x8000000000000000);

#Write to Memory?
mem_readbit = [
	icode in {MRMOVQ, POPQ, RET} : 1;
	1 : 0;
];
mem_writebit = [
	icode in {RMMOVQ, PUSHQ, CALL} : 1;
	1: 0;
];
mem_input = [
	icode in {RMMOVQ, PUSHQ} : reg_outputA;
	icode == CALL : valP;
	1 : 0xbad;
];
mem_addr = [
	icode in {RMMOVQ, MRMOVQ, PUSHQ, CALL} : ALU_OUT;
	icode in {POPQ, RET} : reg_outputB;
	1: 0xbad;
];

# to make progress, we have to update the PC...
wire inc:4;
inc = [
	icode in {HALT, RET, NOP} : 1;
	icode in {RRMOVQ, OPQ, PUSHQ, POPQ} : 2;
	icode in {JXX, CALL} : 9;
	1 : 10;
];

p_pc = [
	icode == JXX && conditionsMet == 1 : valC;
	icode == CALL : valC;
	icode == RET : mem_output;
	1 : P_pc + inc;
];
