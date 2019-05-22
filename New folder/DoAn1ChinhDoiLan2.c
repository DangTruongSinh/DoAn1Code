
#include <16f877a.h>
#use I2C(MASTER,SDA = PIN_A1, SCL =PIN_A2)
#INCLUDE <stdlibm.h>
#include <string.h> // su dung thu vien nay de xai ham so sanh 2 chuoi
#FUSES NOWDT, HS, PUT, NOPROTECT
#use delay(CLOCK=20MHz) 
#include "D:\DoAn1\LCD.c"
#include "D:\Chep Cho SV\Thu Vien\TV_PICKIT2_SHIFT_DS13B07_I2C.C"
// Chuyen doi sang mang ky tu phu hop voi ma tran phim thuc te
unsigned CHAR mangKyTu [] ={'1','2','3','A','4','5','6','B','7','8','9','C','*','0','#','D'};
// Bien quet phim ma tran
unsigned CHAR key;
//Tao 2 mang de luu thong tin tai khoan va mat khau
unsigned CHAR tk[8];
unsigned CHAR mk[8];
//Bien viTriManHinh = 0: Man Hinh Dang Nhap, viTriManHinh = 1: Man hinh sau khi dang nhap thanh cong
unsigned INT1 isDangNhapSuccess = 0;
//Tai khoan admin, su dung con tro cho nhanh
unsigned CHAR *tkAdmin = "123";
//Bien isAdmin de xac dinh xem tai khoan dang nhap co phai la admin hay la user
unsigned INT1 isAdmin =0;
//2 bien nay dung de lam chi so index cho mang tk[] va mk[]
unsigned INT8 vitriTK =0;
unsigned INT8 vitriMK =0;
//Bien nay dung de luu vi tri tai khoan dc chon, dung bien nay de lay mat khau
unsigned INT8 vitriTKSelected;
// Bien nay dung trong ham xulydangNhap de xac dinh dang nhap tai khoan hay mat khau
unsigned INT1 TKMKisInPuted=0;
//bien nay dung de xac dinh coi co tao user hay ko
unsigned INT1 isTaoTK =0;
unsigned INT8 isLevelMhOfAdmin =0;
unsigned INT8 isLevelMhOfUser =0;
unsigned INT1 isInputOK = 0;
unsigned INT8 soLanSai = 0;
//
unsigned CHAR *arr[15];
unsigned INT8 tang = 0;
unsigned INT8 tangcu = 0;
unsigned INT8 giam = 0;
unsigned INT8 numBerOfMangContro = 0;
unsigned INT1 userDaDangNhap = 0;
unsigned CHAR lichSuLogin[8];
// ham quet phim
UNSIGNED CHAR KEY_NHAN()
{
   SIGNED INT8 MAQUETCOT, MAPHIM, HANG, COT;
   MAQUETCOT = 0B11111110; MAPHIM = HANG = 0XFF;
   FOR (COT = 0; COT < 4; COT++)
   {
      OUTPUT_B (MAQUETCOT); MAQUETCOT = (MAQUETCOT<<1) + 0x1;
      IF  (!INPUT (PIN_B4)) {HANG = 0; BREAK; }
      ELSE IF (!INPUT (PIN_B5)) {HANG = 1; BREAK; }
      ELSE IF (!INPUT (PIN_B6)) {HANG = 2; BREAK; }
      ELSE IF (!INPUT (PIN_B7)) {HANG = 3; BREAK; }
   }

   IF (HANG != 0XFF) MAPHIM = mangKyTu[COT * 4 + HANG];
   RETURN (MAPHIM) ;
}

// viet xuong eeprom
void writeEEPROM(UNSIGNED char arr[],unsigned int8 vitri)
{
   INT i;
   FOR (i = 0;arr[i] != '\0'; i++)
   {
      write_eeprom (vitri + i, arr[i]);
   }

   // xoa du lieu con lai
   FOR (; i < 8; i++)
   {
      write_eeprom (vitri + i,0xff);
   }

}

