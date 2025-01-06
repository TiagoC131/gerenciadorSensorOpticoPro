/*
 * gerenciadorComandos.h
 *
 * Descrição: Esta biblioteca fornece uma estrutura para o gerenciamento e
 * processamento de comandos textuais recebidos pela Serial (ou outra interface
 * de comunicação). Ela utiliza uma tabela de despacho (dispatch table) para
 * associar nomes de comandos a funções específicas, permitindo a execução
 * dinâmica de diferentes ações com base nos comandos recebidos.
 *
 * Criação: Esta biblioteca foi criada para modularizar o processamento de
 * comandos, separando-o da lógica principal do sketch. Isso facilita a
 * adição de novos comandos, melhora a organização do código e permite a
 * reutilização do gerenciador em outros projetos.
 *
 * Como Funciona: Snippet de código
 *	flowchart LR
 *  A(lerComandoSerial) --> B {extrairInformacoesComando}
 *  B {extrairInformacoesComando} --> C {buscarComandoNaTabela}
 *  C {Buscar na Tabela} 
 *	 	if C --> (|Encontrado|) {
 *		--> D ((Executar Ação))
 *		} else (|Não Encontrado|) {
 *		--> E ((Comando Desconhecido))
 *		}
 *   
 *
 * Utilização: Para utilizar esta biblioteca, inclua "gerenciadorComandos.h"
 * no seu sketch. Defina as funções de tratamento para cada comando e
 * adicione-as na tabela 'tabelaComandos' no arquivo .cpp correspondente.
 * Os comandos devem ser enviados no formato: "nome_comando valor1 valor2 ...".
 *
 * Dependências:
 *   - Arduino.h (implicitamente, via sensorOpticoPro.h ou no seu sketch principal)
 *   - sensorOpticoPro.h (Biblioteca para o sensor óptico)
 *
 * Autor: Tiago Carvalho Pontes
 * Data: 12/12/2024
 * Versão: 1.0 
 */

//Bibliotecas do Arduino IDE:
#include <Arduino.h> // Inclui a biblioteca principal do Arduino.
#include <string.h> // Inclui a biblioteca para manipulação de strings em C (strcmp).
#include "sensorOpticoPro.h" // Biblioteca Utilizada Para Comunicação com o Sensor Óptico.
#include "gerenciadorComandos.h" // Inclui o cabeçalho desta biblioteca.

// Declaração das variáveis globais (definidas aqui, declaradas com 'extern' no .h)
bool ajustarDistanciaSensor_Ativo = false;            // Flag que indica se o modo de Ajuste do Sensor esta Ativo.
bool lerRPMSensor_Ativo = false;                      // Flag que indica se o modo de Leitura do RPM esta Ativo.

/******************************************************************************
 * Definir Construtor
 ******************************************************************************/

gerenciadorComandos::gerenciadorComandos(uint8_t pinoLigarMotor, uint8_t pinoSentidoGiro) 
	: _pinoLigarMotor(pinoLigarMotor), _pinoSentidoGiro(pinoSentidoGiro)
{
    // Configura os pinos do inversor como saída
    pinMode(_pinoLigarMotor, OUTPUT);
    pinMode(_pinoSentidoGiro, OUTPUT);
}

/******************************************************************************
 * Definir API de Usúario
 ******************************************************************************/

void gerenciadorComandos::iniciar()
{
 	// **Inicialização das variáveis:**
    // Nesta função, as variáveis globais são inicializadas com seus valores padrão.
    // **Importante:** Valores atribuídos a variáveis globais dentro do `setup()` persistem durante toda a execução do programa,
    // a menos que sejam explicitamente modificados em outro ponto do código. 
    // Isso significa que qualquer alteração feita em uma variável global dentro do `loop()` ou em outra função será mantida. 

	// Reset das Variaveis - Os valores Utilizados são Valores Padrões

//  digitalWrite(_pinoLigarMotor, LOW); // Motor Desligado
//  digitalWrite(_pinoSentidoGiro, LOW); // Sentido de Giro
}

