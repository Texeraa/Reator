#include <DallasTemperature.h> // ler a biblioteca
#include <OneWire.h> // ler a biblioteca
#include <HX711.h>

#define PRESSAO_DOUT_PIN  A0
#define PRESSAO_SCK_PIN  A1
#define NIVEL A2
#define SENSOR_TEMPERATURA 22
#define RESISTENCIA 23
#define DIRECTION 2
#define STEP 3
#define SLEEP 4 
#define DIRECTION2 6
#define STEP2 7
#define SLEEP2 8 
#define BOMBA_R1 9
#define BOMBA_R2 11
#define BOMBA_R3 12
#define BOMBA_J 13

#define C1 46
#define C2 47
#define C3 48
#define C4 49
#define C5 50
#define C6 51
#define C7 52
#define C8 53

OneWire oneWire(SENSOR_TEMPERATURA);
DallasTemperature sensors(&oneWire);
HX711 sensor_pressao;

int motorSpeed = 100, PWM_BOMBA_R1 = 0, PWM_BOMBA_R2 = 0, PWM_BOMBA_R3 = 0, PWM_BOMBA_J = 0, RESIS = 0,  VELOCIDADE_AGITADOR = 0, VELOCIDADE_AGITADOR_ANTERIOR= 0;
int VELOCIDADE_AGITADOR2 = 0, VELOCIDADE_AGITADOR_ANTERIOR2 = 100, a;
int PID =0;
float TEMPERATURA_R1 = 0, TEMPERATURA_R2 = 0, TEMPERATURA_J = 0, PERIODO_INTERVALO_AGITADOR = 0, PERIODO_DESLIGADO = 0, PERIODO_AGITADOR, VOLUME = 0, Vi;
float PERIODO_INTERVALO_AGITADOR2 = 0, PERIODO_DESLIGADO2 = 0, PERIODO_AGITADOR2, T_SP = 30;
long PERIODO_DESLIGADO_TEMPO = 0;
long PERIODO_DESLIGADO_TEMPO2 = 0;

float K2 = 40, PWM_ERROZERO2 = 12, VOLUME_SP = 4, ERRO2 = 0, TIV = 500000; 
float KJ = 30, PWM_ERROZEROJ = 60, ERROJ = 0; 

boolean ATIVA_AGITACAO = 1;
boolean ATIVA_AGITACAO2 = 0;
boolean ATIVA_TEMPERATURA = 1;
boolean ATIVA_RESISTENCIA = 1;
boolean ATIVA_BOMBA_R1 = 1;
boolean ATIVA_BOMBA_R2 = 1;
boolean ATIVA_BOMBA_R3 = 1;
boolean ATIVA_BOMBA_J = 1;
boolean ATIVA_NIVEL = 1;
int TESTE = 1;
boolean PERIODO= false;
boolean PERIODO2= false;
int i=0;
boolean LEITURA_NIVEL = 0; 
const long PRESSAO_OFFSET = 50682624;
const long PRESSAO_DIVIDER = 5895655;

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
   pinMode(BOMBA_R1, OUTPUT);
   digitalWrite(BOMBA_R1, LOW);
   pinMode(BOMBA_R2, OUTPUT);
   digitalWrite(BOMBA_R2, LOW);   
   pinMode(BOMBA_R3, OUTPUT);
   digitalWrite(BOMBA_R3, LOW);   
   pinMode(BOMBA_J, OUTPUT);
   digitalWrite(BOMBA_J, LOW);   
   pinMode(STEP, OUTPUT);
   digitalWrite(STEP, HIGH);
   pinMode(DIRECTION, OUTPUT);
   digitalWrite(DIRECTION, LOW);
   pinMode(SLEEP, OUTPUT);
   digitalWrite(SLEEP, LOW);
   pinMode(STEP2, OUTPUT);
   digitalWrite(STEP2, HIGH);
   pinMode(DIRECTION2, OUTPUT);
   digitalWrite(DIRECTION2, LOW);
   pinMode(SLEEP2, OUTPUT);
   digitalWrite(SLEEP2, LOW);   
   pinMode(RESISTENCIA, OUTPUT);
   digitalWrite(RESISTENCIA, LOW);  
   pinMode(NIVEL, INPUT);
      
   sensors.begin();
   sensor_pressao.begin(PRESSAO_DOUT_PIN, PRESSAO_SCK_PIN);
   sensor_pressao.set_scale(PRESSAO_DIVIDER);
   sensor_pressao.set_offset(PRESSAO_OFFSET);
   sensor_pressao.tare();
}

