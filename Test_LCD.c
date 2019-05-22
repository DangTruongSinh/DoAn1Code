#include <16f877a.h>
#FUSES NOWDT, HS, NOPUT, NOPROTECT, NODEBUG, NOBROWNOUT, NOLVP, NOCPD, NOWRT
#use delay(CLOCK=20MHz) 
#use  I2C(MASTER,SLOW,SDA = PIN_C4, SCL = PIN_C3)
#include "D:\Chep Cho SV\Thu Vien\TV_PICKIT2_SHIFT_DS13B07_I2C.C"
#include "D:\DoAn1\LCD.c"

UNSIGNED INT KEY_NHAN()
{     SIGNED INT8  MAQUETCOT,MAPHIM,HANG,COT;
      MAQUETCOT = 0B11111110;   MAPHIM=HANG=0XFF;
      FOR(COT=0;COT<4;COT++)
         {   
            OUTPUT_B(MAQUETCOT);  MAQUETCOT  = (MAQUETCOT<<1) + 0x1;
            IF         (!INPUT(PIN_B4))      {HANG=0;   BREAK;}
            ELSE    IF (!INPUT(PIN_B5))      {HANG=1;   BREAK;}
            ELSE    IF (!INPUT(PIN_B6))      {HANG=2;   BREAK;}
            ELSE    IF (!INPUT(PIN_B7))      {HANG=3;   BREAK;}
          
         }          
      IF (HANG!=0XFF)   MAPHIM   = COT*4 + HANG;
      RETURN(MAPHIM);
  }

void main()
{
   lcd_setup();
   lcd_clear();
  
   port_b_pullups (0b11111111);
   //write_eeprom(0,'1');
   lcd_data("sinh");
   lcd_goto_xy(2,3);
   lcd_data("ha");
   unsigned int8 key;
   THIET_LAP_THOI_GIAN_HIEN_TAI();
   NAP_THOI_GIAN_HTAI_VAO_DS13B07();
   lcd_data("dsad");
   while(true)
   {
       
      DOC_THOI_GIAN_TU_REALTIME();
      LCD_COMMAND (0x8C);   
      LCD_DATA(GIO_DS13/16  +0X30);    LCD_DATA(GIO_DS13%16  +0X30);
      LCD_DATA(' ');
      LCD_DATA(PHUT_DS13/16 +0X30);    LCD_DATA(PHUT_DS13%16 +0X30);
      LCD_DATA(' ');
      LCD_DATA(GIAY_DS13/16 +0X30);    LCD_DATA(GIAY_DS13%16 +0X30);
      LCD_COMMAND (0xCC);  
      LCD_DATA(NGAY_DS13/16 +0X30);    LCD_DATA(NGAY_DS13%16 +0X30);
      LCD_DATA(' ');
      LCD_DATA(THANG_DS13/16 +0X30);   LCD_DATA(THANG_DS13%16 +0X30);
      LCD_DATA(' ');
      LCD_DATA(NAM_DS13/16 +0X30);     LCD_DATA(NAM_DS13%16 +0X30); 
   }

}
