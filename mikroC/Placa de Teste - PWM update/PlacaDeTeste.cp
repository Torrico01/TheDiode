#line 1 "C:/Users/ticto/Documents/Projetos/Projetos mikroC/Painel Modular/Placa de Teste/PlacaDeTeste.c"
#line 15 "C:/Users/ticto/Documents/Projetos/Projetos mikroC/Painel Modular/Placa de Teste/PlacaDeTeste.c"
volatile  unsigned char  buff, letter_counter, read_req;
volatile  unsigned char  rxbuffer[25];
#line 39 "C:/Users/ticto/Documents/Projetos/Projetos mikroC/Painel Modular/Placa de Teste/PlacaDeTeste.c"
void initPinos();
void initI2c();
void initPwm();

void changeColor( unsigned char ,  unsigned char ,  unsigned char ,  unsigned char ,  unsigned char );
void setColor( unsigned char ,  unsigned char );


void interrupt(){

 if (PIR1.SSPIF){

 PIR1.SSPIF = 0;


 if (SSPSTAT.R_W){

 if(read_req) letter_counter = 0;
 read_req = 0;

 SSPBUF = rxbuffer[letter_counter];
 letter_counter++;

 SSPCON.CKP = 1;
 buff = SSPBUF;

 }


 if(!SSPSTAT.R_W & SSPSTAT.D_A & SSPSTAT.BF ){

  unsigned char  tempBuff = SSPBUF;
 if (tempBuff != 0x22) {

 rxbuffer[letter_counter] = tempBuff;
 letter_counter++;

 }

 read_req = 1;
 }

 if(!SSPSTAT.R_W & !SSPSTAT.D_A & SSPSTAT.BF ){

 buff = SSPBUF;

 letter_counter = 0;

 }
 }
}


void initPinos(){
 ADCON1 = 0x07;

 TRISA = 0x00;
 TRISB = 0x00;
 TRISC = 0x00;

 PORTA = 0x3F;
 PORTB = 0x00;
 PORTC = 0xE0;

 TRISC.B3 = 1;
 TRISC.B4 = 1;
}

void initI2c(){


 INTCON = 0b00000000;
 SSPCON = 0b00111110;
 SSPCON2 = 0x00;
 SSPADD = 0xB0;
 SSPSTAT = 0x00;
 SSPCON.SSPEN = 1;
 SSPSTAT.SMP = 1;
 SSPSTAT.CKE = 1;
 PIR1.SSPIF = 0;
 PIR2.BCLIF = 0;

 PIE1.SSPIE = 1;
 INTCON.PEIE = 1;
 INTCON.GIE = 1;
}

void initPwm(){


 PWM1_Init(6500);
 PWM2_Init(6500);

 PWM1_Start();
 PWM2_Start();

 PWM1_Set_Duty(0);
 PWM2_Set_Duty(0);

}



void changeColor( unsigned char  redLeds[],  unsigned char  greenLeds[],  unsigned char  red,  unsigned char  green,  unsigned char  led) {
 if ( redLeds[led] + 4 > red ) redLeds[led] = red;
 if ( redLeds[led] - 4 < 0 ) redLeds[led] = 0;
 if ( greenLeds[led] + 4 > green ) greenLeds[led] = green;
 if ( greenLeds[led] - 4 < 0 ) greenLeds[led] = 0;

 if ( redLeds[led] < red ) redLeds[led] += 4;
 if ( redLeds[led] > red ) redLeds[led] -= 4;

 if ( greenLeds[led] < green ) greenLeds[led] += 4;
 if ( greenLeds[led] > green ) greenLeds[led] -= 4;

}

void setColor( unsigned char  RGB_COM[],  unsigned char  led) {
 delay_us(200);
 if (led > 5) {
 TRISC = TRISC & RGB_COM[led];
 PORTC = PORTC & RGB_COM[led];
 }
 else {
 TRISA = TRISA & RGB_COM[led];
 PORTA = PORTA & RGB_COM[led];
 }
 delay_us(80);
}

