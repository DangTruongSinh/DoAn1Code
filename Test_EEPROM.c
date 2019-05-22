#include <16f877a.h>
#FUSES NOWDT, HS, NOPUT, NOPROTECT, NODEBUG, NOBROWNOUT, NOLVP, NOCPD, NOWRT
#use delay(CLOCK=20MHz) 
#include "D:\DoAn1\LCD.c"
void writeEEPROM(unsigned char arr[],unsigned int8 vitri)
{
   int i;
   for(i = 0;arr[i]!='\0';i++)
   {
      write_eeprom (vitri + i, arr[i]);
   }
   // xoa du lieu con lai
   for(;i<8;i++)
   {
        write_eeprom (vitri + i,0xff);
   }
  
}
// doc tu eeprom
void readEEPROM(unsigned char arr[],unsigned int8 vitri)
{
   unsigned int8 i;
   char c;
   for( i =0;i < 8;i++)
   {
      c =   read_eeprom (vitri + i);
      if(c == 0xff)
        {
         arr[i]='\0';
         return;
        }
      else
         arr[i] =c;

   }
}
 unsigned char c[8] = {0,0,0,0,0,0,0,0};
void main()
{
   lcd_setup();
   lcd_clear();
  /* char arr[]={'1','2','3','\0'};
  writeEEPROM(arr,0);
  writeEEPROM(arr,8);
   delay_ms(300);*/
 unsigned int i=0;
  readEEPROM(c,0);
 lcd_goto_xy(0,0);
 for(i = 0;i < 8;i++)
 {
   if(c[i]!=0)
      lcd_data(c[i]);
 }
  readEEPROM(c,8);
 lcd_goto_xy(1,0);
 for(i = 0;i < 8;i++)
 {
   if(c[i]!=0)
      lcd_data(c[i]);
 }
   while(true)
   {
      output_high(pin_c4);
      
   }
}