// Funções de tratamento dos comandos
void tratarStatus(Comando comando, sensorOpticoPro &sensor) { // Verifica o Status da Conexão Serial

  //As partes comentadas foram removidas pois serão implementadas no sistema web para economizar memoria da placa.
  /*
  if (comando.numValores != 0) {
    Serial.println("Erro: A função 'status' não espera nenhum parâmetro.");
    Serial.print("Número de parâmetros fornecidos: ");
    Serial.println(comando.numValores);
    return; // Saída antecipada da função em caso de erro
  } /* */

	  Serial.println("online"); // Imprime "online" na Serial, indicando que o sistema está funcionando
}

// Funções de tratamento dos comandos
void gerenciadorComandos::tratarLigarMotor(Comando comando, sensorOpticoPro &sensor) { // Liga o Motor

  //As partes comentadas foram removidas pois serão implementadas no sistema web para economizar memoria da placa.
  /*
  if (comando.numValores != 0) {
    Serial.println("Erro: A função 'ligarMotor' não espera nenhum parâmetro.");
    Serial.print("Número de parâmetros fornecidos: ");
    Serial.println(comando.numValores);
    return; // Saída antecipada da função em caso de erro
  } /* */
  
    digitalWrite(_pinoLigarMotor, HIGH);
    Serial.println("Motor Ligado");

}

// Funções de tratamento dos comandos
void gerenciadorComandos::tratarDesligarMotor(Comando comando, sensorOpticoPro &sensor) { //Desliga o Motor

  //As partes comentadas foram removidas pois serão implementadas no sistema web para economizar memoria da placa.
  /*
  if (comando.numValores != 0) {
    Serial.println("Erro: A função 'desligarMotor' não espera nenhum parâmetro.");
    Serial.print("Número de parâmetros fornecidos: ");
    Serial.println(comando.numValores);
    return; // Saída antecipada da função em caso de erro
  } /* */

    digitalWrite(_pinoLigarMotor, LOW);
    Serial.println("Motor Desligado");
}

// Funções de tratamento dos comandos
void gerenciadorComandos::tratarSentidoGiro(Comando comando, sensorOpticoPro &sensor) { // Verifica o Status da Conexão Serial

  //As partes comentadas foram removidas pois serão implementadas no sistema web para economizar memoria da placa.
  /*
  if (comando.numValores != 0) {
    Serial.println("Erro: A função 'sentidoGiro' não espera nenhum parâmetro.");
    Serial.print("Número de parâmetros fornecidos: ");
    Serial.println(comando.numValores);
    return; // Saída antecipada da função em caso de erro
  } /* */

    if(digitalRead(_pinoSentidoGiro) == HIGH){
      digitalWrite(_pinoSentidoGiro, LOW);
      Serial.println("Sentido de giro invertido para Anti-Horario");
    } else {
      digitalWrite(_pinoSentidoGiro, HIGH);
      Serial.println("Sentido de giro invertido para Horario");
    }

} /* */

void tratarConfigurarParametrosSensorOptico(Comando comando, sensorOpticoPro &sensor) { // Configura novo Número de Riscos do Disco e Rpm Solicitado caso seja nescessario.
  
  //As partes comentadas foram removidas pois serão implementadas no sistema web para economizar memoria da placa.
  /*
  if (comando.numValores != 2) {
    Serial.println("Erro: A função 'tratarConfigurarParametrosSensorOptico' espera exatamente dois parâmetro.");
    Serial.print("Número de parâmetros fornecidos: ");
    Serial.println(comando.numValores);
    return; // Saída antecipada da função em caso de erro
  } /* */

  // Converter valores para int 
  int intNumRiscos = comando.valores[0].toInt(); 
  int intRpmMaximo = comando.valores[1].toInt();

  //As partes comentadas foram removidas pois serão implementadas no sistema web para economizar memoria da placa.
  /*
  if (intNumRiscos < 1 || intNumRiscos > 255) {
    Serial.println("Erro: O valor para 'numRiscos' deve estar entre 1 e 255.");
    Serial.print(" Valor fornecido: ");
    Serial.println(intNumRiscos);
    return; // Saída antecipada da função em caso de erro
  }

  if (intRpmMaximo < 1 || intRpmMaximo > 65535) {
    Serial.println("Erro: O valor para 'rpmMaximo' deve estar entre 1 e 65535.");
    Serial.print(" Valor fornecido: ");
    Serial.println(intRpmMaximo);
    return; // Saída antecipada da função em caso de erro
  } /* */
  
  uint8_t numRiscos = (uint8_t)intNumRiscos; // Converter para uint8_t
  uint16_t rpmMaximo = (uint16_t )intRpmMaximo; // Converter para uint16_t

  sensor.configurarParametrosSensorOptico(numRiscos, rpmMaximo); // Chama a função da biblioteca sensorOpticoPro para configurar os parâmetros
  //As partes comentadas foram removidas pois serão implementadas no sistema web para economizar memoria da placa.
  /*
  Serial.print("Parâmetros do sensor óptico configurados:");
  Serial.print(" Número de riscos (pulsos por ciclo): ");
  Serial.print(numRiscos);
  Serial.print(", RPM desejado: ");
  Serial.println(rpmMaximo);
  /* */
}

