#define TRUE 1
#define FALSE 0
#define byte unsigned char

// Definicoes de identificadores na comunicaco I2C
#define id_Hora 'H' // Horario atual
#define id_Slct 'S' // Modo de selecao
#define id_Comp 'C' // Compartimento de componentes
#define id_AQtd 'A' // Todas as quantidades dos componentes
#define id_Qtd  'Q' // Quantidade de componentes no compartimento
#define id_Lim  'L' // Limite da quantidade de componentes

// ---------- Variáveis globais ----------
// I2C
volatile byte buff, letter_counter, read_req;
volatile byte rxbuffer[25];
// RGB
//byte fading_in = TRUE;// Fading RGB
//byte redLeds[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
//byte greenLeds[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
//byte RGB_COM[9] = {0b00111110, 0b00101111, 0b00011111, 0b00111011, 0b00110111, 0b00111101, 0b01111111, 0b10111111, 0b11011111}; // RA0, RA4, RA5, RA2, RA3, RA1, RC7, RC6, RC5 //16F877A: {0b11111110, 0b11111101, 0b11111011, 0b11011111, 0b10111111, 0b01111111};
// Display                                            dp  g   f    e   d   c   b   a
//     RB6 RB5 RB4 RB3 RB2 RB1 RB0
//     g   f   e   d   c   b   a                      g   c   dp   d   e   b   f   a
//  0: 0   1   1   1   1   1   1   0x3F            0: 0   1   0    1   1   1   1   1   0x5F
//  1: 0   0   0   0   1   1   0   0x06            1: 0   1   0    0   0   1   0   0   0x44
//  2: 1   0   1   1   0   1   1   0x5B            2: 1   0   0    1   1   1   0   1   0x9D
//  3: 1   0   0   1   1   1   1   0x4F            3: 1   1   0    1   0   1   0   1   0xD5
//  4: 1   1   0   0   1   1   0   0x66    =>      4: 1   1   0    0   0   1   1   0   0xC6
//  5: 1   1   0   1   1   0   1   0x6D            5: 1   1   0    1   0   0   1   1   0xD3
//  6: 1   1   1   1   1   0   1   0x7D            6: 1   1   0    1   1   0   1   1   0xDB
//  7: 0   0   0   0   1   1   1   0x07            7: 0   1   0    0   0   1   0   1   0x45
//  8: 1   1   1   1   1   1   1   0x7F            8: 1   1   0    1   1   1   1   1   0xDF
//  9: 1   1   0   1   1   1   1   0x6F            9: 1   1   0    1   0   1   1   1   0xD7
// BL: 0   0   0   0   0   0   0   0x00           BL: 0   0   0    0   0   0   0   0   0x00
//const byte segments[11] = {0x5F, 0x44, 0x9D, 0xD5, 0xC6, 0xD3, 0xDB, 0x45, 0xDF, 0xD7, 0x00}; //{0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F, 0x00};

// Declaração de funções
void initPinos();
void initI2c();
void initPwm();

void changeColor(byte, byte, byte, byte, byte);
void setColor(byte, byte);

