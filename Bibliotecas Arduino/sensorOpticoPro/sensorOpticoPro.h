/*
 * Descrição: Esta biblioteca fornece funções para interfacear e obter dados
 * de um sensor óptico baseado em pulsos. Ela oferece recursos para configuração
 * de parâmetros como RPM desejado, número de riscos no disco, ajuste de limiar
 * e cálculo de velocidade angular, buscando otimizar a leitura e minimizar
 * a latência. Além disso, aborda a susceptibilidade a ruídos, oferecendo
 * mecanismos para filtragem e maior controle sobre os dados.
 *
 * Suporte: Projetada para fornecer comunicação rápida e direta com o sensor
 * óptico, permitindo maior controle na interpretação dos dados e customizações
 * para aplicações específicas.
 *
 * Maior Controle: Oferece controle total sobre a interpretação dos dados,
 * permitindo customizações e otimizações para a aplicação.
 *
 * Menor Latência: Ao eliminar intermediários, busca reduzir a latência na leitura
 * dos dados, crucial para aplicações que exigem alta responsividade.
 *
 * Susceptibilidade a Ruídos: Reconhece a susceptibilidade a ruídos elétricos e
 * implementa mecanismos de filtragem para aumentar a precisão dos dados.
 *
 * Dependências:
 *   - Arduino.h
 *   - inttypes.h
 *   - math.h
 *
 * Autor: Tiago Carvalho Pontes
 * Data: 12/12/2024
 * Versão: 1.0 
 */


#ifndef sensorOpticoPro_h // Define um guarda de inclusão para evitar inclusões múltiplas do cabeçalho. Se 'sensorOpticoPro_h' não estiver definido, ele será definido agora.
#define sensorOpticoPro_h // Define o identificador 'sensorOpticoPro_h'.



// Definição das constantes para statusConexaoSensorOptico() ------ Apenas para Depuração;
#define STATUS_REGISTER 0x00 // Endereço do registrador de status do sensor. Precisa ser definido corretamente para o seu sensor.
#define STATUS_BIT_OK 0x01  // Bit que indica status OK no registrador de status. Precisa ser definido corretamente para o seu sensor

  // Estrutura para armazenar informações sobre o movimento detectado.
  struct Movimento {
    bool movimentoDetectado;  // Indica se houve alguma transição de estado no sensor (o que pode indicar movimento).
    float valorFiltrado; // Armazena o valor filtrado do sinal do sensor
  };

class sensorOpticoPro
{
  private:
  protected: //Essas Classes representam detalhes internos da implementação da biblioteca e não precisam ser acessadas diretamente, são acessíveis pela própria classe e classes derivadas.
	//Configuração do Pinos
  uint8_t _pinoSensor; // Pino digital ao qual o sensor está conectado.
  //Cronometragem
  unsigned long _instanteInicial; //Instante exato em que o programa começa a ser executado (usado para calcular tempos decorridos)..
  unsigned long _instanteRpmInicial; // Instante da última medição de RPM, recebe o valor a cada inicio de função (usado para calcular a velocidade angular).
  //Parametros do Sensor
  uint16_t _rpmDesejado; // Valor de RPM solicitado recebido via serial do sistema da Balanceadora (configurado externamente).
  float _rpmAtual; // RPM atual calculado pelo sensor
  float _rpmAtualTemporario = _rpmDesejado; // Valor intermediário para ajuste gradual do RPM (evita mudanças bruscas).
  uint8_t _numRiscos; // Número total de Pulsos (riscos) no disco do sensor óptico. Utilizado no cálculo do RPM.

  //Calcular Velocidade Angular em Radianos por segundo
  float _velocidadeAngular = 0.0; // Inicializa com zero radianos por segundo e armazenar as velocidades angulares calculadas.
  
  /********************************************************** Calcular RPM **********************************************************/
    // Limiar e Tempo
    uint8_t _limiarPulsacoes; // Número mínimo de Pulsos necessários para considerar uma detecção válida. Um valor mais alto aumenta a confiabilidade da detecção, mas pode atrasar a resposta.
    float _fatorAjusteLimiar = 1.0; // Ajuste do Limite de Pulsos - Aumenta a sensibilidade do sensor quando maior que 1.0 e diminui quando menor que 1.0. Utilizado para compensar variações na iluminação ambiente.
      uint16_t _NUM_AMOSTRAS_calcLimiar = 100; // Número de amostras para cálculo do limiar ideal para o Calculo do RPM
      int* _amostras_valorPulso = new int[_NUM_AMOSTRAS_calcLimiar]; //É um array que armazena as últimas amostras das leituras do sensor (_NUM_AMOSTRAS_calcLimiar garantira o espaço nescessario na memoria).
    uint16_t _NUM_AMOSTRAS_detecMov = 100; // Número de amostras do Filtro Movel para Detecção de Movimento
      int* _amostras_detecMov = new int[_NUM_AMOSTRAS_detecMov]; //É um array que armazena as últimas amostras das leituras do sensor (_NUM_AMOSTRAS_detecMov garantira o espaço nescessario na memoria).
    uint16_t _tempoMinimoEntrePulsacoes; // Define o intervalo de tempo mínimo (em milissegundos) entre duas detecções consecutivas de pulsos. Serve para filtrar ruídos e evitar a contagem dupla de pulsos.
    
    //Posição Angular
    float _anguloAtual = 0.0; // Ângulo de rotação atual em graus.
    

