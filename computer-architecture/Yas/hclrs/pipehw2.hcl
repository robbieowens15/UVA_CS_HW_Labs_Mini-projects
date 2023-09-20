# Robert Atticus Owens (rao7utn)
# pipehw2 10/27/2021
# On my honor as a student I have neither given
# nor received aid on this assignment.

########## the PC and condition codes registers #############
register fF { 
    predPC:64 = 0;
}

register cC {
    SF : 1 = 1;
    ZF : 1 = 1;
}

register fD {
    icode:4 = NOP;
    ifun:4 = 0;
	valC:64 = 0;	
	valP:64 = 0;
	Stat:3 = STAT_AOK;
    rA:4 = REG_NONE;
    rB: 4 = REG_NONE;
}

register dE {
    icode:4 = NOP;
    ifun:4 = 0;
	valA:64 = 0;
    valB:64 = 0;
	valC:64 = 0;	
	Stat:3 = STAT_AOK;
    dstE:4 = REG_NONE;
	dstM:4 = REG_NONE;

}

register eM {
    Stat:3 = STAT_AOK;
    icode:4 = NOP;
    valE:64 = 0;
    valA:64 = 0;
    dstE:4 = REG_NONE;
	dstM:4 = REG_NONE;
    cValid:1 = 1;
    regLoadUse:1 = 0;
}

register mW {
    Stat:3 = STAT_AOK;
    icode:4 = NOP;
    valE:64 = 0;
	valM:64 = 0;
    dstE:4 = 0;
	dstM:4 = REG_NONE;
    cValid:1 = 1;
}

########## PC and Status updates #############

Stat = W_Stat;

f_predPC = [
	e_icode == JXX && e_cValid == 0 : e_valA;
	f_icode == JXX : f_valC;
	m_icode == RET : m_valM;
	f_Stat == STAT_HLT || loadUse == 1 : F_predPC;
	1 : f_valP;
];

########## Fetch #############

pc = F_predPC;

f_icode = i10bytes[4..8];
f_ifun = i10bytes[0..4];
f_rA = i10bytes[12..16];
f_rB = i10bytes[8..12];

f_valC = [
	f_icode in { JXX } : i10bytes[8..72];
	1 : i10bytes[16..80];
];

wire incriment:64;
incriment = [
	f_icode in { HALT, NOP, RET } : 1;
	f_icode in { RRMOVQ, OPQ, PUSHQ, POPQ } : 2;
	f_icode in { JXX, CALL } : 9;
	1 : 10;
];

f_valP = F_predPC + incriment;

f_Stat = [
	f_icode == HALT || Stat != STAT_AOK : STAT_HLT;
	f_icode > 0xb : STAT_INS;
	1 : STAT_AOK;
];

stall_F = [
    Stat == STAT_AOK || loadUse == 0 : false;
    1 : true;
];

bubble_D = [
	e_icode == JXX && e_cValid == 0 : 1;
	1:0;
];

########## Decode #############

d_icode = D_icode;
d_ifun = D_ifun;
d_Stat = D_Stat;
d_valC = D_valC;

reg_srcA = [
	d_icode in {IRMOVQ, HALT, NOP, RET, POPQ} : REG_NONE;
	1 : D_rA;
];

reg_srcB = [
	d_icode in {RET, CALL, PUSHQ, POPQ} : 4;
	d_icode in {HALT, NOP, JXX} : REG_NONE;
	1 : D_rB;
];

#Forward values
d_valA = [
	reg_srcA == e_dstE : e_valE;
	reg_srcA == m_dstE : m_valE;
    reg_srcA == W_dstE : W_valE;
	reg_srcA == m_dstM : m_valM;
    reg_srcA == W_dstM : W_valM;
	d_icode == JXX : D_valP; 
	d_icode == CALL : D_valP;
	1 : reg_outputA;
];

#Forward values
d_valB = [
    reg_srcB == e_dstE : e_valE;
	reg_srcB == m_dstE : m_valE;	
    reg_srcB == W_dstE : W_valE;
	reg_srcB == m_dstM : m_valM;
    1 : reg_outputB;
];

d_dstE = [
    D_icode in {RRMOVQ, OPQ, CMOVXX, IRMOVQ} : D_rB;
    D_icode == MRMOVQ : D_rA;
	D_icode in {CALL, RET, POPQ, PUSHQ} : 4;
    1 : REG_NONE;
];

d_dstM = [
	D_icode == POPQ : D_rA;
	1:REG_NONE;
];

stall_D = loadUse;

########## Execute #############