void tratarRpmMaximo(Comando comando, sensorOpticoPro &sensor) { //Configura novo Rpm caso seja nescessario.

  //As partes comentadas foram removidas pois serão implementadas no sistema web para economizar memoria da placa.
  /*
  if (comando.numValores != 1) {
    Serial.println("Erro: A função 'rpmMaximo' espera exatamente um parâmetro.");
    Serial.print("Número de parâmetros fornecidos: ");
    Serial.println(comando.numValores);
    return; // Saída antecipada da função em caso de erro
  } /* */

  int intRpmMaximo = comando.valores[0].toInt(); // Converte para int primeiro

  //As partes comentadas foram removidas pois serão implementadas no sistema web para economizar memoria da placa.
  /*
  if (intRpmMaximo < 1 || intRpmMaximo > 65535) {
    Serial.println("Erro: O valor para 'rpmMaximo' deve estar entre 1 e 65535.");
    Serial.print(" Valor fornecido: ");
    Serial.println(intRpmMaximo);
    return; // Saída antecipada da função em caso de erro
  } /* */

  uint16_t rpmMaximo = static_cast<uint16_t>(intRpmMaximo); // Conversão mais segura
  sensor.novoRpmMaximo(rpmMaximo);
  //As partes comentadas foram removidas pois serão implementadas no sistema web para economizar memoria da placa.
  /*
  Serial.print("RPM Desejado configurado: ");
  Serial.println(sensor.lerRpmMaximo());
  /* */
}

void tratarNumRiscos(Comando comando, sensorOpticoPro &sensor) { //Configura a nova quantidade de Risco do Disco caso seja nescessario.

  //As partes comentadas foram removidas pois serão implementadas no sistema web para economizar memoria da placa.
  /*
  if (comando.numValores != 1) {
    Serial.println("Erro: A função 'numRiscos' espera exatamente um parâmetro.");
    Serial.print("Número de parâmetros fornecidos: ");
    Serial.println(comando.numValores);
    return; // Saída antecipada da função em caso de erro
  } /* */

  int intNumRiscos = comando.valores[0].toInt();

  //As partes comentadas foram removidas pois serão implementadas no sistema web para economizar memoria da placa.
  /*
  if (intNumRiscos < 1 || intNumRiscos > 255) {
    Serial.println("Erro: O valor para 'numRiscos' deve estar entre 1 e 255.");
    Serial.print(" Valor fornecido: ");
    Serial.println(intNumRiscos);
    return; // Saída antecipada da função em caso de erro
  } /* */

  uint8_t numRiscos = static_cast<uint8_t>(intNumRiscos); // Conversão mais segura
  sensor.novoNumRiscos(numRiscos);
  //As partes comentadas foram removidas pois serão implementadas no sistema web para economizar memoria da placa.
  /*
  Serial.print("Número de Pulsos/Ciclo configurado: ");
  Serial.println(sensor.lerNumRiscos());
  /* */
} 