// doc tu eeprom
void readEEPROM(UNSIGNED char arr[],unsigned int8 vitri)
{
   UNSIGNED int8 i;
   CHAR c;
   FOR (i = 0;i < 8; i++)
   {
      c = read_eeprom (vitri + i) ;
      IF (c == 0xff)
      {
         arr[i] = '\0';
         RETURN;
      }

      ELSE
      arr[i] = c;
   }
}

// Man Hinh hien thi thong bao loi sai khi thong tin dang nhap sai
void manHinhThongBaoSai()
{
   lcd_clear () ;
   lcd_GOTO_xy (1, 0) ;
   lcd_data (" Thong tin TK sai ! ");
   delay_ms (3000) ;
}

//input du lieu tu phim nhap vao Mang
void inputData(UNSIGNED char arr[],unsigned int8 vitri)
{
   arr[vitri] = key;
}

// hien thi inout tk va mk
void DisplayInputTK(UNSIGNED char arr[],unsigned int8 hang)
{
   lcd_GOTO_xy (hang, vitriTK + 3) ;
   lcd_data (key) ;
   inputData (arr, vitriTK) ;
}

void DisplayInputMK(UNSIGNED char arr[])
{
   lcd_GOTO_xy (1, vitriMK + 3) ;
   IF (key == ' ')
   lcd_data (' ') ;
   ELSE
   lcd_data('*');
   inputData (arr, vitriMK) ;
}

// Ham Hien Thi Man Hinh Admin
void mHAdmin()
{
   lcd_clear () ;
   lcd_GOTO_xy (0, 0) ;
   lcd_data ("1.Them TK   2.Xoa TK");
   lcd_GOTO_xy (1, 0) ;
   lcd_data ("3.Doi Mk    4.Exit");
   lcd_GOTO_xy (2, 0) ;
   lcd_data ("5.Lich su Dang Nhap") ;
   lcd_GOTO_xy (3, 0) ;
   lcd_data ("Nhap : ") ;
}

// Ham hien thi man hinh user
void mHUser()
{
   lcd_clear () ;
   lcd_GOTO_xy (1, 0) ;
   lcd_data ("1.Doi mat khau") ;
   lcd_GOTO_xy (2, 0) ;
   lcd_data ("2.Thoat") ;
   lcd_GOTO_xy (3, 0) ;
   lcd_data ("Nhap:") ;
}

// Ham reset TK va Mk sau khi dang nhap xong
void resetTKMK()
{
   UNSIGNED int8 i;
   vitriTK = 0;
   vitriMK = 0;
   FOR (i = 0; i < 8; i++)
   {
      tk[i] = 0;
      mk[i] = 0;
   }
}

// Ham check thong tin dang nhap
unsigned INT1 checkMK()
{
   
   UNSIGNED char arr[8];
   mk[vitriMK] = '\0';
   isAdmin = 0;
   readEEPROM (arr, vitriTKSelected + 8) ;
   IF (strcmp (arr, mk) ==  0)
   {
      IF (strcmp (tkAdmin, tk) == 0)
      {
         isAdmin = 1;
         mHAdmin ();
      }

      ELSE
      {
         userDaDangNhap = 1;
         DOC_THOI_GIAN_TU_REALTIME () ;
         lichSuLogin[0] = GIO_DS13 + 1;
         lichSuLogin[1] = PHUT_DS13 + 1;
         lichSuLogin[2] = GIAY_DS13 + 1;
         lichSuLogin[3] = NGAY_DS13 + 1;
         lichSuLogin[4] = THANG_DS13 + 1;
         lichSuLogin[5] = NAM_DS13 + 1;
         lichSuLogin[6] = 0;
         writeEEPROM (lichSuLogin, 248) ;
         writeEEPROM (tk, 240) ;
         mHUser () ;
      }

      
      resetTKMK () ;
      RETURN 1;
   }

   RETURN 0;
}

int8 checkTK()
{
   
   UNSIGNED char arr[8];
   UNSIGNED int16 i;
   tk[vitriTK] = '\0';
   FOR (i = 0;i <= 224; i = i + 16)
   {
      IF (read_eeprom (i) == 0xff)
      CONTINUE;
      readEEPROM (arr, i) ;
      IF (strcmp (arr, tk) ==  0)
      {
         vitriTKSelected = i;
         RETURN 1;
      }
   }

   RETURN 0;
}

