#include <SoftwareSerial.h> //Biblioteca Utilizada Para Comunicação Serial
#include "sensorOpticoPro.h" //Biblioteca Utilizada Para Comunicação com o Sensor Óptico
#include "gerenciadorComandos.h" // Biblioteca Utilizada Para Gerenciar Comandos

// Variaveis Globais
  const int MAX_BUFFER_SIZE = 90; // Tamanho máximo do buffer
  char comandoRecebidoBuffer[MAX_BUFFER_SIZE];
  int comandoRecebidoIndex = 0;
  String comandoRecebido; //Armazena o comando Recebido
  float anguloAtual = 0.0; //Para acessar o valor de anguloAtual a qualquer momento para saber o ângulo atual da sua peça

const int ledPin = 13; // Pino do LED interno
const int sensorOpticoPin = 2; // Terminal do Sinal do Sensor Óptico

// Pinos de Controle do Inversor
const int ligaDesligaPin = 3;
const int sentidoGiroPin = 4;
const int retardaPin = 5;
const int avancaPin = 6;

//Intanciar Classes
sensorOpticoPro sensorOptico(sensorOpticoPin); // Assumindo os pinos de comunicação do Sensor Optico
gerenciadorComandos gerenciador (ligaDesligaPin, sentidoGiroPin); // Assumindo os pinos de comunicação do Motor

void setup() {  
  //Comunicação Serial com o Sistema
  Serial.begin(1000000); // Porta Serial (Servidor Node)
    pinMode(ledPin, OUTPUT); // LED interno
  
    // Configura os pinos do inversor como saída
    pinMode(retardaPin, OUTPUT);
    pinMode(avancaPin, OUTPUT);

  //Comunicação Direta com o Motor
  gerenciador.iniciar(); // Inicializar a comunicação com o Sensor Óptico 

  //Comunicação Direta com o Sensor Óptico
  sensorOptico.iniciar(); // Inicializar a comunicação com o Sensor Óptico 
}

void loop() {
  while (Serial.available() > 0) {
    char c = Serial.read();

    if (c != '\n' && c != '\r') { // Ignora \r e \n durante a leitura
      comandoRecebidoBuffer[comandoRecebidoIndex] = c;
      comandoRecebidoIndex++;

      if (comandoRecebidoIndex >= MAX_BUFFER_SIZE - 1) { // Previne estouro de buffer
        comandoRecebidoIndex = MAX_BUFFER_SIZE - 2; // Mantem espaço para o \0
        break; // Sai do loop de leitura se o buffer estiver cheio
      }
    } else if (comandoRecebidoIndex > 0) { // Processa o comando se buffer NÃO estiver vazio
      comandoRecebidoBuffer[comandoRecebidoIndex] = '\0'; // Finaliza a string no buffer
      String comandoRecebido = String(comandoRecebidoBuffer);
      comandoRecebido.trim(); // Remove espaços extras

      //Serial.print("Enviando Comando: ");
      //Serial.println(comandoRecebido);

      Comando comando = gerenciador.analisarComando(comandoRecebido);

      //Serial.print("Comando: ");
      //Serial.println(comando.nome);
      //Serial.print("Número de valores: ");
      //Serial.println(comando.numValores);
      //Serial.print("Primeiro Valor: ");
      //Serial.println(comando.valores[0]);
      //Serial.print("Segundo Valor: ");
      //Serial.println(comando.valores[1]);

      if (comando.nome.length() > 0) {
          bool comandoEncontrado = false;
          for (int i = 0; tabelaComandos[i].nome != nullptr; i++) {
              if (comando.nome.equals(tabelaComandos[i].nome)) {
                  tabelaComandos[i].funcao(comando, sensorOptico);
                  comandoEncontrado = true;
                  break;
              }
          }
      } else {
          Serial.println("Comando inválido ou vazio.");
      }

      comandoRecebidoIndex = 0; // Reseta o índice *APÓS* processar o comando
    } else {
       comandoRecebidoIndex = 0; // Reseta o índice caso receba apenas quebras de linha
    }
  }


    // Controle do Inversor via Teclado (NOVAS FUNÇÕES)
    if (Serial.available() > 0) {
        char tecla = Serial.read();

        if (tecla == '5') { // F5 (Avança)
            digitalWrite(avancaPin, HIGH);
        } else {
            digitalWrite(avancaPin, LOW);
        }

        if (tecla == '6') { // F6 (Retarda)
            digitalWrite(retardaPin, HIGH);
        } else {
            digitalWrite(retardaPin, LOW);
        }
    }

  // Esta parte controla as Funções do Sensor Óptico e deve permanecer dentro do loop(), pois precisa ser executada repetidamente para funcionar.
  // Ela não usa diretamente a tabelaComandos, mas depende das variáveis globais do Gerenciador de Comandos.
  
  // Controla o ajuste da distancia entre o sensor óptico e o disco decodificador.
  if (ajustarDistanciaSensor_Ativo) { // Verifica se o Ajuste do Sensor Óptico está ativo
    sensorOptico.ajustarDistanciaSensorOptico(); // Chama a função da biblioteca sensorOpticoPro para ajustar distancia do Sensor
  }

  // Controla a leitura do RPM.
  if (lerRPMSensor_Ativo) { // Verifica se a leitura do RPM do Sensor Óptico está ativo
    //sensorOptico.iniciarSensorOptico(); // Chama a função da biblioteca sensorOpticoPro para ler o RPM
    sensorOptico.calcularRPM(); // Chama a função da biblioteca sensorOpticoPro para ler o RPM
  }

}