void tratarFatorAjusteLimiar(Comando comando, sensorOpticoPro &sensor) { // Configura novo fator Limiar caso seja nescessario compensar variações na iluminação ambiente.

  //As partes comentadas foram removidas pois serão implementadas no sistema web para economizar memoria da placa.
  /*
  if (comando.numValores != 1) {
    Serial.println("Erro: A função 'fatorAjusteLimiar' espera exatamente um parâmetro.");
    Serial.print("Número de parâmetros fornecidos: ");
    Serial.println(comando.numValores);
    return; // Saída antecipada da função em caso de erro
  } /* */

  float fatorAjusteLimiar = comando.valores[0].toFloat(); // Converter String para float

  //O valor de ajuste mínimo e maximo do fator limiar deve ser ajustado a cada modelo de sensor.
  const float fatorLimiarMin = 1.0;  // Limite mínimo
  const float fatorLimiarMax = 10.0; // Limite Maximo

  //As partes comentadas foram removidas pois serão implementadas no sistema web para economizar memoria da placa.
  /*
  if (fatorAjusteLimiar < 1.0) {
    Serial.println("Aviso: Fatores de ajuste limiar muito baixos (menores que 1) tornam o sistema extremamente sensível a pequenas variações na leitura do sensor, aumentando o risco de falsos positivos devido a ruído ou pequenas mudanças na iluminação.");
  } else if (fatorAjusteLimiar > 10.0) {
    Serial.println("Aviso: Fatores de ajuste limiar muito altos (maiores que 10) podem tornar o sistema insensível a movimentos reais, exigindo variações muito grandes no sinal do sensor para detectar movimento. Além disso, podem ocorrer problemas de saturação ou overflow dependendo da implementação.");
  } else if (fatorAjusteLimiar > 5.0 && fatorAjusteLimiar <= 10.0){
    Serial.println("Aviso: Fatores de ajuste limiar entre 5 e 10 podem começar a apresentar perda de sensibilidade, teste o sistema para garantir que os movimentos desejados ainda sejam detectados.");
  }

  if (fatorAjusteLimiar < fatorLimiarMin || fatorAjusteLimiar > fatorLimiarMax) {
    Serial.print("Erro: O fator de ajuste limiar deve estar entre ");
    Serial.print(fatorLimiarMin);
    Serial.print(" e ");
    Serial.println(fatorLimiarMax);
    return;
  } /* */

  sensor.novoFatorAjusteLimiar(fatorAjusteLimiar); // Chama a função da biblioteca sensorOpticoPro para configurar o Fator de Ajuste Limiar
  
  //As partes comentadas foram removidas pois serão implementadas no sistema web para economizar memoria da placa.
  /*
  Serial.print("Fator de ajuste limiar definido para: ");
  Serial.println(fatorAjusteLimiar); 
  /* */
}

void tratarNumAmostrasLimiar(Comando comando, sensorOpticoPro &sensor) { // Define novo número de amostras coletadas utilizadas para calcular o limiar ideal.

  //As partes comentadas foram removidas pois serão implementadas no sistema web para economizar memoria da placa.
  /*
  if (comando.numValores != 1) {
    Serial.println("Erro: A função 'numAmostrasLimiar' espera exatamente um parâmetro.");
    Serial.print("Número de parâmetros fornecidos: ");
    Serial.println(comando.numValores);
    return; // Saída antecipada da função em caso de erro
  } /* */

  int intNumAmostrasLimiar = comando.valores[0].toInt(); // Converte para int

  //As partes comentadas foram removidas pois serão implementadas no sistema web para economizar memoria da placa.
  /*
  if (intNumAmostrasLimiar < 1 || intNumAmostrasLimiar > 65535) {
    Serial.println("Erro: O valor para 'numAmostrasDetecMov' deve estar entre 1 e 250.");
    Serial.print(" Valor fornecido: ");
    Serial.println(intNumAmostrasLimiar);
    return; // Saída antecipada da função em caso de erro
  }
 
  if (intNumAmostrasLimiar > 100 && intNumAmostrasLimiar < 251) {
    Serial.println("Aviso: Um valor alto para 'numAmostrasLimiar' (acima de 100) pode aumentar a precisão, mas também pode reduzir o desempenho do sistema.");  
  }

  if (intNumAmostrasLimiar > 250) {
    Serial.println("Aviso: Valores acima de 250 são altamente não recomendados, podendo causar lentidão excessiva.");
    return; // Saída antecipada da função em caso de erro
  } /* */

  uint16_t numAmostrasLimiar = (uint16_t)intNumAmostrasLimiar; // Converter para uint16_t
  sensor.novoNumAmostrasLimiar(numAmostrasLimiar); // Chama a função da biblioteca sensorOpticoPro para configurar o novo Número de Amostras para Calculo do Limiar
  
  //As partes comentadas foram removidas pois serão implementadas no sistema web para economizar memoria da placa.
  /*
  Serial.print("Número de amostras para calcular o Limiar ideal definido para: ");
  Serial.println(numAmostrasLimiar); 
  /* */
}

