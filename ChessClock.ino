 //Autor: Matheus Silva dos Santos
 
 //Pinos utilizados para os Botoes e as LEDs
  #define pinoBotaoPretas 2
  #define pinoBotaoBrancas 3
  #define pinoLedBrancas 10
  #define pinoLedPretas 8

  //Variaveis Globais
  int cont = 0;                             //contador do estouro do temporizador
  int primeiroPrintBrancas = 0;             //Flag para printar o tempo inicial do jogador de Brancas, no nosso caso "10:00", no Monitor Serial
  int primeiroPrintPretas = 0;              //Flag para printar o tempo inicial do jogador de Pretas, no nosso caso "10:00", no Monitor Serial
  int temporizadorBrancas = 600;            //Temporizador do jogador de Brancas (10min = 600s)
  int temporizadorPretas = 600;             //Temporizador do jogador de Pretas (10min = 600s)
  volatile boolean ledOnBrancas = false;    //Flag para habilitar ou desabilitar o LED do jogador de Brancas
  volatile boolean ledOnPretas = false;     //Flag para habilitar ou desabilitar o LED do jogador de Pretas
  volatile boolean printFlag = false;       //Flag para printar o temporizador de um dos jogadores no Monitor Serial
  volatile boolean jogadorBrancas = false;  //Flag para printar que será a vez do jogador de Brancas no Monitor Serial
  volatile boolean jogadorPretas = false;   //Flag para printar que será a vez do jogador de Pretas no Monitor Serial
  
  void setup() {
    Serial.begin(9600);
    cli();   // desabilita interrupcao global
    
    pinMode(pinoLedBrancas,OUTPUT);
    pinMode(pinoLedPretas,OUTPUT);
    pinMode(pinoBotaoBrancas,INPUT_PULLUP);
    pinMode(pinoBotaoPretas,INPUT_PULLUP);
    digitalWrite(pinoLedBrancas, LOW);
    digitalWrite(pinoLedPretas, LOW);  
  
    attachInterrupt(digitalPinToInterrupt(pinoBotaoBrancas),botaoBrancas, RISING);  //Interrupção externa para o Botao do jogador de Brancas
    attachInterrupt(digitalPinToInterrupt(pinoBotaoPretas),botaoPretas, RISING);    //Interrupção externa para o Botao do jogador de Pretas
   
    TCCR2A = 0;                                   // zera registrador TCCR2A
    TCNT2 = 0x06;                                 // Inicia o Timer2 em 6
    TCCR2B = ((TCCR2B & B11111000) | B00000110);  // prescaler 256
    TIMSK2 |= 0x01;                               // Seta o Bit menos significativo
    sei();                                        // habilita interrupcao global
  }
  
  void loop() {
    if(ledOnBrancas){                    //Se a Flag do LED do jogador de Brancas estiver como "true", liga o LED do jogador de Brancas e printa o seu temporizador
      digitalWrite(pinoLedBrancas,HIGH);
      if(jogadorBrancas){                //Printa no Monitor Serial que será a vez do jogador de Brancas
        Serial.println("Vez do Jogador de Brancas");
        jogadorBrancas = !jogadorBrancas;
      }
      if(primeiroPrintBrancas == 0){     //Printa o tempo inicial do jogador de Brancas
        imprime(temporizadorBrancas);
        primeiroPrintBrancas = 1;
      }
      if(printFlag){                     //printa o temporizador do jogador de Brancas no Monitor Serial                  
        imprime(temporizadorBrancas);
        printFlag = !printFlag;
      }
    }
    
    if(!ledOnBrancas) digitalWrite(pinoLedBrancas,LOW); //Se a Flag do LED do jogador de Brancas estiver como "false", desliga o LED do jogador de Brancas
    
    if(ledOnPretas){                    //Se a Flag do LED do jogador de Pretas estiver como "true", liga o LED do jogador de Pretas e printa o seu temporizador
      digitalWrite(pinoLedPretas,HIGH);
      if(jogadorPretas){                //Printa no Monitor Serial que será a vez do jogador de Pretas
        Serial.println("Vez do Jogador de Pretas");
        jogadorPretas = !jogadorPretas;
      }
      if(primeiroPrintPretas == 0){     //Printa o tempo inicial do jogador de Pretas
        imprime(temporizadorPretas);
        primeiroPrintPretas = 1;
      }
      if(printFlag){                    //printa o temporizador do jogador de Pretas no Monitor Serial
        imprime(temporizadorPretas);
        printFlag = !printFlag; 
      }
    }
    
    if(!ledOnPretas) digitalWrite(pinoLedPretas,LOW); //Se a Flag do LED do jogador de Pretas estiver como "false", desliga o LED do jogador de Pretas

    //Caso algum temporizador zere, os tempos de cada jogador será printado e será finalizado o jogo!
    if((temporizadorBrancas == 0) or (temporizadorPretas == 0)){
      Serial.println("O temporizador de um dos jogadores zerou!");
      Serial.println("---------------------------------------------");
      Serial.print("Temporizador do jogador de Brancas: ");
      imprime(temporizadorBrancas);
      Serial.print("Temporizador do jogador de Pretas: ");
      imprime(temporizadorPretas);
      Serial.println("---------------------------------------------");
      Serial.println("JOGO FINALIZADO!");
      while(1);
    }
  }
  
  void botaoBrancas()
  {
    if(digitalRead(pinoLedPretas) != 1){ //Altera a Flag do LED do jogador de Pretas para "true" e a Flag do LED do jogador de Brancas para "false"
                                         //Caso o pino da Led do jogador de Pretas não esteja em HIGH
      ledOnPretas = !ledOnPretas;
      ledOnBrancas = !ledOnPretas;
      jogadorPretas = !jogadorPretas;
    }
  }
  
  void botaoPretas()
  {
    if(digitalRead(pinoLedBrancas) != 1){ //Altera a Flag do LED do jogador de Brancas para "true" e a Flag do LED do jogador de Pretas para "false"
                                          //Caso o pino da Led do jogador de Brancas não esteja em HIGH
      ledOnBrancas = !ledOnBrancas;
      ledOnPretas = !ledOnBrancas;
      jogadorBrancas = !jogadorBrancas;
    }
  }

  //Interrupção de Timer
  //Estou utilizando o "Timer 2" tanto para o temporizador do jogador de Brancas quanto para o jogador de Pretas
  ISR(TIMER2_OVF_vect) {
    if(digitalRead(pinoLedBrancas) == 1)  //verifica se o pino do Led do jogador de Brancas está em HIGH
      cont++;                             //incrementa o contador do estouro do temporizador
      TCNT2 = 0x06;                       //Inicia o Timer2 em 6
      if (cont == 250) {                  //verifica se o contador do estourou do temporizador chegou em 250
        temporizadorBrancas --;           //decrementa o temporizador do jogador de Brancas
        cont = 0;                         //Zera o contador
        printFlag = !printFlag;           //altera a Flag para "true" para printar o temporizador do jogador de Brancas no Monitor Serial
      }
  
      if(digitalRead(pinoLedPretas) == 1) //verifica se o pino do Led do jogador de Pretas está em HIGH
      cont++;                             //incrementa o contador do estouro do temporizador
      TCNT2 = 0x06;                       //Inicia o Timer2 em 6
      if (cont == 250) {                  //verifica se o contador do estourou do temporizador chegou em 250
        temporizadorPretas--;             //decrementa o temporizador do jogador de Pretas
        cont = 0;                         //Zera o contador
        printFlag = !printFlag;           //altera a Flag para "true" para printar o temporizador do jogador de Pretas no Monitor Serial
      }
  }

  //Funcao para printar o temporizador dos jogadores
  //Com ela, evita que a parte do loop() do programa fique cheio de "Serial.prints"
  void imprime(int temp){
    if((temp / 60) < 10)Serial.print("0");
    Serial.print(temp / 60);
    Serial.print(":");
    if((temp % 60) < 10)Serial.print("0"); 
    Serial.println(temp % 60);
  }
