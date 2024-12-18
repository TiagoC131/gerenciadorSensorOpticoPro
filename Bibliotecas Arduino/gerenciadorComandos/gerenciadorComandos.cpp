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
  int intRpmDesejado = comando.valores[1].toInt();

  //As partes comentadas foram removidas pois serão implementadas no sistema web para economizar memoria da placa.
  /*
  if (intNumRiscos < 1 || intNumRiscos > 255) {
    Serial.println("Erro: O valor para 'numRiscos' deve estar entre 1 e 255.");
    Serial.print(" Valor fornecido: ");
    Serial.println(intNumRiscos);
    return; // Saída antecipada da função em caso de erro
  }

  if (intRpmDesejado < 1 || intRpmDesejado > 65535) {
    Serial.println("Erro: O valor para 'rpmDesejado' deve estar entre 1 e 65535.");
    Serial.print(" Valor fornecido: ");
    Serial.println(intRpmDesejado);
    return; // Saída antecipada da função em caso de erro
  } /* */
  
  uint8_t numRiscos = (uint8_t)intNumRiscos; // Converter para uint8_t
  uint16_t rpmDesejado = (uint16_t )intRpmDesejado; // Converter para uint16_t

  sensor.configurarParametrosSensorOptico(numRiscos, rpmDesejado); // Chama a função da biblioteca sensorOpticoPro para configurar os parâmetros
  //As partes comentadas foram removidas pois serão implementadas no sistema web para economizar memoria da placa.
  /*
  Serial.print("Parâmetros do sensor óptico configurados:");
  Serial.print(" Número de riscos (pulsos por ciclo): ");
  Serial.print(numRiscos);
  Serial.print(", RPM desejado: ");
  Serial.println(rpmDesejado);
  /* */
}

void tratarRpmDesejado(Comando comando, sensorOpticoPro &sensor) { //Configura novo Rpm caso seja nescessario.

  //As partes comentadas foram removidas pois serão implementadas no sistema web para economizar memoria da placa.
  /*
  if (comando.numValores != 1) {
    Serial.println("Erro: A função 'rpmDesejado' espera exatamente um parâmetro.");
    Serial.print("Número de parâmetros fornecidos: ");
    Serial.println(comando.numValores);
    return; // Saída antecipada da função em caso de erro
  } /* */

  int intRpmDesejado = comando.valores[0].toInt(); // Converte para int primeiro

  //As partes comentadas foram removidas pois serão implementadas no sistema web para economizar memoria da placa.
  /*
  if (intRpmDesejado < 1 || intRpmDesejado > 65535) {
    Serial.println("Erro: O valor para 'rpmDesejado' deve estar entre 1 e 65535.");
    Serial.print(" Valor fornecido: ");
    Serial.println(intRpmDesejado);
    return; // Saída antecipada da função em caso de erro
  } /* */

  uint16_t rpmDesejado = static_cast<uint16_t>(intRpmDesejado); // Conversão mais segura
  sensor.novoRpmDesejado(rpmDesejado);
  //As partes comentadas foram removidas pois serão implementadas no sistema web para economizar memoria da placa.
  /*
  Serial.print("RPM Desejado configurado: ");
  Serial.println(sensor.lerRpmDesejado());
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

  sensor.ajustarDistanciaSensorOptico(); // Chama a função da biblioteca sensorOpticoPro para ajustar distancia do Sensor
} 

void tratarLerRPM(Comando comando, sensorOpticoPro &sensor) { // Utilizado para Ler o RPM atual

  //As partes comentadas foram removidas pois serão implementadas no sistema web para economizar memoria da placa.
  /*
  if (comando.numValores != 0) {
    Serial.println("Erro: A função 'lerRPM' não espera nenhum parâmetro.");
    Serial.print("Número de parâmetros fornecidos: ");
    Serial.println(comando.numValores);
    return; // Saída antecipada da função em caso de erro
  } /* */

  sensor.iniciarSensorOptico(); // Chama a função da biblioteca sensorOpticoPro para Ler RPM
}  