void tratarNumAmostrasDetecMov(Comando comando, sensorOpticoPro &sensor) { // Define novo número de amostras utilizadas para o Calculo de Detecção de Movimento.

  //As partes comentadas foram removidas pois serão implementadas no sistema web para economizar memoria da placa.
  /*
  if (comando.numValores != 1) {
    Serial.println("Erro: A função 'numAmostrasDetecMov' espera exatamente um parâmetro.");
    Serial.print("Número de parâmetros fornecidos: ");
    Serial.println(comando.numValores);
    return; // Saída antecipada da função em caso de erro
  } /* */

  int intNumAmostrasDetecMov = comando.valores[0].toInt(); // Converte para int

  //As partes comentadas foram removidas pois serão implementadas no sistema web para economizar memoria da placa.
  /*
  if (intNumAmostrasDetecMov < 1 || intNumAmostrasDetecMov > 65535) {
    Serial.println("Erro: O valor para 'numAmostrasDetecMov' deve estar entre 1 e 250.");
    Serial.print(" Valor fornecido: ");
    Serial.println(intNumAmostrasDetecMov);
    return; // Saída antecipada da função em caso de erro
  }
 
  if (intNumAmostrasDetecMov > 100 && intNumAmostrasDetecMov < 251) {
    Serial.println("Aviso: Um valor alto para 'numAmostrasDetecMov' (acima de 100) pode aumentar a precisão, mas também pode reduzir o desempenho do sistema.");  
  }

  if (intNumAmostrasDetecMov > 250) {
    Serial.println("Aviso: Valores acima de 250 são altamente não recomendados, podendo causar lentidão excessiva.");
    return; // Saída antecipada da função em caso de erro
  } /* */

  uint16_t numAmostrasDetecMov = (uint16_t)intNumAmostrasDetecMov; // Converter para uint16_t
  sensor.novoNumAmostrasLimiar(numAmostrasDetecMov); // Chama a função da biblioteca sensorOpticoPro para configurar o novo número de amostras utilizadas para o Calculo de Detecção de Movimento.
  
  //As partes comentadas foram removidas pois serão implementadas no sistema web para economizar memoria da placa.
  /*
  Serial.print("Número de amostras para detecção de movimento definido para: ");
  Serial.println(numAmostrasDetecMov);
  /* */
}

void tratarAjustarDistanciaSensorOptico(Comando comando, sensorOpticoPro &sensor) { // Utilizado para ajustar a distancia do Sensor Óptico.

  //As partes comentadas foram removidas pois serão implementadas no sistema web para economizar memoria da placa.
  /*
  if (comando.numValores != 0) {
    Serial.println("Erro: A função 'ajustarDistanciaSensorOptico' não espera nenhum parâmetro.");
    Serial.print("Número de parâmetros fornecidos: ");
    Serial.println(comando.numValores);
    return; // Saída antecipada da função em caso de erro
  } /* */

  Serial.println(F("Ajuste da distancia entre Sensor Óptico e Disco Decodificador iniciado!"));

  // Ativa a flag `ajustarDistanciaSensorOptico`, indicando que o modo de piscar está em execução.
  ajustarDistanciaSensor_Ativo = true;
} 

