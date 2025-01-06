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



#ifndef gerenciadorComandos_h // Define um guarda de inclusão para evitar inclusões múltiplas do cabeçalho. Se 'gerenciadorComandos_h' não estiver definido, ele será definido agora.
#define gerenciadorComandos_h // Define o identificador 'gerenciadorComandos_h'.

// Forward declaration da biblioteca
class sensorOpticoPro; // Declaração prévia (forward declaration) da classe sensorOpticoPro. Isso diz ao compilador que essa classe existe, mesmo que sua definição completa esteja em outro lugar (sensorOpticoPro.h). Isso é necessário porque ComandoInfo usa sensorOpticoPro&

// Define uma estrutura para representar um comando e seus valores.
struct Comando {// Define uma estrutura chamada 'Comando' para armazenar as informações de um comando.
  //const int maxComandos = 10; // Defina um tamanho máximo de Comandos para a tabela
  String nome; // Armazena o Nome do comando recebido (ex: "status", "configurarRPM").
  const static int maxValores = 5; // Maximo de Valores que um comando pode enviar
  String valores[maxValores]; // Array para armazenar a quantidade de valores de ponto flutuante associados ao comando de acordo com a variavel (maxValores).
  int numValores = 0; // Número de valores efetivamente presentes no array 'valores'. Inicializada com 0.
};

// Define uma estrutura para associar nomes de comandos a funções de tratamento.
struct ComandoInfo { // Define uma estrutura chamada 'ComandoInfo' para associar nomes de comandos a funções.
  const char* nome; // Armazena o nome do comando (string C).
  void (*funcao)(Comando, sensorOpticoPro&); // Ponteiro para uma função que recebe um objeto Comando e uma referência a sensorOpticoPro como Valores e não retorna nada (void). 'sensorOpticoPro&' indica passagem por referência.
      /*
     * Ponteiro para uma função de tratamento de comando.
     *
     * 'funcao' pode apontar para qualquer função que:
     *   - Não retorne valor (void).
     *   - Receba dois Valores:
     *     - Um objeto 'Comando' (passado por valor - uma cópia é feita).
     *     - Uma REFERÊNCIA para um objeto 'sensorOpticoPro' (passado por referência - o objeto original é modificado).
     *
     * Exemplo (usando a função tratarNumRiscos desta Biblioteca):
     *
     * A função tratarNumRiscos se encaixa PERFEITAMENTE na assinatura esperada pelo ponteiro 'funcao':
     *   - Retorno void.
     *   - Recebe um 'Comando' e uma referência 'sensorOpticoPro&'.
     *
     * Para associar o comando "numRiscos" a essa função na tabela 'tabelaComandos', você usa:
     * {"numRiscos", tratarNumRiscos}
     *
     * Quando o comando "numRiscos 10" for recebido e processado (com o valor 10), o código internamente fará algo como:
     * Comando cmd;
     * cmd.nome = "numRiscos";
     * cmd.valores[0] = 10;
     * cmd.numValores = 1;
     * tabelaComandos[i].funcao(cmd, sensor); // Que, neste caso, é equivalente a:
     * tratarNumRiscos(cmd, sensor);
     *
     * Dentro de tratarNumRiscos, a linha 'sensor.novoNumRiscos(numRiscos);' será executada, modificando o objeto 'sensor' original.
     *
     * Onde 'cmd' é o comando que foi analisado (e extraído o nome "numRiscos" e o valor 10) e 'sensor' é o objeto sensorOpticoPro que você está utilizando.
     */
};

// Declara a tabela de comandos. Definida no arquivo .cpp.
// 'extern' indica que a definição real está em outro arquivo. Essencial para evitar erros de múltiplas definições.
extern ComandoInfo tabelaComandos[]; //O uso de extern é crucial para evitar erros de múltiplas definições na Linkagem
extern bool ajustarDistanciaSensor_Ativo;  // Indica se o modo de ajuste do Sensor Óptico está ativo.
extern bool lerRPMSensor_Ativo;            // Indica se o modo de leitura do RPM do Sensor Óptico está ativo.

class gerenciadorComando {
private:
    //int numComandos = 0; // Contador de comandos adicionados.
  uint8_t _pinoLigarMotor; // Pino digital ao qual o motor sera ligado.
  uint8_t _pinoSentidoGiro; // Pino digital ao qual o motor mudara o sentido de rotação.
public:
  gerenciadorComando(uint8_t pinoLigarMotor, uint8_t pinoSentidoGiro); // Construtor com 2 parâmetros

  void iniciar();// Inicializa o Motor e seus parâmetros.

  // Declara as funções de análise e processamento de comandos.
  Comando analisarComando(String comandoRecebido); // Analisa a string de comando e retorna uma estrutura Comando.
  void processarComando(Comando comando, sensorOpticoPro &sensor); // Processa um comando, chamando a função de tratamento correspondente.
  void tratarStatus(Comando comando);
  void tratarLigarMotor(Comando comando);
  void tratarDesligarMotor(Comando comando);
  void tratarSentidoGiro(Comando comando);
  void tratarConfigurarParametrosSensorOptico(Comando comando, sensorOpticoPro &sensor);
  void tratarRpmMaximo(Comando comando, sensorOpticoPro &sensor);
  void tratarNumRiscos(Comando comando, sensorOpticoPro &sensor);
  void tratarFatorAjusteLimiar(Comando comando, sensorOpticoPro &sensor);
  void tratarNumAmostrasLimiar(Comando comando, sensorOpticoPro &sensor);
  void tratarNumAmostrasDetecMov(Comando comando, sensorOpticoPro &sensor);
  void tratarAjustarDistanciaSensorOptico(Comando comando, sensorOpticoPro &sensor);
  void tratarPararAjusteDistanciaSensorOptico(Comando comando, sensorOpticoPro &sensor);
  void tratarLerRPM(Comando comando, sensorOpticoPro &sensor);
  void tratarPararLeituraRpm(Comando comando, sensorOpticoPro &sensor);
  void tratarAjuda(Comando comando, sensorOpticoPro &sensor);

};

#endif // Fim do guarda de inclusão.
