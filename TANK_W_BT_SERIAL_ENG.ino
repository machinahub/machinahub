#include <AltSoftSerial.h>        //  SoftwareSerial header declaration
AltSoftSerial BLESerial;          //  SoftwareSerial Class inheritance


//----Motor control constant and pin number set-up-----------
const int LEFT_F_CONTROL=2; // LEFT motor forward movement pin set-up
const int LEFT_R_CONTROL=3; // LEFT motor backward pin set-up
const int LEFT_EN=6;        // LEFT motor speed control PWM pin set-up

const int RIGHT_F_CONTROL=4;// RIGHT motor forward movement pin set-up
const int RIGHT_R_CONTROL=5;// RIGHT motor backward pin set-up
const int RIGHT_EN=11;      // RIGHT motor speed control PWM pin set-up

const int RUN_STOP=0;       // forward/backward and stop value of each motors 
const int RUN_FORW=1;
const int RUN_BACK=2;

const int FULL_RUN = 255;   // PWM value set-up, when the value goes 255, Duty rate is 1 --> full speed
const int MIDD_RUN = 150;
const int STOP_RUN = 0;     // PWM value set-up, when the value goes 0, Duty rate is 0  --> motor stops
//---------------------------------


//-------BLE module communication string constant declaration--------
const int CON_STATUS=7;                   //Arduino pin of BoT-CLE110 module which connect pin inputs;
char BTINS_BRATE9600[] = "AT+UART=9600";  //String that changes baud rate value of BoT-CLE110 module
char BTREQ_AT[] = "AT";                   //String calls "+OK" ACK to BoT-CLE110 module
char BTREQ_VER[] = "AT+VER?";             //String calls current firmware version to BoT-CLE110 module
char BTREQ_INFO[] = "AT+INFO?";           //String calls MAC adress and Role info to BoT-CLE110 module
char BTREQ_ROLE[] = "AT+ROLE?";           //String calls Role info to BoT-CLE110 module
char BTREQ_TXPWR[] = "AT+TXPWR?";         //String calls current Tx Power info to Bot-CLE110 module
char BTREQ_MANUF[] = "AT+MANUF?";         //String calls maker's name to Bot-CLE110 module
char BTNOT_CONNECTED[]="+CONNECTED";      //Printed out String from when connected
//---------------------------------


//---------initial routine--------------
void setup() {
  //----Set-up each pin numbers and I/O----
  pinMode(CON_STATUS, INPUT);       
  pinMode(LEFT_F_CONTROL, OUTPUT);
  pinMode(LEFT_R_CONTROL, OUTPUT);
  pinMode(LEFT_EN, OUTPUT); 
  pinMode(RIGHT_F_CONTROL, OUTPUT);
  pinMode(RIGHT_R_CONTROL, OUTPUT);
  pinMode(RIGHT_EN, OUTPUT);
  //------------------------------------

  //--Print out initial UART content and set-up for PC communication---
  Serial.begin(9600);
  while (!Serial);
  Serial.println("Hello");
  Serial.println("TANK CONTROL SYSTEM");
  //------------------------------------

  BLESerial.begin(9600); //---UART set-up for communcation with BLE module
  BLE_SERIAL_WRITE_ECHO(BTINS_BRATE9600); // Baud rate set-up of BoT-CLE110 module
  BLE_SERIAL_WRITE_ECHO(BTREQ_VER);       //  request frimware version
  BLE_SERIAL_WRITE_ECHO(BTREQ_INFO);      //  request MAC adress and Role info
  BLE_SERIAL_WRITE_ECHO(BTREQ_ROLE);      //  request Role info
  BLE_SERIAL_WRITE_ECHO(BTREQ_TXPWR);     //  request Tx Power value
  BLE_SERIAL_WRITE_ECHO(BTREQ_MANUF);     //  request the name of maker
  Serial.println("BLE Module Initiantion Completed");
  Serial.println("WAITING Connection");
  while(!(digitalRead(CON_STATUS)));// Awaiting Host connection to Bot-CLE110
  Serial.println("CONNECTED");  //  shows connection when it gets connected
  Serial.write("\n"); 
}
//---------------------------------


