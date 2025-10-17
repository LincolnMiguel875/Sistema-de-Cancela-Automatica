#include <Servo.h>

// --- Servo---
Servo cancelaEntrada;
Servo cancelaSaida;

// --- Pinos sensores ultrassônicos ---
int trigEntrada = 2;
int echoEntrada = 3;
int trigSaida = 7;
int echoSaida = 8;

// --- LED e Buzzer piezo ---
int ledAlerta = 10;
int buzzer = 11;

// --- Variáveis de tempo ---
unsigned long tempoCancelaEntrada = 0;
unsigned long tempoCancelaSaida = 0;
int estadoCancelaEntrada = 0; // 0 = fechada, 1 = aberta
int estadoCancelaSaida = 0;

unsigned long tempoAlerta = 0;
bool estadoAlerta = false;      // controle para LED + buzzer

// --- Contagem de vagas ---
int vagasDisponiveis = 10;

// --- Função para medir distância ---
long medirDistancia(int trigPin, int echoPin){
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duracao = pulseIn(echoPin, HIGH);
  long distancia = duracao / 58; // Convertendo para cm
  return distancia;
}

void setup() {
  Serial.begin(9600);

  // --- Configuração dos pinos ---
  pinMode(trigEntrada, OUTPUT);
  pinMode(echoEntrada, INPUT);
  pinMode(trigSaida, OUTPUT);
  pinMode(echoSaida, INPUT);

  pinMode(ledAlerta, OUTPUT);
  pinMode(buzzer, OUTPUT);

  // --- Servos ---
  cancelaEntrada.attach(9);
  cancelaSaida.attach(6);
}

void loop() {
  unsigned long agora = millis();

  // --- Leitura de distância ---
  long distanciaEntrada = medirDistancia(trigEntrada, echoEntrada);
  long distanciaSaida = medirDistancia(trigSaida, echoSaida);

  // --- Cancela de entrada ---
  if(distanciaEntrada < 50 && estadoCancelaEntrada == 0){
    cancelaEntrada.write(90);            // abre a cancela
    estadoCancelaEntrada = 1;
    tempoCancelaEntrada = agora;
    vagasDisponiveis--;
    Serial.println("Carro entrou, Cancela aberta.");
  }

  if(estadoCancelaEntrada == 1 && (agora - tempoCancelaEntrada >= 3000)){
    cancelaEntrada.write(0);             // fecha a cancela
    estadoCancelaEntrada = 0;
    Serial.println("Cancela de entrada fechada.");
  }

  // --- Cancela de saída ---
  if(distanciaSaida < 50 && estadoCancelaSaida == 0){
    cancelaSaida.write(90);              // abre a cancela
    estadoCancelaSaida = 1;
    tempoCancelaSaida = agora;
    vagasDisponiveis++;
    Serial.println("Carro saiu, Cancela aberta.");
  }

  if(estadoCancelaSaida == 1 && (agora - tempoCancelaSaida >= 3000)){
    cancelaSaida.write(0);               // fecha a cancela
    estadoCancelaSaida = 0;
    Serial.println("Cancela de saída fechada.");
  }

  // --- LED + Buzzer piezo ---
  int intervalo = 200;                   // piscada padrão
  if(distanciaEntrada < 10) intervalo = 50; // colisão iminente

  if(agora - tempoAlerta >= intervalo){
    tempoAlerta = agora;
    estadoAlerta = !estadoAlerta;         // alterna LED + buzzer
    digitalWrite(ledAlerta, estadoAlerta);
    if(estadoAlerta){
      tone(buzzer, 1000);                 // liga buzzer
    } else {
      noTone(buzzer);                     // desliga buzzer
    }
  }

  // --- Serial Monitor ---
  Serial.print("Dist Entrada: "); Serial.print(distanciaEntrada); Serial.print(" cm | ");
  Serial.print("Dist Saída: "); Serial.print(distanciaSaida); Serial.print(" cm | ");
  Serial.print("Vagas Disponíveis: "); Serial.println(vagasDisponiveis);

  delay(1000); // delay para não poluir o Serial Monitor
}
