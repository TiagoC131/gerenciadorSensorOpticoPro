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
#include <Arduino.h> // Inclui a biblioteca principal do Arduino
#include <string.h> // Inclui a biblioteca para manipulação de strings em C (strcmp)
#include "sensorOpticoPro.h" //Biblioteca Utilizada Para Comunicação com o Sensor Óptico
#include "gerenciadorComandos.h" // Inclui o cabeçalho desta biblioteca

// Funções de tratamento dos comandos
void tratarStatus(Comando comando, sensorOpticoPro &sensor) { // Verifica o Status da Conexão Serial
	Serial.println("online\n"); // Imprime "online" na Serial, indicando que o sistema está funcionando

	//O código comentado abaixo é para piscar um LED.

	// Pisca 5 vezes após enviar a resposta 
	//for (int i = 0; i < 5; i++) {
	//  digitalWrite(ledPin, HIGH); // Liga o LED
	//  delay(200);
	//  digitalWrite(ledPin, LOW);  // Desliga o LED
	//  delay(200);
	//}
}

void tratarConfigurarParametrosSensorOptico(Comando comando, sensorOpticoPro &sensor) { // Configura novo Número de Riscos do Disco e Rpm Solicitado caso seja nescessario.
  if (comando.numValores == 2) { // Verifica se foram recebidos 2 valores junto ao comando
    uint8_t numRiscos = comando.valores[0]; // Obtém o número de riscos, primeiro valor recebido
    uint16_t rpm = comando.valores[1]; // Obtém o valor do RPM Desejado, segundo valor recebido
    sensor.configurarParametrosSensorOptico(numRiscos, rpm); // Chama a função da biblioteca sensorOpticoPro para configurar os parâmetros
    //Serial.print("Pulsos por Ciclo: "); Serial.println(numRiscos); // Imprime na Serial o numero de riscos configurado
  	//Serial.print("RPM: "); Serial.println(rpm); // Imprime na Serial o valor do RPM configurado
  } else {
    //Serial.println("Número incorreto de parâmetros para configurarParametrosSensorOptico"); // Mensagem de erro caso o número de parâmetros seja diferente de 2
  }
}

void tratarRpmDesejado(Comando comando, sensorOpticoPro &sensor) { //Configura novo Rpm caso seja nescessario.
  if (comando.numValores == 1) { // Verifica se foram recebidos 1 valor junto ao comando
    uint16_t rpmDesejado = comando.valores[0]; // Obtém o valor do RPM Desejado do valor recebido
    sensor.novoRpmDesejado(rpmDesejado); // Chama a função da biblioteca sensorOpticoPro para configurar o RPM Desejado
    //Serial.print("RPM Desejado: "); Serial.println(rpmDesejado); // Imprime na Serial o RPM Desejado
  } else {
    //Serial.println("Número incorreto de parâmetro para RPM desejado"); // Mensagem de erro caso o número de parâmetros seja diferente de 1
  }
}

void tratarNumRiscos(Comando comando, sensorOpticoPro &sensor) { //Configura a nova quantidade de Risco do Disco caso seja nescessario.
  if (comando.numValores == 1) { // Verifica se foram recebidos 1 valor junto ao comando
    uint8_t numRiscos = comando.valores[0]; // Obtém o Número de Riscos do valor recebido
    sensor.novoNumRiscos(numRiscos); // Chama a função da biblioteca sensorOpticoPro para configurar o Numero de Pulsos por Ciclo
    Serial.print("Pulsos por Ciclo: "); Serial.println(numRiscos); // Imprime na Serial o numero de Pulsos por Ciclo
  } else {
    //Serial.println("Número incorreto de parâmetro para Número de Pulsos por Ciclo"); // Mensagem de erro caso o número de parâmetros seja diferente de 1
  }
}

void tratarFatorAjusteLimiar(Comando comando, sensorOpticoPro &sensor) { // Configura novo fator Limiar caso seja nescessario compensar variações na iluminação ambiente.
  if (comando.numValores == 1) { // Verifica se foram recebidos 1 valor junto ao comando
    float fatorAjusteLimiar = comando.valores[0]; // Obtém o valor do Fator de Ajuste Limiar do valor recebido
    sensor.novoFatorAjusteLimiar(fatorAjusteLimiar); // Chama a função da biblioteca sensorOpticoPro para configurar o Fator de Ajuste Limiar
    //Serial.print("Fator de Ajuste Limiar: "); Serial.println(fatorAjusteLimiar); // Imprime na Serial o Fator de Ajuste Limiar
  } else {
    //Serial.println("Número incorreto de parâmetro para Fator de Ajuste Limiar"); // Mensagem de erro caso o número de parâmetros seja diferente de 1
  }
}

