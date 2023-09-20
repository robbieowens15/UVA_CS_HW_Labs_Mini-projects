########## the PC and condition codes registers #############
register fF { 
    pc:64 = 0;
    # icode:4 = NOP;
    # ifun:4 = 0;
    # Stat:3 = STAT_AOK;
    # rA:4 = REG_NONE;
    # rB: 4 = REG_NONE;
    # valC:64 = 0; 
}

register cC {
    SF : 1 = 1;
    ZF : 1 = 1;
}


########## Fetch #############
pc = F_pc;

f_icode = i10bytes[4..8];
f_ifun = i10bytes[0..4];
f_rA = i10bytes[12..16];
f_rB = i10bytes[8..12];

#Stat = f_Stat;

f_valC = [
	f_icode in { JXX } : i10bytes[8..72];
	1 : i10bytes[16..80];
];

wire offset:64;
offset = [
	f_icode in { HALT, NOP, RET } : 1;
	f_icode in { RRMOVQ, OPQ, PUSHQ, POPQ } : 2;
	f_icode in { JXX, CALL } : 9;
	1 : 10;
];
f_valP = F_pc + offset;

wire nextPc:64;
nextPc = f_valP;

f_Stat = [
	f_icode == HALT : STAT_HLT;
    Stat != STAT_AOK : STAT_HLT;
	f_icode > 0xb : STAT_INS;
	1 : STAT_AOK;
];

stall_F = [
    loadUse == 1 : true;
    Stat == STAT_AOK : false;
    1 : true;
];


########## Decode #############

register fD {
    icode:4 = NOP;
    ifun:4 = 0;
	valC:64 = 0;	
	valP:64 = 0;
	Stat:3 = STAT_AOK;
    rA:4 = REG_NONE;
    rB: 4 = REG_NONE;
}

# source selection
d_icode = D_icode;
d_ifun = D_ifun;
d_Stat = D_Stat;
d_valC = D_valC;
#d_valP = D_valP;
# d_srcA = [
# 	D_icode in {RRMOVQ, OPQ, CMOVXX} : D_rA;
# 	1 : REG_NONE;
# ];
# reg_srcA = d_srcA;
# d_srcB = [
#     D_icode in {RRMOVQ, OPQ, CMOVXX, IRMOVQ} : D_rB;
#     1: REG_NONE;
# ];
# reg_srcB = d_srcB;
reg_srcA = D_rA;
reg_srcB = D_rB;
d_valA = [
	(reg_srcA == e_dstE) : e_valE;
	(reg_srcA == m_dstE) : m_valE;
    (reg_srcA == W_dstE) : W_valE;
	1 : reg_outputA;
];
d_valB = [
    (reg_srcB == e_dstE) : e_valE;
	(reg_srcB == m_dstE) : m_valE;	
    (reg_srcB == W_dstE) : W_valE;
    1 : reg_outputB;
];
d_dstE = [
    D_icode in {RRMOVQ, OPQ, CMOVXX, IRMOVQ} : D_rB;
    D_icode == MRMOVQ : D_rA;
    1 : REG_NONE;
];

stall_D = loadUse;



########## Execute #############
register dE {
    icode:4 = NOP;
    ifun:4 = 0;
	valA:64 = 0;
    valB:64 = 0;
	valC:64 = 0;	
	Stat:3 = STAT_AOK;
    dstE:4 = REG_NONE;
    # valP:64 = 0;
    # dstM:4 = 0;
    #srcA:4 = 0;
    #srcB:4 = 0;
}

wire loadUse:1;
loadUse = [
    E_icode == MRMOVQ && (e_dstE == D_rB) : 1;
    1 : 0;
];
e_regLoadUse = loadUse;

bubble_E = loadUse;

e_Stat = E_Stat;
e_icode = E_icode;
e_dstE = [
    (E_icode == CMOVXX && e_cValid != true) : REG_NONE;
    1 : E_dstE;
];   
# e_dstM = E_dstM;
e_valA = E_valA;
#e_valP = E_valP;

