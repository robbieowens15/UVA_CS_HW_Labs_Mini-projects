# Robert Owens (rao7utn)
# HW #5 irrr pc
# 9/28/2021

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
wire opcode:8, icode:4;
# two register parameters which may be used in an instruction
wire rA:4, rB:4;
# wire for 64 bit constant parameters like constants and addresses
wire valC:64;

# the x[i..j] means "just the bits between i and j".  x[0..1] is the 
# low-order bit, similar to what the c code "x&1" does; "x&7" is x[0..3]
opcode = i10bytes[0..8];   # first byte read from instruction memory
icode = opcode[4..8];      # top nibble of that byte
rB = i10bytes[8..12];
rA = i10bytes[12..16];

#ValC could always 64bits but could start at 8th bit for JXX instruction
#Or could start at 16 for all other instructions that use valC
valC = [
	icode == JXX : i10bytes[8..72];
	1 : i10bytes[16..80];
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
	icode == CALL : STAT_INS;
	icode == RET : STAT_INS;
	icode > 11 : STAT_INS;
	1             : STAT_AOK;
];

reg_srcA = rA;

reg_dstE = [
	icode in {RRMOVQ, IRMOVQ} : rB;
	1 : REG_NONE;
];
# and a value to write.  Let's decide what to do based on the ifun
reg_inputE = [
	icode == IRMOVQ : valC;
	1 : reg_outputA;
];

# to make progress, we have to update the PC...
wire inc:4;
inc = [
	icode in {HALT, RET, NOP} : 1;
	icode in {RRMOVQ, OPQ, PUSHQ, POPQ} : 2;
	icode == CALL : 9;
	1 : 10;
];

p_pc = [
	icode == JXX : valC;
	1 : P_pc + inc;
];
