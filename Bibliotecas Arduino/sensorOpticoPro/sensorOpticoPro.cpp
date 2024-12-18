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

/******************************************************************************
 * Incluir Bibliotecas
 ******************************************************************************/

//Bibliotecas do Arduino IDE:
#include "Arduino.h" // Biblioteca padrão do Arduino, fornece funções básicas para I/O, manipulação de pinos, etc.
#include <inttypes.h> // Inclui tipos de dados inteiros de tamanho fixo (int8_t, uint16_t, etc.) para representação precisa de valores.
#include <math.h>  // Inclui funções matemáticas como "sqrt" (Raiz Quadrada) para calcular o desvio padrão 
                   // das amostras e quantificar a dispersão dos dados, "sin" e "cos" para cálculos de 
                   // velocidade angular, além da constante "PI", essencial para operações trigonométricas.
#include "sensorOpticoPro.h" // Inclui o cabeçalho desta biblioteca


/******************************************************************************
 * Definir Construtor
 ******************************************************************************/

sensorOpticoPro::sensorOpticoPro(uint8_t pinoSensor) 
	: _pinoSensor(pinoSensor)
{
	// Configura o pino como entrada
        pinMode(_pinoSensor, INPUT_PULLUP);
}
		
void sensorOpticoPro::configurarParametrosSensorOptico(uint8_t config_numRiscos, uint16_t config_rpmInicial) 
{
	///* Apenas para Depuração... */ Serial.println("Configurando os parametros (Número de Pulsos e RPM)...");

    if (config_numRiscos <= 0) {
        Serial.println("Número de riscos inválido.");
        return;
    }
    if (config_rpmInicial < 0) {
        Serial.println("O RPM não pode receber um Valor Negativo!");
        return;
    }
    _numRiscos = config_numRiscos;
    _rpmDesejado = config_rpmInicial;

	///* Apenas para Depuração... */ Serial.println("Parametros do Sensor Óptico configurados com sucesso.\n");

	calcularTempoMinimoEntrePulsacoes();
	//exemplo para utilizar a função: sensor.configurarSensor(30, 500); // Define o número de riscos do disco e o RPM desejado

	///* Apenas para Depuração... */ Serial.println("Configuração dos Parametros Finalizadas (Número de Pulsos e RPM)...");

}

/******************************************************************************
 * Funções de Leitura de Valores
 ******************************************************************************/

unsigned long sensorOpticoPro::lerInstanteInicial() 
{
    return _instanteInicial;
}

uint16_t sensorOpticoPro::lerRpmDesejado() const {
    return _rpmDesejado;
}

uint8_t sensorOpticoPro::lerNumRiscos() const {
    return _numRiscos;
}

float sensorOpticoPro::lerRpmAtual() const {
    return _rpmAtual;
}

float sensorOpticoPro::lerAnguloAtual() const {
    return _anguloAtual;
}



// Define o novo RPM desejado. Se o valor for negativo, utiliza o valor padrão.
void sensorOpticoPro::novoRpmDesejado(uint16_t novoRPM) 
{
	/*
	if (novoRPM < 0 || novoRPM > 65535) {
        Serial.print("Erro: O valor para 'rpmDesejado' deve estar entre 1 e 1500.");
        Serial.print(" Valor fornecido: ");
        Serial.println(novoRPM);
        return; // Saída antecipada da função em caso de erro
    } /* */

    _rpmDesejado = novoRPM;
	calcularTempoMinimoEntrePulsacoes();
}

// Define o número de riscos no disco. Se o valor for menor ou igual a zero, utiliza o valor padrão.
void sensorOpticoPro::novoNumRiscos(uint8_t novoNumRiscos) 
{
	/*
	if (novoNumRiscos < 1 || novoNumRiscos > 255) {
        Serial.print("Erro: O valor para 'numRiscos' deve estar entre 1 e 255.");
        Serial.print(" Valor fornecido: ");
        Serial.println(novoNumRiscos);
        return; // Saída antecipada da função em caso de erro
    } /* */

    _numRiscos = novoNumRiscos;
	calcularTempoMinimoEntrePulsacoes();
}

// Define o Fator de ajuste para o limiar de detecção compensar variações na iluminação ambiente, o valor deve ser positivo.
void sensorOpticoPro::novoFatorAjusteLimiar(float novoFator) 
{
	/*
	if (novoFator <= 0.0) {
        Serial.print("Erro: O valor para 'fatorAjusteLimiar' deve ser maior que 0.0.");
        Serial.print(" Valor fornecido: ");
        Serial.println(novoFator);
        return; // Saída antecipada da função em caso de erro
    } /* */

    _fatorAjusteLimiar = novoFator;
}

