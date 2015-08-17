#include <AltSoftSerial.h>        //  SoftwareSerial 헤더 선언
AltSoftSerial BLESerial;          //  SoftwareSerial Class 상속


//----모터 제어용 상수 및 핀 번호 설정------------
const int LEFT_F_CONTROL=2; // LEFT 모터용 전진용 핀설정
const int LEFT_R_CONTROL=3; // LEFT 모터용 후진용 핀설정
const int LEFT_EN=6;        // LEFT 모터 속도제어용 PWM핀 설정

const int RIGHT_F_CONTROL=4;// RIGHT 모터용 전진용 핀설정
const int RIGHT_R_CONTROL=5;// RIGHT 모터용 후진용 핀설정
const int RIGHT_EN=11;      // RIGHT 모터 속도제어용 PWM핀 설정

const int RUN_STOP=0;       // 각 회전 모터의 전/후진 및 정지 설정값
const int RUN_FORW=1;
const int RUN_BACK=2;

const int FULL_RUN = 255;   // PWM값 설정, 255의 경우 Duty rate가 1 --> 풀가속
const int MIDD_RUN = 150;
const int STOP_RUN = 0;     // PWM값 설정, 0의 경우 Duty rate가 0  --> 모터 멈춤
//---------------------------------


//-------BLE모듈 통신용 문자열 상수 선언--------
const int CON_STATUS=7;                   //BoT-CLE110 모듈의 Connect 핀이 입력되는 아두이노 핀;
char BTINS_BRATE9600[] = "AT+UART=9600";  //BoT-CLE110 모듈에 보레이트 설정값을 변경하는 문자열
char BTREQ_AT[] = "AT";                   //BoT-CLE110 모듈에 "+OK" ACK를 요청하는 문자열
char BTREQ_VER[] = "AT+VER?";             //BoT-CLE110 모듈에 현재 펌웨어 버전을 요청하는 문자열
char BTREQ_INFO[] = "AT+INFO?";           //BoT-CLE110 모듈에 맥어드레스와 Role정보를 요청하는 문자열
char BTREQ_ROLE[] = "AT+ROLE?";           //BoT-CLE110 모듈에 Role 정보를 요청하는 문자열
char BTREQ_TXPWR[] = "AT+TXPWR?";         //Bot-CLE110 모듈에 현재 Tx Power 정보를 요청하는 문자열
char BTREQ_MANUF[] = "AT+MANUF?";         //Bot-CLE110 모듈에 저장된 제조사명을 요청하는 문자열
char BTNOT_CONNECTED[]="+CONNECTED";      //접속되었을시 BoT-CLE110 모듈에서 출력되는 문자열
//---------------------------------


//---------초기 루틴---------------
void setup() {
  //----각 핀의 번호와 입출력을 설정----
  pinMode(CON_STATUS, INPUT);       
  pinMode(LEFT_F_CONTROL, OUTPUT);
  pinMode(LEFT_R_CONTROL, OUTPUT);
  pinMode(LEFT_EN, OUTPUT); 
  pinMode(RIGHT_F_CONTROL, OUTPUT);
  pinMode(RIGHT_R_CONTROL, OUTPUT);
  pinMode(RIGHT_EN, OUTPUT);
  //------------------------------------

  //--PC와의 통신용 UART 설정 및 초기내용 출력---
  Serial.begin(9600);
  while (!Serial);
  Serial.println("Hello");
  Serial.println("TANK CONTROL SYSTEM");
  //------------------------------------

  BLESerial.begin(9600); //---BLE 모듈과의 통신용 UART 설정
  BLE_SERIAL_WRITE_ECHO(BTINS_BRATE9600); // BoT-CLE110모듈의 보레이트 설정
  BLE_SERIAL_WRITE_ECHO(BTREQ_VER);       //  펌웨어 버전 요청
  BLE_SERIAL_WRITE_ECHO(BTREQ_INFO);      //  맥어드레스 및 Role 정보 요청
  BLE_SERIAL_WRITE_ECHO(BTREQ_ROLE);      //  Role 정보 요청
  BLE_SERIAL_WRITE_ECHO(BTREQ_TXPWR);     //  Tx Power값 요청
  BLE_SERIAL_WRITE_ECHO(BTREQ_MANUF);     //  제조사 요청
  Serial.println("BLE Module Initiantion Completed");
  Serial.println("WAITING Connection");
  while(!(digitalRead(CON_STATUS)));// Bot-CLE110모듈에 HOST가 접속되길 기다림.
  Serial.println("CONNECTED");  //  접속되었을시 접속되었음을 표시
  Serial.write("\n"); 
}
//---------------------------------


//---------반복 루틴-----------
void loop() {
  // put your main code here, to run repeatedly:
  
  
  int input_char; // BLE모듈로부터 수신된 문자를 저장하기 위한 변수 선언

  //-- 메뉴 출력--
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
  
  input_char = BT_SERIAL_INPUT(); //BLE모듈로부터 수신된 문자가 있을때까지 대기하고, 수신된 문자를 저장
  TANK_MOVE_DECODE(input_char); // 수신된 문자를 탱크움직임에 적용하고자 탱크함수에 전달함.
}
//--------------------------------



