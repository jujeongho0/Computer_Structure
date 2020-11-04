#include <stdio.h>

#include "util.h"
#include "parse.h"

int text_size;
int data_size;

instruction parsing_instr(const char *buffer, const int index)
{
    instruction instr;
    
    char op_temp[7],rs_temp[6],rt_temp[6],rd_temp[6],shamt_temp[6],func_temp[6],imm_temp[17],target_temp[27];
    strncpy(op_temp,buffer,6);    
    op_temp[6] = '\0';
    instr.opcode = (int)strtol(op_temp,NULL,2);
    
    if(instr.opcode == 0x0)//R type
    {
        strncpy(rs_temp,buffer+6,5);
        rs_temp[5] = '\0';
        instr.r_t.r_i.rs = (int)strtol(rs_temp,NULL,2);
        strncpy(rt_temp,buffer+11,5);
        rt_temp[5] = '\0';
        instr.r_t.r_i.rt = (int)strtol(rt_temp,NULL,2);
        strncpy(rd_temp,buffer+16,5);
        rd_temp[5] = '\0';
        instr.r_t.r_i.r_i.r.rd = (int)strtol(rd_temp,NULL,2);
        strncpy(shamt_temp,buffer+21,5);
        shamt_temp[5] = '\0';
        instr.r_t.r_i.r_i.r.shamt = (int)strtol(shamt_temp,NULL,2);
        strncpy(func_temp,buffer+26,6);
        func_temp[6] = '\0';
        instr.func_code = (int)strtol(func_temp,NULL,2);        
    }

    else if(instr.opcode == 0x4 || instr.opcode == 0x5||instr.opcode == 0x9 || instr.opcode == 0xc ||
    instr.opcode == 0xf || instr.opcode == 0xd || instr.opcode == 0xb || instr.opcode == 0x23 || instr.opcode == 0x2b)//I type
    {
        strncpy(rs_temp,buffer+6,5);
        rs_temp[5] = '\0';
        instr.r_t.r_i.rs = (int)strtol(rs_temp,NULL,2);
        strncpy(rt_temp,buffer+11,5);
        rt_temp[5] = '\0';
        instr.r_t.r_i.rt = (int)strtol(rt_temp,NULL,2);
        strncpy(imm_temp,buffer+16,16);
        imm_temp[16] = '\0';
        instr.r_t.r_i.r_i.imm = (int)strtol(imm_temp,NULL,2);        
    }       

    else//J type
    {
        strncpy(target_temp,buffer+6,26);
        target_temp[26]='\0';
        instr.r_t.target = (int)strtol(target_temp,NULL,2);        
    }    
    
    return instr;
}

void parsing_data(const char *buffer, const int index)
{        
    mem_write_32(MEM_DATA_START + index,(int)strtol(buffer,NULL,2));    
}

void print_parse_result()
{
    int i;
    printf("Instruction Information\n");

    for(i = 0; i < text_size/4; i++)
    {
        printf("INST_INFO[%d].value : %x\n",i, INST_INFO[i].value);
        printf("INST_INFO[%d].opcode : %d\n",i, INST_INFO[i].opcode);

       switch(INST_INFO[i].opcode)
        {
            //Type I
            case 0x9:      //(0x001001)ADDIU
            case 0xc:      //(0x001100)ANDI
            case 0xf:      //(0x001111)LUI   
            case 0xd:      //(0x001101)ORI
            case 0xb:      //(0x001011)SLTIU
            case 0x23:      //(0x100011)LW
            case 0x2b:      //(0x101011)SW
            case 0x4:      //(0x000100)BEQ
            case 0x5:      //(0x000101)BNE
                printf("INST_INFO[%d].rs : %d\n",i, INST_INFO[i].r_t.r_i.rs);
                printf("INST_INFO[%d].rt : %d\n",i, INST_INFO[i].r_t.r_i.rt);
                printf("INST_INFO[%d].imm : %d\n",i, INST_INFO[i].r_t.r_i.r_i.imm);
                break;

            //TYPE R
            case 0x0:      //(0x000000)ADDU, AND, NOR, OR, SLTU, SLL, SRL, SUBU  if JR
                printf("INST_INFO[%d].func_code : %d\n",i, INST_INFO[i].func_code);
                printf("INST_INFO[%d].rs : %d\n",i, INST_INFO[i].r_t.r_i.rs);
                printf("INST_INFO[%d].rt : %d\n",i, INST_INFO[i].r_t.r_i.rt);
                printf("INST_INFO[%d].rd : %d\n",i, INST_INFO[i].r_t.r_i.r_i.r.rd);
                printf("INST_INFO[%d].shamt : %d\n",i, INST_INFO[i].r_t.r_i.r_i.r.shamt);
                break;

            //TYPE J
            case 0x2:      //(0x000010)J
            case 0x3:      //(0x000011)JAL
                printf("INST_INFO[%d].target : %d\n",i, INST_INFO[i].r_t.target);
                break;

            default:
                printf("Not available instruction\n");
                assert(0);
        }
    }

    printf("Memory Dump - Text Segment\n");
    for(i = 0; i < text_size; i+=4)
        printf("text_seg[%d] : %x\n", i, mem_read_32(MEM_TEXT_START + i));
    for(i = 0; i < data_size; i+=4)
        printf("data_seg[%d] : %x\n", i, mem_read_32(MEM_DATA_START + i));
    printf("Current PC: %x\n", CURRENT_STATE.PC);
}