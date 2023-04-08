#include <Arduino.h>
#include "EEPROM.h"
#include <Servo.h>
#include <AccelStepper.h>
#include <MultiStepper.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,20,4); //20: SDA; 21: SCL

typedef struct
{
    uint16_t X;
    uint16_t Y;
    uint16_t Z;
    uint8_t kep;
} POS;

POS robot;
POS read_robot;

//nut nhan menu
#define enter   23
#define len 17
#define xuong 16
bool gt_enter, gt_len, gt_xuong;
bool last_enter=1, last_len=1, last_xuong=1;

Servo servo_kep;

int step_X = 54; int Dir_X = 55; int ena_X = 38;
int step_Y = 60; int Dir_Y = 61; int ena_Y = 56;
int step_Z = 46; int Dir_Z = 48; int ena_Z = 62;

AccelStepper Step_X(1, step_X, Dir_X, ena_X);
AccelStepper Step_Y(1, step_Y, Dir_Y, ena_Y);
AccelStepper Step_Z(1, step_Z, Dir_Z, ena_Z);

#define end_X  3
#define end_Y  14
#define end_Z  18

int bt_X = A3; 
int bt_Y = A4; 
int bt_Z = A5; 
int nutnhan = A10;

int gt_bt_X, gt_bt_Y, gt_bt_Z;
int gt_nutnhan; bool macdinh_ser=1; unsigned int dem=0;

int bandau_X = 0, bandau_Y = 0, bandau_Z = 0;
int buoc_X, buoc_Y, buoc_Z; bool goc_ser;

int menu_pos=0;
uint8_t screen_page=0;
uint16_t KT_colenh;
uint16_t vitri=1;
uint16_t pos_save=2;
bool check_run;
uint16_t end_add;

bool KT_lenh() //Kiem tra co lenh duoc luu chua
{
    if(EEPROM.get(0, KT_colenh)==0) //khong co lenh
        return 0;
    else return 1;
}

void toa_do()
{
    lcd.setCursor(0, 1);
    lcd.print("X: ");
    lcd.setCursor(3, 1);
    lcd.print(robot.X);    
    lcd.setCursor(7, 1);
    lcd.print("Y: ");
    lcd.setCursor(10, 1);
    lcd.print(robot.Y);
    lcd.setCursor(14, 1);
    lcd.print("Z: ");
    lcd.setCursor(17, 1);
    lcd.print(robot.Z);
    lcd.setCursor(0, 2);
    lcd.print("Ser: ");
    lcd.setCursor(5, 2);
    lcd.print(robot.kep);        
}

void manhinh_chinh()
{
    lcd.clear();
    toa_do();  
    lcd.setCursor(5, 0);
    lcd.print("ROBOT ARM");            
    if(KT_lenh()==1) 
    {
        lcd.setCursor(0, 3);
        lcd.print("Lenh: YES");
    }
    else 
    {
        lcd.setCursor(0, 3);
        lcd.print("Lenh: NO");
    }
    lcd.setCursor(13, 3);
    lcd.print("Enter->");       
}

void menu_chinh()
{
    if(menu_pos==0)
    {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(">BACK");
        lcd.setCursor(0, 1);
        lcd.print(" Chay lenh");
        lcd.setCursor(0, 2);
        lcd.print(" Them lenh");
        lcd.setCursor(0, 3);
        lcd.print(" Xoa lenh");        
    }
    else if(menu_pos==1)
    {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(" BACK");
        lcd.setCursor(0, 1);
        lcd.print(">Chay lenh");
        lcd.setCursor(0, 2);
        lcd.print(" Them lenh");
        lcd.setCursor(0, 3);
        lcd.print(" Xoa lenh");           
    }
    else if(menu_pos==2)
    {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(" BACK");
        lcd.setCursor(0, 1);
        lcd.print(" Chay lenh");
        lcd.setCursor(0, 2);
        lcd.print(">Them lenh");
        lcd.setCursor(0, 3);
        lcd.print(" Xoa lenh");              
    }
    else if(menu_pos==3)
    {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(" BACK");
        lcd.setCursor(0, 1);
        lcd.print(" Chay lenh");
        lcd.setCursor(0, 2);
        lcd.print(" Them lenh");
        lcd.setCursor(0, 3);
        lcd.print(">Xoa lenh");              
    }    
}

void xoa_lenh()
{
    if(KT_lenh() == 1)
    {
        lcd.clear();
        lcd.setCursor(2, 1);
        lcd.print("DANG XOA LENH...");
        for(int i=0; i<= EEPROM.length(); i++)
        {
            EEPROM.write(i, 0);
        }
        screen_page=0; menu_pos=0;
        manhinh_chinh();
    }
    else 
    {
        lcd.clear();
        lcd.setCursor(2, 1);
        lcd.print("KHONG CO LENH...");    
        delay(2000); 
        screen_page=0; menu_pos=0;
        manhinh_chinh();           
    }
}

