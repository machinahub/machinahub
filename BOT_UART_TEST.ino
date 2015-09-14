#include <AltSoftSerial.h>        //  SoftwareSerial 헤더 선언
AltSoftSerial BLESerial;          //  SoftwareSerial Class 상속

//-------BLE모듈 통신용 문자열 상수 선언--------
const char BT_PLUS = 0x2B;
const char BT_CR = 0xD;

const String BT_NOTIFY_OK = "OK";
const String BT_NOTIFY_RDY = "READY";
const String BT_NOTIFY_ADV = "ADVERTISING";
const String BT_NOTIFY_BRD = "BROADCASTING";
const String BT_NOTIFY_SCAN = "SCANNING";
const String BT_NOTIFY_CONN = "CONNECTED";
const String BT_NOTIFY_DCON = "DISCONNECTED";
const String BT_NOTIFY_ERR = "ERROR";

const String MYSTRING_ERR = "++++ERROR++++";

const int CON_STATUS=7;                   //BoT-CLE110 모듈의 Connect 핀이 입력되는 아두이노 핀;


const String BTINS_BRATE9600 = "AT+UART=9600";  //BoT-CLE110 모듈에 보레이트 설정값을 변경하는 문자열
const String BTREQ_REQ_RESET ="ATZ";      //접속되었을시 BoT-CLE110 모듈에서 출력되는 문자열
const String BTREQ_REQ_FACTORY_RESET ="AT&F";      //접속되었을시 BoT-CLE110 모듈에서 출력되는 문자열

//const String BTREQ_REQ_AT = "AT";                   //BoT-CLE110 모듈에 "+OK" ACK를 요청하는 문자열
const String BTREQ_REQ_VER = "AT+VER?";             //BoT-CLE110 모듈에 현재 펌웨어 버전을 요청하는 문자열
const String BTREQ_REQ_INFO = "AT+INFO?";           //BoT-CLE110 모듈에 맥어드레스와 Role정보를 요청하는 문자열
const String BTREQ_REQ_ROLE = "AT+ROLE?";           //BoT-CLE110 모듈에 Role 정보를 요청하는 문자열
const String BTREQ_REQ_TXPWR = "AT+TXPWR?";         //Bot-CLE110 모듈에 현재 Tx Power 정보를 요청하는 문자열
const String BTREQ_REQ_MANUF = "AT+MANUF?";         //Bot-CLE110 모듈에 저장된 제조사명을 요청하는 문자열


//---------------------------------

void setup() {
  // put your setup code here, to run once:
  //--PC와의 통신용 UART 설정 및 초기내용 출력---
  String ret;
  Serial.begin(9600);
  while (!Serial);
  Serial.println("Hello");
  Serial.println("TANK CONTROL SYSTEM");
  Serial.println();
  //------------------------------------

  BLESerial.begin(9600); //---BLE 모듈과의 통신용 UART 설정
  BLE_REQ_RESULT_RESET(BTREQ_REQ_FACTORY_RESET);
//  BLE_REQ_RESULT_RESET(BTREQ_REQ_RESET);
//  BLE_REQ_RESULT_RESET(BTINS_BRATE9600);

  Serial.println();

  ret = BLE_REQ_INFO(BTREQ_REQ_VER);
  Serial.print("\t[RX Info] = ");
  Serial.println(ret);
  ret = BLE_REQ_INFO(BTREQ_REQ_INFO);
  Serial.print("\t[RX Info] = ");
  Serial.println(ret);
  ret = BLE_REQ_INFO(BTREQ_REQ_ROLE);
  Serial.print("\t[RX Info] = ");
  Serial.println(ret);
  ret = BLE_REQ_INFO(BTREQ_REQ_TXPWR);
  Serial.print("\t[RX Info] = ");
  Serial.println(ret);
  ret = BLE_REQ_INFO(BTREQ_REQ_MANUF);
  Serial.print("\t[RX Info] = ");
  Serial.println(ret);  

  
}

void loop() {
  // put your main code here, to run repeatedly:
  int i,j;

  for(j=0;j<=3;j++)
  {
    for(i=0;i<=250;i=i+100)
    {
      while(!(BLE_INSTRCT_PWM(j,i)));
    }  
  }
  
  while(!(BLE_INSTRCT_PWM(0,0)));
  while(!(BLE_INSTRCT_PWM(1,0)));
  while(!(BLE_INSTRCT_PWM(2,0)));
  while(!(BLE_INSTRCT_PWM(3,0)));
  BLE_INSTRCT_PWM(4,256);

  while(!(BLE_INSTRCT_PWM(0,50)));
  while(!(BLE_INSTRCT_PWM(1,100)));
  while(!(BLE_INSTRCT_PWM(2,150)));
  while(!(BLE_INSTRCT_PWM(3,200)));

  Serial.println(BLE_GET_PWM(0));
  Serial.println(BLE_GET_PWM(1));
  Serial.println(BLE_GET_PWM(2));
  Serial.println(BLE_GET_PWM(3));


  Serial.println(BLE_GET_AIO(0));
  Serial.println(BLE_GET_AIO(1));
  Serial.println(BLE_GET_AIO(2)); 


  
  
}

