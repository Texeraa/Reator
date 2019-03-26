    
/*  Reator: Programa feito para o controle de um reator do tipo
 *  batelada. O programa faz a supervisão de alguns sensores e
 *  envia sinais de controle para alguns motores e uma resistência.
 *
 *  Autor: Julian Jose de Brito
 *
 *  Versão 1.4 01/11/2018: -Exclusão das linas de codigo relacionadas ao sensor ultrassônico
 *                         -Implementação da função filtro
 *  Versão 1.5 26/11/2018: -Mudanças para deixar o código mais legível
 *                         -Conserto em bugs na função filtro
 */

//#######################################################################################################################
// CARREGAMENTO DE BIBLIOTECAS

//#######################################################################################################################
// DEFINIÇÕES DOS PINOS

#define velocidade_bomba1 6 // Define pinos para o pwm da bomba 1.

//#######################################################################################################################
// CONFIGURAÇÕES INICIAIS

// Define uma instancia do oneWire para comunicacao com o sensor de temperatura

//#######################################################################################################################
// PAINEL DE CONTROLE

boolean CH_LIGA_BOMBA1 = 1;

//#######################################################################################################################
// VARIÁVEIS DO SISTEMA

float TEMPERATURA, NIVEL, CONDUTIVIDADE;
int VELOCIDADE_AGITADOR = 100, TEMPERATURA_RESISTENCIA = 50, PWM_BOMBA1 = 0;
volatile double waterFlow, aux;

void setup()
{
   // Inicializa a comunicação serial
   Serial.begin(9600);

   // Seta os pinos como saida ou entrada
   pinMode(velocidade_bomba1, OUTPUT);
   
  waterFlow = 0;
  attachInterrupt(digitalPinToInterrupt(2), pulse, RISING);  //DIGITAL Pin 2: Interrupt 0
}

void loop()
{
  //verifica se algo foi digitado no canal serial
  le_informacao();

  if(CH_LIGA_BOMBA1)
    liga_motor();
  exibe_dados();
}

void le_informacao()
{
  // Lê as informações digitadas na interface serial e atualiza as variáveis do sistema
  while (Serial.available() > 0)
  {
    // Lê primeiro byte digitado pelo usuário
      switch (Serial.read())
      {
        case 'B':
                    PWM_BOMBA1 = Serial.parseInt();
                    break;
      }
   }
}

void liga_motor()
{
  analogWrite(velocidade_bomba1, PWM_BOMBA1);
}

void exibe_dados(){

if(CH_LIGA_BOMBA1)
  {
    Serial.print(" PWM Bomba 1: ");
    Serial.print(PWM_BOMBA1);
    Serial.print(" Vazão: ");
    Serial.println(waterFlow);
    delay(500);
    if(waterFlow >= 1.5){
      PWM_BOMBA1 = 0;
    }
  }

}

void pulse()   //measure the quantity of square wave
{
  waterFlow += 1.0 / 5880.0;
}

