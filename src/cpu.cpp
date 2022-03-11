#include "cpu.h"
    
    cpu::cpu(bus* Bus)
    {           
        fp = fopen ("file.txt", "w+");
        this->Bus = Bus;
        cycles = 0;
        af.af = 0;
        bc.bc = 0;
        de.de = 0;
        sp.sp = 0;
        pc.pc = 0;
        IF = 0;
        IE = 0;
        IME = false;
        bootRomDone = false;
        DIV = 0;
        TIMA = 0;
        TMA = 0;
        TAC = 0;
        TIMA_speed = 0;
        totalTicks_DIV = 0;
        totalTicks_TIMA = 0;
        IME = 0;
        IMEdelay = false;
        servicingInterrupt = false;
        cycles = 0;
    }

    void cpu::checkInterrupts()
    {
        if(!IMEdelay)
        {
            if(IME)
            {
                if(IE & (0b00000001)) //VBLANK
                {
                    if(IF & (0b00000001))
                    {
                        servicingInterrupt = true;
                        IF &= 0b11111110;
                        IME = false;
                        
                        sp.sp--;
                        Bus->write(sp.sp, pc.bytes.p);
                        sp.sp--;
                        Bus->write(sp.sp, pc.bytes.c);

                        pc.pc = 0x40;
                        cycles += 20;
                        return;
                    }
                
                }
                
                if(IE & (0b00000010)) //LCD STAT
                {
                    if(IF & (0b00000010))
                    {
                        servicingInterrupt = true;
                        IF &= 0b11111101;
                        IME = false;
                        
                        sp.sp--;
                        Bus->write(sp.sp, pc.bytes.p);
                        sp.sp--;
                        Bus->write(sp.sp, pc.bytes.c);

                        pc.pc = 0x48;
                        cycles += 20;
                        return;
                    }

                }
                
                if(IE & (0b00000100)) //TIMER
                {
                    if(IF & (0b00000100))
                    {
                        servicingInterrupt = true;
                        IF &= 0b11111011;
                        IME = false;
                        
                        sp.sp--;
                        Bus->write(sp.sp, pc.bytes.p);
                        sp.sp--;
                        Bus->write(sp.sp, pc.bytes.c);

                        pc.pc = 0x50;
                        cycles += 20;
                        return;
                    }
                }
                
                if(IE & (0b00001000)) //SERIAl
                {
                    if(IF & (0b00001000))
                    {
                        servicingInterrupt = true;
                        IF &= 0b11101111;
                        IME = false;
                        sp.sp--;
                        Bus->write(sp.sp, pc.bytes.p);
                        sp.sp--;
                        Bus->write(sp.sp, pc.bytes.c);

                        pc.pc = 0x58;
                        cycles += 20;
                        return;
                    }
                }
                
                if(IE & (0b00010000)) //JOYPAD
                {
                    if(IF & (0b00010000))
                    {
                        servicingInterrupt = true;
                        IF &= 0b11101111;
                        IME = false;
                        
                        sp.sp--;
                        Bus->write(sp.sp, pc.bytes.p);
                        sp.sp--;
                        Bus->write(sp.sp, pc.bytes.c);

                        pc.pc = 0x60;
                        cycles += 20;
                        return;
                    }
                }

            }
            
        }
        
        IMEdelay = false;
    }

void cpu::updateTimers()
{
    int ticks = cycles;
    while(ticks > 0)
    {
        totalTicks_DIV += ticks;
        DIV = ((uint8_t)((totalTicks_DIV & 0xff00) >> 8));
        
        if(TAC & 0b00000100)
        {    
            int newTicks = 0;  
            totalTicks_TIMA += ticks;
            if(totalTicks_TIMA / TIMA_speed)
            {
                newTicks = totalTicks_TIMA / TIMA_speed;
                totalTicks_TIMA -= TIMA_speed;
            }

            int temp = ((uint16_t)((TIMA + newTicks))) & 0xff00;

            if(temp){
                IF |= 0b00000100;
                TIMA = TMA;
                return;
            }

            TIMA += newTicks;
        }

        ticks--;
    }



}

