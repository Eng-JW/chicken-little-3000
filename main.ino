
/*
╔╦═╦╗──────╔╦═╗
║║║║╠═╗╔╗╔╗╠╣═╬═╦═╦╗
║║║║║╬╚╣╚╣╚╣╠═║╬║║║║
╚═╩═╩══╩═╩═╩╩═╩═╩╩═╝
IFSP Guarulhos
*/

#include "DHT.h"
#include <EEPROM.h>
#include <LiquidCrystal_I2C.h>
#include <PID_v1_bc.h>
#include <Stepper.h>            
#include <virtuabotixRTC.h>     

#define bt_01 19           // Botão do menu
#define bt_02 18           // Botão Iniciar
#define bt_03 5            // Botão Finalizar
#define t_control 15       // Mosfet da lampada
#define u_control 2        // Mosfet do umidificador
#define buzzer 13          // Buzzer
#define DHTPIN 4           // Sensor de Temperatura,Umidade
#define DHTTYPE DHT22      // DHT 22  (AM2302), AM2321
#define EEPROM_SIZE 1      // This is 1-Byte

virtuabotixRTC myRTC(27, 14, 12);                   
int PassosPorVolta = 100;                           
Stepper MotorStepper(PassosPorVolta, 25,32,26,33);  

float umidade_a = 0;
float umidade_b = 0;
float temperatura_a = 0;

bool estado = 0;         // 0 = Desligado, 1 = Ligado

int contagem[5];
int painel_off = 0;
int painel_on = 0;
int nextHour;
int currentHour;
int currentMinute;
int modomotor;
int dia_final; 
int dias;

double Setpoint = 0;  // Temperatura de referência
double Input, Output;
double Kp = 20.0;         // Ganho proporcional
double Ki = 5.0;         // Ganho integral
double Kd = 2.0;           // Ganho derivativo

DHT dht(DHTPIN, DHTTYPE);

LiquidCrystal_I2C lcd(0x27, 16, 2);

PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);

void setup() 
{

  Serial.begin(115200);
  Serial.println("Iniciando Sistema!");

  dht.begin();

  pinMode(bt_01, INPUT_PULLUP);
  pinMode(bt_02, INPUT_PULLUP);
  pinMode(bt_03, INPUT_PULLUP);
  pinMode(t_control, OUTPUT);
  pinMode(u_control, OUTPUT);

  analogWrite(t_control, 0);
  digitalWrite(u_control, 0);

  myRTC.setDS1302Time(00, 26, 14, 2, 17, 10, 2023); // Set Data e Hora

  int currentHour = myRTC.hours;          
  int nextHour = 0;  

  MotorStepper.setSpeed(300);    

  EEPROM.begin(EEPROM_SIZE);

  myPID.SetOutputLimits(0, 255);  // Limite de saída PWM entre 0 e 255
  myPID.SetMode(AUTOMATIC);
  myPID.SetSampleTime(1000);  // Tempo de amostragem em milissegundos (1 segundo)

  inicio();

}

