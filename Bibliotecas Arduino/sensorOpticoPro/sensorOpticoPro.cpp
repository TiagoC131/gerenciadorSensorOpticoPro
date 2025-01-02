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
	: _pinoSensor(pinoSensor), _estadoAnterior(LOW), _tempoAlto(0)
{
	// Configura o pino como entrada
        pinMode(_pinoSensor, INPUT);
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
    _rpmMaximo = config_rpmInicial;

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
    return _rpmMaximo;
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
void sensorOpticoPro::novoRpmMaximo(uint16_t novoRPM) 
{
	/*
	if (novoRPM < 0 || novoRPM > 65535) {
        Serial.print("Erro: O valor para 'rpmDesejado' deve estar entre 1 e 1500.");
        Serial.print(" Valor fornecido: ");
        Serial.println(novoRPM);
        return; // Saída antecipada da função em caso de erro
    } /* */

    _rpmMaximo = novoRPM;
	calcularTempoMinimoEntrePulsacoes();
    _limiarCalculado = false; // Calcular o Limiar novamente.. (função calcularLimiarTempo)
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
	configurarParametrosSensorOptico(36, 1000); //Configura novo Número de Riscos do Disco e Rpm Solicitado caso seja nescessario...
	_instanteInicial = millis(); // Serve para marcar o instante exato em que o programa começa a ser executado.
    _instanteRpmInicial = millis(); // Inicializa _instanteRpmInicial
    _velocidadeAngular = 0.0;       // Inicializa _velocidadeAngular
	_limiarPulsacoes = 0; 
	_fatorAjusteLimiar = 1.0; 
	_tempoMinimoEntrePulsacoes = 0; 
	_anguloAtual = 0.0; 
	_rpmAtual = 0; 
	_rpmAtualTemporario = _rpmMaximo; 
	_NUM_AMOSTRAS_calcLimiar = 100;
	_NUM_AMOSTRAS_detecMov = 100;
	_estadoAnterior = -1; // Armazena o tempo alto anterior
	_tempoAlto = 0;; // Armazena o tempo alto 
	_tempoBaixo = 0;; // Armazena o tempo baixo 
	_tempoAnterior = 0; // Declara _tempoAnterior
	_limiarPulsacoes = 0;

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
	float tempoPorPulso = 60000.0 / (_rpmMaximo * _numRiscos);
	//A constante 60000.0 na fórmula representa o número de milissegundos em um minuto. (Não deve ser Alterada, ela representa a conversão de minutos para milissegundos)
	
	// Adicionando uma margem de segurança e arredondando para cima
	_tempoMinimoEntrePulsacoes = ceil(tempoPorPulso * 1.2); // Ajuste o fator de segurança conforme necessário

	///* Apenas para Depuração... */ Serial.println("Calculo de Tempo Minimo Finalizado...");
}

/*
//Calcular Limiar de acordo com o RPM solicitado e o Número de Riscos do Disco Decodificador.
unsigned long calcularLimiarTempo() {
	calcularTempoMinimoEntrePulsacoes ();

	float tempoPorRotacao = 60.0 / _tempoMinimoEntrePulsacoes;
	float tempoPorPulso = tempoPorRotacao / _numRiscos;
	unsigned long limiarTempo = tempoPorPulso * 1000000; // convertendo para microssegundos
	limiarTempo *= 1.2; // adicionando 20% de margem de segurança (ajuste conforme necessário)
	return limiarTempo;
} /* */

//Calcula o limiar ideal para detectar pulsos, utilizando um filtro de média móvel e desvio padrão.
void sensorOpticoPro::calcularLimiarIdeal() {
    /*
    Esta função calcula o limiar ideal para detecção de pulsos,
    usando a média e o desvio padrão de um conjunto de amostras
    do valor *bruto* do sensor.
    */

    // Coletar dados *brutos* do sensor (HIGH/LOW)
    for (int i = 0; i < _NUM_AMOSTRAS_calcLimiar; i++) {
        _amostras_calcLimiar[i] = digitalRead(_pinoSensor); // Lê diretamente o pino
        delayMicroseconds(50); // Adiciona um pequeno delay para espaçar as leituras (opcional)
    }

    // Calcular estatísticas dos dados coletados
    double media = calcularMedia(_amostras_calcLimiar, _NUM_AMOSTRAS_calcLimiar);
    double desvioPadrao = calcularDesvioPadrao(_amostras_calcLimiar, media, _NUM_AMOSTRAS_calcLimiar);

    // Calcular o limiar com base nas estatísticas e no fator de ajuste
    _limiarPulsacoes = static_cast<uint8_t>(media + _fatorAjusteLimiar * desvioPadrao);

	///* Apenas para Depuração... */ Serial.print("O Valor Limiar Ideal é: ");
	///* Apenas para Depuração... */ Serial.println(_limiarPulsacoes);
}

		/************************************** Funções Auxiliares - calcularLimiarIdeal **************************************/

		TemposPulso sensorOpticoPro::lerValorPulso() {
		// Esta função lê o valor do pulso (tempo entre duas bordas de subida e descida) do sensor óptico.
		static unsigned long tempoUltimaSubida = 0;
		static unsigned long tempoUltimaDescida = 0;
		static bool estadoAnterior = LOW;

		unsigned long tempoAtual = micros();
		bool estadoAtual = digitalRead(_pinoSensor);

		TemposPulso tempos;
		tempos.tempoSubida = 0;
		tempos.tempoDescida = 0;

		if (estadoAtual && !estadoAnterior) { // Borda de subida
			tempos.tempoSubida = tempoAtual - tempoUltimaSubida;
			tempoUltimaSubida = tempoAtual;
			tempoUltimaDescida = 0; // Reseta o tempo de descida para evitar leituras incorretas
		} else if (!estadoAtual && estadoAnterior) { // Borda de descida
			tempos.tempoDescida = tempoAtual - tempoUltimaDescida;
			tempoUltimaDescida = tempoAtual;
			tempoUltimaSubida = 0; // Reseta o tempo de subida
		}

		estadoAnterior = estadoAtual;
		return tempos;

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

    Serial.println("Função iniciar Sensor Optico iniciada!"); 

    // Leitura inicial para obter uma estimativa do RPM e limiar
    for (int i = 0; i < 10; i++) { //Inicia um loop que será executado 10 vezes (O objetivo deste loop é coletar dados iniciais do sensor para calcular valores como RPM e limiar.).
		Serial.println("Obtendo Estimativa!"); 
	    lerValorPulso(); // Realiza uma leitura do sensor e armazenar os dados.
        calcularRPM(); // Calcula o RPM com base nos dados coletados.
    }

	Serial.println("Estimativa Obitida!"); 
    // Ajuste gradual do RPM desejado
	int rpmInicial = _rpmMaximo;
    for (int i = 0; i < 10; i++) { // Inicia um novo loop para ajustar gradualmente o RPM desejado.
        int novoRpm = rpmInicial * (i + 1) / 10; // Chama a função novoRpmMaximo para atualizar o RPM desejado para um valor um pouco maior a cada iteração. O cálculo aumenta o RPM gradualmente.
        _rpmAtualTemporario = novoRpm;
		
        // Verificar se o motor atingiu a velocidade desejada
        if (_rpmAtual >= _rpmMaximo * 0.95) {
            break; // Sai do loop se o motor estiver próximo da velocidade desejada
        }
	}



	/**********************************
		// Em outro ponto do seu código
	if (_rpmAtual < _rpmMaximo) {
		// Aumentar a velocidade do motor
	} else if (_rpmAtual > _rpmMaximo) {
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
    // Variável estática para armazenar o tempo do último pulso detectado em microssegundos.
    // "static" significa que a variável mantém seu valor entre as chamadas da função.
    static unsigned long tempoUltimoPulso = 0;

    // Obtém o tempo atual em microssegundos.
    unsigned long tempoAtual = micros();

    // Variável para armazenar o tempo decorrido entre dois pulsos consecutivos.
    unsigned long tempoDecorrido = 0;

    // Variável estática para armazenar o valor atual do RPM.
    // "static" garante que o valor seja mantido entre as chamadas da função.
    static float rpmAtual = 0;

    // Lê o estado atual do pino do sensor (HIGH ou LOW).
    bool estadoAtual_Sensor = digitalRead(_pinoSensor);

    // Variável estática para armazenar o estado anterior do pino do sensor.
    // Usada para detectar a transição de LOW para HIGH (subida do pulso).
    static bool estadoAnterior_Sensor = LOW;

    // Verifica se houve uma transição de LOW para HIGH no sinal do sensor.
    // Isso indica a detecção de um novo pulso.
    if (estadoAtual_Sensor == HIGH && estadoAnterior_Sensor == LOW) {
        // Calcula o tempo decorrido desde o último pulso.
        tempoDecorrido = tempoAtual - tempoUltimoPulso;

        // Atualiza o tempo do último pulso para o tempo atual.
        tempoUltimoPulso = tempoAtual;

        // Verifica se o tempo decorrido é maior que zero para evitar divisão por zero.
        if (tempoDecorrido > 0) {
            // Imprime o tempo decorrido em microssegundos para fins de debug.
            Serial.print("Tempo Decorrido (micros): ");
            Serial.println(tempoDecorrido);

            // Cálculo do RPM com prevenção de overflow e melhor precisão:
            // Usamos ponto flutuante desde o início para evitar perdas de precisão
            // e convertemos _numRiscos para float para evitar overflow na multiplicação

            // Converte o tempo decorrido de microssegundos para segundos.
            float tempoDecorridoSegundos = (float)tempoDecorrido / 1000000.0;

            // Calcula o RPM:
            // 60 segundos/minuto / (número de riscos * tempo entre pulsos em segundos)
            // A conversão de _numRiscos para float garante que a multiplicação seja feita em ponto flutuante,
            // evitando possível overflow se _numRiscos e tempoDecorridoSegundos fossem inteiros.
            rpmAtual = 60.0 / ((float)_numRiscos * tempoDecorridoSegundos);

            // Imprime o valor do RPM calculado para fins de debug.
            Serial.print("RPM: ");
            Serial.println(rpmAtual);
        }
    }

    // Atualiza o estado anterior do sensor para o estado atual para a próxima iteração.
    estadoAnterior_Sensor = estadoAtual_Sensor;

    // Retorna o valor atual do RPM calculado.
    return rpmAtual;
}



// Declaração da função para ajustar a distancia ideal entre Sensor Óptico e Disco Decodificador.
void sensorOpticoPro::ajustarDistanciaSensorOptico() {
	_estadoAtual = digitalRead(_pinoSensor); // Lê o estado atual do sensor.

	// Verifica se o limiar ideal já foi calculado. Se não, calcula e imprime.
	if (!_limiarCalculado) {
		// Chama a função para Calcular o Limiar Ideal.
		calcularLimiarIdeal(); 
        // Imprime o valor do limiar calculado no Serial Monitor.
		Serial.print(F("Limiar Calculado: "));
		Serial.println(_limiarPulsacoes);
		// Marca o limiar como calculado para evitar recalcular a cada chamada da função.
		_limiarCalculado = true;
	}

	const unsigned long tempoLimite = 1000000; // Define o tempo limite para o ajuste (1 segundos - corrigido para melhor performance).
	static unsigned long tempoInicioAjuste = 0; // Variáveis estáticas para controlar o tempo e o estado do ajuste.
	static bool ajusteIniciado = false;

    // Define o número de amostras para calcular a média dos tempos alto e baixo.
	const int NUM_AMOSTRAS = 100; 
	// Variáveis estáticas para armazenar a soma dos tempos alto e baixo e a contagem de amostras.
	static unsigned long somaTemposAlto = 0; // Soma dos tempos em que o sensor está em nível HIGH.
	static unsigned long somaTemposBaixo = 0; // Soma dos tempos em que o sensor está em nível LOW.
	static int contagem = 0;              // Contagem total de amostras coletadas.
	// Índice e buffer para armazenar as recomendações de ajuste (0: Afasta, 1: Aproxima).
	static int indiceBuffer = 0;
	static int bufferAjuste[NUM_AMOSTRAS];

	// Se o ajuste ainda não foi iniciado, inicializa as variáveis e o buffer.
	if (!ajusteIniciado) {
		// Registra o tempo atual como o tempo de início do ajuste.
		tempoInicioAjuste = micros();
		// Marca o ajuste como iniciado.
		ajusteIniciado = true;
		// Reinicializa o índice do buffer.
		indiceBuffer = 0;
		// Reinicializa as somas dos tempos.
		somaTemposAlto = 0;
		somaTemposBaixo = 0;
		// Reinicializa a contagem de amostras.
		contagem = 0;
		// Inicializa o buffer de ajuste com -1 para indicar que não há dados válidos ainda.
		for (int i = 0; i < NUM_AMOSTRAS; i++) {
		bufferAjuste[i] = -1;
		}
	}

	// Verifica se o tempo limite para o ajuste foi atingido (timeout de 1 segundo).
	if (micros() - tempoInicioAjuste >= tempoLimite) {
		// Se nenhum tempo alto ou baixo foi medido (sensor sempre no mesmo estado), exibe a mensagem de distância severa.
		if (_tempoAlto == 0 && _tempoBaixo == 0) {
		Serial.println(distanciaSevera); 
		} else if ((long)_tempoAlto > (long)_tempoBaixo) { 
			// Se o tempo HIGH for maior que o tempo LOW, o sensor está muito próximo.
			distanciaSevera = "Sensor Severamente Próximo... Afaste!";
		} else {
			// Se o tempo LOW for maior que o tempo HIGH, o sensor está muito longe.
			distanciaSevera = "Sensor Severamente Longe... Aproxime!";
		}
    	// Reseta as variáveis para a próxima vez que a função for chamada (reinicia o processo de ajuste).
		ajusteIniciado = false; //Reseta o ajuste para a proxima vez que for chamado
		_tempoAnterior = -1;
		_tempoAlto = 0;
		_tempoBaixo = 0;
		return; // Sai da função após o timeout.
	}

	// Verifica se houve uma mudança no estado do sensor (transição HIGH para LOW ou LOW para HIGH).
	if (_estadoAtual != _estadoAnterior) {
		// Obtém o tempo atual em microssegundos.
		unsigned long tempoAtual = micros();
		unsigned long tempoDecorrido;

    	// Se houver um tempo anterior registrado (não é a primeira leitura).
		if (_tempoAnterior != -1) {
			// Calcula o tempo decorrido desde a última mudança de estado.
			tempoDecorrido = tempoAtual - _tempoAnterior;

			// Reinicializa os tempos alto e baixo para a nova medição.
			_tempoAlto = 0;
			_tempoBaixo = 0;

		// Determina se o tempo decorrido é um tempo alto (sensor HIGH) ou baixo (sensor LOW).
		if (_estadoAnterior == 1) {
			
			_tempoBaixo = tempoDecorrido;
		} else {
			_tempoAlto = tempoDecorrido;
		}

		// Acumula os tempos alto e baixo e incrementa a contagem de amostras.
		somaTemposAlto += _tempoAlto;
		somaTemposBaixo += _tempoBaixo;
		contagem++;

		// Armazena a recomendação de ajuste no buffer: 0 para Afastar, 1 para Aproximar.
		bufferAjuste[indiceBuffer] = (_tempoAlto < _tempoBaixo) ? 0 : 1; // 0: Afasta, 1: Aproxima
		// Atualiza o índice do buffer usando o operador módulo (%) para circular entre as posições do buffer.
		indiceBuffer = (indiceBuffer + 1) % NUM_AMOSTRAS;

		// Se o buffer estiver cheio (coletou NUM_AMOSTRAS amostras), calcula as médias e exibe a recomendação.
		if (indiceBuffer == 0 && ajusteIniciado) {
			unsigned long mediaTempoAlto = 0;
			unsigned long mediaTempoBaixo = 0;
			long mediaDiferenca = 0;

			// Calcula as médias dos tempos alto e baixo.
			if (contagem > 0) {
				mediaTempoAlto = somaTemposAlto / contagem;
				mediaTempoBaixo = somaTemposBaixo / contagem;
				mediaDiferenca = (long)mediaTempoAlto - (long)mediaTempoBaixo;
			}

			// Define a tolerância para considerar a distância como aceitável (diferença entre os tempos dentro dessa tolerância).
			const long TOLERANCIA_PERFEITA = 1000;

			// Exibe a recomendação com base na diferença entre os tempos médios.
			if (abs(mediaDiferenca) <= TOLERANCIA_PERFEITA) {
				Serial.print(F("Recomendação: Distância aceitável! "));
			} else if (mediaTempoAlto < mediaTempoBaixo) {
				Serial.print(F("Recomendação: Afaste! "));
			} else {
				Serial.print(F("Recomendação: Aproxime! "));
			}

			// Imprime os tempos médios e a diferença no Serial Monitor para depuração e acompanhamento.
			Serial.print(F("Tempo Alto Médio: "));
			Serial.print(mediaTempoAlto);
			Serial.print(F(" us, "));
			Serial.print(F("Tempo Baixo Médio: "));
			Serial.print(mediaTempoBaixo);
			Serial.print(F(" us, "));
			Serial.print(F("Diferença Média: "));
			Serial.print(mediaDiferenca);
			Serial.println(F(" us"));

			// Reseta as variáveis de soma e contagem para a próxima medição/ajuste.
			somaTemposAlto = 0;
			somaTemposBaixo = 0;
			contagem = 0;
      }
    }
	// Atualiza o tempo e o estado anterior para a próxima iteração.
    _tempoAnterior = tempoAtual;
    _estadoAnterior = _estadoAtual;
  }
}