void tratarNumAmostrasLimiar(Comando comando, sensorOpticoPro &sensor) { // Define novo número de amostras coletadas utilizadas para calcular o limiar ideal.
  if (comando.numValores == 1) { // Verifica se foram recebidos 1 valor junto ao comando
    uint16_t numAmostrasLimiar = comando.valores[0]; // Obtém o Número de Amostras que sera utilizado para calcular o Limiar do valor recebido
    sensor.novoNumAmostrasLimiar(numAmostrasLimiar); // Chama a função da biblioteca sensorOpticoPro para configurar o Número de Amostra do Fator Limiar
    //Serial.print("Número Amostras Limiar: "); Serial.println(numAmostrasLimiar); // Imprime na Serial o numero de amostras para o calculo do Limiar
  } else {
    //Serial.println("Número de Amostras inválido para calcular o limiar ideal."); // Mensagem de erro caso o número de parâmetros seja diferente de 1
  }
}

void tratarNumAmostrasDetecMov(Comando comando, sensorOpticoPro &sensor) { // Define novo número de amostras utilizadas para o Calculo de Detecção de Movimento.
  if (comando.numValores == 1) { // Verifica se foram recebidos 1 valor junto ao comando
    uint16_t numAmostrasDetecMov = comando.valores[0]; // Obtém o Número de Amostras utilizadas para detecção de Movimento do valor recebido
    sensor.novoNumAmostrasDetecMov(numAmostrasDetecMov); // Chama a função da biblioteca sensorOpticoPro para configurar o Número de Amostras para Detectar Movimento
    //Serial.print("Número Amostras Detecção de Movimento: "); Serial.println(numAmostrasDetecMov); // Imprime na Serial o numero de amostras para detecção de movimento
  } else {
    //Serial.println("Número incorreto de parâmetros para Numero Amostras Detecção de Movimento"); // Mensagem de erro caso o número de parâmetros seja diferente de 1
  }
}

void tratarAjustarDistanciaSensorOptico(Comando comando, sensorOpticoPro &sensor) { // Utilizado para ajustar a distancia do Sensor Óptico.
  sensor.ajustarDistanciaSensorOptico(); // Chama a função da biblioteca sensorOpticoPro para ajustar distancia do Sensor
    //Serial.println("Ajustando distancia do sensor"); // Informa na Serial a distancia entre sensor e disco para que seja feito um ajuste preciso
}

void tratarLerRPM(Comando comando, sensorOpticoPro &sensor) { // Utilizado para Ler o RPM atual
  sensor.iniciarSensorOptico(); // Chama a função da biblioteca sensorOpticoPro para Ler RPM
    //Serial.println("Lendo RPM"); // Imprime na Serial o RPM atual
}

ComandoInfo tabelaComandos[] = { // Tabela de despacho que associa nomes de comandos a funções de tratamento.
    {"status", tratarStatus}, // Associa o comando "status" à função tratarStatus
    {"configurarParametrosSensorOptico", tratarConfigurarParametrosSensorOptico}, // Associa o comando "configurarParametrosSensorOptico" à função tratarConfigurarParametrosSensorOptico
    {"rpmDesejado", tratarRpmDesejado}, // Associa o comando "rpmDesejado" à função tratarRpmDesejado
    {"numRiscos", tratarNumRiscos}, // Associa o comando "numRiscos" à função tratarRpmDesejado
    {"fatorAjusteLimiar", tratarFatorAjusteLimiar}, // Associa o comando "fatorAjusteLimiar" à função tratarFatorAjusteLimiar
    {"numAmostrasLimiar", tratarNumAmostrasLimiar}, // Associa o comando "numAmostrasLimiar" à função tratarNumAmostrasLimiar
    {"numAmostrasDetecMov", tratarNumAmostrasDetecMov}, // Associa o comando "numAmostrasDetecMov" à função tratarNumAmostrasDetecMov
    {"ajustarDistanciaSensorOptico", tratarAjustarDistanciaSensorOptico}, // Associa o comando "ajustarDistanciaSensorOptico" à função tratarAjustarDistanciaSensorOptico
    {"lerRPM", tratarLerRPM}, // Associa o comando "lerRPM" à função tratarLerRPM
    {nullptr, nullptr} // Marcador de fim da tabela (obrigatório)
};