// Define o número de amostras utilizadas para calcular o limiar ideal.
// Um valor maior aumenta a precisão, mas pode diminuir o desempenho.
void sensorOpticoPro::novoNumAmostrasLimiar(uint16_t novoNumAmostrasLimiar) 
{
	/*
	if (novoNumAmostrasLimiar < 0 || novoNumAmostrasLimiar > 65535) {
        Serial.print("Erro: O valor ideal para 'numAmostrasLimiar' deve estar entre 1 e 100, números maiores podera causar atraso no calculo.");
        Serial.print(" Valor fornecido: ");
        Serial.println(novoNumAmostrasLimiar);
        return; // Saída antecipada da função em caso de erro
    } /* */

    _NUM_AMOSTRAS_calcLimiar = novoNumAmostrasLimiar;
}

// Define o número de amostras utilizadas para o Calculo de Detecção de Movimento.
// Um valor maior aumenta a precisão, mas pode diminuir o desempenho.
void sensorOpticoPro::novoNumAmostrasDetecMov(uint16_t novoNumAmostrasDetecMov) 
{
	/*
	if (novoNumAmostrasDetecMov < 0 || novoNumAmostrasDetecMov > 65535) {
        Serial.print("Erro: O valor ideal para 'numAmostrasDetecMov' deve estar entre 1 e 100, números maiores podera causar atraso no calculo.");
        Serial.print(" Valor fornecido: ");
        Serial.println(novoNumAmostrasDetecMov);
        return; // Saída antecipada da função em caso de erro
    } /* */

    _NUM_AMOSTRAS_detecMov = novoNumAmostrasDetecMov;
}

// A função transforma um valor numérico que representa um estado digital (alto ou baixo) em uma string descritiva.
String estadoLogicoParaTexto(int state) {
	return (state == HIGH) ? "Ativo (HIGH)" : "Inativo (LOW)";
}

/******************************************************************************
 * Definir API de Usúario
 ******************************************************************************/

void sensorOpticoPro::iniciar()
{
	/* Apenas para Depuração... */ Serial.println("Comunicação com o Sensor Óptico inicializada...");
	
 	// **Inicialização das variáveis:**
    // Nesta função, as variáveis globais são inicializadas com seus valores padrão.
    // **Importante:** Valores atribuídos a variáveis globais dentro do `setup()` persistem durante toda a execução do programa,
    // a menos que sejam explicitamente modificados em outro ponto do código. 
    // Isso significa que qualquer alteração feita em uma variável global dentro do `loop()` ou em outra função será mantida. 

	// Reset das Variaveis - Os valores Utilizados são Valores Padrões
	configurarParametrosSensorOptico(36, 500); //Configura novo Número de Riscos do Disco e Rpm Solicitado caso seja nescessario...
	_instanteInicial = millis(); // Serve para marcar o instante exato em que o programa começa a ser executado.
    _instanteRpmInicial = millis(); // Inicializa _instanteRpmInicial
    _velocidadeAngular = 0.0;       // Inicializa _velocidadeAngular
	_limiarPulsacoes = 0; 
	_fatorAjusteLimiar = 1.0; 
	_tempoMinimoEntrePulsacoes = 0; 
	_anguloAtual = 0.0; 
	_rpmAtual = 0; 
	_rpmAtualTemporario = _rpmDesejado; 
	_NUM_AMOSTRAS_calcLimiar = 100;
	_NUM_AMOSTRAS_detecMov = 100;

	///* Apenas para Depuração... */ Serial.println("Comunicação com o Sensor Óptico estabilizada...");
}

/******************************************************************************
 * Verificar Status da Conexão com o Sensor Infravermelho ------ Apenas para Depuração
 ******************************************************************************/