void loop() {
  LE_ALAVANCA();
  LE_DADOS();
  ENVIA_DADOS();
  if(ATIVA_NIVEL){
    LEITURA_NIVEL = (analogRead(NIVEL) < 100) ? 1 : 0;
  }
if(TESTE==1){
  EXIBE_DADOS2();
}else{
  EXIBE_DADOS1();
}
  delay(1000);
}

void LE_ALAVANCA(){
  ATIVA_AGITACAO = digitalRead(C6);
  ATIVA_TEMPERATURA = digitalRead(C2);
  ATIVA_RESISTENCIA = digitalRead(C1);
  ATIVA_BOMBA_R1 = digitalRead(C4);
  ATIVA_BOMBA_R2 = digitalRead(C3);
  ATIVA_BOMBA_R3 = digitalRead(C7);
  ATIVA_BOMBA_J = digitalRead(C8);
  PID = digitalRead(C5);
}

void LE_DADOS() {
    if(Serial.available() > 0){
    // Lê primeiro byte digitado pelo usuário e atua no sistema
    
        switch (Serial.read()){
          
          case 'A':
                    delay(2);
                    switch(Serial.read()){
                      case '1':
                          VELOCIDADE_AGITADOR = Serial.parseInt(); //faixa recomendada limite 156
                          if((Serial.available() > 0) && (Serial.read() == 'P'))
                              PERIODO_INTERVALO_AGITADOR = Serial.parseFloat();
                          if((Serial.available() > 0) && (Serial.read() == 'D'))
                              PERIODO_DESLIGADO = Serial.parseFloat();
                          if(VELOCIDADE_AGITADOR > 130) //156
                              VELOCIDADE_AGITADOR = 130;
                          else if(VELOCIDADE_AGITADOR < 30 && VELOCIDADE_AGITADOR != 0)
                          {
                            VELOCIDADE_AGITADOR = 30;
                            Serial.println("Velocidade mínima: 30rpm");
                          }
                          break;
                        case '2':
                          VELOCIDADE_AGITADOR2 = Serial.parseInt(); //faixa recomendada limite 156
                          if((Serial.available() > 0) && (Serial.read() == 'P'))
                              PERIODO_INTERVALO_AGITADOR2 = Serial.parseFloat();
                          if((Serial.available() > 0) && (Serial.read() == 'D'))
                              PERIODO_DESLIGADO2 = Serial.parseFloat();
                          if(VELOCIDADE_AGITADOR2 > 130) //156
                              VELOCIDADE_AGITADOR2 = 130;
                          else if(VELOCIDADE_AGITADOR2 < 30 && VELOCIDADE_AGITADOR2 != 0)
                          {
                            VELOCIDADE_AGITADOR2 = 30;
                            Serial.println("Velocidade mínima: 30rpm");
                          }
                          break;
                    }
            
          case 'B':
                    delay(2);
                    switch(Serial.read()){
                      case '1':
                          PWM_BOMBA_R1 = Serial.parseInt();
                      break;
                      case '2':
                          PWM_BOMBA_R2 = Serial.parseInt();
                      break;
                      case '3':
                          PWM_BOMBA_R3 = Serial.parseInt();
                      break;
                      case 'J':
                          PWM_BOMBA_J = Serial.parseInt();
                      break;
                    }


        
                    break;
           case 'V':
                    Vi = Vi + VOLUME;
                    Serial.println("Volume Zerado");
                    break;
                    
           case 'T':
                    TESTE = Serial.parseInt();
                    if(TESTE != 0){
                      TESTE = 1;
                    }else{
                      TESTE = 0;
                    }
                    break;
           case 'P':
                    if(PID == 0){
                      PID = 1;
                    }else{
                      PID = 0;
                    }
                    Serial.println("PID");
                    break;
            case 'Z':
                    a = Serial.parseInt();
                    break;
        }
      
    }

    if(Serial1.available() > 0){
    // Lê primeiro byte digitado pelo usuário e atua no sistema
    
        switch (Serial1.read()){

           case 'T':
                    T_SP = Serial1.parseInt();
                    break;
           case 'V':
                    VOLUME_SP = Serial1.parseFloat();
                    break;
        }
    }

    if(ATIVA_BOMBA_R1) // Aplica as alterações 
      LIGA_BOMBA_R1();

    if(ATIVA_BOMBA_R2)
      LIGA_BOMBA_R2();
      
    if(ATIVA_BOMBA_R3)
      LIGA_BOMBA_R3();
      
    if(ATIVA_BOMBA_J)
      LIGA_BOMBA_J();

    if(ATIVA_TEMPERATURA){
        LE_TEMPERATURA();
    } 
    if(ATIVA_RESISTENCIA)
    LIGA_RESISTENCIA();  
      
    if(ATIVA_AGITACAO)
    LIGA_AGITACAO();

    if(ATIVA_AGITACAO2)
    LIGA_AGITACAO2();
    
    if(ATIVA_NIVEL)
    LE_NIVEL();

    delay(100);
}