void loop() 
{

  float temperatura = dht.readTemperature();
  float umidade = dht.readHumidity();

  if (isnan(temperatura) || isnan(umidade)) 
  {
    Serial.println(F("Falha ao comunicar com DHT22"));
    return;
  }

  myRTC.updateTime();

  int currentHour = myRTC.hours;
  int currentMinute = myRTC.minutes;
  


  if (digitalRead(bt_03) == LOW)
  {
    
    umidade_a = 0;
    umidade_b = 0;
    temperatura_a = 0;
    estado = 0;
    contagem[4] = 0;
    digitalWrite(u_control, LOW);
    analogWrite(t_control, 0);
    EEPROM.write(0, 0);
    delay(500);

  }

  if(EEPROM.read(0) == 0) // Sistema Desligado
  {

    estado = 0;
    temperatura_a = 0;
    umidade_a = 0;
    umidade_b = 0;
    digitalWrite(u_control, LOW);
    analogWrite(t_control, 0);
    modomotor = 0 ;

  }

  if(EEPROM.read(0) == 1) // Ovo de codorna
  {
    estado = 1;
    temperatura_a = 37;
    umidade_a = 50; 
    umidade_b = 75;
    modomotor = 1 ;
    Setpoint = 37;
    dia_final = 14;
  }

  if(EEPROM.read(0) == 2) // Ovo de Galinha
  {
    estado = 1;
    temperatura_a = 28;
    umidade_a = 50; 
    umidade_b = 60;
    modomotor = 2 ;
    Setpoint = 28;
    dia_final = 19;
  }

  if(estado == 0)
  {

  if (digitalRead(bt_01) == LOW)
  {

    painel_off++;
    delay(500);

  }

  if (painel_off > 1)
  {

    painel_off = 0;

  }

  if ( painel_off == 0)
  {

    if (contagem[2] == 0)
    {

      lcd.clear();
      lcd.print("CODORNA:    D:16");
      lcd.setCursor(0, 1);
      lcd.print("T:37     U:50~75");

      contagem[0] = 0;
      contagem[1] = 0;
      contagem[2]++;
      contagem[3] = 0;
    }
    if(digitalRead(bt_02) == LOW)
    {

      EEPROM.write(0, 1);
      delay(1000);

    }
  }


  if ( painel_off == 1)
  {

    if (contagem[3] == 0)
    {

      lcd.clear();
      lcd.print("GALINHA:    D:21");
      lcd.setCursor(0, 1);
      lcd.print("T:28     U:50~60");

      contagem[0] = 0;
      contagem[1] = 0;
      contagem[2] = 0;
      contagem[3]++;

    }
    if(digitalRead(bt_02) == LOW)
    {
     
     EEPROM.write(0, 2);
     delay(1000);

    }

  }


  }
  if(estado == 1)
  {

  int potencia = Output;

  Input = temperatura;
  myPID.Compute();

  if (contagem[4] == 0)
  {
   nextHour = (currentHour + 2) % 24;  
   contagem[4]++;
  }

  if (nextHour == 24)
  {
    nextHour = 0;
    dias++;
  }

  if (modomotor == 1)
  {
    for (int i = 0; i<=3; i++)
      {
        MotorStepper.step(PassosPorVolta);
      }
  }

  if (modomotor == 2)
  {
    if(dias<=dia_final)
    {
    if ((currentHour % 24) == nextHour) {
      for (int i = 0; i<=3; i++)
        {
          MotorStepper.step(PassosPorVolta);
        }
      nextHour = nextHour + 2;
    }
    }
  }

  if (digitalRead(bt_01) == LOW)
  {

    painel_on++;
    delay(500);

  }
    
  if (painel_on > 1)
  {

    painel_on = 0;

  }

  Serial.println("//------------------------( Monitor Serial )------------------------//");

  Serial.print(F("// Painel: "));
  Serial.print(painel_on);
  Serial.print(F(" Estado: "));
  Serial.print(estado);
  Serial.print(F(" Umidade_Atual: "));
  Serial.print(umidade);
  Serial.print(" Umidade: ");
  Serial.print(umidade_a);
  Serial.print(" ~~ ");
  Serial.print(umidade_b);
  Serial.println(" //");

  Serial.print(F("// Temperatura_Atual: "));
  Serial.print(temperatura);
  Serial.print(F("°C "));
  Serial.print(F(" Temperatura: "));
  Serial.print(temperatura_a);
  Serial.print(" Potencia: ");
  Serial.print(potencia);
  Serial.println("      //");

  Serial.println("//////////////////////////////////////////////////////////////////////");

  if ( painel_on == 0)
  {

    if (contagem[0] == 0)
    {

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("T: ");
      lcd.setCursor(6, 0);
      lcd.print("C");

      lcd.setCursor(0, 1);
      lcd.print("U: "); 
      lcd.setCursor(6, 1);
      lcd.print("%");  

      lcd.setCursor(13,0);
      lcd.print("PWM"); 

      contagem[0]++;
      contagem[1] = 0;

    }
    lcd.setCursor(2, 0);
    lcd.print(temperatura);
    lcd.print("      "); 

    lcd.setCursor(2, 1);
    lcd.print(umidade);
    lcd.print("      "); 

    lcd.setCursor(13, 1);
    lcd.print(potencia);
    lcd.print("   "); 
    
  }

  if ( painel_on == 1)
  {

    if (contagem[1] == 0)
    {
      
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("H: ");
      lcd.setCursor(0, 1);
      lcd.print("D: ");                                                                 
      delay (500);
      contagem[0] = 0;
      contagem[1]++;

    }
    lcd.setCursor(2, 0);
    lcd.print(myRTC.hours);                                                                          
    lcd.print(":");                                                                               
    lcd.print(myRTC.minutes);                                                                        
    lcd.print(":");                                                                                 
    lcd.print(myRTC.seconds); 
    lcd.setCursor(2, 1);
    lcd.print(myRTC.dayofmonth);                                                                      
    lcd.print("/");                                                                                    
    lcd.print(myRTC.month);                                                                           
    lcd.print("/");                                                                                     
    lcd.print(myRTC.year);                                                                             
    lcd.print("  "); 

  }

  analogWrite(t_control, potencia);

  if (umidade > umidade_b)
  {

    digitalWrite(u_control, LOW);

  }

  if (umidade < umidade_a)
  {

    digitalWrite(u_control, HIGH);

  }
  
  }

}

void inicio()
{
  lcd.init();
  lcd.backlight();
  lcd.clear();

  lcd.print(" CHICKEN LITTLE ");
  lcd.setCursor(0,1);
  lcd.print(" ---( 3001 )--- ");

  tone(buzzer,262,200); //DO
  delay(200);
  tone(buzzer,294,300); //RE
  delay(200);
  tone(buzzer,330,300); //MI
  delay(200);
  tone(buzzer,349,300); //FA
  delay(300);
  tone(buzzer,349,300); //FA
  delay(300);

  lcd.clear();

  tone(buzzer,349,300); //FA
  delay(300);
  tone(buzzer,262,100); //DO
  delay(200);
  tone(buzzer,294,300); //RE
  delay(200);   
  tone(buzzer,262,100); //DO
  delay(200);
  tone(buzzer,294,300); //RE
  delay(300);

  lcd.print(" CHICKEN LITTLE ");
  lcd.setCursor(0,1);
  lcd.print(" ---( 3001 )--- ");

  tone(buzzer,294,300); //RE
  delay(300);
  tone(buzzer,294,300); //RE
  delay(300);
  tone(buzzer,262,200); //DO
  delay(200);
  tone(buzzer,392,200); //SOL
  delay(200);
  tone(buzzer,349,200); //FA
  delay(200);
  tone(buzzer,330,300); //MI
  delay(300);

  lcd.clear();

  tone(buzzer,330,300); //MI
  delay(300);
  tone(buzzer,330,300); //MI
  delay(300);
  tone(buzzer,262,200); //DO
  delay(200);
  tone(buzzer,294,300); //RE
  delay(200);

  lcd.print(" CHICKEN LITTLE ");
  lcd.setCursor(0,1);
  lcd.print(" ---( 3001 )--- ");

  tone(buzzer,330,300); //MI
  delay(200);
  tone(buzzer,349,300); //FA
  delay(300);
  tone(buzzer,349,300); //FA
  delay(300);
  tone(buzzer,349,300); //FA
  delay(300);

  lcd.clear();
  
}