// Man hinh giao dien dang nhap
void manHinhDangNhap()
{
   lcd_clear () ;
   lcd_GOTO_xy (0, 0) ;
   lcd_data ("TK:") ;
   lcd_GOTO_xy (1, 0) ;
   lcd_data ("MK:") ;
   lcd_GOTO_xy (2, 0) ;
   lcd_data (" DO dai max = 8");
   lcd_GOTO_xy (3, 0) ;
   lcd_data (" Nhan D de xac nhan") ;
   lcd_GOTO_xy (0, 3) ;
}

void xulyDangNhapSai()
{
   manHinhThongBaoSai () ;
   manHinhDangNhap () ;
}

unsigned INT1 kiemTraTKMK()
{
   IF (checkTK () == 0||checkMK () == 0)
   RETURN 0;
   RETURN 1;
}

void manHinhThemUser()
{
   resetTKMK () ;
   lcd_clear () ;
   lcd_GOTO_xy (0, 0); lcd_data ("TK:"); lcd_goto_xy (1, 0); lcd_data ("MK:") ;
   lcd_GOTO_xy (2, 0); lcd_data (" Tao User"); lcd_goto_xy (0, 3);
}

void manHinhXoaTaiKhoan()
{
   
   resetTKMK () ;
   lcd_clear () ;
   lcd_GOTO_xy (3, 0) ;
   lcd_data ("TK:") ;
}

void manHinhDoiMatKhau()
{
   lcd_clear () ;
   lcd_GOTO_xy (0, 0); lcd_data (" Nhap MK moi:");
   lcd_GOTO_xy (1, 0); lcd_data ("MK:") ;
}

void thongBaoDoiMKTC()
{
   lcd_clear () ;
   lcd_GOTO_xy (1, 0); lcd_data (" Doi MK thanh cong ! "); delay_ms (2000) ;
}

void xuLyInputTK(UNSIGNED int8 hang)
{
   IF (TKMKisInPuted == 0)
   {
      IF (key != 'A'&&key != 'B'&&key != 'C')
      {
         IF (key != 'D'&&key != '#')
         {
            IF (vitriTK < 8)
            {
               DisplayInputTK (tk, hang) ;
               vitriTK++;
            }
         }

         else IF (key == '#')
         {
            IF (vitriTK > 0)
            {
               vitriTK--;
               key = ' ';
               DisplayInputTK (tk, hang) ;
               lcd_GOTO_xy (hang, vitriTK + 3) ;
            }

         }

         ELSE
         {
            TKMKisInPuted = 1;
            key = 0xff;
            lcd_GOTO_xy (1, 3) ;
         }
      }
   }
}

void xulyInputMK()
{
   IF (TKMKisInPuted == 1)
   {
      IF (key != 'A'&&key != 'B'&&key != 'C')
      {
         IF (key != 'D'&&key != 0xff&&key != '#')
         {
            IF (vitriMK < 8)
            {
               DisplayInputMK (mk) ;
               vitriMK++;
            }
         }

         else IF (key == '#')
         {
            IF (vitriMK > 0)
            {
               vitriMK--;
               key = ' ';
               DisplayInputMK (mk) ;
               lcd_GOTO_xy (1, vitriMK + 3) ;
            }

         }

         else IF (key == 'D')
         {
            isInputOK = 1;
         }
      }
   }
}

void thietLapAdmin1()
{
   isLevelMhOfAdmin = 1; resetTKMK (); manHinhThemUser (); TKMKisInPuted =  0;
}

void thietLapAdmin2()
{
   isLevelMhOfAdmin = 2; TKMKisInPuted = 0; resetTKMK (); manHinhXoaTaiKhoan ();
}

void thietLapAdmin3()
{
   resetTKMK () ;
   isLevelMhOfAdmin = 3;
   TKMKisInPuted = 1;
   lcd_clear (); lcd_GOTO_xy (0, 0); lcd_data (" Nhap MK moi:");
   lcd_GOTO_xy (1, 0) ;
   lcd_data ("MK:") ;
}