//---------BLE 모듈 명령전송용 함수-------------
//  - 명령전송은 REQ외에 ACK 신호도 포함되므로 명령을 전송하고
//  - 이에 대한 응답을 수신하여 시리얼로 알려주는 기능을 포함
void BLE_SERIAL_WRITE_ECHO(char tx_msg[])
{
  Serial.write("\t[REQ TO BLE] : ");
  Serial.println(tx_msg); // BLE로 보낼 메세지를 PC에 뿌림.
  BLESerial.println(tx_msg);  //BLE 모듈로 메세지 보냄
  if (tx_msg==BTINS_BRATE9600) // 보레이트 변경 메세지의 경우 최종상태까지 3~5초 가량 걸림
  {
    delay(5000);  //5초 딜레이
  }
  else
  {
    delay(500); // 0.5초 딜레이
  }
  Serial.write("\t[ACK FROM BLE] : ");
  BT_SERIAL_READ();//수신된 문자열 읽어서 PC로 출력함
}
//---------------------------------

//-------BLE로 부터 수신된 문자열 읽기----------------
void BT_SERIAL_READ(void)
{ 
  char rx_char; // 수신된 문자를 저장할 변수
  while(!(BLESerial.available()==NULL)) // 수신 버퍼에 적재되어있는 문자가 하나도 없을때까지 반복적으로 읽어서 PC에 뿌림
  {
    rx_char = BLESerial.read();
    Serial.write(rx_char);
  }
  Serial.write("\n");
}
//--------------------------------- 

//----------BLE로 수신된 한 바이트의 문자 읽기-------------- 
char BT_SERIAL_INPUT(void)
{
  char input_char; // 수신된 문자를 저장할 변수
  while(BLESerial.available()<=0);// 수신 버퍼에 문자열이 하나 들어올때까지 대기
  input_char = BLESerial.read();// 수신 버퍼의 문자를 변수에 저장하고 변수를 리턴
  return input_char;
}
//--------------------------------- 


//-----입력되는 parameter에 따라 어떠한 명령인지 해석하는 함수---------------
void TANK_MOVE_DECODE(int ptank_move_kind)
{
  // 입력되는 파라메터에 따라 TANK_MOVE함수의 파라메터를 조정하여 전달하고 해당되는 내용을 시리얼로 뿌림
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


//-------실제 탱크의 움직임을 제어하는 함수----------------
// LEFT, RIGHT의 축이 Forward/Backward인지
// LEFT, RIGHT의 회전속도를 나타내는 지표인 PWM값
void TANK_MOVE(int LEFT, int RIGHT, int L_VELO, int R_VELO)
{
  if (LEFT==0 && RIGHT==0)//NOP 좌측 우측 모두 정지
  {
    digitalWrite(LEFT_F_CONTROL, HIGH);//좌측 모터 드라이버에 전진용 신호 전송
    digitalWrite(LEFT_R_CONTROL, LOW);
    digitalWrite(RIGHT_F_CONTROL, HIGH);//우측 모터 드라이버에 전진용 신호 전송
    digitalWrite(RIGHT_R_CONTROL, LOW);
    digitalWrite(LEFT_EN, LOW); // 좌측 모터 EN신호를 0으로 내림 -> 정지
    digitalWrite(RIGHT_EN, LOW);//우측 모터 EN신호를 0으로 내림
  }
  else if (LEFT==1 && RIGHT==1) // 좌측 우측 모두 전진
  {
    digitalWrite(LEFT_F_CONTROL, HIGH);
    digitalWrite(LEFT_R_CONTROL, LOW);
    digitalWrite(RIGHT_F_CONTROL, HIGH);
    digitalWrite(RIGHT_R_CONTROL, LOW);
    analogWrite(LEFT_EN, L_VELO); // 좌측 모터의 EN신호에 L_VELO의 PWM신호 공급
    analogWrite(RIGHT_EN, R_VELO);//우측 좌측 모터의 EN신호에 L_VELO의 PWM신호 공급
  }
  else if (LEFT==2 && RIGHT==2) // 좌측 우측 모두 후진
  {
    digitalWrite(LEFT_F_CONTROL, LOW);//좌측 모터 드라이버에 후진용 신호 전송
    digitalWrite(LEFT_R_CONTROL, HIGH);//좌측 모터 드라이버에 후진용 신호 전송
    digitalWrite(RIGHT_F_CONTROL, LOW);//우측 모터 드라이버에 후진용 신호 전송
    digitalWrite(RIGHT_R_CONTROL, HIGH);//우측 모터 드라이버에 후진용 신호 전송
    analogWrite(LEFT_EN, L_VELO);
    analogWrite(RIGHT_EN, R_VELO);
  }
  else if (LEFT==1 && RIGHT==2) //좌측 전진 우측 후진
  {
    digitalWrite(LEFT_F_CONTROL, HIGH);
    digitalWrite(LEFT_R_CONTROL, LOW);
    digitalWrite(RIGHT_F_CONTROL, LOW);
    digitalWrite(RIGHT_R_CONTROL, HIGH);
    analogWrite(LEFT_EN, L_VELO);
    analogWrite(RIGHT_EN, R_VELO);
  }
  else if (LEFT==2 && RIGHT==1)// 좌측 후진 우측 전진
  {
    digitalWrite(LEFT_F_CONTROL, LOW);
    digitalWrite(LEFT_R_CONTROL, HIGH);
    digitalWrite(RIGHT_F_CONTROL, HIGH);
    digitalWrite(RIGHT_R_CONTROL, LOW);
    analogWrite(LEFT_EN, L_VELO);
    analogWrite(RIGHT_EN, R_VELO);
  }
}
  
      