      /**************************************************************************************************************************************
      * _limiarPulsacoes: Define o número mínimo de pulsos detectados em um intervalo de tempo para considerar uma rotação completa. 
      * Um valor mais alto aumenta a confiabilidade dadetecção, mas pode atrasar a resposta. Por exemplo, se o limiar for 5, o sensor 
      * só considerará uma rotação completa se detectar pelo menos 5 pulsos dentro do intervalo definido por _tempoMinimoEntrePulsacoes.
      *  - Fatores que Influenciam o Limiar Ideal:
      *  - Número de riscos no disco: Um disco com mais riscos geralmente permite uma detecção mais precisa, pois há mais transições entre os estados alto e baixo dos sinais.
      *  - Velocidade de rotação: Para altas velocidades, um limiar menor pode ser suficiente, pois haverá mais transições em um intervalo de tempo menor.
      *  - Ruído elétrico: Um ambiente com muito ruído elétrico pode exigir um limiar maior para filtrar as falsas detecções.
      *  - Estabilidade mecânica: Vibrações ou desalinhamentos no sistema podem causar flutuações no sinal, exigindo um ajuste no limiar.
      *
      * _tempoMinimoEntrePulsacoes: Define o intervalo de tempo mínimo (em milissegundos) entre duas detecções consecutivas de pulsos.
      * Essa variável serve para filtrar ruídos e evitar a contagem dupla de pulsos, garantindo uma contagem mais precisa. 
      * Um valor muito baixo pode levar a falsas detecções, enquanto um valor muito alto pode atrasar a resposta do sistema.
      * Ajuste esse valor de acordo com a velocidade de rotação esperada e o nível de ruído do ambiente.
      *
      * _numRiscos: Representa o número total de marcas (riscos) no disco do sensor óptico. 
      * Esse valor é fundamental para o cálculo da rotação por minuto (RPM), pois é utilizado como divisor na fórmula de cálculo do RPM. 
      * A precisão do cálculo do RPM depende diretamente da precisão desse valor.
      ***************************************************************************************************************************************/

  //Status do Sensor
  uint8_t lerDadosDeRegistro(uint8_t registro); // Lê dados de um registrador específico do sensor (para verificar status, por exemplo).

  //Calcular Tempo Minimo Entre Pulsos e Limiar Ideal atraves di RPM
  void calcularLimiarIdeal(); // Calcula e ajusta o limiar de pulsos dinamicamente.
    uint16_t lerValorPulso(); // Lê o valor bruto do pulso do sensor.
    double calcularMedia(const uint16_t* dadosPulsos, int tamanho); // Calcula a média de um conjunto de amostras.
    double calcularDesvioPadrao(const uint16_t* dadosPulsos, double media, int tamanho); // Calcula o desvio padrão de um conjunto de amostras.
    double calcularPotencia(double base, int expoente); // Calcula a potencia de um numero
  void calcularTempoMinimoEntrePulsacoes(); // Calcula o tempo mínimo entre pulsos com base no RPM e número de riscos (Pulsos).

  
  public:
    sensorOpticoPro(uint8_t pinoSensor); // Construtor da classe: inicializa o sensor com o pino especificado.

    void iniciar(void);// Inicializa o sensor e seus parâmetros.
    
    bool statusConexaoSensorOptico(); // Verifica o Status da Conexão com o Sensor
    unsigned long lerInstanteInicial(); // Getter para acessar o instante inicial do Processo.
    uint16_t lerRpmDesejado() const; // Getter para acessar o valor do RPM Desejado.
    uint8_t lerNumRiscos() const; // Getter para acessar o valor da quantidade de Riscos do Disco.
    float lerRpmAtual() const; // Getter para acessar o valor do RPM Atual.
    float lerAnguloAtual() const; // Getter para acessar o valor do Angulo Atual.

    /******************** Calibração e configuraçãos ********************/
      void configurarParametrosSensorOptico(uint8_t config_numRiscos, uint16_t config_rpmInicial); // Inicializa o sensor com o RPM e o número de riscos desejados.

      // Configura Novos Parametros escolhidos pelo Usúario
      void novoRpmDesejado(uint16_t novoRPM); // Configura um novo valor para o RPM desejado.
      void novoNumRiscos(uint8_t novoNumRiscos); // Configura um novo número de riscos no disco. Como é um valor de 8 bits, o máximo de Riscos é 255.
      void novoFatorAjusteLimiar(float novoFator); // Configura um novo fator de ajuste para o limiar de pulsos. Usado para calibrar o sensor em diferentes condições de iluminação ou ruído.
      void novoNumAmostrasLimiar(uint16_t novoNumAmostrasLimiar); // Configura o número de amostras usadas para o cálculo do limiar.
      void novoNumAmostrasDetecMov(uint16_t novoNumAmostrasDetecMov); // Configura o número de amostras usadas para a detecção de movimento.

    // Contagem dos Pulsos e Calculo do RPM
    void iniciarSensorOptico(); // Inicializa o sensor óptico e prepara o sistema para a leitura dos pulsos. Realiza configurações iniciais e calibrações, se necessário.

    Movimento detectarMovimento(bool estadoSensor);  // Detecta a ocorrência de movimento com base no estado do sensor. Retorna informações sobre a detecção.
    float calcularRPM(); // Calcula o RPM com base nas leituras do sensor, utilizando o limiar e o tempo mínimo entre pulsos para filtragem de ruídos.
    void ajustarDistanciaSensorOptico(); // Função para auxiliar no ajuste físico da distância entre o sensor e o disco. Envolve leituras e comparações para indicar a distância ideal.

    // Calcular a velocidade angular
    float calcularVelocidadeAngular(); // Calcula a velocidade angular com base nas variações do ângulo. 
    // ---------- Lógica de cálculo ainda a ser implementada. ----------

};

#endif
