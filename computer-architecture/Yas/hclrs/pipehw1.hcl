#Robert Atticus Owens (rao7utn)
#pipehw1.hcl
#On my honor as a student I have neither
#given nor received unathorized aid on this assignemnt
#10/19/2021

########## the PC Pipline Registers and condition codes registers ############
register fF{
	pc:64 = 0;
	icode:4 = NOP;
	ifun:4 = 0;
	Stat:3 = STAT_AOK;
	rA:4 = REG_NONE;
	rB:4 = REG_NONE;
	valC:64 = 0xBAD;
}

register fD{
	icode:4 = NOP;
	ifun:4 = 0;
	Stat:3 = STAT_AOK;
	rA:4 = REG_NONE;
	rB:4 = REG_NONE;
	valC:64 = 0xBAD;
}

register dE{
	icode:4 = NOP;
	ifun:4 = 0;
	Stat:3 = STAT_AOK;
	valA:64 = 0xBAD;
	valB:64 = 0xBAD;
	valC:64 = 0xBAD;
	dstE:4 = REG_NONE;
}

register eM{
	icode:4 = NOP;
	Stat:3 = STAT_AOK;
	valA:64 = 0xBAD;
	valB:64 = 0xBAD;
	dstE:4 = REG_NONE;
	valE:64 = 0xBAD;
	conditionsMet:1 = 0;
}

register mW{
	icode:4 = NOP;
	Stat:3 = STAT_AOK;
	valA:64 = 0xBAD;
	valB:64 = 0xBAD;
	dstE:4 = REG_NONE;
	valE:64 = 0xBAD;
	conditionsMet:1=0;
	
}

register cC{
	SF:1 = 0;
	ZF:1 = 1;
}

########## Fetch #############

pc = F_pc;

f_icode = i10bytes[4..8];
f_ifun = i10bytes[0..4];
f_rA = i10bytes[12..16];
f_rB = i10bytes[8..12];

f_Stat = [
	Stat != STAT_AOK : STAT_HLT;
	f_icode == HALT : STAT_HLT;
	f_icode > 0xb : STAT_INS;
	1 : STAT_AOK;
];

stall_F = [
	Stat == STAT_AOK : false;
	1 : true;
];

f_valC = [
	f_icode in { CALL, JXX } : i10bytes[8..72];
	1 : i10bytes[16..80];
];

########## Decode #############

reg_srcA = D_rA;
reg_srcB = D_rB;

d_dstE = [
	d_icode in {RRMOVQ, CMOVXX, OPQ, RMMOVQ, MRMOVQ, PUSHQ, POPQ, IRMOVQ} : D_rB;
	1 : REG_NONE;
]; 

d_valA = [
	reg_srcA == e_dstE : e_valE;
	reg_srcA == m_dstE : m_valE;
	reg_srcA == W_dstE : W_valE;
	1 : reg_outputA;
];

d_valB = [
	reg_srcB == e_dstE : e_valE;
	reg_srcB == m_dstE : m_valE;
	reg_srcB == W_dstE : W_valE;
	1 : reg_outputB;
];

d_Stat = D_Stat;
d_icode = D_icode;
d_ifun = D_ifun;
d_valC = D_valC;

########## Execute #############

e_dstE = [
	e_icode == CMOVXX && e_conditionsMet != true : REG_NONE;
	1: E_dstE;
];

e_valE = [
	e_icode == OPQ && E_ifun == ADDQ : e_valA + e_valB;
	e_icode == OPQ && E_ifun == SUBQ : e_valB - e_valA;
	e_icode == OPQ && E_ifun == ANDQ : e_valA & e_valB;
	e_icode == OPQ && E_ifun == XORQ : e_valA ^ e_valB;
	e_icode == IRMOVQ : E_valC;
	e_icode == RRMOVQ : E_valA;
	1 : 0;
];

c_ZF = e_valE == 0;
c_SF = e_valE >= 0x8000000000000000;
stall_C = e_icode != OPQ;

e_conditionsMet = [
	E_ifun == ALWAYS : true;
	E_ifun == LE : C_SF || C_ZF;
	E_ifun == LT : C_SF;
	E_ifun == EQ : C_ZF;
	E_ifun == NE : !C_ZF;
	E_ifun == GE : !C_SF;
	E_ifun == GT : !C_SF && !C_ZF;
	1 : false;
];

e_Stat = E_Stat;
e_icode = E_icode;
e_valA = E_valA;
e_valB = E_valB;

########## Memory #############

m_Stat = M_Stat;
m_icode = M_icode;
m_valA = M_valA;
m_valB = M_valB;
m_dstE = M_dstE;
m_valE = M_valE;
m_conditionsMet = M_conditionsMet;

########## Writeback #############

# destination selection
reg_dstE = [
	W_icode in {RRMOVQ} && W_conditionsMet : W_dstE;
	W_icode in {IRMOVQ, OPQ} : W_dstE;
	1 : REG_NONE;
];

reg_inputE = [ # unlike book, we handle the "forwarding" actions (something + 0) here
	W_icode == RRMOVQ : W_valA;
	W_icode in {IRMOVQ, OPQ} : W_valE;
    1: 0xBAD;
];

########## PC and Status updates #############

Stat = W_Stat;

wire offset:64, valP:64;
offset = [
	f_icode in { HALT, NOP, RET } : 1;
	f_icode in { RRMOVQ, OPQ, PUSHQ, POPQ } : 2;
	f_icode in { JXX, CALL } : 9;
	1 : 10;
];
valP = F_pc + offset;

f_pc = [
	f_Stat == STAT_HLT : F_pc;
	1:valP;
];