void backAdmin()
{
   resetTKMK () ;
   isLevelMhOfAdmin = 0;
   TKMKisInPuted = 0;
   isTaoTK = 0;
   mHAdmin () ;
}

void readAllEpprom()
{
   UNSIGNED int16 i;
   FOR (i = 16; i <= 224;i = i + 16)
   {
      IF (read_eeprom (i) != 0xff)
      {
         CHAR * d = malloc (8);
         readEEPROM (d, i) ;
         arr[numberOfMangContro] = d;
         numberOfMangContro++;
      }

   }
}

void hienthi(UNSIGNED int8 vitri)
{
   UNSIGNED int8 i = 0;
   WHILE (* (arr[vitri] + i) !=  0)
   {
      lcd_data ( * (arr[vitri] + i));
      i++;
   }
}

void hienThiNDungCtRaLCD(UNSIGNED int8 i)
{
   UNSIGNED int8 chuc;
   UNSIGNED int8 donvi;
   chuc = (i + 1) / 10;
   donvi = (i + 1) % 10;
   lcd_GOTO_xy (i % 3, 0) ;
   IF (chuc == 0)
   lcd_data (donvi + 0x30) ;
   ELSE
   {
      lcd_data (chuc + 0x30) ;
      lcd_data (donvi + 0x30) ;
   }

   lcd_data (".") ;
   hienthi (i) ;
   lcd_GOTO_xy (3, 0) ;
   lcd_data ("TK:") ;
}

void xuLyXemThem()
{
   UNSIGNED int8 i;
   IF (numberOfMangContro == 0)
   {
      lcd_GOTO_xy (0, 0) ;
      lcd_data (" Khong co User nao ! ") ;
   }

   else IF (tang < numberOfMangContro)
   {
      lcd_clear () ;
      tangcu = tang;
      IF (numberOfMangContro < tang + 3)
      tang = tang + (numberOfMangContro % 3) ;
      ELSE
      tang = tang + 3;
      FOR (i = tangcu; i < tang;i++)
      hienThiNDungCtRaLCD (i) ;
      giam = tangcu;
   }
}

