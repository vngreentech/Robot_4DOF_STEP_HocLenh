#include "lib.h"

void setup() 
{
  Serial.begin(115200);

  lcd.init();  
  lcd.backlight();

  servo_kep.attach(4); //servo ket noi chan 4

  pinMode(enter, INPUT_PULLUP);
  pinMode(len, INPUT_PULLUP);
  pinMode(xuong, INPUT_PULLUP);

  pinMode(bt_X, INPUT);
  pinMode(bt_Y, INPUT); 
  pinMode(bt_Z, INPUT);
  pinMode(nutnhan, INPUT_PULLUP);

  pinMode(end_X, INPUT_PULLUP);
  pinMode(end_Y, INPUT_PULLUP);
  pinMode(end_Z, INPUT_PULLUP);

  Step_X.setEnablePin(38);
  Step_X.setPinsInverted(false, false, true);
  Step_Y.setEnablePin(56);
  Step_Y.setPinsInverted(false, false, true);
  Step_Z.setEnablePin(62);
  Step_Z.setPinsInverted(false, false, true);    

  home();  

  gt_bt_X = analogRead(bt_X);
  gt_bt_Y = analogRead(bt_Y); 
  gt_bt_Z = analogRead(bt_Z);    
  buoc_X = map(gt_bt_X, 0, 1023, 0, -500);
  buoc_Y = map(gt_bt_Y, 0, 1023, 0, -300);
  buoc_Z = map(gt_bt_Z, 0, 1023, 0, 250);

  Step_X.moveTo(buoc_X);
  Step_Y.moveTo(buoc_Y);
  Step_Z.moveTo(buoc_Z);   

  if (analogRead(bt_X)!=0 or analogRead(bt_Y)!=0 or analogRead(bt_Z)!=0)
  {
    while(Step_X.distanceToGo()!=0 or Step_Y.distanceToGo() != 0 or Step_Z.distanceToGo() != 0)
    {
      Step_X.run();
      Step_Y.run();
      Step_Z.run();      
    }         
  }
  robot.X = buoc_X*(-1); robot.Y = buoc_Y*(-1); robot.Z = buoc_Z; robot.kep = 180;

  Step_X.setMaxSpeed(1000);
  Step_X.setAcceleration(1000); 
  Step_Y.setMaxSpeed(1000);
  Step_Y.setAcceleration(1000); 
  Step_Z.setMaxSpeed(1000);
  Step_Z.setAcceleration(1000);  

  manhinh_chinh();
}

void loop() 
{
  gt_enter = digitalRead(enter);
  gt_len = digitalRead(len);
  gt_xuong = digitalRead(xuong);
  
  if(gt_enter != last_enter) //NUT ENTER
  {
    if(gt_enter == 0)
    {
      screen_page++;

      if(screen_page == 1) menu_chinh();    
      else if(screen_page == 2 && menu_pos == 0) //Nhan Back ve man hinh
      {
        manhinh_chinh();
        screen_page = 0; menu_pos = 0;
      }  
      else if(screen_page == 2 && menu_pos == 3)   //Menu xoa lenh   
      {
        xoa_lenh();
      }
      else if(screen_page == 2 && menu_pos == 2) //Them lenh
      {
        them_lenh();
      }
      else if(screen_page == 3 && menu_pos == 2) //tu menu THEM LENH ve man hinh
      {
        EEPROM.put(0, pos_save);
        screen_page = 0; menu_pos = 0; vitri=1; pos_save = 2;
        manhinh_chinh();
      }  
      else if(screen_page == 2 && menu_pos == 1) //Chay lenh
      {
        check_run = true;
      }  
      else if(screen_page == 3 && menu_pos == 1) // Chay ve vi tri bien tro va ve man hinh chinh
      {
        check_run = false;
        manhinh_chinh();
      }         
    } 
    last_enter = gt_enter;
  }

  if(gt_len != last_len) //Nut LEN
  {
    if(gt_len == 0)
    {
      if(screen_page == 1) //Menu chinh
      {
        menu_pos--;
        if(menu_pos < 0) menu_pos = 3;
        menu_chinh();
      } 
    }
    last_len = gt_len;
  }

  if(gt_xuong != last_xuong) //Nut XUONG
  {
    if(gt_xuong == 0)
    {
      if(screen_page == 1)  //Menu chinh
      {
        menu_pos++;
        if(menu_pos > 3) menu_pos = 0;
        menu_chinh();
      }
      else if(screen_page == 2 && menu_pos == 2) //Them lenh
      {
        luu_lenh();
        them_lenh();  
      }
    }
    last_xuong = gt_xuong;
  }

  if(screen_page == 0 or (screen_page == 2 && menu_pos == 2))
  {
    control();
  }
  else if(screen_page == 2 && menu_pos == 1) //Chay lenh
  {
    check_run = true;
    run_auto();
  }
  else if(screen_page == 3 && menu_pos == 1) // Chay ve vi tri bien tro
  {
    Step_X.setMaxSpeed(100);
    Step_X.setAcceleration(100); 
    Step_Y.setMaxSpeed(50);
    Step_Y.setAcceleration(50); 
    Step_Z.setMaxSpeed(50);
    Step_Z.setAcceleration(50);  

    gt_bt_X = analogRead(bt_X);
    gt_bt_Y = analogRead(bt_Y); 
    gt_bt_Z = analogRead(bt_Z);    
    buoc_X = map(gt_bt_X, 0, 1023, 0, -500);
    buoc_Y = map(gt_bt_Y, 0, 1023, 0, -300);
    buoc_Z = map(gt_bt_Z, 0, 1023, 0, 250);

    Step_X.moveTo(buoc_X);
    Step_Y.moveTo(buoc_Y);
    Step_Z.moveTo(buoc_Z);   

    if (analogRead(bt_X)!=0 or analogRead(bt_Y)!=0 or analogRead(bt_Z)!=0)
    {
      while(Step_X.distanceToGo()!=0 or Step_Y.distanceToGo() != 0 or Step_Z.distanceToGo() != 0)
      {
        Step_X.run();
        Step_Y.run();
        Step_Z.run();   
        screen_page = 0; menu_pos = 0; vitri=1; pos_save = 2;   
      }         
    }
    
    Step_X.setMaxSpeed(1000);
    Step_X.setAcceleration(1000); 
    Step_Y.setMaxSpeed(1000);
    Step_Y.setAcceleration(1000); 
    Step_Z.setMaxSpeed(1000);
    Step_Z.setAcceleration(1000);      
  }

  delay(10);
}