int map(int x, int in_min, int in_max, int out_min, int out_max)
{
 return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


void main(){


  unsigned char  rgb_pos;
  unsigned char  disp_pos, disp_num, DISP_COM[4] = {0b00111110, 0b00111011, 0b00110111, 0b00111101};




  unsigned char  i;
  unsigned char  time_array[4];
  unsigned char  led_selected[4];
  unsigned char  comp_quantity[4];
  unsigned char  selection_mode[4];
  unsigned char  comp_all_limits[18];
  unsigned char  comp_all_quantities[18];

  unsigned char  redLeds[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
  unsigned char  greenLeds[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
  unsigned char  RGB_COM[9] = {0b00111110, 0b00101111, 0b00011111, 0b00111011, 0b00110111, 0b00111101, 0b01111111, 0b10111111, 0b11011111};

 const  unsigned char  segments[11] = {0x5F, 0x44, 0x9D, 0xD5, 0xC6, 0xD3, 0xDB, 0x45, 0xDF, 0xD7, 0x00};







 disp_num = 0;
 disp_pos = 0;
 rgb_pos = 0;

 letter_counter = 0;





 initI2c();
 initPwm();
 initPinos();

 for (i = 0; i < 4; i++) time_array[i] = 0x30;
 for (i = 0; i < 4; i++) led_selected[i] = 0x31;
 for (i = 0; i < 4; i++) comp_quantity[i] = 0x30;
 for (i = 0; i < 4; i++) selection_mode[i] = 0x30;
 for (i = 0; i < 18; i++) comp_all_limits[i] = 0x30;
 for (i = 0; i < 18; i++) comp_all_quantities[i] = 0x30;


 for (i = 0; i < 25; i++) rxbuffer[i] = 0x30;
 rxbuffer[0] =  'H' ;

 while (1){

 if(rxbuffer[0] ==  'S'  && rxbuffer[5] ==  'S' ) {
 for (i = 0; i < 4; i++) selection_mode[i] = rxbuffer[i+1];
 }
 else if(rxbuffer[0] ==  'C'  && rxbuffer[5] ==  'C' ) {
 for (i = 0; i < 4; i++) led_selected[i] = rxbuffer[i+1];
 }
 else if (rxbuffer[0] ==  'H'  && rxbuffer[5] ==  'H' ) {
 for (i = 0; i < 4; i++) time_array[i] = rxbuffer[i+1];
 }
 else if (rxbuffer[0] ==  'A'  && rxbuffer[19] ==  'A' ) {
 for (i = 0; i < 18; i++) comp_all_quantities[i] = rxbuffer[i+1];
 }
 else if (rxbuffer[0] ==  'Q'  && rxbuffer[5] ==  'Q' ) {
 for (i = 0; i < 4; i++) comp_quantity[i] = rxbuffer[i+1];
 }
 else if (rxbuffer[0] ==  'L'  && rxbuffer[19] ==  'L' ) {
 for (i = 0; i < 18; i++) comp_all_limits[i] = rxbuffer[i+1];
 }



 if (selection_mode[0] == 0x30 || selection_mode[0] == 0x32 || selection_mode[0] == 0x33) {
 if (comp_all_quantities[2*rgb_pos] >= 0x30 && comp_all_quantities[2*rgb_pos] <= 0x39 && comp_all_quantities[2*rgb_pos + 1] >= 0x30 && comp_all_quantities[2*rgb_pos + 1] <= 0x39) {
 int int_quantity = 10*(comp_all_quantities[2*rgb_pos] - 0x30) + comp_all_quantities[2*rgb_pos + 1] - 0x30;
 int int_limit = 10*(comp_all_limits[2*rgb_pos] - 0x30) + comp_all_limits[2*rgb_pos + 1] - 0x30;

 if (int_quantity == 0) {
 redLeds[rgb_pos] = 0;
 greenLeds[rgb_pos] = 255;
 }
 else if (int_quantity >= int_limit) {
 redLeds[rgb_pos] = 255;
 greenLeds[rgb_pos] = 0;
 }
 else {
 int green_value = map(int_quantity, 0, int_limit, 150, 30);
 redLeds[rgb_pos] = 255;
 greenLeds[rgb_pos] = green_value;
 }
 }
 }
#line 315 "C:/Users/ticto/Documents/Projetos/Projetos mikroC/Painel Modular/Placa de Teste/PlacaDeTeste.c"
 TRISA = TRISA | 0b00111111;
 TRISC = TRISC | 0b11100000;
 PWM1_Set_Duty(redLeds[rgb_pos]);
 PWM2_Set_Duty(greenLeds[rgb_pos]);

 if (selection_mode[0] == 0x30) PORTC.B0 = 0;
 if (selection_mode[0] == 0x31) PORTC.B0 = 1;
 if (selection_mode[0] == 0x32) {
 if (led_selected[0] > 0x30 && led_selected[0] <= 0x39) {
 if (rgb_pos == led_selected[0] - 0x31) PORTC.B0 = 1;
 else PORTC.B0 = 0;
 }
 }

 setColor(RGB_COM, rgb_pos);
 PWM1_Set_Duty(0);
 PWM2_Set_Duty(0);
 PORTC.B0 = 0;
 delay_us(30);

 rgb_pos++;
 if (selection_mode[0] == 0x33 && led_selected[0] > 0x30 && led_selected[0] <= 0x39) {
 rgb_pos = led_selected[0] - 0x31;
 }
 if (rgb_pos > 8) rgb_pos = 0;




 if (selection_mode[0] == 0x32 || selection_mode[0] == 0x33) {
 if (comp_quantity[0] >= 0x30 && comp_quantity[0] <= 0x39) {
 if (disp_pos == 0) disp_num = 0;
 else if (disp_pos == 1) disp_num = 0;
 else if (disp_pos == 2) disp_num = comp_quantity[0] - 0x30;
 else if (disp_pos == 3) disp_num = comp_quantity[1] - 0x30;
 }
 }
 else {
 if (time_array[disp_pos] >= 0x30 && time_array[disp_pos] <= 0x39) disp_num = time_array[disp_pos] - 0x30;
 }
 TRISA = TRISA | 0b00111111;
 TRISC = TRISC | 0b11100000;
 PORTB = segments[disp_num];
 if(disp_pos == 1) PORTB = PORTB | 0b00100000;


 delay_us(15);
 TRISA = TRISA & DISP_COM[disp_pos];
 PORTA = PORTA & DISP_COM[disp_pos];
 disp_pos++;
 if (disp_pos > 3) disp_pos = 0;


 delay_us(5);
 PORTB = 0x00;


 }
}