Comando gerenciadorComando::analisarComando(String comandoRecebido) {
	/*
  * Objetivo: a Função analisa uma string de comando recebida, separando o nome do comando e seus valores numéricos.
  * Parâmetro: comandoRecebido - A string contendo o comando e seus valores. Ex: "MOTOR 10.5 20 30"
  * Retorno: Um struct Comando contendo o nome do comando e um array de até 5 valores numéricos.
  */

    /* Apenas para Depuração... */ Serial.print("Analizando o Comando: ");
    /* Apenas para Depuração... */ Serial.println(comandoRecebido);

    Comando comando; // Cria uma variável do tipo Comando (struct/classe Comando definido)
    comando.nome = ""; // Reseta o nome do comando para evitar lixo de memória
    comando.numValores = 0; // Reseta o número de valores para evitar lixo de memória
    for (int i = 0; i < gerenciadorComandosInternos::maxValores; i++) {  // Reseta os valores
        comando.valores[i] = 0.0f; // Inicializa com ponto flutuante explicitamente
    }
    comandoRecebido.trim(); // Remove espaços em branco extras no início e no final da string recebida. Ex: "  MOTOR 10 " vira "MOTOR 10"

    /* Apenas para Depuração... */ Serial.print("Comando Analizado: ");
    /* Apenas para Depuração... */ Serial.println(comandoRecebido);

    int pos = comandoRecebido.indexOf(' ');// Encontra a posição do primeiro espaço em branco na string.
                                          // Se não houver espaço, pos será -1.

    if (pos != -1) { // Se encontrou um espaço (ou seja, o comando tem pelo menos um valor)

    /* Apenas para Depuração... */ Serial.println("Foi encontrado um Espaço no Comando");

        comando.nome = comandoRecebido.substring(0, pos); // Extrai o nome do comando, que é a parte da string antes do primeiro espaço.
                                                          // Ex: Se comandoRecebido é "MOTOR 10", comando.nome será "MOTOR".
        comandoRecebido = comandoRecebido.substring(pos + 1); // Atualiza a string comandoRecebido para conter apenas os valores,
                                                              // removendo o nome do comando e o espaço.
                                                              // Ex: Se antes era "MOTOR 10 20", agora será "10 20".
        comandoRecebido.trim(); // Remove espaços extras entre os valores

        int i = 0; // Inicializa um contador para controlar quantos valores foram lidos.
        while (i < gerenciadorComandosInternos::maxValores && comandoRecebido.length() > 0) { // Loop que lê até 5 valores ou até que a string comandoRecebido fique vazia.
                                                        // O limite de 5 valores protege contra estouro de buffer em comando.valores.
            pos = comandoRecebido.indexOf(' '); // Encontra a posição do próximo espaço em branco.
            String valorStr;

            if (pos != -1) { // Se encontrou um espaço (ainda tem mais valores)
                valorStr = comandoRecebido.substring(0, pos);
                //comando.valores[i] = comandoRecebido.substring(0, pos).toFloat();// Converte a parte da string antes do espaço para um número de ponto flutuante (float)
                                                                                 // e armazena no array comando.valores na posição i.
                                                                                 // Ex: Se for "10 20", o "10" será convertido para 10.0 e armazenado.
                comandoRecebido = comandoRecebido.substring(pos + 1); // Atualiza comandoRecebido, removendo o valor que já foi lido e o espaço.
                                                                      // Ex: Se antes era "10 20", agora será "20".
                comandoRecebido.trim(); // Remove espaços extras entre os valores
            } else { // Se não encontrou espaço (é o último valor ou só tem um valor)
                valorStr = comandoRecebido;
                //comando.valores[i] = comandoRecebido.parseFloat(); // Converte o restante da string para float e armazena.
                                                                // Ex: Se for "20", o "20" será convertido para 20.0
                comandoRecebido = ""; // Esvazia a string comandoRecebido, finalizando o loop.
            }

            comando.valores[i] = valorStr.toFloat(); // Converte a string completa para float
            
            i++; // Incrementa o contador de valores lidos.
        }
        comando.numValores = i; // Guarda a quantidade total de valores numéricos que foram lidos e armazenados.
    } else { // Se não encontrou nenhum espaço na string original (o comando não tem valores)
    
    /* Apenas para Depuração... */ Serial.println("Não foi encontrado nenhum Espaço no Comando");

        comando.nome = comandoRecebido; // O comando é a string inteira.
        //comando.numValores = 0; // Não há valores associados ao comando.
    }

    return comando; // Retorna o struct Comando preenchido.
}

void gerenciadorComando::processarComando(Comando comando, sensorOpticoPro &sensor) {
  for (int i = 0; tabelaComandos[i].nome != nullptr; i++) {  // Loop que percorre a tabela de comandos 'tabelaComandos'. O loop continua enquanto o campo 'nome' da entrada atual da tabela não for nulo (nullptr), que indica o fim da tabela.
    if (comando.nome.equals(tabelaComandos[i].nome) == 0) { // Compara o nome do comando extraído com o nome do comando na tabela.
      tabelaComandos[i].funcao(comando, sensor); // Se o comando for encontrado na tabela, chama a função de tratamento correspondente. 'tabelaComandos[i].funcao' é um ponteiro para a função. Os argumentos ‘comando’ e ‘sensor’ são repassados para a função de tratamento.
      return; // Sai da função 'processarComando' após executar o comando.
    }
  }
}