// Funções de tratamento dos comandos
void tratarAjuda(Comando comando, sensorOpticoPro &sensor) { // Verifica o Status da Conexão Serial

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
	Serial.println("rpmDesejado: Define a velocidade de rotação (RPM) desejada para o disco decodificador.");
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
    {"configurarParametrosSensorOptico", tratarConfigurarParametrosSensorOptico}, // Associa o comando "configurarParametrosSensorOptico" à função tratarConfigurarParametrosSensorOptico
    {"rpmDesejado", tratarRpmDesejado}, // Associa o comando "rpmDesejado" à função tratarRpmDesejado
    {"numRiscos", tratarNumRiscos}, // Associa o comando "numRiscos" à função tratarRpmDesejado
    {"fatorAjusteLimiar", tratarFatorAjusteLimiar}, // Associa o comando "fatorAjusteLimiar" à função tratarFatorAjusteLimiar
    {"numAmostrasLimiar", tratarNumAmostrasLimiar}, // Associa o comando "numAmostrasLimiar" à função tratarNumAmostrasLimiar
    {"numAmostrasDetecMov", tratarNumAmostrasDetecMov}, // Associa o comando "numAmostrasDetecMov" à função tratarNumAmostrasDetecMov
    {"ajustarDistanciaSensorOptico", tratarAjustarDistanciaSensorOptico}, // Associa o comando "ajustarDistanciaSensorOptico" à função tratarAjustarDistanciaSensorOptico
    {"lerRPM", tratarLerRPM}, // Associa o comando "lerRPM" à função tratarLerRPM
    {"ajuda", tratarAjuda}, // Associa o comando "ajuda" à função tratarAjuda
    {nullptr, nullptr} // Marcador de fim da tabela (obrigatório)
};

Comando gerenciadorComando::analisarComando(String comandoRecebido) {
	/*
  * Objetivo: a Função analisa uma string de comando recebida, separando o nome do comando e seus valores numéricos.
  * Parâmetro: comandoRecebido - A string contendo o comando e seus valores. Ex: "MOTOR 10.5 20 30"
  * Retorno: Um struct Comando contendo o nome do comando e um array de até 5 valores numéricos.
  */

    Comando comando; // Cria uma instância (variavel) da struct Comando.
    comando.nome = ""; // Reseta o nome do comando para evitar lixo de memória
    comando.numValores = 0; // Reseta o número de valores para evitar lixo de memória
    for (int i = 0; i < Comando::maxValores; i++) {  // Reseta os valores
        comando.valores[i] = ""; // Inicializa as strings
    }
    comandoRecebido.trim(); // Remove espaços em branco extras no início e no final da string recebida. Ex: "  MOTOR 10 " vira "MOTOR 10"

    if (comandoRecebido.length() == 0) return comando; // Retorna comando vazio se a string estiver vazia

    int pos = comandoRecebido.indexOf(' ');// Encontra a posição do primeiro espaço em branco na string.
                                          // Se não houver espaço, pos será -1.

    if (pos != -1) { // Se encontrou um espaço (ou seja, o comando tem pelo menos um valor)

        comando.nome = comandoRecebido.substring(0, pos); // Extrai o nome do comando, que é a parte da string antes do primeiro espaço.
                                                          // Ex: Se comandoRecebido é "MOTOR 10", comando.nome será "MOTOR".
        comandoRecebido = comandoRecebido.substring(pos + 1); // Atualiza a string comandoRecebido para conter apenas os valores,
                                                              // removendo o nome do comando e o espaço.
                                                              // Ex: Se antes era "MOTOR 10 20", agora será "10 20".
        comandoRecebido.trim(); // Remove espaços extras entre os valores

        int i = 0; // Inicializa um contador para controlar quantos valores foram lidos.
        while (i < Comando::maxValores && comandoRecebido.length() > 0) { // Loop que lê até 5 valores ou até que a string comandoRecebido fique vazia.
                                                        // O limite de 5 valores protege contra estouro de buffer em comando.valores.
            pos = comandoRecebido.indexOf(' '); // Encontra a posição do próximo espaço em branco.
            
            if (pos != -1) {
                comando.valores[i] = comandoRecebido.substring(0, pos);
                comandoRecebido = comandoRecebido.substring(pos + 1);
                comandoRecebido.trim();
            } else {
                comando.valores[i] = comandoRecebido;
                comandoRecebido = "";
            }
            i++;
        }
        comando.numValores = i; // Guarda a quantidade total de valores numéricos que foram lidos e armazenados.
    } else { // Se não encontrou nenhum espaço na string original (o comando não tem valores)
    
        comando.nome = comandoRecebido; // O comando é a string inteira.
    }
    return comando; // Retorna o struct Comando preenchido.
}

void gerenciadorComando::processarComando(Comando comando, sensorOpticoPro &sensor) {
  for (int i = 0; tabelaComandos[i].nome != nullptr; i++) {  // Loop que percorre a tabela de comandos 'tabelaComandos'. O loop continua enquanto o campo 'nome' da entrada atual da tabela não for nulo (nullptr), que indica o fim da tabela.
    if (comando.nome.equals(tabelaComandos[i].nome) == 0) { // Compara o nome do comando extraído com o nome do comando na tabela.
      tabelaComandos[i].funcao(comando, sensor); // Se o comando for encontrado na tabela, chama a função de tratamento correspondente. 'tabelaComandos[i].funcao' é um ponteiro para a função. Os valores ‘comando’ e ‘sensor’ são repassados para a função de tratamento.
      return; // Sai da função 'processarComando' após executar o comando.
    }
  }
}
