#ifndef __KeyPad_4x4__
#define __KeyPad_4x4__


#define ROW1 PIN_C0 
#define ROW2 PIN_C1
#define ROW3 PIN_C2
#define ROW4 PIN_C3

#define COL1 input(PIN_C4)
#define COL2 input(PIN_C5)
#define COL3 input(PIN_C6)
#define COL4 input(PIN_C7)


#define breakNoisyTime 10
static unsigned char KEYS_PAD[16] = { '7', '8' , '9' , '/',
                                                    '4' ,'5' , '6', 'x',
                                                    '1' , '2' , '3', '-',
                                                    '*' , '0', '=' , '+' };
void KeyPad_Control(int row, int1 state)
{
    int PIN;
    switch(row)
    {
        case 1: PIN = ROW1; break;
        case 2: PIN = ROW2; break;
        case 3: PIN = ROW3; break;
        case 4: PIN = ROW4; break;
    }
    output_bit(PIN, state);
}
void KeyPad_Init(void)
{
    // Set TRISC REGISTER
    set_tris_b(0xF0);       
    for(int i1=0;i1<4;i1++)
    {
         KeyPad_Control(i1, TRUE);
    }
}