void tratarPararAjusteDistanciaSensorOptico(Comando comando, sensorOpticoPro &sensor) { // Utilizado para ajustar a distancia do Sensor Óptico.

  //As partes comentadas foram removidas pois serão implementadas no sistema web para economizar memoria da placa.
  /*
  if (comando.numValores != 0) {
    Serial.println("Erro: A função 'ajustarDistanciaSensorOptico' não espera nenhum parâmetro.");
    Serial.print("Número de parâmetros fornecidos: ");
    Serial.println(comando.numValores);
    return; // Saída antecipada da função em caso de erro
  } /* */

  Serial.println(F("Ajuste da distancia entre Sensor Óptico e Disco Decodificador finalizado!"));

  // Desativa a flag `ajustarDistanciaSensorOptico`, indicando que o modo de piscar está em execução.
  ajustarDistanciaSensor_Ativo = false;
} 

void tratarLerRPM(Comando comando) { // Utilizado para Ler o RPM atual

  //As partes comentadas foram removidas pois serão implementadas no sistema web para economizar memoria da placa.
  /*
  if (comando.numValores != 0) {
    Serial.println("Erro: A função 'lerRPM' não espera nenhum parâmetro.");
    Serial.print("Número de parâmetros fornecidos: ");
    Serial.println(comando.numValores);
    return; // Saída antecipada da função em caso de erro
  } /* */

    Serial.println("Leitura de RPM iniciada!");

  // Ativa a flag `ajustarDistanciaSensorOptico`, indicando que o modo de piscar está em execução.
  lerRPMSensor_Ativo = true;
}  

void tratarPararLeituraRpm(Comando comando, sensorOpticoPro &sensor) { // Utilizado para Ler o RPM atual

  //As partes comentadas foram removidas pois serão implementadas no sistema web para economizar memoria da placa.
  /*
  if (comando.numValores != 0) {
    Serial.println("Erro: A função 'pararLeituraRpm' não espera nenhum parâmetro.");
    Serial.print("Número de parâmetros fornecidos: ");
    Serial.println(comando.numValores);
    return; // Saída antecipada da função em caso de erro
  } /* */

  Serial.println(F("Leitura de RPM finalizada!"));
  // Desativa a flag `ajustarDistanciaSensorOptico`, indicando que o modo de piscar está em execução.
  lerRPMSensor_Ativo = false;
}  

// Funções de tratamento dos comandos
void tratarAjuda(Comando comando) { // Verifica o Status da Conexão Serial

  //As partes comentadas foram removidas pois serão implementadas no sistema web para economizar memoria da placa.
  /*
  if (comando.numValores != 0) {
    Serial.println("Erro: A função 'ajuda' não espera nenhum parâmetro.");
    Serial.print("Número de parâmetros fornecidos: ");
    Serial.println(comando.numValores);
    return; // Saída antecipada da função em caso de erro
  } 

	Serial.println("Lista de Comandos:");
	Serial.println("------------------");
	Serial.println("status: status: Exibe o estado atual do sistema.");
	Serial.println("configurarParametrosSensorOptico: Define os parâmetros do sensor óptico, como número de riscos e RPM desejado.");
	Serial.println("numRiscos: Define a quantidade de pulsos por ciclo do disco decodificador.");
	Serial.println("rpmMaximo: Define a velocidade de rotação (RPM) desejada para o disco decodificador.");
	Serial.println("fatorAjusteLimiar: Define o fator de ajuste para o cálculo do limiar de detecção, compensando variações na iluminação ambiente.");
	Serial.println("numAmostrasLimiar: Define o número de amostras usadas para calcular o limiar ideal.");
	Serial.println("numAmostrasDetecMov: Define o número de amostras usadas para detectar movimento.");
	Serial.println("ajustarDistanciaSensorOptico: Auxilia no ajuste da distância ideal entre o sensor óptico e o disco decodificador.");
	Serial.println("lerRPM: Inicia a leitura e exibe a velocidade de rotação (RPM) do disco decodificador.");
	Serial.println("ajuda: Exibe esta lista de comandos.");
	Serial.println("------------------"); 
  /* */
}

