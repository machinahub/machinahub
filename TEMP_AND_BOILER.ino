const short Iter_val_max = 16; //ADC 누적 횟수 설정 
const bool debug = LOW; // 디버그모드 설정
const short Inteval_Sec = 5; // 대기시간 설정(초)
const int Interval_1000ms = 1000; // 대기설정을 위한 곱셈상수

const short Adc_Resolution = 10; // ADC 해상도
const float Adc_Max_voltage = 5.0; // ADC Vref 최대치
const float ADC_VOL_MIN = 0.5; // 온도센서 데이터 상에서 리니어구간의 전압 최소값
const float ADC_VOL_MAX = 4; // 온도센서 데이터 상에서 리니어구간의 전압 최대값
const float Degree_Min = 10.0; // 전압 최소값에 대응되는 온도
const float Degree_Max = 110.0;// 전압 최대값에 대응되는 온도
const float Diff = (Degree_Max-Degree_Min)/(ADC_VOL_MAX-ADC_VOL_MIN);//실제 온도를 구하기위한 증분값
const float Boiler_on_temp = 27.0; // 보일러 켜지는 온도
const float Boiler_off_temp = 29.0; // 보일러 꺼지는 온도

float ADC_Step = 0; // ADC 해상도와 Vref를 이용하여 ADC 한스텝의 전압값을 저장할 변수
bool Internal_Boiler = LOW; // 내부 보일러 컨트롤을 위한 변수

const short Boiler_Control_Out = 2; // 보일러 컨트롤 핀 설정

void setup() {
  
  // put your setup code here, to run once:
  pinMode(Boiler_Control_Out, OUTPUT); // 보일러 컨트롤 핀 설정
  
  Serial.begin(9600);
  while(!Serial); //시리얼 OPEN

  ADC_Step = Adc_Max_voltage/pow(2,Adc_Resolution); // ADC Step 산출 

  //------ADC Step 출력 ------
  Serial.print("ADC Resolution ="); 
  Serial.print(Adc_Resolution);
  Serial.print(" bits, Total Step = ");
  Serial.print((short)(pow(2,Adc_Resolution)));
  Serial.print(" Steps\n");

  Serial.print("ADC Voltage Max =");
  Serial.print(Adc_Max_voltage);
  Serial.print(" [V]\n"); 

  Serial.print("Volt per ADC Step = ");
  Serial.print(ADC_Step,10);
  Serial.print(" [V]\n\n\n");
  //------------------------------
}

void loop() {
  // put your main code here, to run repeatedly:
  float result=0; // ADC된 데이터의 전압값을 저장하기 위한 변수
  float result_mapped=0; // 전압값을 온도값으로 저장하기 위한 변수
  
  result = CHECK_TEMP()*ADC_Step; //ADC Step값과 unit Volt per Step값으로 전압값 산출
  result_mapped = result*Diff; // 온도값으로 변환

  if ((result<=ADC_VOL_MIN) | (result>=ADC_VOL_MAX)) // 비선형구간에서는 연산하지 않음 -> 에러
  {
    if (debug==HIGH) // 디버그를 위하여 디버그모드일때만 출력
    {
      Serial.print("\tCurrent Mean ADC Value = ");
      Serial.print(result,10);
      Serial.print(" [V]\n");      
    }
    Serial.println("ADC Value Out of Range !!"); // 비선형구간이라는 것을 출력
  }
  else //선형구간에서의 동작
  {
    if (debug==HIGH)
    {
      Serial.print("\tCurrent Mean ADC Value = "); //위에서 계산된 값을 출력
      Serial.print(result,10); // 전압값출력
      Serial.print(" [V]\n");
      Serial.print("\tMapped Degree =");
      Serial.print(result_mapped,10); // 온도값출력
      Serial.print(" [Degree]\n");      
    }
    else
    {
      Serial.print("\tCurrent Degree = ");
      Serial.print(result_mapped,10); // 온도값만 출력
      Serial.print(" [Degree]\n");
      BOILER_CONTROL(result_mapped); // 현재의 온도값을 Param으로 보내서 보일러 컨트롤
      Serial.print("\n");  
    }  

  }
  delay(Inteval_Sec*Interval_1000ms); //온도값 측정 인터벌
}

int CHECK_TEMP(void) // ADC값을 읽어서 리턴하는 함수
{
  int Temp_current = 0; //현재 온도를 저장하는 변수
  int Temp_accumul = 0; // 평균을 내기 위해 누적된 된 값을 저장하는 변수
  int Temp_mean = 0; // 평균내어진 값을 저장하는 변수;
  int Iter_val = 0; // 평균 산출용 횟수를 저장하는 변수

  while(Iter_val<=(Iter_val_max-1)) // 현재 Iteration 값이 Max보다 작은 동안 계속 실행 
  {
    if (debug==HIGH)
    {
      Serial.print("Iterantion Number = ");
      Serial.print(Iter_val);
      Serial.print("\n");  //현재까지 누적횟수를 프린트
    }
    
    Temp_current = analogRead(A0); // 현재 ADC값 입력받음
    Temp_accumul += Temp_current; // 이전값을 포함하여 누적시킴
    Iter_val += 1;  // 누적횟수 증가
  }

  Iter_val=0; // 누적회수 초기화
  Temp_mean=Temp_accumul/Iter_val_max; //평균치 산출
  return Temp_mean; // 온도 평균값을 리턴함
}

void BOILER_CONTROL(float Temp) //온도값을 입력받아 보일러 컨트롤 신호을 ON/OFF해주는 함수
{
  
  if (Internal_Boiler==LOW) // 현재 보일러가 꺼져있을 경우
  {
    if (Temp <= Boiler_on_temp) // 보일러 가동하는 온도보다 낮으면 가동
    {
      Internal_Boiler = HIGH;
      Serial.println("\tCurrent Boiler Status = [ ON ]");
    }
    else // 아니면 현재 보일러가 꺼져있는 상태로 유지
    {
      Internal_Boiler = LOW;
      Serial.println("\tCurrent Boiler Status = [ OFF ]");
    }
  }
  else // 현재 보일러가 켜져있을 경우
  {
    if (Temp >= Boiler_off_temp) // 보일러는 끄는 온도보다 높으면 보일러 끔
    {
      Internal_Boiler = LOW;
      Serial.println("\tCurrent Boiler Status = [ OFF ]");
        
    }
    else // 아니면 계속 보일러를 켜둠
    {
      Internal_Boiler = HIGH;
      Serial.println("\tCurrent Boiler Status = [ ON ]");
    }
  }
  digitalWrite(Boiler_Control_Out, Internal_Boiler);
 
}
