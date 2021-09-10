#include <LiquidCrystal.h>

LiquidCrystal lcd (6, 7, 8, 9, 10, 11);

//Desenho do carro
byte desenhoCarro[2][8] = {{0b11100, 0b11100, 0b11111, 0b00111, 0b00111, 0b11111, 0b11100, 0b11100},
  {0b11100, 0b11100, 0b11111, 0b11111, 0b11111, 0b11111, 0b11100, 0b11100}
};

int carros[10][2]; //Quantidade máxima de carros na tela = 10
int quantidadeCarros = 0;
int probabilidade = 15; //Chance de aparecer um carro no próximo bloco em porcentagem
int repeticaoMaxima = 2;
int velocidade = 300;

int btn = 4;
unsigned long tempoInicial;

void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(A0));
  pinMode(btn, INPUT);
  lcd.begin(16, 2);
  lcd.createChar(1, desenhoCarro[0]); //Cria o desenho do carro no LCD
  lcd.createChar(2, desenhoCarro[1]); //Cria a segunda parte
  limparArrayCarros(); //Limpar o array para evitar bugs
  adicionarCarro(1, 1); //Carro inicial controlado pelo jogador
  tempoInicial = micros();
  telaInicial();
}

void loop() {
  lcd.clear();
  desenharCarros();
  if (verificarColisao()) telaGameOver();
  moverCarros(velocidade);
  removerProximoCarro();
  adicionarCarros();
  mudarPosicaoCarro();
  delay(150);
}

void mudarPosicaoCarro() {
  if (digitalRead(btn)) {
    if (carros[0][0] == 1) carros[0][0] = 2; else carros[0][0] = 1;
    lcd.clear();
    desenharCarros();
    delay(100);
  }
}

void telaInicial(){
  lcd.clear();
  lcd.setCursor(5, 0);
  lcd.print("Start");
  lcd.setCursor(3, 1);
  lcd.print("Projetuino");
  delay(200);
  while(!digitalRead(btn)){}
  loop();
}

void telaGameOver() {
  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print("Game Over");
  while(!digitalRead(btn)){}
  limparArrayCarros();
  adicionarCarro(1, 1);
  telaInicial();
}

void desenharCarros() {
  for (int i = 0; i < quantidadeCarros; i++) {
    desenharCarro(carros[i][0], carros[i][1]);
  }
}

void desenharCarro(int linha, int coluna) {
  if (coluna >= 1) {
    lcd.setCursor(coluna - 1, linha - 1);
    lcd.write((byte)1);
    lcd.write((byte)2);
  } else if (coluna == 0) {
    lcd.setCursor(0, linha - 1);
    lcd.write((byte)2);
  }
}

void adicionarCarro(int linha, int coluna) {
  carros[quantidadeCarros][0] = linha;
  carros[quantidadeCarros][1] = coluna;
  quantidadeCarros++;
}

void adicionarCarros() {
  if ((15 - carros[quantidadeCarros - 1][1]) >= 3) {
    if (random(1, 101)<=probabilidade) {
      if (linhasRepetidas() <= repeticaoMaxima){
        adicionarCarro(random(1, 3), 16);
      } else if(carros[quantidadeCarros - 1][0]==1) {
        adicionarCarro(2, 16);
      } else {
        adicionarCarro(1, 16);
      }
    }
  }
}

int linhasRepetidas(){
  int repeticao = 1;
  int ultimaLinha = carros[quantidadeCarros - 1][0];
  while(carros[quantidadeCarros - 1 - repeticao][0]==ultimaLinha){
    repeticao++;
  }
  if (ultimaLinha == carros[0][0] && (quantidadeCarros -  repeticao) == 0) repeticao--;
  return repeticao;
}

void removerCarro(int numeroCarro) {
  if (numeroCarro <= quantidadeCarros) {
    for (int i = numeroCarro; i < quantidadeCarros; i++) {
      carros[i - 1][0] = carros[i][0];
      carros[i - 1][1] = carros[i][1];
    }
    carros[quantidadeCarros][0] = 0;
    carros[quantidadeCarros][1] = 0;
    quantidadeCarros--;
  }
}

void removerProximoCarro() {
  if (carros[1][1] < 0) {
    removerCarro(2);
  }
}

void moverCarro(int numeroCarro) {
  if (numeroCarro <= quantidadeCarros) {
    carros[numeroCarro - 1][1] = carros[numeroCarro - 1][1] - 1;
  }
}

void moverCarros(int tempo) {
  if ((micros() - tempoInicial) / 1000 >= tempo) {
    if (quantidadeCarros > 1) {
      for (int i = 2; i <= quantidadeCarros; i++) {
        moverCarro(i);
      }
    }
    tempoInicial = micros();
  }
}

bool verificarColisao() {
  for (int i = 1; i < quantidadeCarros; i++) {
    if (carros[i][0] == carros[0][0]) {
      if (carros[i][1] <= 2) {
        return true;
      }
    }
  }
  return false;
}

void limparArrayCarros() {
  for (int i = 0; i < 10; i++) {
    carros[i][0] = 0;
    carros[i][1] = 0;
    quantidadeCarros = 0;
  }
}