//---------repeat routine-----------
void loop() {
  // put your main code here, to run repeatedly:
  
  
  int input_char; // variable declaration to save letters that received from BLE module

  //-- Menu print out--
  Serial.write("-----TANK Test Program-----\n");
  Serial.write("- 1. NOP\n");
  Serial.write("- 2. RUN FORWARD\n");
  Serial.write("- 3. RUN BACKWARD\n");
  Serial.write("- 4. LEFT FORWARD TURN \n");
  Serial.write("- 5. RIGHT FORWARD TURN \n");
  Serial.write("- 6. LEFT BACKWARD TURN \n");
  Serial.write("- 7. RIGHT BACKWARD TURN \n");
  Serial.write("- 8. LEFT SPIN \n");
  Serial.write("- 9. RIGHT SPIN \n");
  Serial.write("---------------------------\n");
  //---------------
  
  input_char = BT_SERIAL_INPUT(); // stand by until BLE module gives letters, save the letters from the BLE module
  TANK_MOVE_DECODE(input_char); // deliver the receive letters to TANK function for apply it to tank's movement
}
//--------------------------------



//---------BLE module command transfer function-------------
//  - command transfer includes REQ and ACK both, so it contains serial notification that transfers command and respond
void BLE_SERIAL_WRITE_ECHO(char tx_msg[])
{
  Serial.write("\t[REQ TO BLE] : ");
  Serial.println(tx_msg); // distribut messages for BLE to PC
  BLESerial.println(tx_msg);  // send message to BLE module
  if (tx_msg==BTINS_BRATE9600) // Baud rate alteration message it takes 3~5 sec(max) til final state
  {
    delay(5000);  //5ec delay
  }
  else
  {
    delay(500); // 0.5sec delay
  }
  Serial.write("\t[ACK FROM BLE] : ");
  BT_SERIAL_READ();//print out received letters to PC
}
//---------------------------------

//-------Reading received letters from BLE----------------
void BT_SERIAL_READ(void)
{ 
  char rx_char; // variable to save received letters 
  while(!(BLESerial.available()==NULL)) // distribute repeatedly until no letters left in receive buffur
  {
    rx_char = BLESerial.read();
    Serial.write(rx_char);
  }
  Serial.write("\n");
}
//--------------------------------- 

//----------Reading received 1 byte letter to BLE-------------- 
char BT_SERIAL_INPUT(void)
{
  char input_char; // variable to save received letters
  while(BLESerial.available()<=0);// wait until 1 letter comes to receive buffur
  input_char = BLESerial.read();// save the letter of receive buffur and return variable
  return input_char;
}
//--------------------------------- 


//-----command interpretation function by inputted parameter---------------
void TANK_MOVE_DECODE(int ptank_move_kind)
{
  // modify parameter of TANK_MOVE function and distribute correspond content to serial by inputted parameter
  if (ptank_move_kind=='1')
  {
    TANK_MOVE(RUN_STOP, RUN_STOP, STOP_RUN, STOP_RUN);
    Serial.write("!! TANK STOPPED !!!\n");    
  }
  else if (ptank_move_kind=='2')
  {
    TANK_MOVE(RUN_FORW, RUN_FORW, FULL_RUN, FULL_RUN);
    Serial.write("!! TANK RUN FORWARD !!!\n");
  }
  else if (ptank_move_kind=='3')
  {
    TANK_MOVE(RUN_BACK, RUN_BACK, FULL_RUN, FULL_RUN);
    Serial.write("!! TANK RUN BACKWARD !!!\n");
  }
  else if (ptank_move_kind=='4')
  {
    TANK_MOVE(RUN_FORW, RUN_FORW, MIDD_RUN, FULL_RUN);
    Serial.write("!! TANK LEFT FORWARD TURN !!!\n");
  }
  else if (ptank_move_kind=='5')
  {
    TANK_MOVE(RUN_FORW, RUN_FORW, FULL_RUN, MIDD_RUN);
    Serial.write("!! TANK RIGHT FORWARD TURN !!!\n");
  }
  else if (ptank_move_kind=='6')
  {
    TANK_MOVE(RUN_BACK, RUN_BACK, MIDD_RUN, FULL_RUN);
    Serial.write("!! LEFT BACKWARD TURN !!!\n");
  }
  else if (ptank_move_kind=='7')
  {
    TANK_MOVE(RUN_BACK, RUN_BACK, FULL_RUN, MIDD_RUN);
    Serial.write("!! RIGHT BACKWARD TURN !!!\n");
  }
  else if (ptank_move_kind=='8')
  {
    TANK_MOVE(RUN_BACK, RUN_FORW, FULL_RUN, FULL_RUN);
    Serial.write("!! LEFT SPIN !!!\n");
  }
  else if (ptank_move_kind=='9')
  {
    TANK_MOVE(RUN_FORW, RUN_BACK, FULL_RUN, FULL_RUN);
    Serial.write("!! RIGHT SPIN !!!\n");
  }
  else
  {
    Serial.write("!? WHAT YOU WANT TO DO !?!\n");
  }
}
//---------------------------------