wire ALU_Out:64, ALU_In1:64, ALU_In2:64;

ALU_In1 = [
	E_icode == OPQ : E_valA;
	(E_icode == RMMOVQ || E_icode == MRMOVQ) : E_valC;
	E_icode in {PUSHQ, POPQ, CALL, RET} : 0x8;
	1 : 0xbad;
];

ALU_In2 = [
	E_icode == OPQ : E_valB;
	E_icode in {RMMOVQ, MRMOVQ, PUSHQ, POPQ} : E_valB;
	E_icode in {CALL, RET} : E_valA;
	1: 0xbad;
];

ALU_Out = [
	(E_icode == OPQ && E_ifun == ADDQ) : ALU_In1 + ALU_In2;
	(E_icode == OPQ && E_ifun == SUBQ) : ALU_In2 - ALU_In1;
	(E_icode == OPQ && E_ifun == XORQ) : ALU_In1 ^ ALU_In2;
	(E_icode == OPQ && E_ifun == ANDQ) : ALU_In1 & ALU_In2;
	(E_icode == RMMOVQ || E_icode == MRMOVQ) : ALU_In1 + ALU_In2;
	E_icode in {PUSHQ, CALL} : ALU_In2 - ALU_In1;
	E_icode in {POPQ, RET} : ALU_In2 + ALU_In1;
	1 : 0xbad;
];

e_valE = [
	(E_icode == OPQ && E_ifun == ADDQ) : ALU_Out;
	(E_icode == OPQ && E_ifun == SUBQ) : ALU_Out;
	(E_icode == OPQ && E_ifun == XORQ) : ALU_Out;
	(E_icode == OPQ && E_ifun == ANDQ) : ALU_Out;
	E_icode in {PUSHQ, POPQ, CALL, RET} : ALU_Out;
	E_icode == IRMOVQ : E_valC;
	(E_icode == RRMOVQ) : E_valA;
    E_icode in {MRMOVQ, RMMOVQ} : ALU_Out;
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


########## Memory #############
register eM {
    Stat:3 = STAT_AOK;
    icode:4 = 0;
    valE:64 = 0;
    valA:64 = 0;
    dstE:4 = REG_NONE;
    # valP:64 = 0;
    cValid:1 = 1;
    # dstM:4 = 0;
    # valP:64 = 0;
    regLoadUse:1 = 0;
}

m_Stat = M_Stat;
m_icode = M_icode;
m_valE = [
    M_icode == MRMOVQ : mem_output;
    1 : M_valE;
];
# m_valM = M_valA;
m_dstE = M_dstE;
#m_valP = M_valP;
m_cValid = M_cValid;
# m_dstM = M_dstM;

mem_addr = [
	# M_icode in {POPQ, RET} : M_valB;
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
	M_icode in {RMMOVQ, PUSHQ} : M_valA;
	#M_icode == CALL :  M_valP;
	1 : 0xbad;
];



########## Writeback #############
register mW {
    Stat:3 = STAT_AOK;
    icode:4 = 0;
    valE:64 = 0;
    # valM:64 = 0;
    dstE:4 = 0;
    # valP:64 = 0;
    cValid:1 = 1;
    # dstM:4 = 0;
}

# destination selection
reg_dstE = [
	(W_icode == RRMOVQ && W_cValid == 1) : W_dstE;
    W_icode in {OPQ, IRMOVQ, MRMOVQ} : W_dstE;
	1 : REG_NONE;
];

reg_inputE = [
    W_icode in {IRMOVQ, OPQ, RRMOVQ, MRMOVQ} : W_valE;
    1 : 0xbad;
];
# W_valE;
#   unlike book, we handle the "forwarding" actions (something + 0) here
# 	W_icode == RRMOVQ : W_valE;
# 	W_icode in {IRMOVQ} : W_valE;
#     1 : 0xBADBADBAD;
# ];


########## PC and Status updates #############

Stat = W_Stat;

f_pc = [
	f_Stat == STAT_HLT || loadUse == 1 : F_pc;
	1 : nextPc;
];