bool sensorOpticoPro::statusConexaoSensorOptico() {
  // Ler o registro de status do sensor (exemplo)
  uint8_t status = lerDadosDeRegistro(STATUS_REGISTER);

  // Verificar se os bits relevantes estão configurados corretamente
  return (status & STATUS_BIT_OK) == STATUS_BIT_OK;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Métodos Privados (significa que eles só podem ser acessados internamente dentro da própria classe, e não de fora.)

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/******************************************************************************************************************
A função lerDadosDeRegistro tem como objetivo ler um registro específico do sensor óptico.
******************************************************************************************************************/
/*
**Descrição:**
1. **Recebe o índice do registro:** A função recebe um número que identifica o registro desejado. Cada registro contém informações específicas sobre o sensor, como a taxa de amostragem ou a resolução.
2. **Configura o sensor:** A função configura o sensor para ler o registro especificado, enviando os comandos necessários através do pino de dados.
3. **Aguarda a resposta:** Aguarda o sensor retornar os dados do registro solicitado.
4. **Lê os dados:** Lê os dados do sensor através do pino de dados e os armazena em uma variável.
5. **Retorna os dados:** Retorna o valor lido do registro.

**Observações:**
* A implementação específica pode variar dependendo do modelo do sensor e do protocolo de comunicação utilizado.
* A função assume que o sensor está configurado corretamente e que o pino de dados está conectado corretamente.
* Para sensores mais complexos, pode ser necessário implementar funções adicionais para configurar o sensor, iniciar a leitura e sincronizar a comunicação.
*/

uint8_t sensorOpticoPro::lerDadosDeRegistro(uint8_t indiceRegistro)
{
	///* Apenas para Depuração... */ Serial.print("Lendo registro com índice: ");
	///* Apenas para Depuração... */ Serial.println(indiceRegistro, HEX);

	// Essa função agora será responsável por ler os dados do pino de sinal do sensor
	// A lógica de leitura dependerá da sua implementação específica do sensor
	// ... (ex: ativar modo de leitura, enviar comando de leitura, etc.)

	// Aguardar a leitura dos dados (pode envolver interrupções ou polling)
	// ... (ex: aguardar um número específico de pulsos ou um tempo definido)

	// Ler os dados do pino
	uint8_t dados = digitalRead(_pinoSensor);
	
	// Processar os dados lidos (se necessário)
	// ... (ex: converter em um formato específico, aplicar correções)

	///* Apenas para Depuração... */ Serial.print("Dados lidos: ");
	///* Apenas para Depuração... */ Serial.println(dados, BIN);

	return dados;
}

float sensorOpticoPro::calcularVelocidadeAngular() {
  // Lógica para calcular a velocidade angular no eixo X
  // ...
  // Utilize os valores do pino de Sinal e o tempo decorrido desde a última medição
  // Aplique filtros para reduzir o ruído
  //_velocidadeAngular = (anguloAtual - anguloAnterior) / (tempoAtual - tempoAnterior);
  //_velocidadeAngular = resultadoDoCalculo;
  return _velocidadeAngular; // Retorna a velocidade angular em radianos por segundo
}

// Calcula o tempo mínimo entre os pulsos em milissegundos baseado no RPM desejado e o número de riscos.
void sensorOpticoPro::calcularTempoMinimoEntrePulsacoes() {

	///* Apenas para Depuração... */ Serial.println("Calculando Tempo Minimo entre as Pulsações...");

	/*
	 A função calcularTempoMinimoEntrePulsacoes realiza o cálculo do tempo esperado por pulso.

	*/

	// Calculando o tempo em milissegundos entre cada pulso Utilizado o RPM Atual.
	// Pode tambem ser usado o RPM desejado, porem pode não refletir a realidade se o RPM atual estiver significativamente diferente do RPM desejado, especialmente durante a fase de ajuste.
	float tempoPorPulso = 60000.0 / (_rpmAtualTemporario * _numRiscos);
	//A constante 60000.0 na fórmula representa o número de milissegundos em um minuto. (Não deve ser Alterada, ela representa a conversão de minutos para milissegundos)
	
	// Adicionando uma margem de segurança e arredondando para cima
	_tempoMinimoEntrePulsacoes = ceil(tempoPorPulso * 1.2); // Ajuste o fator de segurança conforme necessário

	///* Apenas para Depuração... */ Serial.println("Calculo de Tempo Minimo Finalizado...");
}

//Calcula o limiar ideal para detectar pulsos, utilizando um filtro de média móvel e desvio padrão.
void sensorOpticoPro::calcularLimiarIdeal() {	
		/*
		Fator de ajuste: O fator de ajuste permite controlar a sensibilidade do limiar às variações nos dados. 
						Um valor maior aumentará a robustez do sistema, mas pode reduzir a sensibilidade.
		Coleta de dados: A quantidade de dados coletados para calcular as estatísticas influencia a precisão do resultado. 
						Quanto mais dados, mais precisa será a estimativa do limiar.
		Tempo de execução: A função de cálculo do limiar ideal deve ser eficiente para não afetar a performance do sistema.
		Condições iniciais: Para a primeira execução, pode ser necessário fornecer um valor inicial para o limiar, 
							que será ajustado posteriormente pela função.

		Outras abordagens:
		Aprendizado de máquina: É possível utilizar técnicas de aprendizado de máquina para treinar um modelo que preveja o limiar ideal com base em um conjunto de dados históricos.
		Filtros adaptativos: Utilizar filtros adaptativos para ajustar o limiar em tempo real, de acordo com as variações no sinal.
		Benefícios de uma função dinâmica:

		Maior precisão: O limiar é ajustado de forma contínua para se adaptar às condições de operação.
		Menor tempo de configuração: Não é necessário ajustar manualmente o limiar para cada aplicação.
		Maior robustez: O sistema é mais resistente a variações nas condições ambientais e mecânicas.
		*/

    // Coletar dados de um número suficiente de pulsos
    uint16_t dadosPulsos[_NUM_AMOSTRAS_calcLimiar];

		// Coletar dados de pulsos (exemplo simplificado)
		for (int i = 0; i < _NUM_AMOSTRAS_calcLimiar; i++) {
			dadosPulsos[i] = lerValorPulso();
		}

    // Calcular estatísticas dos dados coletados e do desvio padrão (utilize uma biblioteca estatística ou implemente manualmente)
    double media = calcularMedia(dadosPulsos, _NUM_AMOSTRAS_calcLimiar);
    double desvioPadrao = calcularDesvioPadrao(dadosPulsos, media, _NUM_AMOSTRAS_calcLimiar);

    // Calcular o limiar com base nas estatísticas e no fator de ajuste
    _limiarPulsacoes = static_cast<uint8_t>(media + _fatorAjusteLimiar * desvioPadrao);

	///* Apenas para Depuração... */ Serial.print("O Valor Limiar Ideal é: ");
	///* Apenas para Depuração... */ Serial.println(_limiarPulsacoes);
}

		/************************************** Funções Auxiliares - calcularLimiarIdeal **************************************/
		uint16_t sensorOpticoPro::lerValorPulso() {
		// Esta função lê o valor do pulso (tempo entre duas bordas de subida) do sensor óptico,
		// filtra o valor utilizando uma média móvel e retorna o valor filtrado.
			
  			// Variáveis para armazenar o tempo e o estado anterior do pino
			static unsigned long tempoUltimoPulso = 0; // Tempo da última borda de subida, utilizado para calcular o intervalo de tempo
			static bool estadoAnterior = LOW; // Estado anterior do pino

  			// Índice para acessar o array de amostras
			static int indice_valorPulso = 0;//É um índice que aponta para a próxima posição livre para armazenamento no array onde é iniciado com zero
											 // e a cada nova leitura é incrementado +1, ao atigir o numero maximo de amostras é reiniciado.

			uint16_t valorPulso = 0; // Valor do pulso atual, Inicializado com um valor padrão (ajuste conforme necessário)

			bool estadoAtual = digitalRead(_pinoSensor);// Lê o estado atual do pino

			if (estadoAtual && !estadoAnterior) { // Detecta a borda de subida (transição de LOW para HIGH)
				unsigned long tempoPulsoAtual = millis(); // Calcula o tempo entre a borda de subida atual e a anterior
				valorPulso = tempoPulsoAtual - tempoUltimoPulso;
				tempoUltimoPulso = tempoPulsoAtual;
			} else if (!estadoAtual && estadoAnterior) { // Detecta borda de descida
				// Aqui, você pode realizar alguma ação específica quando a borda de descida ocorre,
				// como resetar um contador ou indicar o fim de um ciclo.
				valorPulso = 0;
			}

			estadoAnterior = estadoAtual; // Atualiza o estado anterior para a próxima iteração

			// Filtro de média móvel 
			_amostras_valorPulso[indice_valorPulso] = valorPulso; // Adiciona o valor do pulso ao final da janela deslizante
			indice_valorPulso = (indice_valorPulso + 1) % _NUM_AMOSTRAS_calcLimiar; // Incrementa o índice, o uso do módulo % _NUM_AMOSTRAS_calcLimiar garante que o índice sempre fique dentro dos limites do array, criando um efeito circular.

			// Calcula a média das últimas _NUM_AMOSTRAS_calcLimiar leituras válidas (maiores que zero)
			float soma_valorPulso = 0; // Inicializa a soma das leituras válidas
			int numAmostrasValidas = 0; // Inicializa o contador de leituras válidas

			// Itera sobre todas as amostras da janela deslizante
			for (int i = 0; i < _NUM_AMOSTRAS_calcLimiar; i++) {
    			// Verifica se a amostra é válida (maior que zero)
				if (_amostras_valorPulso[i] > 0) {
        			// Acumula a amostra válida na soma
					soma_valorPulso += _amostras_valorPulso[i];
        			// Incrementa o contador de amostras válidas
					numAmostrasValidas++;
				}
			}

			// Calcula o valor médio dos pulsos válidos (maiores que zero). Se não houver pulsos válidos, retorna 0.
			float valorPulsoFiltrado = (numAmostrasValidas > 0) ? soma_valorPulso / numAmostrasValidas : 0;  
			
			return valorPulsoFiltrado; // Retorna o valor do pulso filtrado

			///* Apenas para Depuração... */ Serial.print("O valor do Pulso é: ");
			///* Apenas para Depuração... */ Serial.println(valorPulsoFiltrado);
		}

		// Funções auxiliares para calcular média e desvio padrão
		double sensorOpticoPro::calcularMedia(const uint16_t* dadosPulsos, int tamanho_calcMedia) {
			if (tamanho_calcMedia == 0) {
				return 0.0; // Retorna 0 se o vetor estiver vazio
			}

			// Calcula a soma de todos os elementos do vetor `dadosPulsos` de forma manual.
			// A soma dos valores de pulso é calculada manualmente, iterando sobre o vetor 
			// `dadosPulsos`. Essa abordagem é utilizada para evitar a dependência da biblioteca 
			// `<numeric>`, que não está disponível no ambiente Arduino.
			double soma_calcMedia = 0.0;
			for (int i = 0; i < tamanho_calcMedia; i++) {
				soma_calcMedia += dadosPulsos[i];
			}


			// Calculando a média
			double media = soma_calcMedia / tamanho_calcMedia;

			return media;

			///* Apenas para Depuração... */ Serial.print("A média é: ");
			///* Apenas para Depuração... */ Serial.println(media);
		}

		double sensorOpticoPro::calcularDesvioPadrao(const uint16_t* dadosPulsos, double media, int tamanho) {
			if (tamanho <= 1) {
				return 0.0; // Se houver apenas um dado ou nenhum, o desvio padrão é zero
			}

			// Calculando a soma dos quadrados das diferenças em relação à média
			double somaQuadradosDiferencas = 0.0;
			for (int i = 0; i < tamanho; i++) {
				double diferenca = dadosPulsos[i] - media;
				somaQuadradosDiferencas += calcularPotencia(diferenca, 2); // Usando a função personalizada
			}

			// Calculando a variância (média dos quadrados das diferenças)
			double variancia = somaQuadradosDiferencas / tamanho;

			// Calculando o desvio padrão (raiz quadrada da variância)
			double desvioPadrao = sqrt(variancia); // Usando a função sqrt da biblioteca math.h

			return desvioPadrao;

			///* Apenas para Depuração... */ Serial.print("O Desvio Padrão é: ");
			///* Apenas para Depuração... */ Serial.println(desvioPadrao);
		}

		double sensorOpticoPro::calcularPotencia(double base, int expoente) {
		double resultado = 1.0;
		for (int i = 0; i < expoente; ++i) {
			resultado *= base;
		}
		return resultado;
		}
		/************************************** Fim das Funções Auxiliares - calcularLimiarIdeal **************************************/

// Detecta movimento utilizando as transições de estado, qualquer transição para LOW ou HIGH indica movimento.
Movimento sensorOpticoPro::detectarMovimento(bool estadoSensor) {
    Movimento movimento; // Retorna uma estrutura Movimento.
    static int indice_detecMov = 0; // Índice para acessar o vetor de amostras circularmente
    static float soma_detecMov = 0; // Soma das últimas amostras

    int valorSensor = estadoSensor ? 1 : 0; // Converte o valor booleano para um valor numérico para facilitar o filtro (HIGH 1 e LOW 0)

    // Atualiza a soma e o índice do vetor de amostras
    soma_detecMov -= _amostras_detecMov[indice_detecMov]; // Remove a amostra mais antiga da soma
    soma_detecMov += valorSensor; // Adiciona a nova amostra à soma
    _amostras_detecMov[indice_detecMov] = valorSensor; // Atualiza o valor da amostra no vetor
    indice_detecMov = (indice_detecMov + 1) % _NUM_AMOSTRAS_detecMov; // Incrementa o índice e faz o rollover para o início do vetor

    // Calcula a média móvel
    movimento.valorFiltrado = soma_detecMov / _NUM_AMOSTRAS_detecMov; // Calcula a média das últimas amostras

    const float LIMIAR = 0.5; // Ajuste o limiar conforme necessário
    movimento.movimentoDetectado = movimento.valorFiltrado > LIMIAR; // Define se houve movimento baseado em um limiar no valor filtrado

	return movimento; // Retorna as informações sobre o movimento detectado.
}

// Realiza um número pré-definido de leituras do sensor para obter uma estimativa inicial do RPM e calcular o limiar ideal.
// Aumenta gradualmente o RPM desejado para permitir que o sensor se adapte às novas condições e para evitar picos de corrente no motor.
void sensorOpticoPro::iniciarSensorOptico() {
	
    Serial.println("Leitura de dados do Sensor Óptico iniciada!");

    // Leitura inicial para obter uma estimativa do RPM e limiar
    for (int i = 0; i < 10; i++) { //Inicia um loop que será executado 10 vezes (O objetivo deste loop é coletar dados iniciais do sensor para calcular valores como RPM e limiar.).
        lerValorPulso(); // Realiza uma leitura do sensor e armazenar os dados.
        calcularRPM(); // Calcula o RPM com base nos dados coletados.
    }

    // Ajuste gradual do RPM desejado
	int rpmInicial = _rpmDesejado;
    for (int i = 0; i < 10; i++) { // Inicia um novo loop para ajustar gradualmente o RPM desejado.
        int novoRpm = rpmInicial * (i + 1) / 10; // Chama a função novoRpmDesejado para atualizar o RPM desejado para um valor um pouco maior a cada iteração. O cálculo aumenta o RPM gradualmente.
        _rpmAtualTemporario = novoRpm;
		delay(100); // Introduz um atraso para permitir que o motor e o sensor se estabilizem após cada ajuste de RPM. O valor do atraso pode ser ajustado conforme necessário.
    
        // Verificar se o motor atingiu a velocidade desejada
        if (_rpmAtual >= _rpmDesejado * 0.95) {
            break; // Sai do loop se o motor estiver próximo da velocidade desejada
        }
	}



	/**********************************
		// Em outro ponto do seu código
	if (_rpmAtual < _rpmDesejado) {
		// Aumentar a velocidade do motor
	} else if (_rpmAtual > _rpmDesejado) {
		// Diminuir a velocidade do motor
	} else {
		// Manter a velocidade atual
	}
	************************************/
}

enum Estado { //Define os possíveis estados da máquina de estados
    ESPERANDO_PULSO, // Aguarda o início de um pulso (Transição de alto para baixo).
    PULSO_DETECTADO, // Pulso detectado, contando (Transição de alto para baixo).
};


float sensorOpticoPro::calcularRPM() {

  _instanteRpmInicial = millis();  // Instante em que a medição do RPM foi iniciada

  // Contador de pulsos
  static int contadorPulsosIntervalo = 0; // Inicializa um contador para contabilizar o número de pulsos detectados em um intervalo de tempo específico.

  // Variáveis para armazenar o tempo e a quantidade de interrupções
  static unsigned long tempoAnterior = 0; // Armazena o tempo da última verificação, utilizado para calcular o intervalo de tempo

  // Chamar a função detectarMovimento para obter os dados de movimento
  Movimento movimento = detectarMovimento(digitalRead(_pinoSensor));

  // Inicializa no estado de espera por pulso. Isso significa que inicialmente estamos esperando uma transição no pino de Sinal.
  Estado estadoAtual = ESPERANDO_PULSO;

  // Lê o estado atual do pino e armazena em uma variável booleana
  bool estadoAtual_Sensor = digitalRead(_pinoSensor);

  //Variavel Não utilizada...
  static volatile int contadorRiscos = 0; // Contador utilizado para contar o número total de pulsos (riscos) detectados.    

	//Inicia uma estrutura de decisão que avalia o estado atual da máquina e executa o código correspondente a cada caso.
	switch (estadoAtual) {
		
		/*
		** Funcionamento do Sensor:**
		* O sensor detecta a presença de objetos com base na reflexão da luz.
		* A sensibilidade do sensor pode variar dependendo da cor e da superfície do objeto.
		* Para este sistema, o sensor está configurado para detectar apenas a parte branca do disco.
		* **Estados do Sensor:**
			* LOW: Objeto não detectado (parte preta do disco).
			* HIGH: Objeto detectado (parte branca do disco).

		**Configuração do Disco:**
		* O disco deve ter partes brancas e pretas alternadas.
		* A largura das partes branca e preta deve ser igual para garantir pulsos de duração constante.
		* Ajuste a distância do sensor para que apenas a parte branca seja detectada.

		**Calibração:**
		* Determine o número exato de pulsos por rotação do disco (Quantidade de Partes Brancas (_numRiscos)).
		* Ajuste os parâmetros do cálculo do RPM de acordo com a calibração.
		*/
		
    // Detecta a borda de subida em ESPERANDO_PULSO e borda de descida em PULSO_DETECTADO (indicando um pulso completo)
    case ESPERANDO_PULSO: // Se o estado atual é ESPERANDO_PULSO e
        if (estadoAtual_Sensor == HIGH) { // o pino ESPERANDO_PULSO está alto e o pino PULSO_DETECTADO está baixo.
            contadorPulsosIntervalo++; // Incrementa o contador de pulsos.
            estadoAtual = PULSO_DETECTADO; // Muda para o estado de espera por pulso em PULSO_DETECTADO.
			//Serial.print("Contador de intervalo de pulso: ");
			//Serial.println(contadorPulsosIntervalo);
        }
        break;
    // Detecta a borda de descida em ESPERANDO_PULSO e borda de subida em PULSO_DETECTADO (indicando um pulso completo)
    case PULSO_DETECTADO: // Se o estado atual é PULSO_DETECTADO e
        if (estadoAtual_Sensor == LOW) { // o pino ESPERANDO_PULSO está baixo e o pino PULSO_DETECTADO está alto.
            estadoAtual = ESPERANDO_PULSO; // Volta ao estado inicial.
        }
        break;
	}

    // Verifica se o intervalo de tempo expirou
    unsigned long tempoAtual = millis(); // Tempo atual em milissegundos, desde o inicio da função até aqui. 
    if (tempoAtual - tempoAnterior >= _tempoMinimoEntrePulsacoes) { //Verifica se o tempo decorrido desde a última verificação é maior ou igual ao tempo mínimo configurado entre pulsos.
		// Se a condição for verdadeira, inicia o cálculo do RPM baseado no número de pulsos detectados no intervalo e no número total de riscos. 
        float rpm = (float)contadorPulsosIntervalo * 60 / _numRiscos; //O número de pulsos por minuto é calculado e dividido pelo número de riscos por rotação para obter o RPM.
        
    	if (contadorPulsosIntervalo >= _limiarPulsacoes) { //Verifica se o número de pulsos no intervalo excede um limiar pré-definido.
            contadorRiscos++; //Incrementa um contador de riscos (propósito não totalmente claro).
        }

        contadorPulsosIntervalo = 0; //Reinicia o contador de pulsos para a próxima medição.
        tempoAnterior = tempoAtual; //Atualiza o tempo da última verificação para a próxima iteração.
	
		/******************************************************************************************** 
		Filtro de média móvel 
		Objetivo: Suaviza o valor do RPM, reduzindo o impacto de ruídos e variações aleatórias.
		Funcionamento: Armazena as últimas N amostras de RPM em um vetor e calcula a média dessas amostras para obter um valor mais representativo.
		********************************************************************************************/
		// Vetor para armazenar as últimas 10 N amostras
		static const int NUM_AMOSTRAS_rpm = 10;
		//static float amostras_rpm[NUM_AMOSTRAS_rpm];
		static float amostras_rpm[NUM_AMOSTRAS_rpm] = {0}; // Inicializa o vetor com zeros;
		static int indice_rpm = 0;

		// Adiciona a nova amostra ao vetor
		amostras_rpm[indice_rpm] = rpm; // Adiciona a nova medição ao vetor
		indice_rpm = (indice_rpm + 1) % NUM_AMOSTRAS_rpm; // Incrementa o índice circularmente

		// Calcula a média das últimas N amostras (filtro de média móvel)
		float soma = 0;
		for (int i = 0; i < NUM_AMOSTRAS_rpm; i++) {
		soma += amostras_rpm[i];
		}
		float rpm_filtrado = soma / NUM_AMOSTRAS_rpm;

		// Calcula a velocidade angular em radianos por segundo com o valor filtrado
		float velocidadeAngular = rpm_filtrado * 2 * PI / 60;
		
		// Houve movimento, atualizar ângulo e última leitura
		_anguloAtual += velocidadeAngular * (tempoAtual - _instanteRpmInicial) / 1000.0;
		_anguloAtual = fmod(_anguloAtual, 360);
			

		// Calcular RPM, filtrar e atualizar ângulo apenas se houve movimento
		if (movimento.movimentoDetectado) { // Verifica se ouve movimento		
			if (rpm_filtrado > 0) {
				Serial.print("RPM: ");
				Serial.print(rpm_filtrado);
				//Serial.print(" Ângulo: ");
				//Serial.println(_anguloAtual);
				//Serial.println(" graus");
				}
		}

        _rpmAtual = rpm_filtrado; // Atualiza o RPM atual com o valor calculado

		return rpm_filtrado;

		}
}

//Utilizado para ajustar a distancia ideal entre Sensor Óptico e Disco Decodificador.
void sensorOpticoPro::ajustarDistanciaSensorOptico() {
    Serial.println("Ajuste da distancia entre Sensor Óptico e disco decodificador iniciada!");
  /*
    // Medir o tempo alto
    unsigned long tempoInicio = millis();
    while (digitalRead(_pinoSensor) == LOW) {
        // Esperar o sinal ficar alto
    }
    unsigned long tempoAlto = millis() - tempoInicio;

    // Medir o tempo baixo
    tempoInicio = millis();
    while (digitalRead(_pinoSensor) == HIGH) {
        // Esperar o sinal ficar baixo
    }
    unsigned long tempoBaixo = millis() - tempoInicio;

    // Calcular a diferença e realizar as comparações
    long diferenca = tempoAlto - tempoBaixo;

  // Definição das faixas de tolerância (ajuste conforme necessário)
  const long TOLERANCIA_MAXIMA = 100; // Ajuste este valor de acordo com a sua aplicação
  const long TOLERANCIA_MEDIA = 60; // Ajuste este valor de acordo com a sua aplicação
  const long TOLERANCIA_MINIMA = 20; // Ajuste este valor de acordo com a sua aplicação
  const long TOLERANCIA_Perfeita = 5; // Ajuste este valor de acordo com a sua aplicação

// Verificação das condições e impressão das mensagens
if (tempoAlto == 0 && tempoBaixo == 0) {
    Serial.println("Sensor Severamente Longe do Disco; Aproxime o Sensor!");
} else if (tempoAlto > tempoBaixo) {
    if (abs(diferenca) >= TOLERANCIA_MAXIMA) {
        Serial.println("Sensor Severamente Próximo ao Disco; Afaste o Sensor!");
    } else if (abs(diferenca) >= TOLERANCIA_MEDIA && abs(diferenca) < TOLERANCIA_MAXIMA) {
        Serial.println("Sensor Muito Próximo ao Disco; Afaste o Sensor!");
    } else if (abs(diferenca) >= TOLERANCIA_MINIMA && abs(diferenca) < TOLERANCIA_MEDIA) {
        Serial.println("Sensor Próximo ao Disco; Afaste um pouco o Sensor!");
    } else { // Se nenhuma das condições anteriores for verdadeira
        Serial.print("Distância Quase Perfeita (mais próximo)! Tolerância: ");
        Serial.print(abs(diferenca));
        Serial.println("%");
    }
} else if (tempoAlto < tempoBaixo) {
    if (abs(diferenca) >= TOLERANCIA_MAXIMA) {
        Serial.println("Sensor Severamente Longe do Disco; Aproxime o Sensor!");
    } else if (abs(diferenca) >= TOLERANCIA_MEDIA) {
        Serial.println("Sensor Muito Longe do Disco; Aproxime o Sensor!");
    } else {
        Serial.print("Distância Quase Perfeita (mais longe)! Tolerância: ");
        Serial.print(abs(diferenca));
        Serial.println("%");
    }
} else {
    Serial.println("Distância Perfeita entre o Sensor e o Disco!");
}
/* */
}