//-------control function of actual movement of tank----------------

void TANK_MOVE(int LEFT, int RIGHT, int L_VELO, int R_VELO)
{
  if (LEFT==0 && RIGHT==0)//NOP left/right wheel both stop
  {
    digitalWrite(LEFT_F_CONTROL, HIGH);//send forward signal to LEFT moter driver
    digitalWrite(LEFT_R_CONTROL, LOW);
    digitalWrite(RIGHT_F_CONTROL, HIGH);//send forward signal to RIGHT moter driver
    digitalWrite(RIGHT_R_CONTROL, LOW);
    digitalWrite(LEFT_EN, LOW); // down LEFT moter's EN signal to '0' -> Stop
    digitalWrite(RIGHT_EN, LOW);// down RIGHT moter's EN signal to '0'
  }
  else if (LEFT==1 && RIGHT==1) // LEFT/RIGHT move forward both
  {
    digitalWrite(LEFT_F_CONTROL, HIGH);
    digitalWrite(LEFT_R_CONTROL, LOW);
    digitalWrite(RIGHT_F_CONTROL, HIGH);
    digitalWrite(RIGHT_R_CONTROL, LOW);
    analogWrite(LEFT_EN, L_VELO); // send PWM signal of L_VELO to LEFT motor's EN signal
    analogWrite(RIGHT_EN, R_VELO);// send PWM signal of L_VELO to RIGHT motor's EN signal
  }
  else if (LEFT==2 && RIGHT==2) // LEFT/RIGHT move backward both
  {
    digitalWrite(LEFT_F_CONTROL, LOW);// send backward signal to LEFT motor driver
    digitalWrite(LEFT_R_CONTROL, HIGH);// send backward signal to LEFT motor driver
    digitalWrite(RIGHT_F_CONTROL, LOW);// send backward signal to RIGHT motor driver
    digitalWrite(RIGHT_R_CONTROL, HIGH);// send backward signal to RIGHT motor driver
    analogWrite(LEFT_EN, L_VELO);
    analogWrite(RIGHT_EN, R_VELO);
  }
  else if (LEFT==1 && RIGHT==2) // LEFT move forward, RIGHT move backward
  {
    digitalWrite(LEFT_F_CONTROL, HIGH);
    digitalWrite(LEFT_R_CONTROL, LOW);
    digitalWrite(RIGHT_F_CONTROL, LOW);
    digitalWrite(RIGHT_R_CONTROL, HIGH);
    analogWrite(LEFT_EN, L_VELO);
    analogWrite(RIGHT_EN, R_VELO);
  }
  else if (LEFT==2 && RIGHT==1)// LEFT move backward RIGHT move forward
  {
    digitalWrite(LEFT_F_CONTROL, LOW);
    digitalWrite(LEFT_R_CONTROL, HIGH);
    digitalWrite(RIGHT_F_CONTROL, HIGH);
    digitalWrite(RIGHT_R_CONTROL, LOW);
    analogWrite(LEFT_EN, L_VELO);
    analogWrite(RIGHT_EN, R_VELO);
  }
}
  
      