void main()
{
   UNSIGNED int16 i;
   UNSIGNED char c;
   // UNSIGNED char arr2[8];
   UNSIGNED char arrXoa[8] = {0, 0, 0, 0, 0, 0, 0, 0};
   UNSIGNED int1 resultTaoTK;
 /*  THIET_LAP_THOI_GIAN_HIEN_TAI () ;
   NAP_THOI_GIAN_HTAI_VAO_DS13B07 () ;*/
   lcd_setup () ;
   lcd_clear () ;
   set_tris_d (0x00) ;
   set_tris_b (0b11110000) ;
   port_b_pullups (0b00001111) ;
   lcd_setup () ;
   manHinhDangNhap () ;
   UNSIGNED char arr1[] = {'1', '2', '3', '\0'};
   writeEEPROM (arr1, 0); writeEEPROM (arr1, 8);
   WHILE (true)
   {
      key = KEY_NHAN (); // quet phim
      IF (key != 0xff)
      {
         delay_ms (50); // cho phim on dinh
         key = KEY_NHAN (); // kiem tra phim lai lan nua
         IF (key != 0xff)
         {
            // Xu ly O man hinh dang nhap
            IF (isDangNhapSuccess == 0)
            {
               xuLyInputTK (0);
               xulyInputMK () ;
               IF (isInputOK == 1)
               {
                  IF (kiemTraTKMK ())
                  {
                     isDangNhapSuccess = 1; soLanSai = 0;
                  }

                  ELSE
                  { xulyDangNhapSai (); soLanSai++; }
                  resetTKMK () ;
                  TKMKisInPuted = 0;
                  isInputOK = 0;
               }

               IF (soLanSai == 3)
               {
                  lcd_clear () ;
                  lcd_GOTO_xy (1, 0) ;
                  lcd_data (" Bi khoa trong (s)");
                  output_high (pin_c0) ;
                  FOR (i = 5;i > 0; i--)
                  {
                     sl (i, 2, 8) ;
                     delay_ms (1000) ;
                  }

                  output_low (pin_c0) ;
                  lcd_clear () ;
                  manHinhDangNhap () ;
                  soLanSai = 0;
               }
            }

            ELSE
            {
               IF (isAdmin)
               {
                  IF (isLevelMhOfAdmin == 0)
                  {
                     IF (key == '1')
                     thietLapAdmin1 () ;
                     else IF (key == '2')
                     {thietLapAdmin2 (); tang = 0;numberOfMangContro = 0; readAllEpprom (); xuLyXemThem (); }
                     else IF (key == '3')
                     thietLapAdmin3 () ;
                     else IF (key == '4')
                     {
                        resetTKMK () ;
                        isLevelMhOfAdmin = 0;
                        TKMKisInPuted = 0; isDangNhapSuccess = 0;
                        manHinhDangNhap () ;
                     }

                     else IF (key == '5')
                     {
                        isLevelMhOfAdmin = 5;
                        lcd_clear () ;
                        lcd_GOTO_xy (0, 0) ;
                        lcd_data ("Thong Tin Dang Nhap") ;
                        IF (userDaDangNhap == 1)
                        {
                           readEEPROM (tk, 0xF0) ;
                           readEEPROM (lichSuLogin, 0xF8) ;
                           lcd_GOTO_Xy (1, 0) ;
                           lcd_data ("User:") ;
                           i = 0;
                           WHILE (tk[i] != 0) lcd_data (tk[i++]);
                           lcd_GOTO_xy (2, 4) ;
                           LCD_DATA ( (lichSuLogin[0] - 1) / 16 + 0X30) ; LCD_DATA( (lichSuLogin[0] - 1) % 16   + 0X30);
                           LCD_DATA (' ') ;
                           LCD_DATA ( (lichSuLogin[1] - 1) / 16 + 0X30); LCD_DATA ( (lichSuLogin[1] - 1) % 16  + 0X30);
                           LCD_DATA (' ') ;
                           LCD_DATA ( (lichSuLogin[2] - 1) / 16 + 0X30); LCD_DATA ( (lichSuLogin[2] - 1) % 16  + 0X30);
                           lcd_GOTO_xy (3, 4);
                           LCD_DATA ( (lichSuLogin[3] - 1) / 16 + 0X30); LCD_DATA ( (lichSuLogin[3] - 1) % 16  + 0X30);
                           LCD_DATA (' ') ;
                           LCD_DATA ( (lichSuLogin[4] - 1) / 16 + 0X30); LCD_DATA( (lichSuLogin[4] - 1) % 16  + 0X30);
                           LCD_DATA (' ') ;
                           lcd_data ("20") ;
                           LCD_DATA ( (lichSuLogin[5] - 1) / 16 + 0X30); LCD_DATA( (lichSuLogin[5] - 1) % 16 + 0X30);
                        }

                        else IF (userDaDangNhap == 0)
                        {
                           lcd_GOTO_xy (1, 0) ;
                           lcd_data ("Chua co US Dang Nhap") ;
                        }
                     }
                  }

                  else IF (isLevelMhOfAdmin == 1)
                  {
                     
                     IF (isTaoTK == 0)
                     {
                        IF (key == 'D')
                        {
                           resultTaoTK = checkTK ();
                           IF (resultTaoTK == 1)
                           {
                              lcd_clear (); lcd_GOTO_xy (1, 0); lcd_data (" TK da ton tai ! "); delay_ms (2000);
                              resetTKMK () ;
                              manHinhThemUser () ;
                           }

                           ELSE
                           {
                              isTaoTK = 1;
                              TKMKisInPuted = 1;
                              lcd_GOTO_xy (1, 3) ;
                           }
                        }

                        else IF (key == 'C')
                        backAdmin () ;
                        ELSE xuLyInputTK (0);
                     }

                     ELSE
                     {
                        IF (key == 'D')
                        {
                           IF (vitriMK > 0)
                           {
                              // TIm vung nho de luu tai khoa vao
                              FOR (i = 16; i <= 224; i = i + 16)
                              {
                                 c = read_eeprom (i);
                                 IF (c == 0xff)
                                 BREAK;
                              }

                              IF (i <= 224)
                              {
                                 writeEEPROM (tk, i) ;
                                 writeEEPROM (mk, i + 8) ;
                                 lcd_clear (); lcd_GOTO_xy (1, 0) ;
                                 lcd_data (" Tao Tk thanh cong ! ") ;
                                 delay_ms (2000) ;
                                 // reset toan bo bien de tro ve man hinh admin
                                 isLevelMhOfAdmin = 0;
                                 isTaoTK = 0;
                                 mHAdmin ();
                              }

                              ELSE
                              {
                                 lcd_clear (); lcd_GOTO_xy (1, 0); lcd_data (" Het Dung luong"); delay_ms (1000); mHAdmin();
                              }
                           }
                        }

                        else IF (key == 'C')
                        backAdmin () ;
                        ELSE
                        xulyInputMK () ;
                     }

                  }

                  else IF (isLevelMhOfAdmin == 2)
                  {
                     IF (key == 'D')
                     {
                        // ko cho xoa tk admin
                        IF (checkTK () &&strcmp (tk, tkAdmin))
                        {
                           writeEEPROM (arrXoa, vitriTKSelected) ;
                           writeEEPROM (arrXoa, vitriTKSelected + 8) ;
                           lcd_clear () ;
                           lcd_GOTO_xy (1, 0); lcd_data (" Xoa thanh cong ! ");
                        }

                        ELSE
                        {
                           lcd_clear () ;
                           lcd_GOTO_xy (1, 0); lcd_data ("Tai Khoan ko ton tai") ;
                        }

                        
                        isLevelMhOfAdmin = 0;
                        delay_ms (2000) ;
                        mHAdmin () ;
                     }

                     else IF (key == 'C')
                     { backAdmin (); FOR (i = 0; i < 10;i++) free (arr[i]); }
                     else IF (key == 'B')
                     xuLyXemThem () ;
                     else IF (key == 'A')
                     {
                        IF (giam > 0)
                        {
                           lcd_clear () ;
                           FOR (i = giam - 3; i < giam ;i++)
                           hienThiNDungCtRaLCD (i);
                           tang = giam;
                           giam = giam - 3;
                        }
                     }

                     ELSE
                     xulyInputTK (3) ;
                  }

                  else IF (isLevelMhOfAdmin == 3)
                  {
                     IF (key == 'D')
                     {
                        mk[vitriMK] = '\0';
                        writeEEPROM (mk, 8) ;
                        thongBaoDoiMKTC () ;
                        isLevelMhOfAdmin = 0;
                        mHAdmin () ;
                     }

                     else IF (key == 'C')
                     backAdmin () ;
                     ELSE
                     xulyInputMK () ;
                  }

                  else IF (isLevelMhOfAdmin == 5)
                  {
                     
                     
                     IF (key == 'C')
                     {
                        backAdmin () ;
                     }
                  }

               }

               ELSE
               {
                  IF (isLevelMhOfUser == 0)
                  {
                     IF (key == '1')
                     {
                        resetTKMK ();
                        manHinhDoiMatKhau () ;
                        TKMKisInPuted = 1;
                        isLevelMhOfUser = 1;
                     }

                     else IF (key == '2')
                     {
                        resetTKMK ();
                        isLevelMhOfUser = 0;
                        TKMKisInPuted = 0; isDangNhapSuccess = 0;
                        manHinhDangNhap () ;
                     }
                  }

                  else IF (isLevelMhOfUser == 1)
                  {
                     IF (key == 'D')
                     {
                        mk[vitriMK] = '\0';
                        writeEEPROM (mk, vitriTKSelected + 8) ;
                        thongBaoDoiMKTC () ;
                        mHUser () ;
                        isLevelMhOfUser = 0;
                     }

                     else IF (key == 'C')
                     {
                        resetTKMK () ;
                        isLevelMhOfUser = 0;
                        mHUser () ;
                     }

                     ELSE
                     {
                        xulyInputMK () ;
                     }
                  }
               }
            }

            WHILE (KEY_NHAN () != 0xff); // cho buong phim
            delay_ms (50); // cho phim on dinh
         }
      }
   } }