void ENVIA_DADOS(){
  //Serial.print("A");
  //Serial.println(a);
  Serial1.print(TEMPERATURA_R1);
  Serial1.println(TEMPERATURA_J);
  Serial1.print("A");
  Serial1.println(TEMPERATURA_R1, 2);
  delay(2);
  Serial1.print("B");
  Serial1.println(TEMPERATURA_J, 2);
}

void LE_NIVEL()
{
    //VOLUME = (sensor_pressao.get_units()*25.795 + Vi);
   
    if(sensor_pressao.is_ready())
    VOLUME = sensor_pressao.read_average(15)*0.0050086/1000. - 1.2495 +8.21 -Vi;
    //VOLUME = (sensor_pressao.read()*4229885.285946/1000000+7188.26145767);
    
}

void LE_TEMPERATURA(){

        sensors.requestTemperatures();
        TEMPERATURA_R1 = sensors.getTempCByIndex(0);
        TEMPERATURA_R2 = sensors.getTempCByIndex(1);
        TEMPERATURA_J = sensors.getTempCByIndex(2);
  
}

void LIGA_AGITACAO()
{  
  int step = 30;
  // Implementa uma variação suave de velocidade
  if(abs(VELOCIDADE_AGITADOR - VELOCIDADE_AGITADOR_ANTERIOR) <= step)
  {
      VELOCIDADE_AGITADOR_ANTERIOR = VELOCIDADE_AGITADOR;
  }
  else if((VELOCIDADE_AGITADOR - VELOCIDADE_AGITADOR_ANTERIOR) > step)
      VELOCIDADE_AGITADOR_ANTERIOR+= step;
      
  else if((VELOCIDADE_AGITADOR - VELOCIDADE_AGITADOR_ANTERIOR) < step)
      VELOCIDADE_AGITADOR_ANTERIOR-= step;

  define_periodo();
  
  if((VELOCIDADE_AGITADOR == 0) || not(PERIODO))
  {
      noTone(STEP);
      digitalWrite(SLEEP, LOW);
  }
  else if((VELOCIDADE_AGITADOR != 0) && PERIODO)
  {
      digitalWrite(SLEEP, HIGH);
      tone(STEP, VELOCIDADE_AGITADOR_ANTERIOR*200/60); // Manda uma onda quadrada para o driver do motor com a velocidade em rpm desejada.
  }
}

