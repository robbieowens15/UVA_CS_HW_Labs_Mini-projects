########## the PC and condition codes registers #############
register fF { pc:64 = 0; }

########## Fetch #############
pc = F_pc;

wire ifun:4, rA:4;

d_icode = i10bytes[4..8];
ifun = i10bytes[0..4];
rA = i10bytes[12..16];
d_rB = i10bytes[8..12];

d_valC = [
	d_icode in { JXX } : i10bytes[8..72];
	1 : i10bytes[16..80];
];

wire offset:64, valP:64;
offset = [
	d_icode in { HALT, NOP, RET } : 1;
	d_icode in { RRMOVQ, OPQ, PUSHQ, POPQ } : 2;
	d_icode in { JXX, CALL } : 9;
	1 : 10;
];
valP = F_pc + offset;


########## PC and Status updates #############

d_Stat = [
	d_icode == HALT : STAT_HLT;
	d_icode > 0xb : STAT_INS;
	1 : STAT_AOK;
];


########## Decode #############

# source selection
reg_srcA = [
	d_icode in {RRMOVQ} : rA;
	1 : REG_NONE;
];
d_rvalA = [
	reg_dstE == reg_srcA && reg_dstE != REG_NONE : reg_inputE;
	1: reg_outputA;
];

########## PIPLEINE ############
########## Pipeline Register ############# (Two stages {Fetch, Decode | Execute, Writeback, Memory})
register dW {
	icode:4 = NOP;
	rB:4 = REG_NONE;
	rvalA:64 = 0xBADBADBAD;
	valC:64 = 0;
	Stat:3 = STAT_AOK;
}

########## Execute #############



########## Memory #############




########## Writeback #############


# destination selection
reg_dstE = [
	W_icode in {IRMOVQ, RRMOVQ} : W_rB;
	1 : REG_NONE;
];

reg_inputE = [ # unlike book, we handle the "forwarding" actions (something + 0) here
	W_icode == RRMOVQ : W_rvalA;
	W_icode in {IRMOVQ} : W_valC;
    1: 0xBADBADBAD;
];

#Update Status
Stat = W_Stat;

f_pc = [
	W_Stat == STAT_HLT : F_pc;
	1 : valP;
];