void them_lenh()
{
    if(KT_lenh() == 0) //neu CHUA luu lenh -> cho LUU
    {
        lcd.clear();
        toa_do();
        lcd.setCursor(3, 0);
        lcd.print("THEM LENH MOI");    
        lcd.setCursor(0, 3);
        lcd.print("VT: ");
        lcd.setCursor(10, 3);
        lcd.print("Enter->Luu");    
        lcd.setCursor(4, 3);
        lcd.print(vitri);
    }   
    else
    {
        lcd.clear();
        lcd.setCursor(3, 1);
        lcd.print("DA CO LENH...");  
        delay(2000);   
        screen_page=0; menu_pos=0;
        manhinh_chinh();          
    }
}

void luu_lenh() //luu toa do vao EEPROM
{
    EEPROM.put(pos_save, robot);
    pos_save += sizeof(robot);
    vitri++;
        // for(int i=0; i<=20; i++)
        // {
        //   Serial.print(i); Serial.print("= ");
        //   Serial.print(EEPROM.read(i)); Serial.print(", ");
        // }          
}

void show_pos(int x, int y, int gt)
{
    if(gt < 10)
    {
        lcd.setCursor(y, x);
        lcd.print(gt);
        lcd.setCursor(y, x);
        lcd.print(" ");                       
    }
    else if(gt < 100)
    {
        lcd.setCursor(y, x);
        lcd.print(gt);  
        lcd.setCursor(y, x);
        lcd.print(" ");                   
    }
    else 
    {
        lcd.setCursor(y, x);
        lcd.print(gt);         
    }
}

void screen_run()
{
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("X: ");
    show_pos(1, 3, read_robot.X);   

    lcd.setCursor(7, 1);
    lcd.print("Y: ");
    show_pos(1, 10, read_robot.Y);

    lcd.setCursor(14, 1);
    lcd.print("Z: ");
    show_pos(1, 17, read_robot.Z);

    lcd.setCursor(0, 2);
    lcd.print("Ser: ");
    show_pos(2, 5, read_robot.kep);

    lcd.setCursor(6, 0);
    lcd.print("RUN AUTO");  
    lcd.setCursor(9, 3);
    lcd.print("Enter->STOP");            
}

void run_auto()
{
    Step_X.setMaxSpeed(500);
    Step_X.setAcceleration(500); 
    Step_Y.setMaxSpeed(100);
    Step_Y.setAcceleration(100); 
    Step_Z.setMaxSpeed(100);
    Step_Z.setAcceleration(100);  

    Step_X.enableOutputs();
    Step_Y.enableOutputs();
    Step_Z.enableOutputs();

    EEPROM.get(0, end_add);

    if(KT_lenh()==true)
    {
        while(check_run == true)
        {
            gt_enter = digitalRead(enter);
            if(gt_enter == 1)
            {
                while ( pos_save <= (end_add-sizeof(robot)) ) 
                {
                    gt_enter = digitalRead(enter);
                    if(gt_enter == 1)
                    {
                        EEPROM.get(pos_save, read_robot);

                        screen_run();

                        buoc_X = read_robot.X*(-1);
                        buoc_Y = read_robot.Y*(-1);
                        buoc_Z = read_robot.Z;

                        Step_X.moveTo(buoc_X);
                        Step_Y.moveTo(buoc_Y);
                        Step_Z.moveTo(buoc_Z);  
                        servo_kep.write(read_robot.kep);  
                        delay(200);
                        while(Step_X.distanceToGo() != 0 or Step_Y.distanceToGo() != 0 or Step_Z.distanceToGo() != 0)
                        {
                            gt_enter = digitalRead(enter);
                            if(gt_enter == 1)
                            {
                                // Serial.print("X: "); Serial.print(buoc_X);
                                // Serial.print(" - Y: "); Serial.print(buoc_Y);
                                // Serial.print(" - Z: "); Serial.print(read_robot.Z);
                                // Serial.print(" - Kep: "); Serial.println(read_robot.kep);
                                Step_X.run();
                                Step_Y.run();
                                Step_Z.run();
                            } 
                            else 
                            {
                                pos_save = 2;
                                check_run = false;
                                break;
                            }
                        }    
                        pos_save += sizeof(robot);
                    }
                    else 
                    {
                        pos_save = 2;
                        check_run = false;
                        break;
                    }
                }
                pos_save = 2;
            }
            else 
            {
                pos_save = 2;
                check_run = false;
                break;
            }
        }
    }
    else //neu chua co lenh 
    {
        lcd.clear();
        lcd.setCursor(3, 1);
        lcd.print("KHONG CO LENH...");  
        delay(2000);   
        screen_page=0; menu_pos=0; vitri=1; pos_save = 2; check_run = false;
        manhinh_chinh(); 
    }
}