void define_periodo()
{
    if(not(PERIODO))  // Permite usar o acionamento suave no motor de passo usando os períodos de intervalo.
     VELOCIDADE_AGITADOR_ANTERIOR = 0;

    if(PERIODO_INTERVALO_AGITADOR != 0)
    {
      if((millis() - PERIODO_AGITADOR)/1000 >= (PERIODO_INTERVALO_AGITADOR+PERIODO_DESLIGADO))
      {
         PERIODO = false;
         PERIODO_DESLIGADO_TEMPO = millis(); 
         PERIODO_AGITADOR = millis();
      }
      else if((millis() - PERIODO_DESLIGADO_TEMPO)/1000 >= PERIODO_DESLIGADO)
      {
            PERIODO = true; 
      }     
     }
    else
     PERIODO = true;
}

void LIGA_AGITACAO2()
{  
  int step = 30;
  // Implementa uma variação suave de velocidade
  if(abs(VELOCIDADE_AGITADOR2 - VELOCIDADE_AGITADOR_ANTERIOR2) <= step)
  {
      VELOCIDADE_AGITADOR_ANTERIOR2 = VELOCIDADE_AGITADOR2;
  }
  else if((VELOCIDADE_AGITADOR2 - VELOCIDADE_AGITADOR_ANTERIOR2) > step)
      VELOCIDADE_AGITADOR_ANTERIOR2+= step;
      
  else if((VELOCIDADE_AGITADOR2 - VELOCIDADE_AGITADOR_ANTERIOR2) < step)
      VELOCIDADE_AGITADOR_ANTERIOR2-= step;

  define_periodo2();
  
  if((VELOCIDADE_AGITADOR2 == 0) || not(PERIODO2))
  {
      noTone(STEP2);
      digitalWrite(SLEEP2, LOW);
  }
  else if((VELOCIDADE_AGITADOR2 != 0) && PERIODO2)
  {
      digitalWrite(SLEEP2, HIGH);
      tone(STEP2, VELOCIDADE_AGITADOR_ANTERIOR2*200/60); // Manda uma onda quadrada para o driver do motor com a velocidade em rpm desejada.
  }
}
void define_periodo2()
{
    if(not(PERIODO2))  // Permite usar o acionamento suave no motor de passo usando os períodos de intervalo.
     VELOCIDADE_AGITADOR_ANTERIOR2 = 0;

    if(PERIODO_INTERVALO_AGITADOR2 != 0)
    {
      if((millis() - PERIODO_AGITADOR2)/1000 >= (PERIODO_INTERVALO_AGITADOR2+PERIODO_DESLIGADO2))
      {
         PERIODO2 = false;
         PERIODO_DESLIGADO_TEMPO2 = millis(); 
         PERIODO_AGITADOR2 = millis();
      }
      else if((millis() - PERIODO_DESLIGADO_TEMPO2)/1000 >= PERIODO_DESLIGADO2)
      {
            PERIODO2 = true; 
      }     
     }
    else
     PERIODO2 = true;
}

void LIGA_BOMBA_R1(){
        analogWrite(BOMBA_R1, map(PWM_BOMBA_R1,0,100,0,253));
}
double INT_ERRO = 0;
int A= 1, TEMPO_ANTES = 0;
void LIGA_BOMBA_R2(){
  if(PID == 1){
    if(A == 1){
      TEMPO_ANTES = millis();
      A = 0;
    }
        ERRO2 = VOLUME - VOLUME_SP;
        INT_ERRO = INT_ERRO + ERRO2*(millis()-TEMPO_ANTES);
        PWM_BOMBA_R2 = PWM_ERROZERO2 + K2*ERRO2+ K2*INT_ERRO/TIV;
        if(PWM_BOMBA_R2 < 0){
            PWM_BOMBA_R2 = 0;
        }
        if(PWM_BOMBA_R2 > 100){
            PWM_BOMBA_R2 = 100;
        }
  }
        TEMPO_ANTES = millis();
        analogWrite(BOMBA_R2, map(PWM_BOMBA_R2,0,100,0,253));
}

void LIGA_BOMBA_R3(){

        analogWrite(BOMBA_R3, map(PWM_BOMBA_R3,0,100,0,253));
}

