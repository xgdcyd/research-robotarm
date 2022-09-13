#include "main.h"
extern int inByte, num;
extern int x_openmv, y_openmv;
extern int x_uarm, y_uarm;
extern unsigned long times;
unsigned long time2;
extern char buf[1024], flag;
extern char color_sel, AS;
extern int x_offest, y_offest;
int counters;
enum commuincation openmv_state;
int bbc;

void openmv_connect()
{
  switch (color_sel) {
    case 0: Serial3.write('y'); break;
    case 1: Serial3.write('r'); break;
    case 2: Serial3.write('g'); break;
    default: break;
  }
  Serial3.write('S');//send vision start command
}
void error_page()
{
  switch (openmv_state)
  {
    case beready: second_page();
      break;
    case bedoing: fifth_page(); break;
    case befailed: fourth_page(); break;
  }
}
void wait_for_finish_moving()
{
  AS = ""; //clear the buffer
  while (AS != 'k') {
    if (Serial2.available() > 0) {
      AS = Serial2.read();
      Serial.println(AS);
    }
  }
  Serial.println("OUT");
}

//get object coordinates from openm
unsigned char get_openmv_data()
{
  if (Serial3.available() > 0)
  {
    time2 = millis();
    inByte = Serial3.read();
    buf[num++] = inByte;
    //    Serial.print(num);
    //    Serial.write(":");
    //    Serial.write(inByte);
    //    Serial.write("\n");
    if ((buf[0] == 's') && (num == 1) | (buf[1] == 's'))
    {
      openmv_state = beready;
      times = millis();
      return 0;
    }
    if (inByte == 'x')
    {
      times = millis();
      openmv_state = bedata;
      counters = num;
    }
    if (inByte == '\n' )
    {
      times = millis();
      openmv_state = bedoing;
      Serial.write("get openmv data\n");
      //jump the letter x
      x_openmv = 0;
      do {
        x_openmv = x_openmv * 10;
        x_openmv += buf[counters++] - 48;
      } while ((buf[counters] >= 0x30) && (buf[counters] <= 0x39));
      y_openmv = 0;
      counters++;//jump the letter y
      do {
        y_openmv = y_openmv * 10;
        y_openmv += buf[counters++] - 48;
      } while (counters + 1 < num);
      Serial.write("move\n");//confirm the openmv data
      Serial3.end();
      //new algorithm
      x_uarm = (y_openmv * (-0.7035) - 3.635 + 88 + 70 + 200) + x_offest -65;
      y_uarm = (x_openmv * (-0.7488) + 12.391 + 107.5 + 5) + y_offest ;
      Serial.println(y_openmv);
      Serial.println(x_openmv);
      fifth_page();
      String commands = "G0 X";
      commands.concat(x_uarm);
      commands += " Y";
      commands.concat(y_uarm);
      commands += " Z100 F10000\n";
      Serial2.print(commands);
      Serial.print(commands);
      pick_and_palce();
      num = 0;
      flag = 0;//vision end
      num = 0;
      return 1;
    }
  }
  else
  {
    //openmv_state=befailed;
  }
  if ((millis() - times > 3000) && ( openmv_state == befailed)) //if no object detected, reset the flag every 10s
  {
    num = 0;
    times = millis();
    Serial.write("status 1\n");//NO OBJECT IN CAMERA
  }
  return 0;
}
//move the detected object to the fixed position
void pick_and_palce()
{
  
  if  (color_sel == 1)
  {
    Serial.write('1');
  Serial2.write("G0 Z23 F10000\n");
  Serial2.write("M2232 V1\n");
   delay(8000);
  Serial2.write("G0 Z120 F10000\n");
  
  Serial2.write("G2202 N0 V45\n");
  Serial2.write("G0 Z50 F10000\n");
  //      wait_for_finish_moving();
delay(5000);
  Serial2.write("M2232 V0\n");
   delay(5000);
  Serial2.write("G0 Z80 F10000\n");
  //      wait_for_finish_moving();
 delay(5000);
  Serial2.write("G2202 N0 V90\n");

  delay(5000);
  }
  else if  (color_sel == 0)
  {
      Serial.write('0');
    Serial2.write("G0 Z23 F10000\n");
  Serial2.write("M2232 V1\n");
   delay(8000);
  Serial2.write("G0 Z120 F10000\n");
  
  Serial2.write("G2202 N0 V5\n");
  Serial2.write("G0 Z50 F10000\n");
  //      wait_for_finish_moving();
delay(5000);
  Serial2.write("M2232 V0\n");
   delay(5000);
  Serial2.write("G0 Z80 F10000\n");
  //      wait_for_finish_moving();
 delay(5000);
  Serial2.write("G2202 N0 V90\n");

  delay(5000);
  
  }
    else if  (color_sel == 2)
  {
      Serial.write('2');
    Serial2.write("G0 Z23 F10000\n");
  Serial2.write("M2232 V1\n");
   delay(8000);
  Serial2.write("G0 Z120 F10000\n");
  
  Serial2.write("G2202 N0 V170\n");
  Serial2.write("G0 Z50 F10000\n");
  //      wait_for_finish_moving();
delay(5000);
  Serial2.write("M2232 V0\n");
   delay(5000);
  Serial2.write("G0 Z80 F10000\n");
  //      wait_for_finish_moving();
 delay(5000);
  Serial2.write("G2202 N0 V90\n");

  delay(5000);
  
  }
  //    wait_for_finish_moving();
  //change the color of tracking
  //color_sel++;
  //color_sel = color_sel%3;
}