ComandoInfo tabelaComandos[] = { // Tabela de despacho que associa nomes de comandos a funções de tratamento.
  {"status", tratarStatus}, // Associa o comando "status" à função tratarStatus
  {"ligarMotor", tratarLigarMotor}, // Associa o comando "ligarMotor" à função tratarLigarMotor
  {"desligarMotor", tratarDesligarMotor}, // Associa o comando "desligarMotor" à função tratarDesligarMotor
  {"sentidoGiro", tratarSentidoGiro}, // Associa o comando "sentidoGiro" à função tratarSentidoGiro
  {"lerRPM", tratarLerRPM}, // Associa o comando "lerRPM" à função tratarLerRPM
  {"configurarParametrosSensorOptico", tratarConfigurarParametrosSensorOptico}, // Associa o comando "configurarParametrosSensorOptico" à função tratarConfigurarParametrosSensorOptico
  {"rpmMaximo", tratarRpmMaximo}, // Associa o comando "rpmMaximo" à função tratarRpmMaximo
  {"numRiscos", tratarNumRiscos}, // Associa o comando "numRiscos" à função tratarRpmMaximo
  {"fatorAjusteLimiar", tratarFatorAjusteLimiar}, // Associa o comando "fatorAjusteLimiar" à função tratarFatorAjusteLimiar
  {"numAmostrasLimiar", tratarNumAmostrasLimiar}, // Associa o comando "numAmostrasLimiar" à função tratarNumAmostrasLimiar
  {"numAmostrasDetecMov", tratarNumAmostrasDetecMov}, // Associa o comando "numAmostrasDetecMov" à função tratarNumAmostrasDetecMov
  {"ajustarSensor", tratarAjustarDistanciaSensorOptico}, // Associa o comando "ajustarDistanciaSensorOptico" à função tratarAjustarDistanciaSensorOptico
  {"pararAjuste", tratarPararAjusteDistanciaSensorOptico}, // Associa o comando "pararAjuste" à função tratarPararAjusteDistanciaSensorOptico
  {"lerRPM", tratarLerRPM}, // Associa o comando "lerRPM" à função tratarLerRPM
  {"pararLeituraRPM", tratarPararLeituraRpm}, // Associa o comando "pararLeituraRpm" à função tratarPararLeituraRpm
  {"ajuda", tratarAjuda}, // Associa o comando "ajuda" à função tratarAjuda
  {nullptr, nullptr} // Marcador de fim da tabela (obrigatório)
};