void home_YZ()
{
  int homeY = 0; int homeZ = 0;

  Step_Y.setMaxSpeed(100);
  Step_Y.setAcceleration(100);
  Step_Z.setMaxSpeed(100);
  Step_Z.setAcceleration(100);  
  Step_Y.enableOutputs();
  Step_Z.enableOutputs();

  while(digitalRead(end_Y)==1 and digitalRead(end_Z)==1)
  {
    Step_Y.moveTo(homeY);
    Step_Z.moveTo(homeZ);
    homeY ++;
    homeZ --;
    Step_Y.run();
    Step_Z.run();
  }

  while(digitalRead(end_Y)==1)
  {
    Step_Y.moveTo(homeY);
    homeY ++;
    Step_Y.run();
  }
  Step_Y.setCurrentPosition(0);

  while(digitalRead(end_Z)==1)
  {
    Step_Z.moveTo(homeZ);
    homeZ --;
    Step_Z.run();        
  }
  Step_Z.setCurrentPosition(0);
    
  homeY = 0; homeZ = 0;  
}

void home_X()
{
  int homeX = 0;
  Step_X.setMaxSpeed(100);
  Step_X.setAcceleration(100);  
  Step_X.enableOutputs();

  while(digitalRead(end_X)==1)
  {
    Step_X.moveTo(homeX);
    homeX ++;
    Step_X.run();
  }
  Step_X.setCurrentPosition(0);

  homeX = 0;
}

void home()
{
  home_YZ();
  home_X();
}

void control()
{
    gt_bt_X = analogRead(bt_X);
    gt_bt_Y = analogRead(bt_Y); 
    gt_bt_Z = analogRead(bt_Z);    
    gt_nutnhan = digitalRead(nutnhan);

    if(gt_nutnhan != macdinh_ser) //KEP
    {
        if(gt_nutnhan == 0) dem++;
        if(screen_page == 0) //man hinh chinh
            manhinh_chinh();
        else them_lenh();
        macdinh_ser = gt_nutnhan;
    }
    if(dem%2 == 0)  servo_kep.write(180); //MO
    else servo_kep.write(0); //KEP

    if(((gt_bt_Y > bandau_Y+2) or (gt_bt_Y < bandau_Y-2)) or 
        ((gt_bt_Z > bandau_Z+2) or (gt_bt_Z < bandau_Z-2)) or 
        ((gt_bt_X > bandau_X+2) or (gt_bt_X < bandau_X-2)))
    {
        Step_X.enableOutputs();
        Step_Y.enableOutputs();
        Step_Z.enableOutputs();

        buoc_X = map(gt_bt_X, 0, 1023, 0, -500);
        buoc_Y = map(gt_bt_Y, 0, 1023, 0, -300);
        buoc_Z = map(gt_bt_Z, 0, 1023, 0, 250);

        Step_X.moveTo(buoc_X);
        Step_Y.moveTo(buoc_Y);
        Step_Z.moveTo(buoc_Z);    
        while(Step_X.distanceToGo() != 0 or Step_Y.distanceToGo() != 0 or Step_Z.distanceToGo() != 0)
        {
            Step_X.run();
            Step_Y.run();
            Step_Z.run();
        }

        if(screen_page == 0) //man hinh chinh
            manhinh_chinh();
        else them_lenh();

        bandau_X = gt_bt_X;
        bandau_Y = gt_bt_Y;
        bandau_Z = gt_bt_Z;
    }

    robot.X = buoc_X*(-1); robot.Y = buoc_Y*(-1); robot.Z = buoc_Z; robot.kep = servo_kep.read();

    // Serial.print("BT X: "); Serial.print(gt_bt_X);
    // Serial.print(" - BT Y: "); Serial.print(gt_bt_Y);
    // Serial.print(" - BT Z: "); Serial.print(gt_bt_Z);
    // Serial.print(" - Button: "); Serial.print(gt_nutnhan);
    // Serial.print(" | DEM: "); Serial.print(dem);
    // Serial.print(" - BUOC X: "); Serial.print(robot.X);
    // Serial.print(" - BUOC Y: "); Serial.print(robot.Y);
    // Serial.print(" - BUOC Z: "); Serial.print(robot.Z);
    // Serial.print(" - KEP: "); Serial.print(robot.kep);
    // Serial.println();    
}