//==============================================================================
void interrupt(){
     // Interrupção MSSP (I2C)
     if (PIR1.SSPIF){

        PIR1.SSPIF = 0;                // Apagamos la bandera SSP
        
        // Transmitir para o mestre
        if (SSPSTAT.R_W){              // Read request from master
        
          if(read_req) letter_counter = 0;
          read_req = 0;
        
          SSPBUF = rxbuffer[letter_counter];  // Get data to send
          letter_counter++;
          
          SSPCON.CKP = 1;                     // Release SCL line
          buff = SSPBUF;                      // That's it

        }
        
        // Receber dados do mestre
        if(!SSPSTAT.R_W & SSPSTAT.D_A & SSPSTAT.BF ){

          byte tempBuff = SSPBUF;
          if (tempBuff != 0x22) {                      // Byte recebido diferente de "
          
            rxbuffer[letter_counter] = tempBuff;       // Guardamos um byte
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

//==============================================================================
void initPinos(){
     ADCON1 = 0x07;          // Todos los puertos como digitales
     //CMCON = 0x07;           // Desabilita comparadores
     TRISA = 0x00;           // Set PORTA as output
     TRISB = 0x00;           // Set PORTB as output
     TRISC = 0x00;           // Set PORTC as output
     //TRISD = 0b11100111;
     PORTA = 0x3F;
     PORTB = 0x00;
     PORTC = 0xE0;
     
     TRISC.B3 = 1;           // Set SCL (PORTC,3) pin como ingreso
     TRISC.B4 = 1;           // Set SDA (PORTC,4) pin como ingreso
}

void initI2c(){
     // Interrupção para comunicação I2C
     // Configuração do MSSP
     INTCON = 0b00000000;
     SSPCON = 0b00111110;         // Modo escravo 7 bits
     SSPCON2 = 0x00;
     SSPADD = 0xB0;               // Endereço
     SSPSTAT = 0x00;
     SSPCON.SSPEN = 1;
     SSPSTAT.SMP = 1;
     SSPSTAT.CKE = 1;
     PIR1.SSPIF = 0;
     PIR2.BCLIF = 0;

     PIE1.SSPIE = 1;         // Habilita interrupção do módulo SSP
     INTCON.PEIE = 1;        // Habilita interrupção dos periféricos
     INTCON.GIE = 1;         // Habilita interrupções globais
}

void initPwm(){
     // PWM para leds RGB
     // Configuração dos PWMs 1 e 2 (R & G) usando a biblioteca
     PWM1_Init(6500); // 1221
     PWM2_Init(6500); // 1221
     
     PWM1_Start();
     PWM2_Start();
     
     PWM1_Set_Duty(0);
     PWM2_Set_Duty(0);

}


//==============================================================================
void changeColor(byte redLeds[], byte greenLeds[], byte red, byte green, byte led) {
    if ( redLeds[led] + 4 > red ) redLeds[led] = red;
    if ( redLeds[led] - 4 < 0 ) redLeds[led] = 0;
    if ( greenLeds[led] + 4 > green ) greenLeds[led] = green;
    if ( greenLeds[led] - 4 < 0 ) greenLeds[led] = 0;
    
    if ( redLeds[led] < red ) redLeds[led] += 4;
    if ( redLeds[led] > red ) redLeds[led] -= 4;

    if ( greenLeds[led] < green ) greenLeds[led] += 4;
    if ( greenLeds[led] > green ) greenLeds[led] -= 4;

}

void setColor(byte RGB_COM[], byte led) {
    delay_us(200); // 150
    if (led > 5) {
      TRISC = TRISC & RGB_COM[led];
      PORTC = PORTC & RGB_COM[led];
    }
    else {
      TRISA = TRISA & RGB_COM[led];
      PORTA = PORTA & RGB_COM[led];
    }
    delay_us(80); // 80
}

int map(int x, int in_min, int in_max, int out_min, int out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


void main(){
     // Variáveis locais
     //int rgb_delay;
     byte rgb_pos;
     byte disp_pos, disp_num, DISP_COM[4] = {0b00111110, 0b00111011, 0b00110111, 0b00111101}; // RA0, RA2, RA3, RA1 //16f877A: {0b11111011, 0b11011111, 0b10111111, 0b01111111};
     //byte SCAN_RIGHT[9] = {6,3,0,7,4,1,8,5,2};
     //byte SCAN_BOTTOM[9] = {8,7,6,5,4,3,2,1,0};
     //byte SCAN_LEFT[9] = {2,5,8,1,4,7,0,3,6};
     //byte rgb_fading_type; //, RGB_SUM[9];
     byte i;
     byte time_array[4];
     byte led_selected[4];
     byte comp_quantity[4];
     byte selection_mode[4];
     byte comp_all_limits[18];
     byte comp_all_quantities[18];
     
     byte redLeds[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
     byte greenLeds[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
     byte RGB_COM[9] = {0b00111110, 0b00101111, 0b00011111, 0b00111011, 0b00110111, 0b00111101, 0b01111111, 0b10111111, 0b11011111};
     
     const byte segments[11] = {0x5F, 0x44, 0x9D, 0xD5, 0xC6, 0xD3, 0xDB, 0x45, 0xDF, 0xD7, 0x00};

     //byte execute_once = 0;
     //int duty1_, duty2_; // Duty cicles modificados
     //int duty1, duty2; // Duty cicles sincronos
     
     //rgb_fading_type = 1;
     //rgb_delay = 0;
     disp_num = 0;
     disp_pos = 0;
     rgb_pos = 0;

     letter_counter = 0;

     //duty1 = 0;
     //duty2 = 0;
     
     // Inicializações
     initI2c();
     initPwm();
     initPinos();
     
     for (i = 0; i < 4; i++) time_array[i] = 0x30;
     for (i = 0; i < 4; i++) led_selected[i] = 0x31;
     for (i = 0; i < 4; i++) comp_quantity[i] = 0x30;
     for (i = 0; i < 4; i++) selection_mode[i] = 0x30;
     for (i = 0; i < 18; i++) comp_all_limits[i] = 0x30;
     for (i = 0; i < 18; i++) comp_all_quantities[i] = 0x30;
     
     ////for (i = 0; i < 9; i++) RGB_SUM[i] = 20*i;
     for (i = 0; i < 25; i++) rxbuffer[i] = 0x30;
     rxbuffer[0] = id_Hora;

     while (1){
       // Salva variavel I2C corretamente
       if(rxbuffer[0] == id_Slct && rxbuffer[5] == id_Slct) {
         for (i = 0; i < 4; i++) selection_mode[i] = rxbuffer[i+1];
       }
       else if(rxbuffer[0] == id_Comp && rxbuffer[5] == id_Comp) {
         for (i = 0; i < 4; i++) led_selected[i] = rxbuffer[i+1];
       }
       else if (rxbuffer[0] == id_Hora && rxbuffer[5] == id_Hora) {
         for (i = 0; i < 4; i++) time_array[i] = rxbuffer[i+1];
       }
       else if (rxbuffer[0] == id_AQtd && rxbuffer[19] == id_AQtd) {
         for (i = 0; i < 18; i++) comp_all_quantities[i] = rxbuffer[i+1];
       }
       else if (rxbuffer[0] == id_Qtd && rxbuffer[5] == id_Qtd) {
         for (i = 0; i < 4; i++) comp_quantity[i] = rxbuffer[i+1];
       }
       else if (rxbuffer[0] == id_Lim && rxbuffer[19] == id_Lim) {
         for (i = 0; i < 18; i++) comp_all_limits[i] = rxbuffer[i+1];
       }

        // -------------------- Selection --------------------
       // Display occupation
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
     
       // -------------------- RGB --------------------
       /*
       // Verifica o ciclo do PWM
       if (rgb_fading_type == 1) {
         redLeds[rgb_pos] = duty1;
         if (duty1 == 255) rgb_fading_type = 2;
       }
       if (rgb_fading_type == 2) {
         redLeds[rgb_pos] = duty1;
         if (duty1 == 0) rgb_fading_type = 3;
       }
       if (rgb_fading_type == 3) {
         greenLeds[rgb_pos] = duty2;
         if (duty2 == 255) rgb_fading_type = 4;
       }
       if (rgb_fading_type == 4) {
         greenLeds[rgb_pos] = duty2;
         if (duty2 == 0) rgb_fading_type = 5;
       }
       
       if (rgb_fading_type == 5) {
         if (rgb_pos == 0 || rgb_pos == 1 || rgb_pos == 2) {
           redLeds[rgb_pos] = duty1;
           if (duty2 < 30) greenLeds[rgb_pos] = duty2;
           else greenLeds[rgb_pos] = 30;
         }
         if (rgb_pos == 3 || rgb_pos == 4 || rgb_pos == 5) {
           redLeds[rgb_pos] = duty1;
           if (duty2 < 60) greenLeds[rgb_pos] = duty2;
           else greenLeds[rgb_pos] = 70;
         }
         if (rgb_pos == 6 || rgb_pos == 7 || rgb_pos == 8) {
           redLeds[rgb_pos] = duty1;
           if (duty2 < 90) greenLeds[rgb_pos] = duty2;
           else greenLeds[rgb_pos] = 110;
         }
         if (duty1 == 255) rgb_fading_type = 6;
       }
       */
       

       // Seta PWMs
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
       delay_us(30); // 30
       
       rgb_pos++;
       if (selection_mode[0] == 0x33 && led_selected[0] > 0x30 && led_selected[0] <= 0x39) {
         rgb_pos = led_selected[0] - 0x31;
       }
       if (rgb_pos > 8) rgb_pos = 0;

       // -------------------- DISLPAY --------------------
       //if (selection_mode[0] > 0x30 && (rgb_pos == 1 || rgb_pos == 2 || rgb_pos == 6 || rgb_pos == 7) ) {
       // Muda o número do display
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

       // Acende o display correspondente à posição
       delay_us(15); //
       TRISA = TRISA & DISP_COM[disp_pos];
       PORTA = PORTA & DISP_COM[disp_pos];
       disp_pos++;
       if (disp_pos > 3) disp_pos = 0;

       // Reseta configurações de pinos para acender LED RGB
       delay_us(5); //2
       PORTB = 0x00;
       //}

     }
}