wire loadUse:1;
loadUse = [
    E_icode == MRMOVQ && E_dstE == D_rB : 1;
	e_icode == MRMOVQ && D_icode in {RRMOVQ, RMMOVQ, OPQ, POPQ, PUSHQ} && E_dstE == D_rA : 1;
	e_icode == POPQ && D_icode == RRMOVQ && E_dstM == D_rA : 1;
	e_icode == POPQ && D_icode in { MRMOVQ, OPQ} && E_dstM == D_rA : 1;
	e_icode == POPQ && D_icode in {RRMOVQ, RMMOVQ, OPQ, POPQ, PUSHQ} && E_dstM == D_rA : 1;
	e_icode == POPQ && D_icode in {RMMOVQ, MRMOVQ, OPQ} && E_dstM == D_rB : 1;
    1 : 0;
];

e_regLoadUse = loadUse;

bubble_E = [
	e_icode == JXX && e_cValid == 0 : 1;
	1: loadUse;
];

e_Stat = E_Stat;
e_icode = E_icode;
e_dstE = [
    E_icode == CMOVXX && e_cValid != true : REG_NONE;
    1 : E_dstE;
];   

e_valA = E_valA;

wire ALU_Out:64, ALU_In1:64, ALU_In2:64;

ALU_In1 = [
	E_icode == OPQ : E_valA;
	E_icode == RMMOVQ || E_icode == MRMOVQ : E_valC;
	E_icode in {PUSHQ, POPQ, CALL, RET} : 0x8;
	1 : 0xBADBADBAD;
];

ALU_In2 = [
	E_icode == OPQ : E_valB;
	E_icode in {RMMOVQ, MRMOVQ, PUSHQ, POPQ} : E_valB;
	E_icode in {CALL, RET} : E_valB;
	1: 0xBADBADBAD;
];

ALU_Out = [
	E_icode == OPQ && E_ifun == ADDQ : ALU_In1 + ALU_In2;
	E_icode == OPQ && E_ifun == SUBQ : ALU_In2 - ALU_In1;
	E_icode == OPQ && E_ifun == XORQ : ALU_In1 ^ ALU_In2;
	E_icode == OPQ && E_ifun == ANDQ : ALU_In1 & ALU_In2;
	E_icode == RMMOVQ || E_icode == MRMOVQ : ALU_In1 + ALU_In2;
	E_icode in {PUSHQ, CALL} : ALU_In2 - ALU_In1;
	E_icode in {POPQ, RET} : ALU_In2 + ALU_In1;
	1 : 0xBADBADBAD;
];

e_valE = [
	E_icode == OPQ || E_icode in {PUSHQ, POPQ, CALL, RET, MRMOVQ, RMMOVQ} : ALU_Out;
	E_icode == IRMOVQ : E_valC;
	E_icode == RRMOVQ : E_valA;
	1 : reg_outputA;
];

c_ZF = e_valE == 0;
c_SF = e_valE >= 0x8000000000000000;
stall_C = E_icode != OPQ;

e_cValid = [
	E_ifun == ALWAYS : true;
	E_ifun == LE : C_SF || C_ZF;
	E_ifun == LT : C_SF;
	E_ifun == EQ : C_ZF;
	E_ifun == NE : !C_ZF;
	E_ifun == GE : !C_SF;
	E_ifun == GT : !C_SF && !C_ZF;
	1 : false;
];

e_dstM = E_dstM;

########## Memory #############

m_Stat = M_Stat;
m_icode = M_icode;
m_valE = [
    M_icode == MRMOVQ : mem_output;
    1 : M_valE;
];

m_valM = [
	M_icode in {POPQ, RET} : mem_output;
	1 : 0xBADBADBAD;
];

m_dstE = M_dstE;
m_dstM = M_dstM;
m_cValid = M_cValid;

mem_addr = [
	M_icode in {POPQ, RET}  : M_valE - 8;
	1 : M_valE;
];

mem_readbit = [
	M_icode in {MRMOVQ, POPQ, RET} : 1;
	1 : 0;
];

mem_writebit = [
	M_icode in {RMMOVQ, PUSHQ, CALL} : 1;
	1 : 0;
];

mem_input = [
	M_icode in {RMMOVQ, PUSHQ, CALL} : M_valA;
	1 : 0xBADBADBAD;
];

########## Writeback #############

reg_inputM = W_valM;

reg_dstM = [
	W_icode == POPQ : W_dstM;
	1 : REG_NONE;
];

reg_dstE = [
	W_icode == RRMOVQ && W_cValid == 1 : W_dstE;
    W_icode in {OPQ, IRMOVQ, MRMOVQ, PUSHQ, POPQ, CALL, RET} : W_dstE;
	1 : REG_NONE;
];

reg_inputE = [
    W_icode in {IRMOVQ, OPQ, RRMOVQ, MRMOVQ, PUSHQ, POPQ, CALL, RET} : W_valE;
    1 : 0xBADBADBAD;
];
