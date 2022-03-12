#include "cpu.h"

    static const unsigned char CYCLE_TABLE[0x100] = {
    4,12,8,8,4,4,8,4,20,8,8,8,4,4,8,4,4,
    12,8,8,4,4,8,4,12,8,8,8,4,4,8,4,8,
    12,8,8,4,4,8,4,8,8,8,8,4,4,8,4,8,
    12,8,8,12,12,12,4,8,8,8,8,4,4,8,4,4,
    4,4,4,4,4,8,4,4,4,4,4,4,4,8,4,4,
    4,4,4,4,4,8,4,4,4,4,4,4,4,8,4,4,
    4,4,4,4,4,8,4,4,4,4,4,4,4,8,4,8,
    8,8,8,8,8,4,8,4,4,4,4,4,4,8,4,4,
    4,4,4,4,4,8,4,4,4,4,4,4,4,8,4,4,
    4,4,4,4,4,8,4,4,4,4,4,4,4,8,4,4,
    4,4,4,4,4,8,4,4,4,4,4,4,4,8,4,4,
    4,4,4,4,4,8,4,4,4,4,4,4,4,8,4,8,
    12,12,16,12,16,8,16,8,16,12,4,12,24,8,16,8,
    12,12,0,12,16,8,16,8,16,12,0,12,0,8,16,12,
    12,8,0,0,16,8,16,16,4,16,0,0,0,8,16,12,
    12,8,4,0,16,8,16,12,8,16,4,0,0,8,16,
    };

    static const unsigned char CYCLE_TABLE_CB[0x100] = {
    8,8,8,8,8,8,16,8,8,8,8,8,8,8,16,8,
    8,8,8,8,8,8,16,8,8,8,8,8,8,8,16,8,
    8,8,8,8,8,8,16,8,8,8,8,8,8,8,16,8,
    8,8,8,8,8,8,16,8,8,8,8,8,8,8,16,8,
    8,8,8,8,8,8,12,8,8,8,8,8,8,8,12,8,
    8,8,8,8,8,8,12,8,8,8,8,8,8,8,12,8,
    8,8,8,8,8,8,12,8,8,8,8,8,8,8,12,8,
    8,8,8,8,8,8,12,8,8,8,8,8,8,8,12,8,
    8,8,8,8,8,8,16,8,8,8,8,8,8,8,16,8,
    8,8,8,8,8,8,16,8,8,8,8,8,8,8,16,8,
    8,8,8,8,8,8,16,8,8,8,8,8,8,8,16,8,
    8,8,8,8,8,8,16,8,8,8,8,8,8,8,16,8,
    8,8,8,8,8,8,16,8,8,8,8,8,8,8,16,8,
    8,8,8,8,8,8,16,8,8,8,8,8,8,8,16,8,
    8,8,8,8,8,8,16,8,8,8,8,8,8,8,16,8,
    8,8,8,8,8,8,16,8,8,8,8,8,8,8,16,8,
    };

    static const unsigned char CYCLE_TABLE_BRANCH[0x100] = {
    1,3,2,2,1,1,2,1,5,2,2,2,1,1,2,1,
    0,3,2,2,1,1,2,1,3,2,2,2,1,1,2,1,
    3,3,2,2,1,1,2,1,3,2,2,2,1,1,2,1,
    3,3,2,2,3,3,3,1,3,2,2,2,1,1,2,1,
    1,1,1,1,1,1,2,1,1,1,1,1,1,1,2,1,
    1,1,1,1,1,1,2,1,1,1,1,1,1,1,2,1,
    1,1,1,1,1,1,2,1,1,1,1,1,1,1,2,1,
    2,2,2,2,2,2,0,2,1,1,1,1,1,1,2,1,
    1,1,1,1,1,1,2,1,1,1,1,1,1,1,2,1,
    1,1,1,1,1,1,2,1,1,1,1,1,1,1,2,1,
    1,1,1,1,1,1,2,1,1,1,1,1,1,1,2,1,
    1,1,1,1,1,1,2,1,1,1,1,1,1,1,2,1,
    5,3,4,4,6,4,2,4,5,4,4,0,6,6,2,4,
    5,3,4,0,6,4,2,4,5,4,4,0,6,0,2,4,
    3,3,2,0,0,4,2,4,4,1,4,0,0,0,2,4,
    3,3,2,1,0,4,2,4,3,2,4,1,0,0,2,4,
    };


    __attribute__((always_inline)) void cpu::Zflag(uint8_t a, uint8_t b)
    {
        uint16_t temp = ((uint16_t)(a + b));
        if(((uint8_t)temp) == 0) {
            af.bytes.f |= 0b10000000;
        } else {
            af.bytes.f &= 0b01111111;
        }
    }

    __attribute__((always_inline)) void cpu::Zflag_sub(uint16_t a, int b)
        {
            if(!(a+b) || ( (a+b) >= 256) ){ //Z flag
                af.bytes.f |= 0b10000000;
            } else {
                af.bytes.f &= 0b01111111;
            }     
        }
    
    __attribute__((always_inline)) void cpu::Hflag(uint8_t a, uint8_t b)
    {
        if(((a & 0x0f) + (b & 0x0f)) > 0xf){
            af.bytes.f |= 0b00100000;
        } else {
            af.bytes.f &= 0b11011111;
        }
    }
    
    __attribute__((always_inline)) void cpu::Hflag(uint16_t a, uint16_t b)
    {
        if(((a & 0xfff) + ((b & 0xfff)) > 0xfff)){ 
            af.bytes.f |= 0b00100000;
        } else {
            af.bytes.f &= 0b11011111;
        }
    }

    __attribute__((always_inline)) void cpu::Hflag_sub(uint8_t a, uint8_t b)
    {
        if((((a & 0xf) - (b & 0xf)))  < 0){
            af.bytes.f |= 0b00100000;
        } else {
            af.bytes.f &= 0b11011111;
        }
    }

    __attribute__((always_inline)) void cpu::Cflag(uint8_t a, uint8_t b)
    {
        int temp = (a + b) >> 8;
        if(temp){
            af.bytes.f |= 0b00010000;
        } else{
            af.bytes.f &= 0b11101111;
        }
    }

    __attribute__((always_inline)) void cpu::Cflag(uint16_t a, uint16_t b)
    {
        int temp = (a + b) >> 16;
        if(temp){
            af.bytes.f |= 0b00010000;
        } else{
            af.bytes.f &= 0b11101111;
        }
    }

    __attribute__((always_inline)) void cpu::Cflag_sub(uint8_t a, uint8_t b)
    {
        int temp = (a - b) >> 8;
        if(temp){
            af.bytes.f |= 0b00010000;
        } else{
            af.bytes.f &= 0b11101111;
        }
    }

    __attribute__((always_inline)) void cpu::LD_d16(uint8_t &high, uint8_t &low)
    {
        low = Bus->read(pc.pc+1);
        high = Bus->read(pc.pc+2);
        pc.pc+=3;
               
    }

    __attribute__((always_inline)) void cpu::LD_d8(uint8_t &byte)
    {
        byte = Bus->read(pc.pc+1);
        pc.pc+=2;
            }

    __attribute__((always_inline)) void cpu::LD_REG1_REG2(uint8_t &a, uint8_t b)
    {
        a = b;
        pc.pc+=1;
            }
    
    __attribute__((always_inline)) void cpu::LD_HL_REG(uint8_t reg)
    {
        Bus->write(hl.hl, reg);
        pc.pc+=1;
            }

    __attribute__((always_inline)) void cpu::LD_REG_HL(uint8_t &reg)
    {
        reg = Bus->read(hl.hl);
        pc.pc+=1;
            }
    
    __attribute__((always_inline)) void cpu::LD_ADDRESS_A(uint16_t address)
    {
        Bus->write(address, af.bytes.a);
        pc.pc+=1;
            }

    __attribute__((always_inline)) void cpu::LD_A_ADDRESS(uint16_t address)
    {
        af.bytes.a = Bus->read(address);
        pc.pc+=1;
            }

    __attribute__((always_inline)) void cpu::JP_a16()
    {
        uint16_t temp = pc.pc;
        pc.bytes.c = Bus->read(temp+1);
        pc.bytes.p = Bus->read(temp+2);
        
    }

    __attribute__((always_inline)) void cpu::JR()
    {
        int8_t r8 = (int8_t)(Bus->read(pc.pc+1));
        pc.pc += r8 + 2;
          
    }

    __attribute__((always_inline)) void cpu::JR_cond(bool flag)
    {
        if(flag)
        {
            JR();
            cycles = CYCLE_TABLE_BRANCH[opcode] * 4;
        } else {
            pc.pc+=2;
                    }
    }

    __attribute__((always_inline)) void cpu::JP_cond(bool flag)
    {
        if(flag)
        {
            JP_a16();
            cycles = CYCLE_TABLE_BRANCH[opcode] * 4;
        } else {
            pc.pc+=3;
            
        }
    }

    __attribute__((always_inline)) void cpu::POP_16b(uint8_t &high, uint8_t &low)
    {
        low = Bus->read(sp.sp);
        high = Bus->read(sp.sp+=1);
        sp.sp+=1;
        pc.pc+=1;
        
    }

    __attribute__((always_inline)) void cpu::PUSH_16b(uint8_t high, uint8_t low)
    {
        sp.sp--;
        Bus->write(sp.sp, high);
        sp.sp--;
        Bus->write(sp.sp, low); 
        pc.pc+=1;
            
    }

    __attribute__((always_inline)) void cpu::CALL()
    {
        uint16_t temp = 0;
        temp = pc.pc + 3;
        
        sp.sp--;
        Bus->write(sp.sp, (temp & 0xff00) >> 8);
        sp.sp--;
        Bus->write(sp.sp, temp & 0x00ff);
        
        temp = pc.pc;

        pc.bytes.c = Bus->read(temp+1);
        pc.bytes.p = Bus->read(temp+2);
        
    }

    __attribute__((always_inline)) void cpu::CALL_cond(bool flag)
    {
        if(flag)
        {
            CALL();
            cycles = CYCLE_TABLE_BRANCH[opcode] * 4;
        } else {
            pc.pc+=3;
            
        }
    }

    __attribute__((always_inline)) void cpu::RST(uint8_t H)
    {      
        uint16_t temp = 0;
        temp = pc.pc + 1;
        
        sp.sp--;
        Bus->write(sp.sp, (temp & 0xff00) >> 8);
        sp.sp--;
        Bus->write(sp.sp, temp & 0x00ff);

        pc.pc = 0x0000+H;
        
    }

    __attribute__((always_inline)) void cpu::RET()
    {
        pc.bytes.c = Bus->read(sp.sp);
        pc.bytes.p = Bus->read(sp.sp+=1);
        sp.sp+=1;
        
    }

    __attribute__((always_inline)) void cpu::RET_cond(bool flag)
    {
        if(flag)
        {
            RET();
            cycles = CYCLE_TABLE_BRANCH[opcode] * 4;

        } else {
            pc.pc+=1;
            
        }

    }

    __attribute__((always_inline)) void cpu::ADD(uint16_t &a, uint16_t b)
    {
  
        af.bytes.f &= 0b10111111; //N flag
        Hflag(a, b);
        Cflag(a, b);
        a += b;
        pc.pc+=1;
            }
    
    __attribute__((always_inline)) void cpu::ADD(uint8_t &a, uint8_t b)
    {
        Zflag(a, b);
        af.bytes.f &= 0b10111111; //N flag
        Hflag(a, b);
        Cflag(a, b);
        a = (uint8_t)((uint16_t)(a + b));
        pc.pc+=1;
            }

    __attribute__((always_inline)) void cpu::ADC(uint8_t &a, uint8_t b)
    {
        uint16_t result = (uint16_t)(a + b + ((af.bytes.f & 0b00010000) >> 4));
        uint16_t halfResult = (uint16_t)((a & 0xf) + (b & 0xf) + ((af.bytes.f & 0b00010000) >> 4));

        if((uint8_t)(result) == 0){
            af.bytes.f |= 0b10000000;
        } else {
            af.bytes.f &= 0b01111111;
        }

        if(result > 0xff){
            af.bytes.f |= 0b00010000;
        } else {
            af.bytes.f &= 0b11101111;
        }

        if(halfResult > 0xf)
        {
            af.bytes.f |= 0b00100000;
        } else {
            af.bytes.f &= 0b11011111;
        }

        af.bytes.f &= 0b10111111; //N flag

        a = (uint8_t)(result);
        pc.pc+=1;
            }

    __attribute__((always_inline)) void cpu::SUB(uint8_t &a, uint8_t b)
    {
        Zflag_sub(a, -b);
        af.bytes.f |= 0b01000000; //N flag
        Hflag_sub(a, b);
        Cflag_sub(a, b);
        a -= b;
        pc.pc+=1;
             
    }

    __attribute__((always_inline)) void cpu::SBC(uint8_t &a, uint8_t b)
    {
        uint16_t result = (uint16_t)(a - b - ((af.bytes.f & 0b00010000) >> 4));
        uint16_t halfResult = (uint16_t)((a & 0xf) - (b & 0xf) - ((af.bytes.f & 0b00010000) >> 4));

        if((uint8_t)(result) == 0){
            af.bytes.f |= 0b10000000;
        } else {
            af.bytes.f &= 0b01111111;
        }

        if(result > 0xff){
            af.bytes.f |= 0b00010000;
        } else {
            af.bytes.f &= 0b11101111;
        }

        if(halfResult > 0xf)
        {
            af.bytes.f |= 0b00100000;
        } else {
            af.bytes.f &= 0b11011111;
        }

        af.bytes.f |= 0b01000000; //N flag

        a = (uint8_t)(result);
        pc.pc+=1;
           
    }

    __attribute__((always_inline)) void cpu::AND(uint8_t &a, uint8_t b)
    {
        a&=b;
        
        if(a==0)
        {
            af.bytes.f |= 0b10000000;//Z Flag
        } else {
            af.bytes.f &= 0b01111111;
        }

        af.bytes.f |= 0b00100000;//H Flag
        af.bytes.f &= 0b10101111;//N and C flag
        
        pc.pc+=1;
            }

    __attribute__((always_inline)) void cpu::XOR(uint8_t &a, uint8_t b)
    {
        a^=b;
        
        if(a==0)
        {
            af.bytes.f |= 0b10000000;//Z Flag
        } else {
            af.bytes.f &=0b01111111;
        }


        af.bytes.f &= 0b10001111;//N and H and C flag
        
        pc.pc+=1;
            }
    
    __attribute__((always_inline)) void cpu::OR(uint8_t &a, uint8_t b)
    {
        a|=b;        
        
        if(a==0) 
        {
            af.bytes.f |= 0b10000000;//Z Flag
        } else {
            af.bytes.f &= 0b01111111;//Z Flag
        }

        af.bytes.f &= 0b10001111;//N and H and C flag
        
        pc.pc+=1;
            }

    __attribute__((always_inline)) void cpu::CP(uint8_t &a, uint8_t b)
    {
        if(a == b){
            af.bytes.f |= 0b10000000;
        } else {
            af.bytes.f &= 0b01111111;
        }

        af.bytes.f |= 0b01000000; //N flag
        Hflag_sub(a, b);
       
        if(a < b)
        {
            af.bytes.f |= 0b00010000;
        } else {
            af.bytes.f &= 0b11101111;
        }

        pc.pc+=1;
            }


    __attribute__((always_inline)) void cpu::INC(uint16_t &value)
    {
        value+=1;
        pc.pc+=1;
            }

    __attribute__((always_inline)) void cpu::INC(uint8_t &byte)
    {
        Zflag(byte, 1);
        af.bytes.f &= 0b10111111; //N flag
        Hflag(byte, 1);
        byte+=1;
        pc.pc+=1;
            }

    __attribute__((always_inline)) void cpu::DEC(uint16_t &value)
    {
        value-=1;
        pc.pc+=1;
            }

    __attribute__((always_inline)) void cpu::DEC(uint8_t &byte)
    {
        Zflag_sub(byte, -1);
        af.bytes.f |= 0b01000000;
        Hflag_sub(byte, 1);
        byte-=1;
        pc.pc+=1;
            }
    
    __attribute__((always_inline)) void cpu::RLC(uint8_t &byte)
    {
        bool carry = byte & 0b10000000;
        byte <<= 1;
        byte += carry;
        
        if(carry){
            af.bytes.f |= 0b00010000;
        } else {
            af.bytes.f &= 0b11101111;
        }  

        af.bytes.f &= 0b10011111;  

        if(!byte)
        {
            af.bytes.f |= 0b10000000;
        }  else {
            af.bytes.f &= 0b01111111;
        }                    
        
        pc.pc+=1;
               
    }

    __attribute__((always_inline)) void cpu::RRC(uint8_t &byte)
    {
        uint8_t carry = byte & 0b00000001;
        byte >>= 1;
        byte += (carry << 7);
        
        if(carry){
            af.bytes.f |= 0b00010000;
        } else {
            af.bytes.f &= 0b11101111;
        }  

        af.bytes.f &= 0b10011111;  

        if(!byte)
        {
            af.bytes.f |= 0b10000000;
        }  else {
            af.bytes.f &= 0b01111111;
        }                    
        
        pc.pc+=1;
            }
    
    __attribute__((always_inline)) void cpu::RL(uint8_t &byte)
    {
        bool carry = byte & 0b10000000;
        byte <<= 1;
        byte += ((af.bytes.f & 0b00010000) >> 4);
        
        if(carry){
            af.bytes.f |= 0b00010000;
        } else {
            af.bytes.f &= 0b11101111;
        } 
       
        af.bytes.f &= 0b10011111;   

        if(!byte)
        {
            af.bytes.f |= 0b10000000;
        }  else {
            af.bytes.f &= 0b01111111;
        }                   
        
        pc.pc+=1;
            }

    __attribute__((always_inline)) void cpu::RR(uint8_t &byte)
    {
        bool carry = byte & 0b00000001;
        byte >>= 1;
        byte += ((af.bytes.f & 0b00010000) << 3);
        
        if(carry){
            af.bytes.f |= 0b00010000;
        } else {
            af.bytes.f &= 0b11101111;
        } 

        af.bytes.f &= 0b10011111;  

        if(!byte)
        {
            af.bytes.f |= 0b10000000;
        }  else {
            af.bytes.f &= 0b01111111;
        }                   
        
        pc.pc+=1;
                 
    }

    __attribute__((always_inline)) void cpu::SLA(uint8_t &byte)
    {
        bool carry = byte & 0b10000000;
        byte <<= 1;
        
        if(carry){
            af.bytes.f |= 0b00010000;
        } else {
            af.bytes.f &= 0b11101111;
        }   

        af.bytes.f &= 0b10010000;  

        if(!byte)
        {
            af.bytes.f |= 0b10000000;
        }  else {
            af.bytes.f &= 0b01111111;
        }               
        
        pc.pc+=1;
            }

    __attribute__((always_inline)) void cpu::SRA(uint8_t &byte)
    {
        uint8_t carry = byte & 0b00000001;
        byte = (uint8_t)((((char)byte) >> 1));
        
        if(carry){
            af.bytes.f |= 0b00010000;
        } else {
            af.bytes.f &= 0b11101111;
        }  

        af.bytes.f &= 0b10011111;    
        
        if(byte == 0)
        {
            af.bytes.f |= 0b10000000;
        }  else {
            af.bytes.f &= 0b01111111;
        }             
        
        pc.pc+=1;
              
    }

    __attribute__((always_inline)) void cpu::SWAP(uint8_t &byte)
    {
        uint8_t newLow = (byte & 0xf0) >> 4;
        byte <<= 4;
        byte |= newLow; 
        
        if(!byte)
        {
            af.bytes.f |= 0b10000000;
        }  else {
            af.bytes.f &= 0b01111111;
        }

        af.bytes.f &= 0b10001111;  
        pc.pc+=1;

    }

    __attribute__((always_inline)) void cpu::SRL(uint8_t &byte)
    {
        bool carry = byte & 0b00000001;
        byte >>= 1;
        byte & 0b00000001;
        
        if(carry){
            af.bytes.f |= 0b00010000;
        } else {
            af.bytes.f &= 0b11101111;
        }       
        
        af.bytes.f &= 0b10010000;

        if(!byte)
        {
            af.bytes.f |= 0b10000000;
        }  else {
            af.bytes.f &= 0b01111111;
        }             
        
        pc.pc+=1;
        
    }

    __attribute__((always_inline)) void cpu::BIT(uint8_t &byte, uint8_t bitNum)
    {
        if(!(byte & bitNum))
        {
            af.bytes.f |= 0b10000000;
        } else {
            af.bytes.f &= 0b01111111;  
        }

        af.bytes.f &= 0b10111111;
        af.bytes.f |= 0b00100000;  

        pc.pc+=1;
        }

    __attribute__((always_inline)) void cpu::RES(uint8_t &byte, uint8_t bitNum)
    {
        byte &= ~bitNum;
        pc.pc+=1;
            }


    __attribute__((always_inline)) void cpu::SET(uint8_t &byte, uint8_t bitNum)
    {
        byte |= bitNum;
        pc.pc+=1;
            }

    void cpu::execOP()
    {
        //////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////

        checkInterrupts();

        opcode = Bus->read(pc.pc);

        cycles = CYCLE_TABLE[opcode];

        switch (opcode)
        {
            case 0x0: //NOP
                pc.pc+=1;
                break;
            case 0x1: //LD BC,d16
                LD_d16(bc.bytes.b, bc.bytes.c);
                break;                
            case 0x2: //LD (BC),A
                LD_ADDRESS_A(bc.bc);
                break;
            case 0x3: //INC BC
                INC(bc.bc);
                break;
            case 0x4: //INC B (Z 0 H -) 
                INC(bc.bytes.b);
                break;
            case 0x5: //DEC B (Z 1 H -)
                DEC(bc.bytes.b);
                break;
            case 0x6: //LD B,d8
                LD_d8(bc.bytes.b);
                break;
            case 0x7: //RLCA (0 0 0 A7)
                RLC(af.bytes.a);
                af.bytes.f &= 0b01111111;
                break;
            case 0x8: //LD a16, SP
            {
                uint16_t a16 = 0;
                a16 |= Bus->read(pc.pc+1);//get low byte
                a16 |= (Bus->read(pc.pc+2) << 8); // get high byte
                Bus->write(a16, sp.sp & 0x00ff);
                Bus->write(a16 + 1, (sp.sp & 0xff00) >> 8);
                pc.pc+=3;
                
                break;
            }
            case 0x9: //ADD HL, BC (- 0 H C)
                ADD(hl.hl, bc.bc);
                break;
            case 0xa: //LD A, (BC)
                LD_A_ADDRESS(bc.bc);
                break;
            case 0xb: //DEC BC
                DEC(bc.bc);
                break;
            case 0xc: //INC C (Z 0 H -)
                INC(bc.bytes.c);
                break;
            case 0xd: //DEC C (Z 1 H -)
                DEC(bc.bytes.c);
                break;
            case 0xe: //LD C, d8
                LD_d8(bc.bytes.c);
                break;
            case 0xf: //RRCA (0 0 0 A0)
                RRC(af.bytes.a);
                af.bytes.f &= 0b01111111;
                break;  

            case 0x10://STOP
                DIV = 0;
                TMA = 0;
                for(int i = 0; i <= 4; i++)
                {
                    if(((IE & (0b00000001 << i))) && (IF & (0b00000001 << i)))
                    {
                        pc.pc+=1;
                                                    } else {
                        break;
                    }
                }    

            case 0x11://LD DE, d16
                LD_d16(de.bytes.d, de.bytes.e);
                break;
            case 0x12://LD DE, a
                LD_ADDRESS_A(de.de);
                break;
            case 0x13://INC DE
                INC(de.de);
                break;
            case 0x14://INC D (Z 0 H -)
                INC(de.bytes.d);
                break;
            case 0x15:// DEC D (Z 1 H -)
                DEC(de.bytes.d);
                break;
            case 0x16://LD D,d8
                LD_d8(de.bytes.d);
                break;
            case 0x17://RLA (0 0 0 A7)
                RL(af.bytes.a);
                af.bytes.f &= 0b01111111;
                break;
            case 0x18://JR s8
                JR();
                break;
            case 0x19://ADD HL, DE
                ADD(hl.hl, de.de);
                break;
            case 0x1a://LD A, (DE)
                LD_A_ADDRESS(de.de);
                break;
            case 0x1b://DEC DE
                DEC(de.de);
                break;
            case 0x1c://INC E
                INC(de.bytes.e);
                break;
            case 0x1d://DEC E
                DEC(de.bytes.e);
                break;
            case 0x1e://LDE E,d8
                LD_d8(de.bytes.e);
                break;
            case 0x1f://RRA (0 0 0 A7)
                RR(af.bytes.a);
                af.bytes.f &= 0b01111111;
                break;
    
            case 0x20://JR NZ,r8
                JR_cond(!(af.bytes.f & 0b10000000));
                break;
            case 0x21://LD HL, d16
                LD_d16(hl.bytes.h, hl.bytes.l);
                break;
            case 0x22://LD (HL+), A
                LD_ADDRESS_A(hl.hl);
                hl.hl++;
                break;
            case 0x23://INC HL (Z 0 H -)
                INC(hl.hl);
                //extra this instruction
                break;
            case 0x24://INC H (Z 0 H -)
                INC(hl.bytes.h);
                break;
            case 0x25://DEC H (Z 1 H -)
                DEC(hl.bytes.h);
                //extra this instruction
                break;
            case 0x26://LD H, d8
                LD_d8(hl.bytes.h);
                break;
            case 0x27://DAA (Z - 0 C) //TODO
            {
                int tmp = af.bytes.a;

                if ( ! ( af.bytes.f & 0b01000000 ) ) {
                    if ( ( af.bytes.f & 0b00100000 ) || ( tmp & 0x0F ) > 9 )
                        tmp += 6;
                    if ( ( af.bytes.f & 0b00010000 ) || tmp > 0x9F )
                        tmp += 0x60;
                } else {
                    if ( af.bytes.f & 0b00100000 ) {
                        tmp -= 6;
                        if ( ! ( af.bytes.f & 0b00010000 ) )
                            tmp &= 0xFF;
                    }
                    if ( af.bytes.f & 0b00010000 )
                            tmp -= 0x60;
                }
                af.bytes.f &= ~ ( 0b00100000 | 0b10000000 );
                if ( tmp & 0x100 )
                    af.bytes.f |= 0b00010000;
                af.bytes.a = tmp & 0xFF;
                if ( ! af.bytes.a )
                    af.bytes.f |= 0b10000000;
                
                pc.pc+=1;
                                        break;
            }
            case 0x28: //JR Z, r8
                JR_cond(af.bytes.f & 0b10000000);
                break; 
            case 0x29://ADD HL, HL (- 0 H C)
                ADD(hl.hl, hl.hl);
                break;
            case 0x2a://LD A,(HL+)
                LD_A_ADDRESS(hl.hl);
                hl.hl++;
                break;
            case 0x2b://DEC HL
                DEC(hl.hl);
                break;
            case 0x2c://INC L
                INC(hl.bytes.l);
                break;
            case 0x2d://DEC L
                DEC(hl.bytes.l);
                break;
            case 0x2e://LD L,d8
                LD_d8(hl.bytes.l);
                break;
            case 0x2f://CPL
                af.bytes.a = ~af.bytes.a;
                af.bytes.f |= 0b01000000;
                af.bytes.f |= 0b00100000;
                pc.pc+=1;
                break;

            case 0x30://JR NC,r8
                JR_cond(!(af.bytes.f & 0b00010000));
                break;
            case 0x31://LD SP,d16
                LD_d16(sp.bytes.s, sp.bytes.p);
                break;
            case 0x32://LD (HL-),d16
                LD_ADDRESS_A(hl.hl);
                hl.hl--;
                break;
            case 0x33://INC SP
                INC(sp.sp);
                break;
            case 0x34://INC (HL) (Z 0 H -)
            {
                uint8_t temp = Bus->read(hl.hl);
                Zflag(temp, 1);
                af.bytes.f &= 0b10111111; //N flag
                Hflag(temp, 1);
                temp++;
                Bus->write(hl.hl, temp);
                pc.pc+=1;
                
                break;
            }
            case 0x35://DEC (HL) (Z 1 H -)
            {
                uint8_t temp = Bus->read(hl.hl);
                Zflag_sub(temp, -1);
                af.bytes.f |= 0b01000000; //N flag
                Hflag_sub(temp, 1);
                temp--;
                Bus->write(hl.hl, temp);
                pc.pc+=1;
                
                break;
            }
            case 0x36://LD (HL),d8
                Bus->write(hl.hl, Bus->read(pc.pc+1));
                pc.pc+=2;
                
                break;
            case 0x37://SCF (- 0 0 1)
                af.bytes.f &= 0b10011111; //sets N and H to 0
                af.bytes.f |= 0b00010000; //sets C to 1
                pc.pc+=1;
                                        break;
            case 0x38://JR C,r8
                JR_cond(af.bytes.f & 0b00010000);
                break; 
            case 0x39://ADD HL,SP (- 0 H C)
                ADD(hl.hl, sp.sp);
                break;
            case 0x3a://LD A, (HL-)
                LD_A_ADDRESS(hl.hl);
                hl.hl--;
                break;
            case 0x3b://DEC SP
                DEC(sp.sp);
                break;
            case 0x3c://INC A (Z 0 H -)
                INC(af.bytes.a);
                break;
            case 0x3d://DEC A (Z 1 H -)
                DEC(af.bytes.a);
                break;
            case 0x3e://LD A,d8
                LD_d8(af.bytes.a);
                break;
            case 0x3f://CCF (- 0 0 !C)
                af.bytes.f ^= 0b00010000;
                af.bytes.f &= 0b10011111; //N and H
                pc.pc+=1;
                break;

            case 0x40://LD B,B
                LD_REG1_REG2(bc.bytes.b, bc.bytes.b);
                break;
            case 0x41://LD B,C
                LD_REG1_REG2(bc.bytes.b, bc.bytes.c);
                break;
            case 0x42://LD B,D
                LD_REG1_REG2(bc.bytes.b, de.bytes.d);
                break;
            case 0x43://LD B,E
                LD_REG1_REG2(bc.bytes.b, de.bytes.e);
                break;
            case 0x44://LD B,H
                LD_REG1_REG2(bc.bytes.b, hl.bytes.h);
                break;
            case 0x45://LD B,L
                LD_REG1_REG2(bc.bytes.b, hl.bytes.l);
                break;
            case 0x46://LD B,(HL)
                LD_REG_HL(bc.bytes.b);
                break;
            case 0x47://LD B,A
                LD_REG1_REG2(bc.bytes.b, af.bytes.a);
                break;
            case 0x48://LD C,B
                LD_REG1_REG2(bc.bytes.c, bc.bytes.b);
                break;
            case 0x49://LD C,C
                LD_REG1_REG2(bc.bytes.c, bc.bytes.c);
                break;
            case 0x4a://LD C,D
                LD_REG1_REG2(bc.bytes.c, de.bytes.d);
                break;
            case 0x4b://LD C,E
                LD_REG1_REG2(bc.bytes.c, de.bytes.e);
                break;
            case 0x4c://LD C,H
                LD_REG1_REG2(bc.bytes.c, hl.bytes.h);
                break;
            case 0x4d://LD C,L
                LD_REG1_REG2(bc.bytes.c, hl.bytes.l);
                break;
            case 0x4e://LD C,(HL)
                LD_REG_HL(bc.bytes.c);
                break;
            case 0x4f://LD C,A
                LD_REG1_REG2(bc.bytes.c, af.bytes.a);
                break;

            case 0x50://LD D,B
                LD_REG1_REG2(de.bytes.d, bc.bytes.b);
                break;
            case 0x51://LD D,C
                LD_REG1_REG2(de.bytes.d, bc.bytes.c);
                break;
            case 0x52://LD D,D
                LD_REG1_REG2(de.bytes.d, de.bytes.d);
                break;
            case 0x53://LD D,E
                LD_REG1_REG2(de.bytes.d, de.bytes.e);
                break;
            case 0x54://LD D,H
                LD_REG1_REG2(de.bytes.d, hl.bytes.h);
                break;
            case 0x55://LD D,L
                LD_REG1_REG2(de.bytes.d, hl.bytes.l);
                break;
            case 0x56://LD D,(HL)
                LD_REG_HL(de.bytes.d);
                break;
            case 0x57://LD D,A
                LD_REG1_REG2(de.bytes.d, af.bytes.a);
                break;
            case 0x58://LD E,B
                LD_REG1_REG2(de.bytes.e, bc.bytes.b);
                break;
            case 0x59://LD E,C
                LD_REG1_REG2(de.bytes.e, bc.bytes.c);
                break;
            case 0x5a://LD E,D
                LD_REG1_REG2(de.bytes.e, de.bytes.d);
                break;
            case 0x5b://LD E,E
                LD_REG1_REG2(de.bytes.e, de.bytes.e);
                break;
            case 0x5c://LD E,H
                LD_REG1_REG2(de.bytes.e, hl.bytes.h);
                break;
            case 0x5d://LD E,L
                LD_REG1_REG2(de.bytes.e, hl.bytes.l);
                break;
            case 0x5e://LD E,(HL)
                LD_REG_HL(de.bytes.e);
                break;
            case 0x5f://LD E,A
                LD_REG1_REG2(de.bytes.e, af.bytes.a);
                break;

            case 0x60://LD H,B
                LD_REG1_REG2(hl.bytes.h, bc.bytes.b);
                break;
            case 0x61://LD H,C
                LD_REG1_REG2(hl.bytes.h, bc.bytes.c);
                break;
            case 0x62://LD H,D
                LD_REG1_REG2(hl.bytes.h, de.bytes.d);
                break;
            case 0x63://LD H,E
                LD_REG1_REG2(hl.bytes.h, de.bytes.e);
                break;
            case 0x64://LD H,H
                LD_REG1_REG2(hl.bytes.h, hl.bytes.h);
                break;
            case 0x65://LD H,L
                LD_REG1_REG2(hl.bytes.h, hl.bytes.l);
                break;
            case 0x66://LD H,(HL)
                LD_REG_HL(hl.bytes.h);
                break;
            case 0x67://LD H,A
                LD_REG1_REG2(hl.bytes.h, af.bytes.a);
                break;
            case 0x68://LD L,B
                LD_REG1_REG2(hl.bytes.l, bc.bytes.b);
                break;
            case 0x69://LD L,C
                LD_REG1_REG2(hl.bytes.l, bc.bytes.c);
                break;
            case 0x6a://LD L,D
                LD_REG1_REG2(hl.bytes.l, de.bytes.d);
                break;
            case 0x6b://LD L,E
                LD_REG1_REG2(hl.bytes.l, de.bytes.e);
                break;
            case 0x6c://LD L,H
                LD_REG1_REG2(hl.bytes.l, hl.bytes.h);
                break;
            case 0x6d://LD L,L
                LD_REG1_REG2(hl.bytes.l, hl.bytes.l);
                break;
            case 0x6e://LD L,(HL)
                LD_REG_HL(hl.bytes.l);
                break;
            case 0x6f://LD L,A
                LD_REG1_REG2(hl.bytes.l, af.bytes.a);
                break;
    
            case 0x70://LD (HL),B
                LD_HL_REG(bc.bytes.b);
                break;
            case 0x71://LD (HL),C
                LD_HL_REG(bc.bytes.c);
                break;
            case 0x72://LD (HL),D
                LD_HL_REG(de.bytes.d);
                break;
            case 0x73://LD (HL),E
                LD_HL_REG(de.bytes.e);
                break;
            case 0x74://LD (HL),H
                LD_HL_REG(hl.bytes.h);
                break;
            case 0x75://LD (HL),L
                LD_HL_REG(hl.bytes.l);
                break;
            case 0x76://HALT
                for(int i = 0; i <= 4; i++)
                {
                    if(((IE & (0b00000001 << i))) && (IF & (0b00000001 << i)))
                    {
                        pc.pc+=1;
                        return;
                    } else {
                        cycles = 4;
                    }
                }    
                break;
            case 0x77://LD (HL),A
                LD_HL_REG(af.bytes.a);
                break;
            case 0x78://LD A,B
                LD_REG1_REG2(af.bytes.a, bc.bytes.b);
                break;
            case 0x79://LD A,C
                LD_REG1_REG2(af.bytes.a, bc.bytes.c);
                break;
            case 0x7a://LD A,D
                LD_REG1_REG2(af.bytes.a, de.bytes.d);
                break;
            case 0x7b://LD A,E
                LD_REG1_REG2(af.bytes.a, de.bytes.e);
                break;
            case 0x7c://LD A, H
                LD_REG1_REG2(af.bytes.a, hl.bytes.h);
                break;
            case 0x7d://LD A,L
                LD_REG1_REG2(af.bytes.a, hl.bytes.l);
                break;
            case 0x7e://LD A,(HL)
                LD_REG_HL(af.bytes.a);
                break;
            case 0x7f://LD A,A
                LD_REG1_REG2(af.bytes.a, af.bytes.a);
                break;

            case 0x80://ADD A,B (Z 0 H C)
                ADD(af.bytes.a, bc.bytes.b);
                break;
            case 0x81://ADD A,C (Z 0 H C)
                ADD(af.bytes.a, bc.bytes.c);
                break;
            case 0x82://ADD A,D (Z 0 H C)
                ADD(af.bytes.a, de.bytes.d);
                break;
            case 0x83://ADD A,E (Z 0 H C)
                ADD(af.bytes.a, de.bytes.e);
                break;
            case 0x84://ADD A,H (Z 0 H C)
                ADD(af.bytes.a, hl.bytes.h);
                break;
            case 0x85://ADD A,L (Z 0 H C)
                ADD(af.bytes.a, hl.bytes.l);
                break;
            case 0x86://ADD A,(HL) (Z 0 H C)
                ADD(af.bytes.a, Bus->read(hl.hl));
                //extra time for hl
                break;
            case 0x87://ADD A,A (Z 0 H C)
                ADD(af.bytes.a, af.bytes.a);
                break;
            case 0x88://ADC A,B (Z 0 H C)
                ADC(af.bytes.a, bc.bytes.b);
                break;
            case 0x89://ADC A,C (Z 0 H C)
                ADC(af.bytes.a, bc.bytes.c);
                break;
            case 0x8a://ADC A,D (Z 0 H C)
                ADC(af.bytes.a, de.bytes.d);
                break;
            case 0x8b://ADC A,E (Z 0 H C)
                ADC(af.bytes.a, de.bytes.e);
                break;
            case 0x8c://ADC A,H (Z 0 H C)
                ADC(af.bytes.a, hl.bytes.h);
                break;
            case 0x8d://ADC A,L (Z 0 H C)
                ADC(af.bytes.a, hl.bytes.l);
                break;
            case 0x8e://ADC A,(HL) (Z 0 H C)
                ADC(af.bytes.a, Bus->read(hl.hl));
                //extra time for hl
                break;
            case 0x8f://ADC A,A (Z 0 H C)
                ADC(af.bytes.a, af.bytes.a);
            
            case 0x90://SUB B (Z 1 H C)
                SUB(af.bytes.a, bc.bytes.b);
                break;
            case 0x91://SUB C (Z 1 H C)
                SUB(af.bytes.a, bc.bytes.c);
                break;
            case 0x92://SUB D (Z 1 H C)
                SUB(af.bytes.a, de.bytes.d);
                break;
            case 0x93://SUB E (Z 1 H C)
                SUB(af.bytes.a, de.bytes.e);
                break;
            case 0x94://SUB H (Z 1 H C)
                SUB(af.bytes.a, hl.bytes.h);
                break;
            case 0x95://SUB L (Z 1 H C)
                SUB(af.bytes.a, hl.bytes.l);
                break;
            case 0x96://SUB (HL) (Z 1 H C)
                SUB(af.bytes.a, Bus->read(hl.hl));
                //extra time for hl
                break;
            case 0x97://SUB A (Z 1 H C)
                SUB(af.bytes.a, af.bytes.a);
                break;
            case 0x98://SBC A,B (Z 1 H C)
                SBC(af.bytes.a, bc.bytes.b);
                break;
            case 0x99://SBC A,C (Z 1 H C)
                SBC(af.bytes.a, bc.bytes.c);
                break;
            case 0x9a://SBC A,D (Z 1 H C)
                SBC(af.bytes.a, de.bytes.d);
                break;
            case 0x9b://SBC A,E (Z 1 H C)
                SBC(af.bytes.a, de.bytes.e);
                break;
            case 0x9c://SBC A,H (Z 1 H C)
                SBC(af.bytes.a, hl.bytes.h);
                break;
            case 0x9d://SBC A,L (Z 1 H C)
                SBC(af.bytes.a, hl.bytes.l);
                break;
            case 0x9e://SBC A,(HL) (Z 1 H C)
                SBC(af.bytes.a, Bus->read(hl.hl));
                                        break;
            case 0x9f://SBC A,A (Z 1 H C)
                SBC(af.bytes.a, af.bytes.a);
                break;
            
            case 0xa0://AND B (Z 0 1 0)
                AND(af.bytes.a, bc.bytes.b);
                break;
            case 0xa1://AND C (Z 0 1 0)
                AND(af.bytes.a, bc.bytes.c);
                break;
            case 0xa2://AND D (Z 0 1 0)
                AND(af.bytes.a, de.bytes.d);
                break;
            case 0xa3://AND E (Z 0 1 0)
                AND(af.bytes.a, de.bytes.e);
                break;
            case 0xa4://AND H (Z 0 1 0)
                AND(af.bytes.a, hl.bytes.h);
                break;
            case 0xa5://AND L (Z 0 1 0)
                AND(af.bytes.a, hl.bytes.l);
                break;
            case 0xa6://AND (HL) (Z 0 1 0)
                AND(af.bytes.a, Bus->read(hl.hl));
                //extra time for hl
                break;
            case 0xa7://AND A (Z 0 1 0)
                AND(af.bytes.a, af.bytes.a);
                break;
            case 0xa8://XOR B (Z 0 0 0)
                XOR(af.bytes.a, bc.bytes.b);
                break;
            case 0xa9://XOR C (Z 0 0 0)
                XOR(af.bytes.a, bc.bytes.c);
                break;
            case 0xaa://XOR D (Z 0 0 0)
                XOR(af.bytes.a, de.bytes.d);
                break;
            case 0xab://XOR E (Z 0 0 0)
                XOR(af.bytes.a, de.bytes.e);
                break;
            case 0xac://XOR H (Z 0 0 0)
                XOR(af.bytes.a, hl.bytes.h);
                break;
            case 0xad://XOR L (Z 0 0 0)
                XOR(af.bytes.a, hl.bytes.l);
                break;
            case 0xae://XOR (HL) (Z 0 0 0)
                XOR(af.bytes.a, Bus->read(hl.hl));
                                        break;
            case 0xaf://XOR A (Z 0 0 0)
                XOR(af.bytes.a, af.bytes.a);
                break;
                        
            case 0xb0://OR B (Z 0 0 0)
                OR(af.bytes.a, bc.bytes.b);
                break;
            case 0xb1://OR C (Z 0 0 0)
                OR(af.bytes.a, bc.bytes.c);
                break;
            case 0xb2://OR D (Z 0 0 0)
                OR(af.bytes.a, de.bytes.d);
                break;
            case 0xb3://OR E (Z 0 0 0)
                OR(af.bytes.a, de.bytes.e);
                break;
            case 0xb4://OR H (Z 0 0 0)
                OR(af.bytes.a, hl.bytes.h);
                break;
            case 0xb5://OR L (Z 0 0 0)
                OR(af.bytes.a, hl.bytes.l);
                break;
            case 0xb6://OR (HL) (Z 0 0 0)
                OR(af.bytes.a, Bus->read(hl.hl));
                //extra time for hl
                break;
            case 0xb7://OR A (Z 0 0 0)
                OR(af.bytes.a, af.bytes.a);
                break;
            case 0xb8://CP B (Z 1 H C)
                CP(af.bytes.a, bc.bytes.b);
                break;
            case 0xb9://CP C (Z 1 H C)
                CP(af.bytes.a, bc.bytes.c);
                break;
            case 0xba://CP D (Z 1 H C)
                CP(af.bytes.a, de.bytes.d);
                break;
            case 0xbb://CP E (Z 1 H C)
                CP(af.bytes.a, de.bytes.e);
                break;
            case 0xbc://CP H (Z 1 H C)
                CP(af.bytes.a, hl.bytes.h);
                break;
            case 0xbd://CP L (Z 1 H C)
                CP(af.bytes.a, hl.bytes.l);
                break;
            case 0xbe://CP (HL) (Z 1 H C)
                CP(af.bytes.a, Bus->read(hl.hl));
                                        break;
            case 0xbf://CP A (Z 1 H C)
                CP(af.bytes.a, af.bytes.a);
                break;

            case 0xc0://RET NZ
                if(af.bytes.f & 0b10000000)
                {
                    pc.pc+=1;
                } else {
                    pc.bytes.c = Bus->read(sp.sp);
                    pc.bytes.p = Bus->read(sp.sp+=1);
                    sp.sp+=1;
                    cycles = CYCLE_TABLE_BRANCH[opcode] * 4;      
                }
                break;
            case 0xc1://POP BC
                POP_16b(bc.bytes.b, bc.bytes.c);
                break;
            case 0xc2://JP NZ,a16
                JP_cond(!(af.bytes.f & 0b10000000));
                break;
            case 0xc3://JP a16
            {
                uint16_t temp = pc.pc;
                pc.bytes.c = Bus->read(temp+1);
                pc.bytes.p = Bus->read(temp+2);
                                        break;
            }
            case 0xc4://CALL NZ,a16
                CALL_cond(!(af.bytes.f & 0b10000000));
                break;
            case 0xc5://PUSH BC
                PUSH_16b(bc.bytes.b, bc.bytes.c);
                break;
            case 0xc6://ADD A,d8
                ADD(af.bytes.a, Bus->read(pc.pc+1));
                pc.pc+=1;//Extra time and length for this one
                                        break;
            case 0xc7://RST 00H
                RST(0x00);
                break;
            case 0xc8://RET Z
                RET_cond(af.bytes.f & 0b10000000);
                break;
            case 0xc9://RET
                RET();
                break;
            case 0xca://JP Z,a16
                JP_cond(af.bytes.f & 0b10000000);
                break;
            case 0xcb://PREFIX CB
                pc.pc+=1;
                goto cb;
                break;
            case 0xcc://CALL Z,a16
                CALL_cond(af.bytes.f & 0b10000000);
                break;
            case 0xcd://CALL a16
                CALL();
                break;
            case 0xce://ADC A,d8
                ADC(af.bytes.a, Bus->read(pc.pc+1));
                
                pc.pc+=1; //extra byte for this instruction
                break;
            case 0xcf://RST 08H
                RST(0x08);
                break;
            
            case 0xd0://RET NC
                RET_cond(!(af.bytes.f & 0b00010000));
                break;
            case 0xd1://POP DE
                POP_16b(de.bytes.d, de.bytes.e);
                break;
            case 0xd2://JP NC,a16
                JP_cond(!(af.bytes.f & 0b00010000));
                break;
            case 0xd3://NO OP
                break;
            case 0xd4://CALL NC,a16
                CALL_cond(!(af.bytes.f & 0b00010000));
                break;
            case 0xd5://PUSH DE
                PUSH_16b(de.bytes.d, de.bytes.e);
                break;
            case 0xd6://SUB d8 (Z 1 H C)
                SUB(af.bytes.a, Bus->read(pc.pc+1));
                                        pc.pc+=1;//extra byte for this instruction
                break;
            case 0xd7://RST 10H
                RST(0x10);
                break;
            case 0xd8://RET C
                RET_cond(af.bytes.f & 0b00010000);
                break;
            case 0xd9://RETI
                servicingInterrupt = false;
                IME = true;
                RET();
                break;
            case 0xda://JP C,a16
                JP_cond(af.bytes.f & 0b00010000);
                break;
            case 0xdb://NO OP
                break;
            case 0xdc://CALL C,a16
                if(af.bytes.f & 0b00010000)
                {
                    uint16_t temp = 0;
                    temp = pc.pc + 3;
                    
                    sp.sp--;
                    Bus->write(sp.sp, (temp & 0xff00) >> 8);
                    sp.sp--;
                    Bus->write(sp.sp, temp & 0x00ff);
                    
                    temp = pc.pc;

                    pc.bytes.c = Bus->read(temp+1);
                    pc.bytes.p = Bus->read(temp+2);
                    cycles = 24;
                } else {
                    pc.pc+=1;
                    cycles = 4;
                }
                break;
            case 0xdd://NO OP
                break;
            case 0xde://SBC A,d8 (Z 1 H C)
                SBC(af.bytes.a, Bus->read(pc.pc+1));
                                        pc.pc+=1; //extra byte for this instruction
                break;
            case 0xdf://RST 18H
                RST(0x18);
                break;

            case 0xe0://LD (a8), A
                Bus->write((Bus->read(pc.pc+1))+0xff00, af.bytes.a);
                pc.pc+=2;
                
                break;
            case 0xe1://POP HL
                POP_16b(hl.bytes.h, hl.bytes.l);
                break;
            case 0xe2://LD (C),A
                Bus->write((0xff00+bc.bytes.c), af.bytes.a);
                pc.pc+=1;
                                        break;
            case 0xe3://NO OP
                break;
            case 0xe4://NO OP
                break;
            case 0xe5://PUSH HL
                PUSH_16b(hl.bytes.h, hl.bytes.l);
                break;
            case 0xe6://AND d8
                AND(af.bytes.a, Bus->read(pc.pc+1));
                pc.pc+=1;
                                        break;
            case 0xe7://RST 00h
                RST(0x20);
                break;
            case 0xe8://ADD SP, r8
            {
                int32_t r8;
                r8 = (int8_t)(Bus->read(pc.pc+1));
                
                if((sp.sp & 0xff) + (uint8_t)(r8 & 0xff) > 0xff)
                {
                    af.bytes.f |= 0b00010000;
                } else {
                    af.bytes.f &= 0b11101111;
                }

                if( (sp.sp & 0xf) + (r8 & 0xf) > 0x0f)
                {
                    af.bytes.f |= 0b00100000;
                } else {
                    af.bytes.f &= 0b11011111;
                }

                sp.sp = (uint16_t)(sp.sp + r8);


                af.bytes.f &= 0b00111111;
                pc.pc+=2; // extra for this instruction
                    //
                break;
            }
            case 0xe9://JP HL
                pc.pc = hl.hl;
                                        break;
            case 0xea://LD (a16),A
            {
                uint16_t a16 = 0;
                a16 |= Bus->read(pc.pc+1);//get low byte
                a16 |= (Bus->read(pc.pc+2) << 8); // get high byte
                Bus->write(a16, af.bytes.a);
                pc.pc+=3;
                
                break;
            }
            case 0xeb://NO OP
                break;
            case 0xec://NO OP
                break;
            case 0xed://NO OP
                break;
            case 0xee://XOR d8
                XOR(af.bytes.a, Bus->read(pc.pc+1));
                pc.pc+=1; //extra
                                        break;
            case 0xef://RST 28H
                RST(0x28);
                break;
            
            case 0xf0://LDH A,(a8)
                af.bytes.a = (Bus->read( (Bus->read(pc.pc+1)) + 0xff00) );
                pc.pc+=2;
                
                break;
            case 0xf1://POP AF
                POP_16b(af.bytes.a, af.bytes.f);
                af.bytes.f &= 0xf0;
                break;
            case 0xf2://LD A,(C)
                af.bytes.a = (Bus->read(0xff00 + bc.bytes.c));
                pc.pc +=1;
                
                break;
            case 0xf3://DI
                IME = false;
                pc.pc+=1;
                                        break;
            case 0xf4:
                break;//NO OP
            case 0xf5://PUSH AF
                PUSH_16b(af.bytes.a, af.bytes.f);
                break;
            case 0xf6://OR d8
                OR(af.bytes.a, Bus->read(pc.pc+1));
                
                pc.pc+=1;
                break;
            case 0xf7://RST 30H
                RST(0x30);
                break;
            case 0xf8://LD HL,SP+r8
            {
                int32_t r8;
                r8 = (int8_t)(Bus->read(pc.pc+1));
                
                if((sp.sp & 0xff) + (uint8_t)(r8 & 0xff) > 0xff)
                {
                    af.bytes.f |= 0b00010000;
                } else {
                    af.bytes.f &= 0b11101111;
                }

                if( (sp.sp & 0xf) + (r8 & 0xf) > 0x0f)
                {
                    af.bytes.f |= 0b00100000;
                } else {
                    af.bytes.f &= 0b11011111;
                }

                hl.hl = (uint16_t)(sp.sp + r8);


                af.bytes.f &= 0b00111111;
                pc.pc+=2; // extra for this instruction
                    //
                break;
            }
            case 0xf9://LD SP,HL
                sp.sp = hl.hl;
                pc.pc+=1;
                                        break;
            case 0xfa://LD A,(a16)
            {
                uint16_t a16 = 0;
                a16 |= Bus->read(pc.pc+1);//get low byte
                a16 |= (Bus->read(pc.pc+2) << 8); // get high byte
                af.bytes.a = Bus->read(a16);
                pc.pc+=3;
                
                break;
            }
            case 0xfb://EI
                IME = true;
                IMEdelay = true;
                pc.pc+=1;
                                        break;
            case 0xfc://NO OP
                break;
            case 0xfd://NO OP
                break;
            case 0xfe://CP d8
                CP(af.bytes.a, Bus->read(pc.pc+1));
                pc.pc+=1;
                // extra for this instruction
                break;
            case 0xff:
                RST(0x38);
                break;       

        }

        return; // stop executing to prevent from spilling into the CB instructions

        //////CB//////CB//////CB//////CB//////CB//////CB//////CB//////CB//////CB//////CB//////CB
        //////CB//////CB//////CB//////CB//////CB//////CB//////CB//////CB//////CB//////CB//////CB
        //////CB//////CB//////CB//////CB//////CB//////CB//////CB//////CB//////CB//////CB//////CB
        //////CB//////CB//////CB//////CB//////CB//////CB//////CB//////CB//////CB//////CB//////CB
        //////CB//////CB//////CB//////CB//////CB//////CB//////CB//////CB//////CB//////CB//////CB
        //////CB//////CB//////CB//////CB//////CB//////CB//////CB//////CB//////CB//////CB//////CB

        cb:
        opcode = Bus->read(pc.pc);//gets new opcode
        uint8_t opcodeH = (opcode & 0xF0) >> 4;
        uint8_t opcodeL = opcode & 0x0F;

        cycles = CYCLE_TABLE_CB[opcode];

        switch(opcodeH)
        {
            case 0x0:
                switch(opcodeL)
                {
                    case 0x0://RLC B (Z 0 0 REG7)
                        RLC(bc.bytes.b);
                        break;
                    case 0x1://RLC C (Z 0 0 REG7)
                        RLC(bc.bytes.c);
                        break;
                    case 0x2://RLC D (Z 0 0 REG7)
                        RLC(de.bytes.d);
                        break;
                    case 0x3://RLC E (Z 0 0 REG7)
                        RLC(de.bytes.e);
                        break;
                    case 0x4://RLC H (Z 0 0 REG7)
                        RLC(hl.bytes.h);
                        break;
                    case 0x5://RLC L (Z 0 0 REG7)
                        RLC(hl.bytes.l);
                        break;
                    case 0x6://RLC (HL) (Z 0 0 REG7)
                    {
                        uint8_t temp = Bus->read(hl.hl);
                        RLC(temp);
                        Bus->write(hl.hl, temp);
                        //extra time for hl
                        break;
                    }
                    case 0x7://RLC A (Z 0 0 REG7)
                        RLC(af.bytes.a);
                        break;
                    case 0x8://RRC B (Z 0 0 0)
                        RRC(bc.bytes.b);
                        break;
                    case 0x9://RRC C (Z 0 0 0)
                        RRC(bc.bytes.c);
                        break;
                    case 0xa://RRC D (Z 0 0 0)
                        RRC(de.bytes.d);
                        break;
                    case 0xb://RRC E (Z 0 0 0)
                        RRC(de.bytes.e);
                        break;
                    case 0xc://RRC H (Z 0 0 0)
                        RRC(hl.bytes.h);
                        break;
                    case 0xd://RRC L (Z 0 0 0)
                        RRC(hl.bytes.l);
                        break;
                    case 0xe://RRC (HL) (Z 0 0 0)
                    {
                        uint8_t temp = Bus->read(hl.hl);
                        RRC(temp);
                        Bus->write(hl.hl, temp);
                        //extra time for hl
                        break;
                    }
                    case 0xf://RRC A (Z 0 0 0)
                        RRC(af.bytes.a);
                        break;
                }
                break;
                
            case 0x1:
                switch(opcodeL)
                {
                    case 0x0://RL B (Z 0 0 REG7)
                        RL(bc.bytes.b);
                        break;
                    case 0x1://RL C (Z 0 0 REG7)
                        RL(bc.bytes.c);
                        break;
                    case 0x2://RL D (Z 0 0 REG7)
                        RL(de.bytes.d);
                        break;
                    case 0x3://RL E (Z 0 0 REG7)
                        RL(de.bytes.e);
                        break;
                    case 0x4://RL H (Z 0 0 REG7)
                        RL(hl.bytes.h);
                        break;
                    case 0x5://RL L (Z 0 0 REG7)
                        RL(hl.bytes.l);
                        break;
                    case 0x6://RL (HL) (Z 0 0 REG7)
                    {
                        uint8_t temp = Bus->read(hl.hl);
                        RL(temp);
                        Bus->write(hl.hl, temp);
                        //extra time for hl
                        break;
                    }
                    case 0x7://RL A (Z 0 0 REG7)
                        RL(af.bytes.a);
                        break;
                    case 0x8://RR B (Z 0 0 REG7)
                        RR(bc.bytes.b);
                        break;
                    case 0x9://RL C (Z 0 0 REG7)
                        RR(bc.bytes.c);
                        break;
                    case 0xa://RR D (Z 0 0 REG7)
                        RR(de.bytes.d);
                        break;
                    case 0xb://RR E (Z 0 0 REG7)
                        RR(de.bytes.e);
                        break;
                    case 0xc://RR H (Z 0 0 REG7)
                        RR(hl.bytes.h);
                        break;
                    case 0xd://RR L (Z 0 0 REG7)
                        RR(hl.bytes.l);
                        break;
                    case 0xe://RR (HL) (Z 0 0 REG7)
                    {
                        uint8_t temp = Bus->read(hl.hl);
                        RR(temp);
                        Bus->write(hl.hl, temp);
                        //extra time for hl
                        break;
                    }
                    case 0xf://RR A (Z 0 0 REG7)
                        RR(af.bytes.a);
                        break;
                }
                break;

            case 0x2:
                switch(opcodeL)
                {
                    case 0x0://SLA B (Z 0 0 REG7)
                        SLA(bc.bytes.b);
                        break;
                    case 0x1://SLA C (Z 0 0 REG7)
                        SLA(bc.bytes.c);
                        break;
                    case 0x2://SLA D (Z 0 0 REG7)
                        SLA(de.bytes.d);
                        break;
                    case 0x3://SLA E (Z 0 0 REG7)
                        SLA(de.bytes.e);
                        break;
                    case 0x4://SLA H (Z 0 0 REG7)
                        SLA(hl.bytes.h);
                        break;
                    case 0x5://SLA L (Z 0 0 REG7)
                        SLA(hl.bytes.l);
                        break;
                    case 0x6://SLA (HL) (Z 0 0 REG7)
                    {
                        uint8_t temp = Bus->read(hl.hl);
                        SLA(temp);
                        Bus->write(hl.hl, temp);
                        //extra time for hl
                        break;
                    }
                    case 0x7://SLA A (Z 0 0 REG7)
                        SLA(af.bytes.a);
                        break;
                    case 0x8://SRA B (Z 0 0 0)
                        SRA(bc.bytes.b);
                        break;
                    case 0x9://SRA C (Z 0 0 0)
                        SRA(bc.bytes.c);
                        break;
                    case 0xa://SRA D (Z 0 0 0)
                        SRA(de.bytes.d);
                        break;
                    case 0xb://SRA E (Z 0 0 0)
                        SRA(de.bytes.e);
                        break;
                    case 0xc://SRA H (Z 0 0 0)
                        SRA(hl.bytes.h);
                        break;
                    case 0xd://SRA L (Z 0 0 0)
                        SRA(hl.bytes.l);
                        break;
                    case 0xe://SRA (HL) (Z 0 0 0)
                    {
                        uint8_t temp = Bus->read(hl.hl);
                        SRA(temp);
                        Bus->write(hl.hl, temp);
                        //extra time for hl
                        break;
                    }
                    case 0xf://SRA A (Z 0 0 0)
                        SRA(af.bytes.a);
                        break;
                }
                break;

            case 0x3:
                switch(opcodeL)
                {
                    case 0x0://SWAP B (Z 0 0 REG7)
                        SWAP(bc.bytes.b);
                        break;
                    case 0x1://SWAP C (Z 0 0 REG7)
                        SWAP(bc.bytes.c);
                        break;
                    case 0x2://SWAP D (Z 0 0 REG7)
                        SWAP(de.bytes.d);
                        break;
                    case 0x3://SWAP E (Z 0 0 REG7)
                        SWAP(de.bytes.e);
                        break;
                    case 0x4://SWAP H (Z 0 0 REG7)
                        SWAP(hl.bytes.h);
                        break;
                    case 0x5://SWAP L (Z 0 0 REG7)
                        SWAP(hl.bytes.l);
                        break;
                    case 0x6://SWAP (HL) (Z 0 0 REG7)
                    {
                        uint8_t temp = Bus->read(hl.hl);
                        SWAP(temp);
                        Bus->write(hl.hl, temp);
                        //extra time for hl
                        break;
                    }
                    case 0x7://SWAP A (Z 0 0 REG7)
                        SWAP(af.bytes.a);
                        break;
                    case 0x8://SRL B (Z 0 0 0)
                        SRL(bc.bytes.b);
                        break;
                    case 0x9://SRL C (Z 0 0 0)
                        SRL(bc.bytes.c);
                        break;
                    case 0xa://SRL D (Z 0 0 0)
                        SRL(de.bytes.d);
                        break;
                    case 0xb://SRL E (Z 0 0 0)
                        SRL(de.bytes.e);
                        break;
                    case 0xc://SRL H (Z 0 0 0)
                        SRL(hl.bytes.h);
                        break;
                    case 0xd://SRL L (Z 0 0 0)
                        SRL(hl.bytes.l);
                        break;
                    case 0xe://SRL (HL) (Z 0 0 0)
                    {
                        uint8_t temp = Bus->read(hl.hl);
                        SRL(temp);
                        Bus->write(hl.hl, temp);
                        //extra time for hl
                        break;
                    }
                    case 0xf://SRL A (Z 0 0 0)
                        SRL(af.bytes.a);
                        break;
                }
                break;

            case 0x4:
                switch(opcodeL)
                {
                    case 0x0://BIT 0,B (Z 0 1 -)
                        BIT(bc.bytes.b, 0b00000001);
                        break;
                    case 0x1://BIT 0,C (Z 0 1 -)
                        BIT(bc.bytes.c, 0b00000001);
                        break;
                    case 0x2://BIT 0,D (Z 0 1 -)
                        BIT(de.bytes.d, 0b00000001);
                        break;
                    case 0x3://BIT 0,E (Z 0 1 -)
                        BIT(de.bytes.e, 0b00000001);
                        break;
                    case 0x4://BIT 0,H (Z 0 1 -)
                        BIT(hl.bytes.h, 0b00000001);
                        break;
                    case 0x5://BIT 0,L (Z 0 1 -)
                        BIT(hl.bytes.l, 0b00000001);
                        break;
                    case 0x6://BIT 0,(HL) (Z 0 1 -)
                    {
                        uint8_t temp = Bus->read(hl.hl);
                        BIT(temp, 0b00000001);
                        Bus->write(hl.hl, temp);
                        //extra time for hl
                        break;
                    }
                    case 0x7://BIT 0,A (Z 0 1 -)
                        BIT(af.bytes.a, 0b00000001);
                        break;
                    case 0x8://BIT 1,B (Z 0 1 -)
                        BIT(bc.bytes.b, 0b00000010);
                        break;
                    case 0x9://BIT 1,C (Z 0 1 -)
                        BIT(bc.bytes.c, 0b00000010);
                        break;
                    case 0xa://BIT 1,D (Z 0 1 -)
                        BIT(de.bytes.d, 0b00000010);
                        break;
                    case 0xb://BIT 1,E (Z 0 1 -)
                        BIT(de.bytes.e, 0b00000010);
                        break;
                    case 0xc://BIT 1,H (Z 0 1 -)
                        BIT(hl.bytes.h, 0b00000010);
                        break;
                    case 0xd://BIT 1,L (Z 0 1 -)
                        BIT(hl.bytes.l, 0b00000010);
                        break;
                    case 0xe://BIT 1,(HL) (Z 0 1 -)
                    {
                        uint8_t temp = Bus->read(hl.hl);
                        BIT(temp, 0b00000010);
                        Bus->write(hl.hl, temp);
                        //extra time for hl
                        break;
                    }
                    case 0xf://BIT 1,A (Z 0 1 -)
                        BIT(af.bytes.a, 0b00000010);
                        break;
                }
                break;

            case 0x5:
                switch(opcodeL)
                {
                    case 0x0://BIT 2,B (Z 0 1 -)
                        BIT(bc.bytes.b, 0b00000100);
                        break;
                    case 0x1://BIT 2,C (Z 0 1 -)
                        BIT(bc.bytes.c, 0b00000100);
                        break;
                    case 0x2://BIT 2,D (Z 0 1 -)
                        BIT(de.bytes.d, 0b00000100);
                        break;
                    case 0x3://BIT 2,E (Z 0 1 -)
                        BIT(de.bytes.e, 0b00000100);
                        break;
                    case 0x4://BIT 2,H (Z 0 1 -)
                        BIT(hl.bytes.h, 0b00000100);
                        break;
                    case 0x5://BIT 2,L (Z 0 1 -)
                        BIT(hl.bytes.l, 0b00000100);
                        break;
                    case 0x6://BIT 2,(HL) (Z 0 1 -)
                    {
                        uint8_t temp = Bus->read(hl.hl);
                        BIT(temp, 0b00000100);
                        Bus->write(hl.hl, temp);
                        //extra time for hl
                        break;
                    }
                    case 0x7://BIT 2,A (Z 0 1 -)
                        BIT(af.bytes.a, 0b00000100);
                        break;
                    case 0x8://BIT 3,B (Z 0 1 -)
                        BIT(bc.bytes.b, 0b00001000);
                        break;
                    case 0x9://BIT 3,C (Z 0 1 -)
                        BIT(bc.bytes.c, 0b00001000);
                        break;
                    case 0xa://BIT 3,D (Z 0 1 -)
                        BIT(de.bytes.d, 0b00001000);
                        break;
                    case 0xb://BIT 3,E (Z 0 1 -)
                        BIT(de.bytes.e, 0b00001000);
                        break;
                    case 0xc://BIT 3,H (Z 0 1 -)
                        BIT(hl.bytes.h, 0b00001000);
                        break;
                    case 0xd://BIT 3,L (Z 0 1 -)
                        BIT(hl.bytes.l, 0b00001000);
                        break;
                    case 0xe://BIT 3,(HL) (Z 0 1 -)
                    {
                        uint8_t temp = Bus->read(hl.hl);
                        BIT(temp, 0b00001000);
                        Bus->write(hl.hl, temp);
                        //extra time for hl
                        break;
                    }
                    case 0xf://BIT 3,A (Z 0 1 -)
                        BIT(af.bytes.a, 0b00001000);
                        break;
                }
                break;

            case 0x6:
                switch(opcodeL)
                {
                    case 0x0://BIT 4,B (Z 0 1 -)
                        BIT(bc.bytes.b, 0b00010000);
                        break;
                    case 0x1://BIT 4,C (Z 0 1 -)
                        BIT(bc.bytes.c, 0b00010000);
                        break;
                    case 0x2://BIT 4,D (Z 0 1 -)
                        BIT(de.bytes.d, 0b00010000);
                        break;
                    case 0x3://BIT 4,E (Z 0 1 -)
                        BIT(de.bytes.e, 0b00010000);
                        break;
                    case 0x4://BIT 4,H (Z 0 1 -)
                        BIT(hl.bytes.h, 0b00010000);
                        break;
                    case 0x5://BIT 4,L (Z 0 1 -)
                        BIT(hl.bytes.l, 0b00010000);
                        break;
                    case 0x6://BIT 4,(HL) (Z 0 1 -)
                    {
                        uint8_t temp = Bus->read(hl.hl);
                        BIT(temp, 0b00010000);
                        Bus->write(hl.hl, temp);
                        //extra time for hl
                        break;
                    }
                    case 0x7://BIT 4,A (Z 0 1 -)
                        BIT(af.bytes.a, 0b00010000);
                        break;
                    case 0x8://BIT 5,B (Z 0 1 -)
                        BIT(bc.bytes.b, 0b00100000);
                        break;
                    case 0x9://BIT 5,C (Z 0 1 -)
                        BIT(bc.bytes.c, 0b00100000);
                        break;
                    case 0xa://BIT 5,D (Z 0 1 -)
                        BIT(de.bytes.d, 0b00100000);
                        break;
                    case 0xb://BIT 5,E (Z 0 1 -)
                        BIT(de.bytes.e, 0b00100000);
                        break;
                    case 0xc://BIT 5,H (Z 0 1 -)
                        BIT(hl.bytes.h, 0b00100000);
                        break;
                    case 0xd://BIT 5,L (Z 0 1 -)
                        BIT(hl.bytes.l, 0b00100000);
                        break;
                    case 0xe://BIT 5,(HL) (Z 0 1 -)
                    {
                        uint8_t temp = Bus->read(hl.hl);
                        BIT(temp, 0b00100000);
                        Bus->write(hl.hl, temp);
                        //extra time for hl
                        break;
                    }
                    case 0xf://BIT 5,A (Z 0 1 -)
                        BIT(af.bytes.a, 0b00100000);
                        break;
                }
                break;

            case 0x7:
                switch(opcodeL)
                {
                    case 0x0://BIT 6,B (Z 0 1 -)
                        BIT(bc.bytes.b, 0b01000000);
                        break;
                    case 0x1://BIT 6,C (Z 0 1 -)
                        BIT(bc.bytes.c, 0b01000000);
                        break;
                    case 0x2://BIT 6,D (Z 0 1 -)
                        BIT(de.bytes.d, 0b01000000);
                        break;
                    case 0x3://BIT 6,E (Z 0 1 -)
                        BIT(de.bytes.e, 0b01000000);
                        break;
                    case 0x4://BIT 6,H (Z 0 1 -)
                        BIT(hl.bytes.h, 0b01000000);
                        break;
                    case 0x5://BIT 6,L (Z 0 1 -)
                        BIT(hl.bytes.l, 0b01000000);
                        break;
                    case 0x6://BIT 6,(HL) (Z 0 1 -)
                    {
                        uint8_t temp = Bus->read(hl.hl);
                        BIT(temp, 0b01000000);
                        Bus->write(hl.hl, temp);
                        //extra time for hl
                        break;
                    }
                    case 0x7://BIT 6,A (Z 0 1 -)
                        BIT(af.bytes.a, 0b01000000);
                        break;
                    case 0x8://BIT 7,B (Z 0 1 -)
                        BIT(bc.bytes.b, 0b10000000);
                        break;
                    case 0x9://BIT 7,C (Z 0 1 -)
                        BIT(bc.bytes.c, 0b10000000);
                        break;
                    case 0xa://BIT 7,D (Z 0 1 -)
                        BIT(de.bytes.d, 0b10000000);
                        break;
                    case 0xb://BIT 7,E (Z 0 1 -)
                        BIT(de.bytes.e, 0b10000000);
                        break;
                    case 0xc://BIT 7,H (Z 0 1 -)
                        BIT(hl.bytes.h, 0b10000000);
                        break;
                    case 0xd://BIT 7,L (Z 0 1 -)
                        BIT(hl.bytes.l, 0b10000000);
                        break;
                    case 0xe://BIT 7,(HL) (Z 0 1 -)
                    {
                        uint8_t temp = Bus->read(hl.hl);
                        BIT(temp, 0b10000000);
                        Bus->write(hl.hl, temp);
                        //extra time for hl
                        break;
                    }
                    case 0xf://BIT 7,A (Z 0 1 -)
                        BIT(af.bytes.a, 0b10000000);
                        break;
                }
                break;

            case 0x8:
                switch(opcodeL)
                {
                    case 0x0://RES 0,B
                        RES(bc.bytes.b, 0b00000001);
                        break;
                    case 0x1://RES 0,C
                        RES(bc.bytes.c, 0b00000001);
                        break;
                    case 0x2://RES 0,D
                        RES(de.bytes.d, 0b00000001);
                        break;
                    case 0x3://RES 0,E
                        RES(de.bytes.e, 0b00000001);
                        break;
                    case 0x4://RES 0,H
                        RES(hl.bytes.h, 0b00000001);
                        break;
                    case 0x5://RES 0,L
                        RES(hl.bytes.l, 0b00000001);
                        break;
                    case 0x6://RES 0,(HL)
                    {
                        uint8_t temp = Bus->read(hl.hl);
                        RES(temp, 0b00000001);
                        Bus->write(hl.hl, temp);
                        //extra time for hl
                        break;
                    }
                    case 0x7://RES 0,A
                        RES(af.bytes.a, 0b00000001);
                        break;
                    case 0x8://RES 1,B
                        RES(bc.bytes.b, 0b00000010);
                        break;
                    case 0x9://RES 1,C
                        RES(bc.bytes.c, 0b00000010);
                        break;
                    case 0xa://RES 1,D
                        RES(de.bytes.d, 0b00000010);
                        break;
                    case 0xb://RES 1,E
                        RES(de.bytes.e, 0b00000010);
                        break;
                    case 0xc://RES 1,H
                        RES(hl.bytes.h, 0b00000010);
                        break;
                    case 0xd://RES 1,L
                        RES(hl.bytes.l, 0b00000010);
                        break;
                    case 0xe://RES 1,(HL)
                    {
                        uint8_t temp = Bus->read(hl.hl);
                        RES(temp, 0b00000010);
                        Bus->write(hl.hl, temp);
                        //extra time for hl
                        break;
                    }
                    case 0xf://RES 1,A
                        RES(af.bytes.a, 0b00000010);
                        break;
                }
            break;

            case 0x9:
                switch(opcodeL)
                {
                    case 0x0://RES 2,B
                        RES(bc.bytes.b, 0b00000100);
                        break;
                    case 0x1://RES 2,C
                        RES(bc.bytes.c, 0b00000100);
                        break;
                    case 0x2://RES 2,D
                        RES(de.bytes.d, 0b00000100);
                        break;
                    case 0x3://RES 2,E
                        RES(de.bytes.e, 0b00000100);
                        break;
                    case 0x4://RES 2,H
                        RES(hl.bytes.h, 0b00000100);
                        break;
                    case 0x5://RES 2,L
                        RES(hl.bytes.l, 0b00000100);
                        break;
                    case 0x6://RES 2,(HL)
                    {
                        uint8_t temp = Bus->read(hl.hl);
                        RES(temp, 0b00000100);
                        Bus->write(hl.hl, temp);
                        //extra time for hl
                        break;
                    }
                    case 0x7://RES 2,A
                        RES(af.bytes.a, 0b00000100);
                        break;
                    case 0x8://RES 3,B
                        RES(bc.bytes.b, 0b00001000);
                        break;
                    case 0x9://RES 3,C
                        RES(bc.bytes.c, 0b00001000);
                        break;
                    case 0xa://RES 3,D
                        RES(de.bytes.d, 0b00001000);
                        break;
                    case 0xb://RES 3,E
                        RES(de.bytes.e, 0b00001000);
                        break;
                    case 0xc://RES 3,H
                        RES(hl.bytes.h, 0b00001000);
                        break;
                    case 0xd://RES 3,L
                        RES(hl.bytes.l, 0b00001000);
                        break;
                    case 0xe://RES 3,(HL)
                    {
                        uint8_t temp = Bus->read(hl.hl);
                        RES(temp, 0b00001000);
                        Bus->write(hl.hl, temp);
                        //extra time for hl
                        break;
                    }
                    case 0xf://RES 3,A
                        RES(af.bytes.a, 0b00001000);
                        break;
                }
            break;

            case 0xa:
                switch(opcodeL)
                {
                    case 0x0://RES 4,B
                        RES(bc.bytes.b, 0b00010000);
                        break;
                    case 0x1://RES 4,C
                        RES(bc.bytes.c, 0b00010000);
                        break;
                    case 0x2://RES 4,D
                        RES(de.bytes.d, 0b00010000);
                        break;
                    case 0x3://RES 4,E
                        RES(de.bytes.e, 0b00010000);
                        break;
                    case 0x4://RES 4,H
                        RES(hl.bytes.h, 0b00010000);
                        break;
                    case 0x5://RES 4,L
                        RES(hl.bytes.l, 0b00010000);
                        break;
                    case 0x6://RES 4,(HL)
                    {
                        uint8_t temp = Bus->read(hl.hl);
                        RES(temp, 0b00010000);
                        Bus->write(hl.hl, temp);
                        //extra time for hl
                        break;
                    }
                    case 0x7://RES 4,A
                        RES(af.bytes.a, 0b00010000);
                        break;
                    case 0x8://RES 5,B
                        RES(bc.bytes.b, 0b00100000);
                        break;
                    case 0x9://RES 5,C
                        RES(bc.bytes.c, 0b00100000);
                        break;
                    case 0xa://RES 5,D
                        RES(de.bytes.d, 0b00100000);
                        break;
                    case 0xb://RES 5,E
                        RES(de.bytes.e, 0b00100000);
                        break;
                    case 0xc://RES 5,H
                        RES(hl.bytes.h, 0b00100000);
                        break;
                    case 0xd://RES 5,L
                        RES(hl.bytes.l, 0b00100000);
                        break;
                    case 0xe://RES 5,(HL)
                    {
                        uint8_t temp = Bus->read(hl.hl);
                        RES(temp, 0b00100000);
                        Bus->write(hl.hl, temp);
                        //extra time for hl
                        break;
                    }
                    case 0xf://RES 5,A
                        RES(af.bytes.a, 0b00100000);
                        break;
                }
            break;

            case 0xb:
                switch(opcodeL)
                {
                    case 0x0://RES 6,B
                        RES(bc.bytes.b, 0b01000000);
                        break;
                    case 0x1://RES 6,C
                        RES(bc.bytes.c, 0b01000000);
                        break;
                    case 0x2://RES 6,D
                        RES(de.bytes.d, 0b01000000);
                        break;
                    case 0x3://RES 6,E
                        RES(de.bytes.e, 0b01000000);
                        break;
                    case 0x4://RES 6,H
                        RES(hl.bytes.h, 0b01000000);
                        break;
                    case 0x5://RES 6,L
                        RES(hl.bytes.l, 0b01000000);
                        break;
                    case 0x6://RES 6,(HL)
                    {
                        uint8_t temp = Bus->read(hl.hl);
                        RES(temp, 0b01000000);
                        Bus->write(hl.hl, temp);
                        //extra time for hl
                        break;
                    }
                    case 0x7://RES 6,A
                        RES(af.bytes.a, 0b01000000);
                        break;
                    case 0x8://RES 7,B
                        RES(bc.bytes.b, 0b10000000);
                        break;
                    case 0x9://RES 7,C
                        RES(bc.bytes.c, 0b10000000);
                        break;
                    case 0xa://RES 7,D
                        RES(de.bytes.d, 0b10000000);
                        break;
                    case 0xb://RES 7,E
                        RES(de.bytes.e, 0b10000000);
                        break;
                    case 0xc://RES 7,H
                        RES(hl.bytes.h, 0b10000000);
                        break;
                    case 0xd://RES 7,L
                        RES(hl.bytes.l, 0b10000000);
                        break;
                    case 0xe://RES 7,(HL)
                    {
                        uint8_t temp = Bus->read(hl.hl);
                        RES(temp, 0b10000000);
                        Bus->write(hl.hl, temp);
                        //extra time for hl
                        break;
                    }
                    case 0xf://RES 7,A
                        RES(af.bytes.a, 0b10000000);
                        break;
                }
            break;

            case 0xc:
                switch(opcodeL)
                {
                    case 0x0://SET 0,B
                        SET(bc.bytes.b, 0b00000001);
                        break;
                    case 0x1://SET 0,C
                        SET(bc.bytes.c, 0b00000001);
                        break;
                    case 0x2://SET 0,D
                        SET(de.bytes.d, 0b00000001);
                        break;
                    case 0x3://SET 0,E
                        SET(de.bytes.e, 0b00000001);
                        break;
                    case 0x4://SET 0,H
                        SET(hl.bytes.h, 0b00000001);
                        break;
                    case 0x5://SET 0,L
                        SET(hl.bytes.l, 0b00000001);
                        break;
                    case 0x6://SET 0,(HL)
                    {
                        uint8_t temp = Bus->read(hl.hl);
                        SET(temp, 0b00000001);
                        Bus->write(hl.hl, temp);
                        //extra time for hl
                        break;
                    }
                    case 0x7://SET 0,A
                        SET(af.bytes.a, 0b00000001);
                        break;
                    case 0x8://SET 1,B
                        SET(bc.bytes.b, 0b00000010);
                        break;
                    case 0x9://SET 1,C
                        SET(bc.bytes.c, 0b00000010);
                        break;
                    case 0xa://SET 1,D
                        SET(de.bytes.d, 0b00000010);
                        break;
                    case 0xb://SET 1,E
                        SET(de.bytes.e, 0b00000010);
                        break;
                    case 0xc://SET 1,H
                        SET(hl.bytes.h, 0b00000010);
                        break;
                    case 0xd://SET 1,L
                        SET(hl.bytes.l, 0b00000010);
                        break;
                    case 0xe://SET 1,(HL)
                    {
                        uint8_t temp = Bus->read(hl.hl);
                        SET(temp, 0b00000010);
                        Bus->write(hl.hl, temp);
                        //extra time for hl
                        break;
                    }
                    case 0xf://SET 1,A
                        SET(af.bytes.a, 0b00000010);
                        break;
                }
            break;

            case 0xd:
                switch(opcodeL)
                {
                    case 0x0://SET 2,B
                        SET(bc.bytes.b, 0b00000100);
                        break;
                    case 0x1://SET 2,C
                        SET(bc.bytes.c, 0b00000100);
                        break;
                    case 0x2://SET 2,D
                        SET(de.bytes.d, 0b00000100);
                        break;
                    case 0x3://SET 2,E
                        SET(de.bytes.e, 0b00000100);
                        break;
                    case 0x4://SET 2,H
                        SET(hl.bytes.h, 0b00000100);
                        break;
                    case 0x5://SET 2,L
                        SET(hl.bytes.l, 0b00000100);
                        break;
                    case 0x6://SET 2,(HL)
                    {
                        uint8_t temp = Bus->read(hl.hl);
                        SET(temp, 0b00000100);
                        Bus->write(hl.hl, temp);
                        //extra time for hl
                        break;
                    }
                    case 0x7://SET 2,A
                        SET(af.bytes.a, 0b00000100);
                        break;
                    case 0x8://SET 3,B
                        SET(bc.bytes.b, 0b00001000);
                        break;
                    case 0x9://SET 3,C
                        SET(bc.bytes.c, 0b00001000);
                        break;
                    case 0xa://SET 3,D
                        SET(de.bytes.d, 0b00001000);
                        break;
                    case 0xb://SET 3,E
                        SET(de.bytes.e, 0b00001000);
                        break;
                    case 0xc://SET 3,H
                        SET(hl.bytes.h, 0b00001000);
                        break;
                    case 0xd://SET 3,L
                        SET(hl.bytes.l, 0b00001000);
                        break;
                    case 0xe://SET 3,(HL)
                    {
                        uint8_t temp = Bus->read(hl.hl);
                        SET(temp, 0b00001000);
                        Bus->write(hl.hl, temp);
                        //extra time for hl
                        break;
                    }
                    case 0xf://SET 3,A
                        SET(af.bytes.a, 0b00001000);
                        break;
                }
            break;

            case 0xe:
                switch(opcodeL)
                {
                    case 0x0://SET 4,B
                        SET(bc.bytes.b, 0b00010000);
                        break;
                    case 0x1://SET 4,C
                        SET(bc.bytes.c, 0b00010000);
                        break;
                    case 0x2://SET 4,D
                        SET(de.bytes.d, 0b00010000);
                        break;
                    case 0x3://SET 4,E
                        SET(de.bytes.e, 0b00010000);
                        break;
                    case 0x4://SET 4,H
                        SET(hl.bytes.h, 0b00010000);
                        break;
                    case 0x5://SET 4,L
                        SET(hl.bytes.l, 0b00010000);
                        break;
                    case 0x6://SET 4,(HL)
                    {
                        uint8_t temp = Bus->read(hl.hl);
                        SET(temp, 0b00010000);
                        Bus->write(hl.hl, temp);
                        //extra time for hl
                        break;
                    }
                    case 0x7://SET 4,A
                        SET(af.bytes.a, 0b00010000);
                        break;
                    case 0x8://SET 5,B
                        SET(bc.bytes.b, 0b00100000);
                        break;
                    case 0x9://SET 5,C
                        SET(bc.bytes.c, 0b00100000);
                        break;
                    case 0xa://SET 5,D
                        SET(de.bytes.d, 0b00100000);
                        break;
                    case 0xb://SET 5,E
                        SET(de.bytes.e, 0b00100000);
                        break;
                    case 0xc://SET 5,H
                        SET(hl.bytes.h, 0b00100000);
                        break;
                    case 0xd://SET 5,L
                        SET(hl.bytes.l, 0b00100000);
                        break;
                    case 0xe://SET 5,(HL)
                    {
                        uint8_t temp = Bus->read(hl.hl);
                        SET(temp, 0b00100000);
                        Bus->write(hl.hl, temp);
                        //extra time for hl
                        break;
                    }
                    case 0xf://SET 5,A
                        SET(af.bytes.a, 0b00100000);
                        break;
                }
                break;

            case 0xf:
                switch(opcodeL)
                {
                    case 0x0://SET 6,B
                        SET(bc.bytes.b, 0b01000000);
                        break;
                    case 0x1://SET 6,C
                        SET(bc.bytes.c, 0b01000000);
                        break;
                    case 0x2://SET 6,D
                        SET(de.bytes.d, 0b01000000);
                        break;
                    case 0x3://SET 6,E
                        SET(de.bytes.e, 0b01000000);
                        break;
                    case 0x4://SET 6,H
                        SET(hl.bytes.h, 0b01000000);
                        break;
                    case 0x5://SET 6,L
                        SET(hl.bytes.l, 0b01000000);
                        break;
                    case 0x6://SET 6,(HL)
                    {
                        uint8_t temp = Bus->read(hl.hl);
                        SET(temp, 0b01000000);
                        Bus->write(hl.hl, temp);
                        //extra time for hl
                        break;
                    }
                    case 0x7://SET 6,A
                        SET(af.bytes.a, 0b01000000);
                        break;
                    case 0x8://SET 7,B
                        SET(bc.bytes.b, 0b10000000);
                        break;
                    case 0x9://SET 7,C
                        SET(bc.bytes.c, 0b10000000);
                        break;
                    case 0xa://SET 7,D
                        SET(de.bytes.d, 0b10000000);
                        break;
                    case 0xb://SET 7,E
                        SET(de.bytes.e, 0b10000000);
                        break;
                    case 0xc://SET 7,H
                        SET(hl.bytes.h, 0b10000000);
                        break;
                    case 0xd://SET 7,L
                        SET(hl.bytes.l, 0b10000000);
                        break;
                    case 0xe://SET 7,(HL)
                    {
                        uint8_t temp = Bus->read(hl.hl);
                        SET(temp, 0b10000000);
                        Bus->write(hl.hl, temp);
                        //extra time for hl
                        break;
                    }
                    case 0xf://SET 7,A
                        SET(af.bytes.a, 0b10000000);
                        break;
                }
             
                break;
        }
    }