void BLE_REQ_RESULT_RESET(String req_msg)
{
  Serial.print("\t[Req MSG] = ");
  Serial.println(req_msg);
  BLESerial.println(req_msg);
  Serial.println("BLE Module Reset...");
  delay(5000);
  
  if (!(BLE_RX_SYS_MSG_COMP(BT_NOTIFY_OK)))
  {
    Serial.println(MYSTRING_ERR);
//    return false; 
  }
  else
  {
    Serial.print("\tReset Result : ");
    Serial.println(BT_NOTIFY_OK);
  }
  
  if (!(BLE_RX_SYS_MSG_COMP(BT_NOTIFY_RDY)))
  {
    Serial.println(MYSTRING_ERR);
//    return false; 
  }
  else
  {
    Serial.print("\tReset Result : ");
    Serial.println(BT_NOTIFY_RDY);
  }
  if (!(BLE_RX_SYS_MSG_COMP(BT_NOTIFY_ADV)))
  {
    Serial.println(MYSTRING_ERR);
//    return false; 
  }
  else
  {
    Serial.print("\tReset Result : ");
    Serial.println(BT_NOTIFY_ADV);
  }
}

String BLE_REQ_INFO(String req_msg)
{
  String RX_STRING;  
  Serial.print("\t[Req MSG] = ");
  Serial.println(req_msg);
  BLESerial.println(req_msg);
  delay(100);

  if (BLE_READ_A_SYS_MSG()==BT_NOTIFY_OK)
  { 
    return RX_STRING = BLE_FIND_MSG();
  }
  else
  {
    return MYSTRING_ERR;
  }
}


boolean BLE_RX_SYS_MSG_COMP(String comp_msg)
{
  String RX_STRING;
  RX_STRING = BLE_READ_A_SYS_MSG();

  if (RX_STRING==comp_msg)
  {
    return true;
  }
  else
  {
    return false;
  }
  
}

String BLE_READ_A_SYS_MSG(void)
{
  String RX_STRING;
  if(BLE_FIND_START())
  {
    RX_STRING=BLE_FIND_MSG();
  }
  else
  {
    return MYSTRING_ERR;
  }
  return RX_STRING;
}

boolean BLE_FIND_START(void)
{
  char rx_char;
  while(BLESerial.available()>0)
  {
    rx_char=(char)BLESerial.read();
    if (rx_char==BT_PLUS)
    {
      return true;   
    }
  }
  return false;
}

String BLE_FIND_MSG(void)
{
  String RX_STRING;
  char rx_char;
  while(BLESerial.available()>0)
  {
    rx_char=(char)BLESerial.read();
    if (rx_char==BT_CR)
    {
      return RX_STRING;   
    }
    else
    {
      RX_STRING += rx_char;
    }
  }
}





boolean BLE_INSTRCT_PWM(unsigned int p_num, unsigned int value)
{
  String TX_INSTRCT="AT+PWM";
  if ((!(p_num<=3)) | (!(value<=255)))
  {
    Serial.println(MYSTRING_ERR);
    return false;
  }
  else
  {
    TX_INSTRCT += (String)p_num+"="+(String)value;
    BLESerial.println(TX_INSTRCT);
    delay(100);

    if (BLE_READ_A_SYS_MSG()==BT_NOTIFY_OK)
    { 
      return true;
    }
    else
    {
      Serial.println(MYSTRING_ERR);
      return false;
    } 
  }
}

String BLE_GET_PWM(unsigned int p_num)
{
  String TX_INSTRCT="AT+PWM";
  if (!(p_num<=3))
  {
    Serial.println(MYSTRING_ERR);
    return MYSTRING_ERR;
  }
  else
  {
    TX_INSTRCT += (String)p_num+"?";
    return BLE_REQ_INFO(TX_INSTRCT);
   }
}

String BLE_GET_AIO(unsigned int p_num)
{
  String TX_INSTRCT="AT+AIO";
  if (!(p_num<=2))
  {
    Serial.println(MYSTRING_ERR);
    return MYSTRING_ERR;
  }
  else
  {
    TX_INSTRCT += (String)p_num+"?";
    return BLE_REQ_INFO(TX_INSTRCT);
  }
}


