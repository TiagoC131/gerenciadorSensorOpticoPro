#include <SoftwareSerial.h> //Biblioteca Utilizada Para Comunicação Serial
#include "sensorOpticoPro.h" //Biblioteca Utilizada Para Comunicação com o Sensor Óptico
#include "gerenciadorComandos.h" // Biblioteca Utilizada Para Gerenciar Comandos

// Variaveis Globais
  const int MAX_BUFFER_SIZE = 64; // Tamanho máximo do buffer
  char comandoRecebidoBuffer[MAX_BUFFER_SIZE];
  int comandoRecebidoIndex = 0;
  String comandoRecebido; //Armazena o comando Recebido
  float anguloAtual = 0.0; //Para acessar o valor de anguloAtual a qualquer momento para saber o ângulo atual da sua peça

const int ledPin = 13; // Pino do LED interno
const int sensorOpticoPin = 2; // Terminal do Sinal do Sensor Óptico

//Intanciar Classes
sensorOpticoPro sensorOptico(sensorOpticoPin); // Assumindo que os pinos de comunicação do Sensor Optico
gerenciadorComando gerenciador; // <--- CRIA O OBJETO 'gerenciador'

void setup() {  
  //Comunicação Serial com o Sistema
  Serial.begin(9600); // Porta Serial (Servidor Node)
    pinMode(ledPin, OUTPUT); // LED interno
  
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

      Serial.print("Enviando Comando: ");
      Serial.println(comandoRecebido);

      Comando comando = gerenciador.analisarComando(comandoRecebido);

      Serial.print("Comando: ");
      Serial.println(comando.nome);
      Serial.print("Número de valores: ");
      Serial.println(comando.numValores);

      if (comando.nome.length() > 0) {
          bool comandoEncontrado = false;
          for (int i = 0; tabelaComandos[i].nome != nullptr; i++) {
              if (comando.nome.equals(tabelaComandos[i].nome)) {
                  tabelaComandos[i].funcao(comando, sensorOptico);
                  comandoEncontrado = true;
                  break;
              }
          }
          if(!comandoEncontrado){
              Serial.println("Comando não encontrado na tabela de comandos");
          }
      } else {
          Serial.println("Comando inválido ou vazio.");
      }

      comandoRecebidoIndex = 0; // Reseta o índice *APÓS* processar o comando
    } else {
       comandoRecebidoIndex = 0; // Reseta o índice caso receba apenas quebras de linha
  
      // Aqui você pode colocar o código para leitura continua do sensor se necessário
      // anguloAtual += sensorOptico.getDeltaAngulo(); // Um exemplo de uso
      //sensorOptico.calcularRPM();
    }
  }
}