Comando gerenciadorComandos::analisarComando(String comandoRecebido) {
  /*
   * Objetivo: Esta função analisa uma string de comando recebida, separando o nome do comando e seus valores numéricos.
   * Parâmetro: comandoRecebido - A string contendo o comando e seus valores. Ex: "piscarLed 10 200 300"
   * Retorno: Um struct Comando contendo o nome do comando e um array de até o maximo de valores numéricos (representados como strings, para posterior conversão).
   */

  Comando comando; // Cria uma variável chamada comando do tipo struct Comando. Essa variável armazenará as informações do comando que será analisado.
  comando.nome = ""; // Limpa o nome do comando, colocando uma string vazia. Isso evita que restos de comandos anteriores causem erros.
  comando.numValores = 0; // Zera o contador de valores. Assim como o nome, isso evita que valores antigos interfiram na análise do novo comando.
  for (int i = 0; i < Comando::maxValores; i++) { // Este loop percorre o array de valores dentro do struct Comando.
    comando.valores[i] = ""; // Inicializa cada posição do array de valores com uma string vazia. Importante para evitar "lixo" na memória.
  }
  comandoRecebido.trim(); // Remove espaços em branco extras no início e no final da string recebida. Por exemplo: "   piscarLed 10 " se torna "piscarLed 10".

  if (comandoRecebido.length() == 0) return comando; // Se a string estiver vazia (só espaços ou nada), retorna o comando vazio. Isso evita erros ao tentar processar uma string sem conteúdo.

  int pos = comandoRecebido.indexOf(' '); // Procura o primeiro espaço em branco na string. Este espaço separa o nome do comando dos seus valores.
                                          // Se não houver espaço, pos será -1.

  if (pos != -1) { // Verifica se encontrou um espaço. Se sim, significa que o comando tem pelo menos um valor.

    comando.nome = comandoRecebido.substring(0, pos); // Extrai o nome do comando. É a parte da string que vem antes do primeiro espaço.
                                                      // Ex: Se comandoRecebido é "piscarLed 10", comando.nome será "piscarLed".
    comandoRecebido = comandoRecebido.substring(pos + 1); // Remove o nome do comando e o espaço da string original, deixando apenas os valores.
                                                          // Ex: Se antes era "piscarLed 10 200", agora será "10 200".
    comandoRecebido.trim(); // Remove espaços extras entre os valores, como múltiplos espaços entre "10" e "200".

    int i = 0; // Cria um contador para controlar quantos valores foram lidos.
    while (i < Comando::maxValores && comandoRecebido.length() > 0) { // Loop que lê os valores. Ele continua enquanto não atingir o limite máximo de valores (Comando::maxValores) e enquanto ainda houver texto na string comandoRecebido.
                                                                    // O limite maximo de valores protege contra erros de acessar posições inválidas na memória (estouro de buffer) em comando.valores.
      pos = comandoRecebido.indexOf(' '); // Procura o próximo espaço em branco.

      if (pos != -1) { // Se encontrou um espaço, significa que ainda há mais valores a serem lidos.
        comando.valores[i] = comandoRecebido.substring(0, pos); // Extrai o valor atual (até o próximo espaço).
        comandoRecebido = comandoRecebido.substring(pos + 1); // Remove o valor lido e o espaço da string.
        comandoRecebido.trim(); // Remove espaços extras após o valor lido.
      } else { // Se não encontrou espaço, significa que este é o último valor.
        comando.valores[i] = comandoRecebido; // Extrai o último valor restante.
        comandoRecebido = ""; // Limpa a string comandoRecebido para encerrar o loop.
      }
      i++; // Incrementa o contador de valores lidos.
    }
    comando.numValores = i; // Armazena a quantidade total de valores numéricos que foram lidos.
  } else { // Se não encontrou nenhum espaço na string original, significa que o comando não tem valores.

    comando.nome = comandoRecebido; // O nome do comando é a string inteira. Ex: "piscarLed".
  }
  return comando; // Retorna o struct Comando preenchido com o nome do comando e seus valores.
}

void gerenciadorComandos::processarComando(Comando comando, sensorOpticoPro &sensor) {
  // Esta função recebe um struct Comando (que contém o nome do comando e seus valores) e procura na tabela de comandos a função que deve ser executada para esse comando.

  for (int i = 0; tabelaComandos[i].nome != nullptr; i++) {  // Loop que percorre a tabela de comandos 'tabelaComandos'.
    // A tabela 'tabelaComandos' é uma lista de pares: nome do comando e a função que o executa.
    // O loop continua enquanto não chegar ao final da tabela, que é marcado por um 'nullptr' no campo 'nome'.
    // 'i' é o índice que indica a posição atual na tabela.

    if (comando.nome == tabelaComandos[i].nome) { // Verifica se o nome do comando que foi recebido ('comando.nome') é igual ao nome de um comando que está na tabela ('tabelaComandos[i].nome').
      // Esta é a parte principal da função: encontrar o comando correto na tabela.

      tabelaComandos[i].funcao(comando, sensor); // Se encontrou o comando na tabela, esta linha chama a função correspondente para executar o comando.
      // 'tabelaComandos[i].funcao' é um "ponteiro para função". Isso significa que ele armazena o endereço da função que deve ser executada.
      // O 'comando' é passado como argumento para a função de tratamento, para que a função tenha acesso aos valores que foram enviados junto com o comando.

      return; // Após executar a função do comando, a função 'processarComando' termina. Isso evita que o loop continue procurando, o que seria desnecessário.
    }
  }
    // Se o loop terminar sem encontrar o comando:
  Serial.print("O comando '"); // Imprime uma mensagem indicando que o comando é inválido.
  Serial.println(comando.nome);       // Imprime o nome do comando que foi digitado incorretamente.
  Serial.println("' não existe. Digite 'ajuda' para listar os comandos disponíveis.");
}