void LIGA_BOMBA_J(){
    if(PID == 1){
        ERROJ = TEMPERATURA_R1 - 25;
        PWM_BOMBA_J = PWM_ERROZEROJ + KJ*ERROJ;
        if(PWM_BOMBA_J < 0){
            PWM_BOMBA_J = 0;
        }
        if(PWM_BOMBA_J > 100){
            PWM_BOMBA_J = 100;
        }
  }
        analogWrite(BOMBA_J, map(PWM_BOMBA_J,0,100,0,253));
}
void LIGA_RESISTENCIA(){
  if(TEMPERATURA_R2 < T_SP){
        digitalWrite(RESISTENCIA, HIGH);
        RESIS =1;  
  }else{
    digitalWrite(RESISTENCIA, LOW);
    RESIS = 0;
  }
}
void EXIBE_DADOS2(){
  Serial.print(millis()/1000);
    if(ATIVA_AGITACAO){
         Serial.print(", ");
         Serial.print(VELOCIDADE_AGITADOR_ANTERIOR);
         
    }if(ATIVA_AGITACAO2){
         Serial.print(", ");
         Serial.print(VELOCIDADE_AGITADOR_ANTERIOR2);
         
    }
    if(ATIVA_TEMPERATURA){
         Serial.print(", ");
         Serial.print(TEMPERATURA_R1);
         Serial.print(", ");
         Serial.print(TEMPERATURA_R2);
         Serial.print(", ");
         Serial.print(TEMPERATURA_J);
    }
    if(ATIVA_BOMBA_R1){
         Serial.print(", ");
         Serial.print(PWM_BOMBA_R1);
    }
    if(ATIVA_BOMBA_R2){
         Serial.print(", ");
         Serial.print(PWM_BOMBA_R2);
    }
    if(ATIVA_BOMBA_R3){
         Serial.print(", ");
         Serial.print(PWM_BOMBA_R3);
    }
    if(ATIVA_BOMBA_J){
         Serial.print(", ");
         Serial.print(PWM_BOMBA_J);
    }
    if(ATIVA_RESISTENCIA){
         Serial.print(", ");
         Serial.print(RESIS);
    }
    if(ATIVA_NIVEL){
         Serial.print(", ");
         Serial.print(VOLUME, 2);
    }
    
  Serial.print(", ");
  Serial.print(T_SP);
  Serial.print(", ");
  Serial.println(VOLUME_SP);

}


void EXIBE_DADOS1(){
  Serial.print("\n************************************************************************\n");

    if(ATIVA_AGITACAO){
         Serial.print("Pot. Agitacao: ");
         Serial.println(VELOCIDADE_AGITADOR_ANTERIOR);
    }if(ATIVA_AGITACAO2){
         Serial.print("Pot. Agitacao 2: ");
         Serial.println(VELOCIDADE_AGITADOR_ANTERIOR2);
    }
    if(ATIVA_TEMPERATURA){
         Serial.print("Temp R1: ");
         Serial.println(TEMPERATURA_R1);
         Serial.print("Temp R2: ");
         Serial.println(TEMPERATURA_R2);
         Serial.print("Temp Jaq: ");
         Serial.println(TEMPERATURA_J);
    }
    if(ATIVA_BOMBA_R1){
         Serial.print("BR1: ");
         Serial.println(PWM_BOMBA_R1);
    }
    if(ATIVA_BOMBA_R2){
         Serial.print("BR2: ");
         Serial.println(PWM_BOMBA_R2);
    }
        if(ATIVA_BOMBA_R3){
         Serial.print("BR2: ");
         Serial.println(PWM_BOMBA_R3);
    }
    if(ATIVA_BOMBA_J){
         Serial.print("BJq: ");
         Serial.println(PWM_BOMBA_J);
    }
    if(ATIVA_RESISTENCIA){
         Serial.print("RESISTENCIA: ");
         if(RESIS == 1){
            Serial.println("LIGADA");
         }else{
          Serial.println("DESLIGADA");
         }
    }
    if(ATIVA_NIVEL){
         Serial.print("VOLUME: ");
         Serial.println(VOLUME);
    }
}
