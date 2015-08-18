const short Iter_val_max = 16; //ADC accumualte number set-up 
const bool debug = LOW; // Debug mode set-up
const short Inteval_Sec = 5; // Waiting time set-up(sec)
const int Interval_1000ms = 1000; // Multification constant for waiting set-up

const short Adc_Resolution = 10; // ADC resolution
const float Adc_Max_voltage = 5.0; // ADC Vref Maximum
const float ADC_VOL_MIN = 0.5; // Min value of linear sector on temp sensor data
const float ADC_VOL_MAX = 4; // Max value of linear sector on temp sensor data
const float Degree_Min = 10.0; // Temp correspond to minimum voltage
const float Degree_Max = 110.0;// Temp correspond to maximum voltage
const float Diff = (Degree_Max-Degree_Min)/(ADC_VOL_MAX-ADC_VOL_MIN);// Increment value to get actual temperature
const float Boiler_on_temp = 27.0; // Boiler on temp
const float Boiler_off_temp = 29.0; // Boiler off temp

float ADC_Step = 0; // Variable to save 1 step of voltage value using ADC resolution and Vref
bool Internal_Boiler = LOW; // Variable to control internal boiler

const short Boiler_Control_Out = 2; // Boiler control pin set-up

void setup() {
  
  // put your setup code here, to run once:
  pinMode(Boiler_Control_Out, OUTPUT); // Boiler control pin set-up
  
  Serial.begin(9600);
  while(!Serial); // Serial OPEN

  ADC_Step = Adc_Max_voltage/pow(2,Adc_Resolution); // ADC Step calculation 

  //------ADC Step output ------
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
  float result=0; // Variable to save the voltage value of ADC data
  float result_mapped=0; // Variable to save voltage value as a temp value
  
  result = CHECK_TEMP()*ADC_Step; //ADC Step값과 unit Volt per Step값으로 전압값 산출
  result_mapped = result*Diff; // Convert into temp value

  if ((result<=ADC_VOL_MIN) | (result>=ADC_VOL_MAX)) // Don't calculate within nonlinear sector -> ERROR
  {
    if (debug==HIGH) // Print out if only when its debug mode, for debugging
    {
      Serial.print("\tCurrent Mean ADC Value = ");
      Serial.print(result,10);
      Serial.print(" [V]\n");      
    }
    Serial.println("ADC Value Out of Range !!"); // Print out 'Nonlinear sector'
  }
  else // Operate in linear sector
  {
    if (debug==HIGH)
    {
      Serial.print("\tCurrent Mean ADC Value = "); // Print out the calculated value from above
      Serial.print(result,10); // Print out Voltage value
      Serial.print(" [V]\n");
      Serial.print("\tMapped Degree =");
      Serial.print(result_mapped,10); // Print out Temp value
      Serial.print(" [Degree]\n");      
    }
    else
    {
      Serial.print("\tCurrent Degree = ");
      Serial.print(result_mapped,10); // Print out just Temp value
      Serial.print(" [Degree]\n");
      BOILER_CONTROL(result_mapped); // Boiler control through send current temp value to Param
      Serial.print("\n");  
    }  

  }
  delay(Inteval_Sec*Interval_1000ms); // Temp value measurement interval
}

int CHECK_TEMP(void) // Return function after reading ADC value
{
  int Temp_current = 0; // Variable to save present temperature
  int Temp_accumul = 0; // Variable to save accumulated values to average it
  int Temp_mean = 0; // Variable to save averaged value
  int Iter_val = 0; // Variable to save number of average calculation number

  while(Iter_val<=(Iter_val_max-1)) // Keep operating while present Iteration value is lower than Max 
  {
    if (debug==HIGH)
    {
      Serial.print("Iterantion Number = ");
      Serial.print(Iter_val);
      Serial.print("\n");  // Print out accumulated number till persent
    }
    
    Temp_current = analogRead(A0); // Inputed present ADC value
    Temp_accumul += Temp_current; // Accumulate include past values
    Iter_val += 1;  // Accumulation numeber increase
  }

  Iter_val=0; // Initialization Accumulate number
  Temp_mean=Temp_accumul/Iter_val_max; // Calculate average value
  return Temp_mean; // Return average temp value
}

void BOILER_CONTROL(float Temp) // Function makes that boiler control sign on/off through inputted temp value
{
  
  if (Internal_Boiler==LOW) // When presen boiler is off
  {
    if (Temp <= Boiler_on_temp) // Operate when temp is lower than boiler operating temp
    {
      Internal_Boiler = HIGH;
      Serial.println("\tCurrent Boiler Status = [ ON ]");
    }
    else // Otherwise maintain OFF state of boiler
    {
      Internal_Boiler = LOW;
      Serial.println("\tCurrent Boiler Status = [ OFF ]");
    }
  }
  else // If boiler OFF
  {
    if (Temp >= Boiler_off_temp) // If the present temp is higher than boiler OFF datum, OFF the boiler
    {
      Internal_Boiler = LOW;
      Serial.println("\tCurrent Boiler Status = [ OFF ]");
        
    }
    else // Otherwise keep operation boiler
    {
      Internal_Boiler = HIGH;
      Serial.println("\tCurrent Boiler Status = [ ON ]");
    }
  }
  digitalWrite(Boiler_Control_Out, Internal_Boiler);
 
}
