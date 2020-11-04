#include <stdio.h>

#include "util.h"
#include "run.h"

instruction* get_inst_info(uint32_t pc)
{
    return &INST_INFO[(pc - MEM_TEXT_START) >> 2];
}

void process_instruction()
{   
    int i = (CURRENT_STATE.PC-MEM_TEXT_START)/4;   
    RUN_BIT = FALSE;
    
  
    
    switch(INST_INFO[i].opcode)
    {        
        case 0x9:      //(0x001001)ADDIU
            
            RUN_BIT = TRUE;
            CURRENT_STATE.PC += 4;
            CURRENT_STATE.REGS[INST_INFO[i].r_t.r_i.rt]=CURRENT_STATE.REGS[INST_INFO[i].r_t.r_i.rs] + INST_INFO[i].r_t.r_i.r_i.imm;            
            break;
        case 0xc:      //(0x001100)ANDI
            
            RUN_BIT = TRUE;
            CURRENT_STATE.PC += 4;
            CURRENT_STATE.REGS[INST_INFO[i].r_t.r_i.rt]=CURRENT_STATE.REGS[INST_INFO[i].r_t.r_i.rs] & INST_INFO[i].r_t.r_i.r_i.imm;            
            break;
        case 0xf:      //(0x001111)LUI
            
            RUN_BIT = TRUE;
            CURRENT_STATE.PC += 4;
           CURRENT_STATE.REGS[INST_INFO[i].r_t.r_i.rt]=INST_INFO[i].r_t.r_i.r_i.imm *16*16*16*16;           
            break;
        case 0xd:      //(0x001101)ORI
            
            RUN_BIT = TRUE;
            CURRENT_STATE.PC += 4;
            CURRENT_STATE.REGS[INST_INFO[i].r_t.r_i.rt]=CURRENT_STATE.REGS[INST_INFO[i].r_t.r_i.rs] | INST_INFO[i].r_t.r_i.r_i.imm;            
            break;
        case 0xb:      //(0x001011)SLTIU
            
            RUN_BIT = TRUE;
            CURRENT_STATE.PC += 4;
            if(CURRENT_STATE.REGS[INST_INFO[i].r_t.r_i.rs] < INST_INFO[i].r_t.r_i.r_i.imm)
                CURRENT_STATE.REGS[INST_INFO[i].r_t.r_i.rt] = 1;
            else
                CURRENT_STATE.REGS[INST_INFO[i].r_t.r_i.rt] = 0;            
            break;            
        case 0x23:      //(0x100011)LW
            
            RUN_BIT = TRUE;
            CURRENT_STATE.PC += 4;
            CURRENT_STATE.REGS[INST_INFO[i].r_t.r_i.rt] = mem_read_32(CURRENT_STATE.REGS[INST_INFO[i].r_t.r_i.rs]  + INST_INFO[i].r_t.r_i.r_i.imm);
            break;
        case 0x2b:      //(0x101011)SW
            
            RUN_BIT = TRUE;
            CURRENT_STATE.PC += 4;
            mem_write_32(CURRENT_STATE.REGS[INST_INFO[i].r_t.r_i.rs]  + INST_INFO[i].r_t.r_i.r_i.imm, CURRENT_STATE.REGS[INST_INFO[i].r_t.r_i.rt]);
            break;
        case 0x4:      //(0x000100)BEQ
            
            RUN_BIT = TRUE;
            CURRENT_STATE.PC += 4;
            if(CURRENT_STATE.REGS[INST_INFO[i].r_t.r_i.rt] == CURRENT_STATE.REGS[INST_INFO[i].r_t.r_i.rs])
                CURRENT_STATE.PC = CURRENT_STATE.PC + (INST_INFO[i].r_t.r_i.r_i.imm * 4);            
            break;    
        case 0x5:      //(0x000101)BNE
            
            RUN_BIT = TRUE;
            CURRENT_STATE.PC += 4;
            if(CURRENT_STATE.REGS[INST_INFO[i].r_t.r_i.rt] != CURRENT_STATE.REGS[INST_INFO[i].r_t.r_i.rs])
                CURRENT_STATE.PC = CURRENT_STATE.PC + (INST_INFO[i].r_t.r_i.r_i.imm * 4);            
            break;
        //TYPE R
        case 0x0:      //(0x000000)ADDU, AND, NOR, OR, SLTU, SLL, SRL, SUBU  if JR
        if(INST_INFO[i].r_t.r_i.r_i.r.shamt > 0)
        {            
            if(INST_INFO[i].func_code == 2)//SRL
            {
                
                RUN_BIT = TRUE;
                CURRENT_STATE.PC += 4;
                CURRENT_STATE.REGS[INST_INFO[i].r_t.r_i.r_i.r.rd]=CURRENT_STATE.REGS[INST_INFO[i].r_t.r_i.rt] >> INST_INFO[i].r_t.r_i.r_i.r.shamt;                
                break;
            }
            else//SLL
            {
                
                RUN_BIT = TRUE;
                CURRENT_STATE.PC += 4;
                CURRENT_STATE.REGS[INST_INFO[i].r_t.r_i.r_i.r.rd]=CURRENT_STATE.REGS[INST_INFO[i].r_t.r_i.rt] << INST_INFO[i].r_t.r_i.r_i.r.shamt;                
                break;
            }
        }
        else{            
            if(INST_INFO[i].func_code == 33)//ADDU
            {
                
                RUN_BIT = TRUE;
                CURRENT_STATE.PC += 4;                
                CURRENT_STATE.REGS[INST_INFO[i].r_t.r_i.r_i.r.rd]=CURRENT_STATE.REGS[INST_INFO[i].r_t.r_i.rs] + CURRENT_STATE.REGS[INST_INFO[i].r_t.r_i.rt];               
                break;
            }
            else if(INST_INFO[i].func_code == 36)//AND
            {
                
                RUN_BIT = TRUE;
                CURRENT_STATE.PC += 4;
                CURRENT_STATE.REGS[INST_INFO[i].r_t.r_i.r_i.r.rd]=CURRENT_STATE.REGS[INST_INFO[i].r_t.r_i.rs] & CURRENT_STATE.REGS[INST_INFO[i].r_t.r_i.rt];                
                break;
            }
            else if(INST_INFO[i].func_code == 39)//NOR
            {
                
                RUN_BIT = TRUE;
                CURRENT_STATE.PC += 4;
                CURRENT_STATE.REGS[INST_INFO[i].r_t.r_i.r_i.r.rd]=~(CURRENT_STATE.REGS[INST_INFO[i].r_t.r_i.rs] | CURRENT_STATE.REGS[INST_INFO[i].r_t.r_i.rt]);                
                break;
            }
            else if(INST_INFO[i].func_code == 37)//OR
            {
                
                RUN_BIT = TRUE;
                CURRENT_STATE.PC += 4;
                CURRENT_STATE.REGS[INST_INFO[i].r_t.r_i.r_i.r.rd]=CURRENT_STATE.REGS[INST_INFO[i].r_t.r_i.rs] | CURRENT_STATE.REGS[INST_INFO[i].r_t.r_i.rt];                
                break;
            }
            else if(INST_INFO[i].func_code == 43)//SLTU
            {
                
                RUN_BIT = TRUE;
                CURRENT_STATE.PC += 4;
                if(CURRENT_STATE.REGS[INST_INFO[i].r_t.r_i.rs] < CURRENT_STATE.REGS[INST_INFO[i].r_t.r_i.rt])
                    CURRENT_STATE.REGS[INST_INFO[i].r_t.r_i.r_i.r.rd] = 1;
                else
                    CURRENT_STATE.REGS[INST_INFO[i].r_t.r_i.r_i.r.rd] = 0;                
                break;  
            }
            else if(INST_INFO[i].func_code == 35)//SUBU
            {
                
                RUN_BIT = TRUE;
                CURRENT_STATE.PC += 4;                
                CURRENT_STATE.REGS[INST_INFO[i].r_t.r_i.r_i.r.rd]=CURRENT_STATE.REGS[INST_INFO[i].r_t.r_i.rs] - CURRENT_STATE.REGS[INST_INFO[i].r_t.r_i.rt];                
                break;
            }
            else if(INST_INFO[i].func_code == 8)//JR
            {
                
                RUN_BIT = TRUE;
                CURRENT_STATE.PC += 4;
                CURRENT_STATE.PC = CURRENT_STATE.REGS[INST_INFO[i].r_t.target];                
                break;
            }
            else
            {                             
                break;
            }
        }
        
        //TYPE J
        case 0x2:      //(0x000010)J
            
            RUN_BIT = TRUE;
            CURRENT_STATE.PC += 4;
            CURRENT_STATE.PC = INST_INFO[i].r_t.target * 4;            
            break;
        case 0x3:      //(0x000011)JAL
            
            RUN_BIT = TRUE;
            CURRENT_STATE.PC += 4;
            CURRENT_STATE.REGS[31] = CURRENT_STATE.PC +4;
            CURRENT_STATE.PC = INST_INFO[i].r_t.target * 4;            
            break;                          
    }            
}