
_interrupt:
	MOVWF      R15+0
	SWAPF      STATUS+0, 0
	CLRF       STATUS+0
	MOVWF      ___saveSTATUS+0
	MOVF       PCLATH+0, 0
	MOVWF      ___savePCLATH+0
	CLRF       PCLATH+0

;PlacaDeTeste.c,47 :: 		void interrupt(){
;PlacaDeTeste.c,49 :: 		if (PIR1.SSPIF){
	BTFSS      PIR1+0, 3
	GOTO       L_interrupt0
;PlacaDeTeste.c,51 :: 		PIR1.SSPIF = 0;                // Apagamos la bandera SSP
	BCF        PIR1+0, 3
;PlacaDeTeste.c,54 :: 		if (SSPSTAT.R_W){              // Read request from master
	BTFSS      SSPSTAT+0, 2
	GOTO       L_interrupt1
;PlacaDeTeste.c,56 :: 		if(read_req) letter_counter = 0;
	MOVF       _read_req+0, 0
	BTFSC      STATUS+0, 2
	GOTO       L_interrupt2
	CLRF       _letter_counter+0
L_interrupt2:
;PlacaDeTeste.c,57 :: 		read_req = 0;
	CLRF       _read_req+0
;PlacaDeTeste.c,59 :: 		SSPBUF = rxbuffer[letter_counter];  // Get data to send
	MOVF       _letter_counter+0, 0
	ADDLW      _rxbuffer+0
	MOVWF      FSR
	MOVF       INDF+0, 0
	MOVWF      R0+0
	MOVF       R0+0, 0
	MOVWF      SSPBUF+0
;PlacaDeTeste.c,60 :: 		letter_counter++;
	INCF       _letter_counter+0, 0
	MOVWF      R0+0
	MOVF       R0+0, 0
	MOVWF      _letter_counter+0
;PlacaDeTeste.c,62 :: 		SSPCON.CKP = 1;                     // Release SCL line
	BSF        SSPCON+0, 4
;PlacaDeTeste.c,63 :: 		buff = SSPBUF;                      // That's it
	MOVF       SSPBUF+0, 0
	MOVWF      _buff+0
;PlacaDeTeste.c,65 :: 		}
L_interrupt1:
;PlacaDeTeste.c,68 :: 		if(!SSPSTAT.R_W & SSPSTAT.D_A & SSPSTAT.BF ){
	BTFSC      SSPSTAT+0, 2
	GOTO       L__interrupt141
	BSF        3, 0
	GOTO       L__interrupt142
L__interrupt141:
	BCF        3, 0
L__interrupt142:
	BTFSS      3, 0
	GOTO       L__interrupt143
	BTFSS      SSPSTAT+0, 5
	GOTO       L__interrupt143
	BSF        113, 0
	GOTO       L__interrupt144
L__interrupt143:
	BCF        113, 0
L__interrupt144:
	BTFSS      113, 0
	GOTO       L__interrupt145
	BTFSS      SSPSTAT+0, 0
	GOTO       L__interrupt145
	BSF        3, 0
	GOTO       L__interrupt146
L__interrupt145:
	BCF        3, 0
L__interrupt146:
	BTFSS      3, 0
	GOTO       L_interrupt3
;PlacaDeTeste.c,70 :: 		byte tempBuff = SSPBUF;
	MOVF       SSPBUF+0, 0
	MOVWF      R2+0
;PlacaDeTeste.c,71 :: 		if (tempBuff != 0x22) {                      // Byte recebido diferente de "
	MOVF       R2+0, 0
	XORLW      34
	BTFSC      STATUS+0, 2
	GOTO       L_interrupt4
;PlacaDeTeste.c,73 :: 		rxbuffer[letter_counter] = tempBuff;       // Guardamos um byte
	MOVF       _letter_counter+0, 0
	ADDLW      _rxbuffer+0
	MOVWF      FSR
	MOVF       R2+0, 0
	MOVWF      INDF+0
;PlacaDeTeste.c,74 :: 		letter_counter++;
	INCF       _letter_counter+0, 0
	MOVWF      R0+0
	MOVF       R0+0, 0
	MOVWF      _letter_counter+0
;PlacaDeTeste.c,76 :: 		}
L_interrupt4:
;PlacaDeTeste.c,78 :: 		read_req = 1;
	MOVLW      1
	MOVWF      _read_req+0
;PlacaDeTeste.c,79 :: 		}
L_interrupt3:
;PlacaDeTeste.c,81 :: 		if(!SSPSTAT.R_W & !SSPSTAT.D_A & SSPSTAT.BF ){
	BTFSC      SSPSTAT+0, 2
	GOTO       L__interrupt147
	BSF        113, 0
	GOTO       L__interrupt148
L__interrupt147:
	BCF        113, 0
L__interrupt148:
	BTFSC      SSPSTAT+0, 5
	GOTO       L__interrupt149
	BSF        3, 0
	GOTO       L__interrupt150
L__interrupt149:
	BCF        3, 0
L__interrupt150:
	BTFSS      113, 0
	GOTO       L__interrupt151
	BTFSS      3, 0
	GOTO       L__interrupt151
	BSF        113, 0
	GOTO       L__interrupt152
L__interrupt151:
	BCF        113, 0
L__interrupt152:
	BTFSS      113, 0
	GOTO       L__interrupt153
	BTFSS      SSPSTAT+0, 0
	GOTO       L__interrupt153
	BSF        3, 0
	GOTO       L__interrupt154
L__interrupt153:
	BCF        3, 0
L__interrupt154:
	BTFSS      3, 0
	GOTO       L_interrupt5
;PlacaDeTeste.c,83 :: 		buff = SSPBUF;
	MOVF       SSPBUF+0, 0
	MOVWF      _buff+0
;PlacaDeTeste.c,85 :: 		letter_counter = 0;
	CLRF       _letter_counter+0
;PlacaDeTeste.c,87 :: 		}
L_interrupt5:
;PlacaDeTeste.c,88 :: 		}
L_interrupt0:
;PlacaDeTeste.c,89 :: 		}
L_end_interrupt:
L__interrupt140:
	MOVF       ___savePCLATH+0, 0
	MOVWF      PCLATH+0
	SWAPF      ___saveSTATUS+0, 0
	MOVWF      STATUS+0
	SWAPF      R15+0, 1
	SWAPF      R15+0, 0
	RETFIE
; end of _interrupt

_initPinos:

;PlacaDeTeste.c,92 :: 		void initPinos(){
;PlacaDeTeste.c,93 :: 		ADCON1 = 0x07;          // Todos los puertos como digitales
	MOVLW      7
	MOVWF      ADCON1+0
;PlacaDeTeste.c,95 :: 		TRISA = 0x00;           // Set PORTA as output
	CLRF       TRISA+0
;PlacaDeTeste.c,96 :: 		TRISB = 0x00;           // Set PORTB as output
	CLRF       TRISB+0
;PlacaDeTeste.c,97 :: 		TRISC = 0x00;           // Set PORTC as output
	CLRF       TRISC+0
;PlacaDeTeste.c,99 :: 		PORTA = 0x3F;
	MOVLW      63
	MOVWF      PORTA+0
;PlacaDeTeste.c,100 :: 		PORTB = 0x00;
	CLRF       PORTB+0
;PlacaDeTeste.c,101 :: 		PORTC = 0xE0;
	MOVLW      224
	MOVWF      PORTC+0
;PlacaDeTeste.c,103 :: 		TRISC.B3 = 1;           // Set SCL (PORTC,3) pin como ingreso
	BSF        TRISC+0, 3
;PlacaDeTeste.c,104 :: 		TRISC.B4 = 1;           // Set SDA (PORTC,4) pin como ingreso
	BSF        TRISC+0, 4
;PlacaDeTeste.c,105 :: 		}
L_end_initPinos:
	RETURN
; end of _initPinos

_initI2c:

;PlacaDeTeste.c,107 :: 		void initI2c(){
;PlacaDeTeste.c,110 :: 		INTCON = 0b00000000;
	CLRF       INTCON+0
;PlacaDeTeste.c,111 :: 		SSPCON = 0b00111110;         // Modo escravo 7 bits
	MOVLW      62
	MOVWF      SSPCON+0
;PlacaDeTeste.c,112 :: 		SSPCON2 = 0x00;
	CLRF       SSPCON2+0
;PlacaDeTeste.c,113 :: 		SSPADD = 0xB0;               // Endereço
	MOVLW      176
	MOVWF      SSPADD+0
;PlacaDeTeste.c,114 :: 		SSPSTAT = 0x00;
	CLRF       SSPSTAT+0
;PlacaDeTeste.c,115 :: 		SSPCON.SSPEN = 1;
	BSF        SSPCON+0, 5
;PlacaDeTeste.c,116 :: 		SSPSTAT.SMP = 1;
	BSF        SSPSTAT+0, 7
;PlacaDeTeste.c,117 :: 		SSPSTAT.CKE = 1;
	BSF        SSPSTAT+0, 6
;PlacaDeTeste.c,118 :: 		PIR1.SSPIF = 0;
	BCF        PIR1+0, 3
;PlacaDeTeste.c,119 :: 		PIR2.BCLIF = 0;
	BCF        PIR2+0, 3
;PlacaDeTeste.c,121 :: 		PIE1.SSPIE = 1;         // Habilita interrupção do módulo SSP
	BSF        PIE1+0, 3
;PlacaDeTeste.c,122 :: 		INTCON.PEIE = 1;        // Habilita interrupção dos periféricos
	BSF        INTCON+0, 6
;PlacaDeTeste.c,123 :: 		INTCON.GIE = 1;         // Habilita interrupções globais
	BSF        INTCON+0, 7
;PlacaDeTeste.c,124 :: 		}
L_end_initI2c:
	RETURN
; end of _initI2c

_initPwm:

;PlacaDeTeste.c,126 :: 		void initPwm(){
;PlacaDeTeste.c,129 :: 		PWM1_Init(6500); // 1221
	BSF        T2CON+0, 0
	BCF        T2CON+0, 1
	MOVLW      192
	MOVWF      PR2+0
	CALL       _PWM1_Init+0
;PlacaDeTeste.c,130 :: 		PWM2_Init(6500); // 1221
	BSF        T2CON+0, 0
	BCF        T2CON+0, 1
	MOVLW      192
	MOVWF      PR2+0
	CALL       _PWM2_Init+0
;PlacaDeTeste.c,132 :: 		PWM1_Start();
	CALL       _PWM1_Start+0
;PlacaDeTeste.c,133 :: 		PWM2_Start();
	CALL       _PWM2_Start+0
;PlacaDeTeste.c,135 :: 		PWM1_Set_Duty(0);
	CLRF       FARG_PWM1_Set_Duty_new_duty+0
	CALL       _PWM1_Set_Duty+0
;PlacaDeTeste.c,136 :: 		PWM2_Set_Duty(0);
	CLRF       FARG_PWM2_Set_Duty_new_duty+0
	CALL       _PWM2_Set_Duty+0
;PlacaDeTeste.c,138 :: 		}
L_end_initPwm:
	RETURN
; end of _initPwm

_changeColor:

;PlacaDeTeste.c,142 :: 		void changeColor(byte redLeds[], byte greenLeds[], byte red, byte green, byte led) {
;PlacaDeTeste.c,143 :: 		if ( redLeds[led] + 4 > red ) redLeds[led] = red;
	MOVF       FARG_changeColor_led+0, 0
	ADDWF      FARG_changeColor_redLeds+0, 0
	MOVWF      FSR
	MOVLW      4
	ADDWF      INDF+0, 0
	MOVWF      R1+0
	CLRF       R1+1
	BTFSC      STATUS+0, 0
	INCF       R1+1, 1
	MOVLW      128
	MOVWF      R0+0
	MOVLW      128
	XORWF      R1+1, 0
	SUBWF      R0+0, 0
	BTFSS      STATUS+0, 2
	GOTO       L__changeColor159
	MOVF       R1+0, 0
	SUBWF      FARG_changeColor_red+0, 0
L__changeColor159:
	BTFSC      STATUS+0, 0
	GOTO       L_changeColor6
	MOVF       FARG_changeColor_led+0, 0
	ADDWF      FARG_changeColor_redLeds+0, 0
	MOVWF      FSR
	MOVF       FARG_changeColor_red+0, 0
	MOVWF      INDF+0
L_changeColor6:
;PlacaDeTeste.c,144 :: 		if ( redLeds[led] - 4 < 0 ) redLeds[led] = 0;
	MOVF       FARG_changeColor_led+0, 0
	ADDWF      FARG_changeColor_redLeds+0, 0
	MOVWF      FSR
	MOVLW      4
	SUBWF      INDF+0, 0
	MOVWF      R1+0
	CLRF       R1+1
	BTFSS      STATUS+0, 0
	DECF       R1+1, 1
	MOVLW      128
	XORWF      R1+1, 0
	MOVWF      R0+0
	MOVLW      128
	SUBWF      R0+0, 0
	BTFSS      STATUS+0, 2
	GOTO       L__changeColor160
	MOVLW      0
	SUBWF      R1+0, 0
L__changeColor160:
	BTFSC      STATUS+0, 0
	GOTO       L_changeColor7
	MOVF       FARG_changeColor_led+0, 0
	ADDWF      FARG_changeColor_redLeds+0, 0
	MOVWF      FSR
	CLRF       INDF+0
L_changeColor7:
;PlacaDeTeste.c,145 :: 		if ( greenLeds[led] + 4 > green ) greenLeds[led] = green;
	MOVF       FARG_changeColor_led+0, 0
	ADDWF      FARG_changeColor_greenLeds+0, 0
	MOVWF      FSR
	MOVLW      4
	ADDWF      INDF+0, 0
	MOVWF      R1+0
	CLRF       R1+1
	BTFSC      STATUS+0, 0
	INCF       R1+1, 1
	MOVLW      128
	MOVWF      R0+0
	MOVLW      128
	XORWF      R1+1, 0
	SUBWF      R0+0, 0
	BTFSS      STATUS+0, 2
	GOTO       L__changeColor161
	MOVF       R1+0, 0
	SUBWF      FARG_changeColor_green+0, 0
L__changeColor161:
	BTFSC      STATUS+0, 0
	GOTO       L_changeColor8
	MOVF       FARG_changeColor_led+0, 0
	ADDWF      FARG_changeColor_greenLeds+0, 0
	MOVWF      FSR
	MOVF       FARG_changeColor_green+0, 0
	MOVWF      INDF+0
L_changeColor8:
;PlacaDeTeste.c,146 :: 		if ( greenLeds[led] - 4 < 0 ) greenLeds[led] = 0;
	MOVF       FARG_changeColor_led+0, 0
	ADDWF      FARG_changeColor_greenLeds+0, 0
	MOVWF      FSR
	MOVLW      4
	SUBWF      INDF+0, 0
	MOVWF      R1+0
	CLRF       R1+1
	BTFSS      STATUS+0, 0
	DECF       R1+1, 1
	MOVLW      128
	XORWF      R1+1, 0
	MOVWF      R0+0
	MOVLW      128
	SUBWF      R0+0, 0
	BTFSS      STATUS+0, 2
	GOTO       L__changeColor162
	MOVLW      0
	SUBWF      R1+0, 0
L__changeColor162:
	BTFSC      STATUS+0, 0
	GOTO       L_changeColor9
	MOVF       FARG_changeColor_led+0, 0
	ADDWF      FARG_changeColor_greenLeds+0, 0
	MOVWF      FSR
	CLRF       INDF+0
L_changeColor9:
;PlacaDeTeste.c,148 :: 		if ( redLeds[led] < red ) redLeds[led] += 4;
	MOVF       FARG_changeColor_led+0, 0
	ADDWF      FARG_changeColor_redLeds+0, 0
	MOVWF      FSR
	MOVF       FARG_changeColor_red+0, 0
	SUBWF      INDF+0, 0
	BTFSC      STATUS+0, 0
	GOTO       L_changeColor10
	MOVF       FARG_changeColor_led+0, 0
	ADDWF      FARG_changeColor_redLeds+0, 0
	MOVWF      R1+0
	MOVF       R1+0, 0
	MOVWF      FSR
	MOVLW      4
	ADDWF      INDF+0, 0
	MOVWF      R0+0
	MOVF       R1+0, 0
	MOVWF      FSR
	MOVF       R0+0, 0
	MOVWF      INDF+0
L_changeColor10:
;PlacaDeTeste.c,149 :: 		if ( redLeds[led] > red ) redLeds[led] -= 4;
	MOVF       FARG_changeColor_led+0, 0
	ADDWF      FARG_changeColor_redLeds+0, 0
	MOVWF      FSR
	MOVF       INDF+0, 0
	SUBWF      FARG_changeColor_red+0, 0
	BTFSC      STATUS+0, 0
	GOTO       L_changeColor11
	MOVF       FARG_changeColor_led+0, 0
	ADDWF      FARG_changeColor_redLeds+0, 0
	MOVWF      R1+0
	MOVF       R1+0, 0
	MOVWF      FSR
	MOVLW      4
	SUBWF      INDF+0, 0
	MOVWF      R0+0
	MOVF       R1+0, 0
	MOVWF      FSR
	MOVF       R0+0, 0
	MOVWF      INDF+0
L_changeColor11:
;PlacaDeTeste.c,151 :: 		if ( greenLeds[led] < green ) greenLeds[led] += 4;
	MOVF       FARG_changeColor_led+0, 0
	ADDWF      FARG_changeColor_greenLeds+0, 0
	MOVWF      FSR
	MOVF       FARG_changeColor_green+0, 0
	SUBWF      INDF+0, 0
	BTFSC      STATUS+0, 0
	GOTO       L_changeColor12
	MOVF       FARG_changeColor_led+0, 0
	ADDWF      FARG_changeColor_greenLeds+0, 0
	MOVWF      R1+0
	MOVF       R1+0, 0
	MOVWF      FSR
	MOVLW      4
	ADDWF      INDF+0, 0
	MOVWF      R0+0
	MOVF       R1+0, 0
	MOVWF      FSR
	MOVF       R0+0, 0
	MOVWF      INDF+0
L_changeColor12:
;PlacaDeTeste.c,152 :: 		if ( greenLeds[led] > green ) greenLeds[led] -= 4;
	MOVF       FARG_changeColor_led+0, 0
	ADDWF      FARG_changeColor_greenLeds+0, 0
	MOVWF      FSR
	MOVF       INDF+0, 0
	SUBWF      FARG_changeColor_green+0, 0
	BTFSC      STATUS+0, 0
	GOTO       L_changeColor13
	MOVF       FARG_changeColor_led+0, 0
	ADDWF      FARG_changeColor_greenLeds+0, 0
	MOVWF      R1+0
	MOVF       R1+0, 0
	MOVWF      FSR
	MOVLW      4
	SUBWF      INDF+0, 0
	MOVWF      R0+0
	MOVF       R1+0, 0
	MOVWF      FSR
	MOVF       R0+0, 0
	MOVWF      INDF+0
L_changeColor13:
;PlacaDeTeste.c,154 :: 		}
L_end_changeColor:
	RETURN
; end of _changeColor

_setColor:

;PlacaDeTeste.c,156 :: 		void setColor(byte RGB_COM[], byte led) {
;PlacaDeTeste.c,157 :: 		delay_us(200); // 150
	MOVLW      2
	MOVWF      R12+0
	MOVLW      75
	MOVWF      R13+0
L_setColor14:
	DECFSZ     R13+0, 1
	GOTO       L_setColor14
	DECFSZ     R12+0, 1
	GOTO       L_setColor14
;PlacaDeTeste.c,158 :: 		if (led > 5) {
	MOVF       FARG_setColor_led+0, 0
	SUBLW      5
	BTFSC      STATUS+0, 0
	GOTO       L_setColor15
;PlacaDeTeste.c,159 :: 		TRISC = TRISC & RGB_COM[led];
	MOVF       FARG_setColor_led+0, 0
	ADDWF      FARG_setColor_RGB_COM+0, 0
	MOVWF      R0+0
	MOVF       R0+0, 0
	MOVWF      FSR
	MOVF       INDF+0, 0
	ANDWF      TRISC+0, 1
;PlacaDeTeste.c,160 :: 		PORTC = PORTC & RGB_COM[led];
	MOVF       R0+0, 0
	MOVWF      FSR
	MOVF       INDF+0, 0
	ANDWF      PORTC+0, 1
;PlacaDeTeste.c,161 :: 		}
	GOTO       L_setColor16
L_setColor15:
;PlacaDeTeste.c,163 :: 		TRISA = TRISA & RGB_COM[led];
	MOVF       FARG_setColor_led+0, 0
	ADDWF      FARG_setColor_RGB_COM+0, 0
	MOVWF      R0+0
	MOVF       R0+0, 0
	MOVWF      FSR
	MOVF       INDF+0, 0
	ANDWF      TRISA+0, 1
;PlacaDeTeste.c,164 :: 		PORTA = PORTA & RGB_COM[led];
	MOVF       R0+0, 0
	MOVWF      FSR
	MOVF       INDF+0, 0
	ANDWF      PORTA+0, 1
;PlacaDeTeste.c,165 :: 		}
L_setColor16:
;PlacaDeTeste.c,166 :: 		delay_us(80); // 80
	MOVLW      133
	MOVWF      R13+0
L_setColor17:
	DECFSZ     R13+0, 1
	GOTO       L_setColor17
;PlacaDeTeste.c,167 :: 		}
L_end_setColor:
	RETURN
; end of _setColor

_map:

;PlacaDeTeste.c,169 :: 		int map(int x, int in_min, int in_max, int out_min, int out_max)
;PlacaDeTeste.c,171 :: 		return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
	MOVF       FARG_map_in_min+0, 0
	SUBWF      FARG_map_x+0, 0
	MOVWF      R4+0
	MOVF       FARG_map_in_min+1, 0
	BTFSS      STATUS+0, 0
	ADDLW      1
	SUBWF      FARG_map_x+1, 0
	MOVWF      R4+1
	MOVF       FARG_map_out_min+0, 0
	SUBWF      FARG_map_out_max+0, 0
	MOVWF      R0+0
	MOVF       FARG_map_out_min+1, 0
	BTFSS      STATUS+0, 0
	ADDLW      1
	SUBWF      FARG_map_out_max+1, 0
	MOVWF      R0+1
	CALL       _Mul_16X16_U+0
	MOVF       FARG_map_in_min+0, 0
	SUBWF      FARG_map_in_max+0, 0
	MOVWF      R4+0
	MOVF       FARG_map_in_min+1, 0
	BTFSS      STATUS+0, 0
	ADDLW      1
	SUBWF      FARG_map_in_max+1, 0
	MOVWF      R4+1
	CALL       _Div_16x16_S+0
	MOVF       FARG_map_out_min+0, 0
	ADDWF      R0+0, 1
	MOVF       FARG_map_out_min+1, 0
	BTFSC      STATUS+0, 0
	ADDLW      1
	ADDWF      R0+1, 1
;PlacaDeTeste.c,172 :: 		}
L_end_map:
	RETURN
; end of _map

_main:

;PlacaDeTeste.c,175 :: 		void main(){
;PlacaDeTeste.c,179 :: 		byte disp_pos, disp_num, DISP_COM[4] = {0b00111110, 0b00111011, 0b00110111, 0b00111101}; // RA0, RA2, RA3, RA1 //16f877A: {0b11111011, 0b11011111, 0b10111111, 0b01111111};
	MOVLW      62
	MOVWF      main_DISP_COM_L0+0
	MOVLW      59
	MOVWF      main_DISP_COM_L0+1
	MOVLW      55
	MOVWF      main_DISP_COM_L0+2
	MOVLW      61
	MOVWF      main_DISP_COM_L0+3
	CLRF       main_redLeds_L0+0
	CLRF       main_redLeds_L0+1
	CLRF       main_redLeds_L0+2
	CLRF       main_redLeds_L0+3
	CLRF       main_redLeds_L0+4
	CLRF       main_redLeds_L0+5
	CLRF       main_redLeds_L0+6
	CLRF       main_redLeds_L0+7
	CLRF       main_redLeds_L0+8
	CLRF       main_greenLeds_L0+0
	CLRF       main_greenLeds_L0+1
	CLRF       main_greenLeds_L0+2
	CLRF       main_greenLeds_L0+3
	CLRF       main_greenLeds_L0+4
	CLRF       main_greenLeds_L0+5
	CLRF       main_greenLeds_L0+6
	CLRF       main_greenLeds_L0+7
	CLRF       main_greenLeds_L0+8
	MOVLW      62
	MOVWF      main_RGB_COM_L0+0
	MOVLW      47
	MOVWF      main_RGB_COM_L0+1
	MOVLW      31
	MOVWF      main_RGB_COM_L0+2
	MOVLW      59
	MOVWF      main_RGB_COM_L0+3
	MOVLW      55
	MOVWF      main_RGB_COM_L0+4
	MOVLW      61
	MOVWF      main_RGB_COM_L0+5
	MOVLW      127
	MOVWF      main_RGB_COM_L0+6
	MOVLW      191
	MOVWF      main_RGB_COM_L0+7
	MOVLW      223
	MOVWF      main_RGB_COM_L0+8
;PlacaDeTeste.c,204 :: 		disp_num = 0;
	CLRF       main_disp_num_L0+0
;PlacaDeTeste.c,205 :: 		disp_pos = 0;
	CLRF       main_disp_pos_L0+0
;PlacaDeTeste.c,206 :: 		rgb_pos = 0;
	CLRF       main_rgb_pos_L0+0
;PlacaDeTeste.c,208 :: 		letter_counter = 0;
	CLRF       _letter_counter+0
;PlacaDeTeste.c,214 :: 		initI2c();
	CALL       _initI2c+0
;PlacaDeTeste.c,215 :: 		initPwm();
	CALL       _initPwm+0
;PlacaDeTeste.c,216 :: 		initPinos();
	CALL       _initPinos+0
;PlacaDeTeste.c,218 :: 		for (i = 0; i < 4; i++) time_array[i] = 0x30;
	CLRF       main_i_L0+0
L_main18:
	MOVLW      4
	SUBWF      main_i_L0+0, 0
	BTFSC      STATUS+0, 0
	GOTO       L_main19
	MOVF       main_i_L0+0, 0
	ADDLW      main_time_array_L0+0
	MOVWF      FSR
	MOVLW      48
	MOVWF      INDF+0
	INCF       main_i_L0+0, 1
	GOTO       L_main18
L_main19:
;PlacaDeTeste.c,219 :: 		for (i = 0; i < 4; i++) led_selected[i] = 0x31;
	CLRF       main_i_L0+0
L_main21:
	MOVLW      4
	SUBWF      main_i_L0+0, 0
	BTFSC      STATUS+0, 0
	GOTO       L_main22
	MOVF       main_i_L0+0, 0
	ADDLW      main_led_selected_L0+0
	MOVWF      FSR
	MOVLW      49
	MOVWF      INDF+0
	INCF       main_i_L0+0, 1
	GOTO       L_main21
L_main22:
;PlacaDeTeste.c,220 :: 		for (i = 0; i < 4; i++) comp_quantity[i] = 0x30;
	CLRF       main_i_L0+0
L_main24:
	MOVLW      4
	SUBWF      main_i_L0+0, 0
	BTFSC      STATUS+0, 0
	GOTO       L_main25
	MOVF       main_i_L0+0, 0
	ADDLW      main_comp_quantity_L0+0
	MOVWF      FSR
	MOVLW      48
	MOVWF      INDF+0
	INCF       main_i_L0+0, 1
	GOTO       L_main24
L_main25:
;PlacaDeTeste.c,221 :: 		for (i = 0; i < 4; i++) selection_mode[i] = 0x30;
	CLRF       main_i_L0+0
L_main27:
	MOVLW      4
	SUBWF      main_i_L0+0, 0
	BTFSC      STATUS+0, 0
	GOTO       L_main28
	MOVF       main_i_L0+0, 0
	ADDLW      main_selection_mode_L0+0
	MOVWF      FSR
	MOVLW      48
	MOVWF      INDF+0
	INCF       main_i_L0+0, 1
	GOTO       L_main27
L_main28:
;PlacaDeTeste.c,222 :: 		for (i = 0; i < 18; i++) comp_all_limits[i] = 0x30;
	CLRF       main_i_L0+0
L_main30:
	MOVLW      18
	SUBWF      main_i_L0+0, 0
	BTFSC      STATUS+0, 0
	GOTO       L_main31
	MOVF       main_i_L0+0, 0
	ADDLW      main_comp_all_limits_L0+0
	MOVWF      FSR
	MOVLW      48
	MOVWF      INDF+0
	INCF       main_i_L0+0, 1
	GOTO       L_main30
L_main31:
;PlacaDeTeste.c,223 :: 		for (i = 0; i < 18; i++) comp_all_quantities[i] = 0x30;
	CLRF       main_i_L0+0
L_main33:
	MOVLW      18
	SUBWF      main_i_L0+0, 0
	BTFSC      STATUS+0, 0
	GOTO       L_main34
	MOVF       main_i_L0+0, 0
	ADDLW      main_comp_all_quantities_L0+0
	MOVWF      FSR
	MOVLW      48
	MOVWF      INDF+0
	INCF       main_i_L0+0, 1
	GOTO       L_main33
L_main34:
;PlacaDeTeste.c,226 :: 		for (i = 0; i < 25; i++) rxbuffer[i] = 0x30;
	CLRF       main_i_L0+0
L_main36:
	MOVLW      25
	SUBWF      main_i_L0+0, 0
	BTFSC      STATUS+0, 0
	GOTO       L_main37
	MOVF       main_i_L0+0, 0
	ADDLW      _rxbuffer+0
	MOVWF      FSR
	MOVLW      48
	MOVWF      INDF+0
	INCF       main_i_L0+0, 1
	GOTO       L_main36
L_main37:
;PlacaDeTeste.c,227 :: 		rxbuffer[0] = id_Hora;
	MOVLW      72
	MOVWF      _rxbuffer+0
;PlacaDeTeste.c,229 :: 		while (1){
L_main39:
;PlacaDeTeste.c,231 :: 		if(rxbuffer[0] == id_Slct && rxbuffer[5] == id_Slct) {
	MOVF       _rxbuffer+0, 0
	XORLW      83
	BTFSS      STATUS+0, 2
	GOTO       L_main43
	MOVF       _rxbuffer+5, 0
	XORLW      83
	BTFSS      STATUS+0, 2
	GOTO       L_main43
L__main138:
;PlacaDeTeste.c,232 :: 		for (i = 0; i < 4; i++) selection_mode[i] = rxbuffer[i+1];
	CLRF       main_i_L0+0
L_main44:
	MOVLW      4
	SUBWF      main_i_L0+0, 0
	BTFSC      STATUS+0, 0
	GOTO       L_main45
	MOVF       main_i_L0+0, 0
	ADDLW      main_selection_mode_L0+0
	MOVWF      R2+0
	MOVF       main_i_L0+0, 0
	ADDLW      1
	MOVWF      R0+0
	CLRF       R0+1
	BTFSC      STATUS+0, 0
	INCF       R0+1, 1
	MOVF       R0+0, 0
	ADDLW      _rxbuffer+0
	MOVWF      FSR
	MOVF       INDF+0, 0
	MOVWF      R0+0
	MOVF       R2+0, 0
	MOVWF      FSR
	MOVF       R0+0, 0
	MOVWF      INDF+0
	INCF       main_i_L0+0, 1
	GOTO       L_main44
L_main45:
;PlacaDeTeste.c,233 :: 		}
	GOTO       L_main47
L_main43:
;PlacaDeTeste.c,234 :: 		else if(rxbuffer[0] == id_Comp && rxbuffer[5] == id_Comp) {
	MOVF       _rxbuffer+0, 0
	XORLW      67
	BTFSS      STATUS+0, 2
	GOTO       L_main50
	MOVF       _rxbuffer+5, 0
	XORLW      67
	BTFSS      STATUS+0, 2
	GOTO       L_main50
L__main137:
;PlacaDeTeste.c,235 :: 		for (i = 0; i < 4; i++) led_selected[i] = rxbuffer[i+1];
	CLRF       main_i_L0+0
L_main51:
	MOVLW      4
	SUBWF      main_i_L0+0, 0
	BTFSC      STATUS+0, 0
	GOTO       L_main52
	MOVF       main_i_L0+0, 0
	ADDLW      main_led_selected_L0+0
	MOVWF      R2+0
	MOVF       main_i_L0+0, 0
	ADDLW      1
	MOVWF      R0+0
	CLRF       R0+1
	BTFSC      STATUS+0, 0
	INCF       R0+1, 1
	MOVF       R0+0, 0
	ADDLW      _rxbuffer+0
	MOVWF      FSR
	MOVF       INDF+0, 0
	MOVWF      R0+0
	MOVF       R2+0, 0
	MOVWF      FSR
	MOVF       R0+0, 0
	MOVWF      INDF+0
	INCF       main_i_L0+0, 1
	GOTO       L_main51
L_main52:
;PlacaDeTeste.c,236 :: 		}
	GOTO       L_main54
L_main50:
;PlacaDeTeste.c,237 :: 		else if (rxbuffer[0] == id_Hora && rxbuffer[5] == id_Hora) {
	MOVF       _rxbuffer+0, 0
	XORLW      72
	BTFSS      STATUS+0, 2
	GOTO       L_main57
	MOVF       _rxbuffer+5, 0
	XORLW      72
	BTFSS      STATUS+0, 2
	GOTO       L_main57
L__main136:
;PlacaDeTeste.c,238 :: 		for (i = 0; i < 4; i++) time_array[i] = rxbuffer[i+1];
	CLRF       main_i_L0+0
L_main58:
	MOVLW      4
	SUBWF      main_i_L0+0, 0
	BTFSC      STATUS+0, 0
	GOTO       L_main59
	MOVF       main_i_L0+0, 0
	ADDLW      main_time_array_L0+0
	MOVWF      R2+0
	MOVF       main_i_L0+0, 0
	ADDLW      1
	MOVWF      R0+0
	CLRF       R0+1
	BTFSC      STATUS+0, 0
	INCF       R0+1, 1
	MOVF       R0+0, 0
	ADDLW      _rxbuffer+0
	MOVWF      FSR
	MOVF       INDF+0, 0
	MOVWF      R0+0
	MOVF       R2+0, 0
	MOVWF      FSR
	MOVF       R0+0, 0
	MOVWF      INDF+0
	INCF       main_i_L0+0, 1
	GOTO       L_main58
L_main59:
;PlacaDeTeste.c,239 :: 		}
	GOTO       L_main61
L_main57:
;PlacaDeTeste.c,240 :: 		else if (rxbuffer[0] == id_AQtd && rxbuffer[19] == id_AQtd) {
	MOVF       _rxbuffer+0, 0
	XORLW      65
	BTFSS      STATUS+0, 2
	GOTO       L_main64
	MOVF       _rxbuffer+19, 0
	XORLW      65
	BTFSS      STATUS+0, 2
	GOTO       L_main64
L__main135:
;PlacaDeTeste.c,241 :: 		for (i = 0; i < 18; i++) comp_all_quantities[i] = rxbuffer[i+1];
	CLRF       main_i_L0+0
L_main65:
	MOVLW      18
	SUBWF      main_i_L0+0, 0
	BTFSC      STATUS+0, 0
	GOTO       L_main66
	MOVF       main_i_L0+0, 0
	ADDLW      main_comp_all_quantities_L0+0
	MOVWF      R2+0
	MOVF       main_i_L0+0, 0
	ADDLW      1
	MOVWF      R0+0
	CLRF       R0+1
	BTFSC      STATUS+0, 0
	INCF       R0+1, 1
	MOVF       R0+0, 0
	ADDLW      _rxbuffer+0
	MOVWF      FSR
	MOVF       INDF+0, 0
	MOVWF      R0+0
	MOVF       R2+0, 0
	MOVWF      FSR
	MOVF       R0+0, 0
	MOVWF      INDF+0
	INCF       main_i_L0+0, 1
	GOTO       L_main65
L_main66:
;PlacaDeTeste.c,242 :: 		}
	GOTO       L_main68
L_main64:
;PlacaDeTeste.c,243 :: 		else if (rxbuffer[0] == id_Qtd && rxbuffer[5] == id_Qtd) {
	MOVF       _rxbuffer+0, 0
	XORLW      81
	BTFSS      STATUS+0, 2
	GOTO       L_main71
	MOVF       _rxbuffer+5, 0
	XORLW      81
	BTFSS      STATUS+0, 2
	GOTO       L_main71
L__main134:
;PlacaDeTeste.c,244 :: 		for (i = 0; i < 4; i++) comp_quantity[i] = rxbuffer[i+1];
	CLRF       main_i_L0+0
L_main72:
	MOVLW      4
	SUBWF      main_i_L0+0, 0
	BTFSC      STATUS+0, 0
	GOTO       L_main73
	MOVF       main_i_L0+0, 0
	ADDLW      main_comp_quantity_L0+0
	MOVWF      R2+0
	MOVF       main_i_L0+0, 0
	ADDLW      1
	MOVWF      R0+0
	CLRF       R0+1
	BTFSC      STATUS+0, 0
	INCF       R0+1, 1
	MOVF       R0+0, 0
	ADDLW      _rxbuffer+0
	MOVWF      FSR
	MOVF       INDF+0, 0
	MOVWF      R0+0
	MOVF       R2+0, 0
	MOVWF      FSR
	MOVF       R0+0, 0
	MOVWF      INDF+0
	INCF       main_i_L0+0, 1
	GOTO       L_main72
L_main73:
;PlacaDeTeste.c,245 :: 		}
	GOTO       L_main75
L_main71:
;PlacaDeTeste.c,246 :: 		else if (rxbuffer[0] == id_Lim && rxbuffer[19] == id_Lim) {
	MOVF       _rxbuffer+0, 0
	XORLW      76
	BTFSS      STATUS+0, 2
	GOTO       L_main78
	MOVF       _rxbuffer+19, 0
	XORLW      76
	BTFSS      STATUS+0, 2
	GOTO       L_main78
L__main133:
;PlacaDeTeste.c,247 :: 		for (i = 0; i < 18; i++) comp_all_limits[i] = rxbuffer[i+1];
	CLRF       main_i_L0+0
L_main79:
	MOVLW      18
	SUBWF      main_i_L0+0, 0
	BTFSC      STATUS+0, 0
	GOTO       L_main80
	MOVF       main_i_L0+0, 0
	ADDLW      main_comp_all_limits_L0+0
	MOVWF      R2+0
	MOVF       main_i_L0+0, 0
	ADDLW      1
	MOVWF      R0+0
	CLRF       R0+1
	BTFSC      STATUS+0, 0
	INCF       R0+1, 1
	MOVF       R0+0, 0
	ADDLW      _rxbuffer+0
	MOVWF      FSR
	MOVF       INDF+0, 0
	MOVWF      R0+0
	MOVF       R2+0, 0
	MOVWF      FSR
	MOVF       R0+0, 0
	MOVWF      INDF+0
	INCF       main_i_L0+0, 1
	GOTO       L_main79
L_main80:
;PlacaDeTeste.c,248 :: 		}
L_main78:
L_main75:
L_main68:
L_main61:
L_main54:
L_main47:
;PlacaDeTeste.c,252 :: 		if (selection_mode[0] == 0x30 || selection_mode[0] == 0x32 || selection_mode[0] == 0x33) {
	MOVF       main_selection_mode_L0+0, 0
	XORLW      48
	BTFSC      STATUS+0, 2
	GOTO       L__main132
	MOVF       main_selection_mode_L0+0, 0
	XORLW      50
	BTFSC      STATUS+0, 2
	GOTO       L__main132
	MOVF       main_selection_mode_L0+0, 0
	XORLW      51
	BTFSC      STATUS+0, 2
	GOTO       L__main132
	GOTO       L_main84
L__main132:
;PlacaDeTeste.c,253 :: 		if (comp_all_quantities[2*rgb_pos] >= 0x30 && comp_all_quantities[2*rgb_pos] <= 0x39 && comp_all_quantities[2*rgb_pos + 1] >= 0x30 && comp_all_quantities[2*rgb_pos + 1] <= 0x39) {
	MOVF       main_rgb_pos_L0+0, 0
	MOVWF      R0+0
	CLRF       R0+1
	RLF        R0+0, 1
	RLF        R0+1, 1
	BCF        R0+0, 0
	MOVF       R0+0, 0
	ADDLW      main_comp_all_quantities_L0+0
	MOVWF      FSR
	MOVLW      48
	SUBWF      INDF+0, 0
	BTFSS      STATUS+0, 0
	GOTO       L_main87
	MOVF       main_rgb_pos_L0+0, 0
	MOVWF      R0+0
	CLRF       R0+1
	RLF        R0+0, 1
	RLF        R0+1, 1
	BCF        R0+0, 0
	MOVF       R0+0, 0
	ADDLW      main_comp_all_quantities_L0+0
	MOVWF      FSR
	MOVF       INDF+0, 0
	SUBLW      57
	BTFSS      STATUS+0, 0
	GOTO       L_main87
	MOVF       main_rgb_pos_L0+0, 0
	MOVWF      R0+0
	CLRF       R0+1
	RLF        R0+0, 1
	RLF        R0+1, 1
	BCF        R0+0, 0
	INCF       R0+0, 1
	BTFSC      STATUS+0, 2
	INCF       R0+1, 1
	MOVF       R0+0, 0
	ADDLW      main_comp_all_quantities_L0+0
	MOVWF      FSR
	MOVLW      48
	SUBWF      INDF+0, 0
	BTFSS      STATUS+0, 0
	GOTO       L_main87
	MOVF       main_rgb_pos_L0+0, 0
	MOVWF      R0+0
	CLRF       R0+1
	RLF        R0+0, 1
	RLF        R0+1, 1
	BCF        R0+0, 0
	INCF       R0+0, 1
	BTFSC      STATUS+0, 2
	INCF       R0+1, 1
	MOVF       R0+0, 0
	ADDLW      main_comp_all_quantities_L0+0
	MOVWF      FSR
	MOVF       INDF+0, 0
	SUBLW      57
	BTFSS      STATUS+0, 0
	GOTO       L_main87
L__main131:
;PlacaDeTeste.c,254 :: 		int int_quantity = 10*(comp_all_quantities[2*rgb_pos] - 0x30) + comp_all_quantities[2*rgb_pos + 1] - 0x30;
	MOVF       main_rgb_pos_L0+0, 0
	MOVWF      FLOC__main+4
	CLRF       FLOC__main+5
	RLF        FLOC__main+4, 1
	RLF        FLOC__main+5, 1
	BCF        FLOC__main+4, 0
	MOVF       FLOC__main+4, 0
	ADDLW      main_comp_all_quantities_L0+0
	MOVWF      FSR
	MOVLW      48
	SUBWF      INDF+0, 0
	MOVWF      R0+0
	CLRF       R0+1
	BTFSS      STATUS+0, 0
	DECF       R0+1, 1
	MOVLW      10
	MOVWF      R4+0
	MOVLW      0
	MOVWF      R4+1
	CALL       _Mul_16X16_U+0
	MOVF       FLOC__main+4, 0
	ADDLW      1
	MOVWF      FLOC__main+2
	MOVLW      0
	BTFSC      STATUS+0, 0
	ADDLW      1
	ADDWF      FLOC__main+5, 0
	MOVWF      FLOC__main+3
	MOVF       FLOC__main+2, 0
	ADDLW      main_comp_all_quantities_L0+0
	MOVWF      FSR
	MOVF       INDF+0, 0
	ADDWF      R0+0, 1
	BTFSC      STATUS+0, 0
	INCF       R0+1, 1
	MOVLW      48
	SUBWF      R0+0, 0
	MOVWF      FLOC__main+0
	MOVLW      0
	BTFSS      STATUS+0, 0
	ADDLW      1
	SUBWF      R0+1, 0
	MOVWF      FLOC__main+1
	MOVF       FLOC__main+0, 0
	MOVWF      main_int_quantity_L3+0
	MOVF       FLOC__main+1, 0
	MOVWF      main_int_quantity_L3+1
;PlacaDeTeste.c,255 :: 		int int_limit = 10*(comp_all_limits[2*rgb_pos] - 0x30) + comp_all_limits[2*rgb_pos + 1] - 0x30;
	MOVF       FLOC__main+4, 0
	ADDLW      main_comp_all_limits_L0+0
	MOVWF      FSR
	MOVLW      48
	SUBWF      INDF+0, 0
	MOVWF      R0+0
	CLRF       R0+1
	BTFSS      STATUS+0, 0
	DECF       R0+1, 1
	MOVLW      10
	MOVWF      R4+0
	MOVLW      0
	MOVWF      R4+1
	CALL       _Mul_16X16_U+0
	MOVF       FLOC__main+2, 0
	ADDLW      main_comp_all_limits_L0+0
	MOVWF      FSR
	MOVF       INDF+0, 0
	ADDWF      R0+0, 0
	MOVWF      main_int_limit_L3+0
	MOVF       R0+1, 0
	BTFSC      STATUS+0, 0
	ADDLW      1
	MOVWF      main_int_limit_L3+1
	MOVLW      48
	SUBWF      main_int_limit_L3+0, 1
	BTFSS      STATUS+0, 0
	DECF       main_int_limit_L3+1, 1
;PlacaDeTeste.c,257 :: 		if (int_quantity == 0) {
	MOVLW      0
	XORWF      FLOC__main+1, 0
	BTFSS      STATUS+0, 2
	GOTO       L__main166
	MOVLW      0
	XORWF      FLOC__main+0, 0
L__main166:
	BTFSS      STATUS+0, 2
	GOTO       L_main88
;PlacaDeTeste.c,258 :: 		redLeds[rgb_pos] = 0;
	MOVF       main_rgb_pos_L0+0, 0
	ADDLW      main_redLeds_L0+0
	MOVWF      FSR
	CLRF       INDF+0
;PlacaDeTeste.c,259 :: 		greenLeds[rgb_pos] = 255;
	MOVF       main_rgb_pos_L0+0, 0
	ADDLW      main_greenLeds_L0+0
	MOVWF      FSR
	MOVLW      255
	MOVWF      INDF+0
;PlacaDeTeste.c,260 :: 		}
	GOTO       L_main89
L_main88:
;PlacaDeTeste.c,261 :: 		else if (int_quantity >= int_limit) {
	MOVLW      128
	XORWF      main_int_quantity_L3+1, 0
	MOVWF      R0+0
	MOVLW      128
	XORWF      main_int_limit_L3+1, 0
	SUBWF      R0+0, 0
	BTFSS      STATUS+0, 2
	GOTO       L__main167
	MOVF       main_int_limit_L3+0, 0
	SUBWF      main_int_quantity_L3+0, 0
L__main167:
	BTFSS      STATUS+0, 0
	GOTO       L_main90
;PlacaDeTeste.c,262 :: 		redLeds[rgb_pos] = 255;
	MOVF       main_rgb_pos_L0+0, 0
	ADDLW      main_redLeds_L0+0
	MOVWF      FSR
	MOVLW      255
	MOVWF      INDF+0
;PlacaDeTeste.c,263 :: 		greenLeds[rgb_pos] = 0;
	MOVF       main_rgb_pos_L0+0, 0
	ADDLW      main_greenLeds_L0+0
	MOVWF      FSR
	CLRF       INDF+0
;PlacaDeTeste.c,264 :: 		}
	GOTO       L_main91
L_main90:
;PlacaDeTeste.c,266 :: 		int green_value = map(int_quantity, 0, int_limit, 150, 30);
	MOVF       main_int_quantity_L3+0, 0
	MOVWF      FARG_map_x+0
	MOVF       main_int_quantity_L3+1, 0
	MOVWF      FARG_map_x+1
	CLRF       FARG_map_in_min+0
	CLRF       FARG_map_in_min+1
	MOVF       main_int_limit_L3+0, 0
	MOVWF      FARG_map_in_max+0
	MOVF       main_int_limit_L3+1, 0
	MOVWF      FARG_map_in_max+1
	MOVLW      150
	MOVWF      FARG_map_out_min+0
	CLRF       FARG_map_out_min+1
	MOVLW      30
	MOVWF      FARG_map_out_max+0
	MOVLW      0
	MOVWF      FARG_map_out_max+1
	CALL       _map+0
	MOVF       R0+0, 0
	MOVWF      main_green_value_L4+0
	MOVF       R0+1, 0
	MOVWF      main_green_value_L4+1
;PlacaDeTeste.c,267 :: 		redLeds[rgb_pos] = 255;
	MOVF       main_rgb_pos_L0+0, 0
	ADDLW      main_redLeds_L0+0
	MOVWF      FSR
	MOVLW      255
	MOVWF      INDF+0
;PlacaDeTeste.c,268 :: 		greenLeds[rgb_pos] = green_value;
	MOVF       main_rgb_pos_L0+0, 0
	ADDLW      main_greenLeds_L0+0
	MOVWF      FSR
	MOVF       main_green_value_L4+0, 0
	MOVWF      INDF+0
;PlacaDeTeste.c,269 :: 		}
L_main91:
L_main89:
;PlacaDeTeste.c,270 :: 		}
L_main87:
;PlacaDeTeste.c,271 :: 		}
L_main84:
;PlacaDeTeste.c,315 :: 		TRISA = TRISA | 0b00111111;
	MOVLW      63
	IORWF      TRISA+0, 1
;PlacaDeTeste.c,316 :: 		TRISC = TRISC | 0b11100000;
	MOVLW      224
	IORWF      TRISC+0, 1
;PlacaDeTeste.c,317 :: 		PWM1_Set_Duty(redLeds[rgb_pos]);
	MOVF       main_rgb_pos_L0+0, 0
	ADDLW      main_redLeds_L0+0
	MOVWF      FSR
	MOVF       INDF+0, 0
	MOVWF      FARG_PWM1_Set_Duty_new_duty+0
	CALL       _PWM1_Set_Duty+0
;PlacaDeTeste.c,318 :: 		PWM2_Set_Duty(greenLeds[rgb_pos]);
	MOVF       main_rgb_pos_L0+0, 0
	ADDLW      main_greenLeds_L0+0
	MOVWF      FSR
	MOVF       INDF+0, 0
	MOVWF      FARG_PWM2_Set_Duty_new_duty+0
	CALL       _PWM2_Set_Duty+0
;PlacaDeTeste.c,320 :: 		if (selection_mode[0] == 0x30) PORTC.B0 = 0;
	MOVF       main_selection_mode_L0+0, 0
	XORLW      48
	BTFSS      STATUS+0, 2
	GOTO       L_main92
	BCF        PORTC+0, 0
L_main92:
;PlacaDeTeste.c,321 :: 		if (selection_mode[0] == 0x31) PORTC.B0 = 1;
	MOVF       main_selection_mode_L0+0, 0
	XORLW      49
	BTFSS      STATUS+0, 2
	GOTO       L_main93
	BSF        PORTC+0, 0
L_main93:
;PlacaDeTeste.c,322 :: 		if (selection_mode[0] == 0x32) {
	MOVF       main_selection_mode_L0+0, 0
	XORLW      50
	BTFSS      STATUS+0, 2
	GOTO       L_main94
;PlacaDeTeste.c,323 :: 		if (led_selected[0] > 0x30 && led_selected[0] <= 0x39) {
	MOVF       main_led_selected_L0+0, 0
	SUBLW      48
	BTFSC      STATUS+0, 0
	GOTO       L_main97
	MOVF       main_led_selected_L0+0, 0
	SUBLW      57
	BTFSS      STATUS+0, 0
	GOTO       L_main97
L__main130:
;PlacaDeTeste.c,324 :: 		if (rgb_pos == led_selected[0] - 0x31) PORTC.B0 = 1;
	MOVLW      49
	SUBWF      main_led_selected_L0+0, 0
	MOVWF      R1+0
	CLRF       R1+1
	BTFSS      STATUS+0, 0
	DECF       R1+1, 1
	MOVLW      0
	XORWF      R1+1, 0
	BTFSS      STATUS+0, 2
	GOTO       L__main168
	MOVF       R1+0, 0
	XORWF      main_rgb_pos_L0+0, 0
L__main168:
	BTFSS      STATUS+0, 2
	GOTO       L_main98
	BSF        PORTC+0, 0
	GOTO       L_main99
L_main98:
;PlacaDeTeste.c,325 :: 		else PORTC.B0 = 0;
	BCF        PORTC+0, 0
L_main99:
;PlacaDeTeste.c,326 :: 		}
L_main97:
;PlacaDeTeste.c,327 :: 		}
L_main94:
;PlacaDeTeste.c,329 :: 		setColor(RGB_COM, rgb_pos);
	MOVLW      main_RGB_COM_L0+0
	MOVWF      FARG_setColor_RGB_COM+0
	MOVF       main_rgb_pos_L0+0, 0
	MOVWF      FARG_setColor_led+0
	CALL       _setColor+0
;PlacaDeTeste.c,330 :: 		PWM1_Set_Duty(0);
	CLRF       FARG_PWM1_Set_Duty_new_duty+0
	CALL       _PWM1_Set_Duty+0
;PlacaDeTeste.c,331 :: 		PWM2_Set_Duty(0);
	CLRF       FARG_PWM2_Set_Duty_new_duty+0
	CALL       _PWM2_Set_Duty+0
;PlacaDeTeste.c,332 :: 		PORTC.B0 = 0;
	BCF        PORTC+0, 0
;PlacaDeTeste.c,333 :: 		delay_us(30); // 30
	MOVLW      49
	MOVWF      R13+0
L_main100:
	DECFSZ     R13+0, 1
	GOTO       L_main100
	NOP
	NOP
;PlacaDeTeste.c,335 :: 		rgb_pos++;
	INCF       main_rgb_pos_L0+0, 1
;PlacaDeTeste.c,336 :: 		if (selection_mode[0] == 0x33 && led_selected[0] > 0x30 && led_selected[0] <= 0x39) {
	MOVF       main_selection_mode_L0+0, 0
	XORLW      51
	BTFSS      STATUS+0, 2
	GOTO       L_main103
	MOVF       main_led_selected_L0+0, 0
	SUBLW      48
	BTFSC      STATUS+0, 0
	GOTO       L_main103
	MOVF       main_led_selected_L0+0, 0
	SUBLW      57
	BTFSS      STATUS+0, 0
	GOTO       L_main103
L__main129:
;PlacaDeTeste.c,337 :: 		rgb_pos = led_selected[0] - 0x31;
	MOVLW      49
	SUBWF      main_led_selected_L0+0, 0
	MOVWF      main_rgb_pos_L0+0
;PlacaDeTeste.c,338 :: 		}
L_main103:
;PlacaDeTeste.c,339 :: 		if (rgb_pos > 8) rgb_pos = 0;
	MOVF       main_rgb_pos_L0+0, 0
	SUBLW      8
	BTFSC      STATUS+0, 0
	GOTO       L_main104
	CLRF       main_rgb_pos_L0+0
L_main104:
;PlacaDeTeste.c,344 :: 		if (selection_mode[0] == 0x32 || selection_mode[0] == 0x33) {
	MOVF       main_selection_mode_L0+0, 0
	XORLW      50
	BTFSC      STATUS+0, 2
	GOTO       L__main128
	MOVF       main_selection_mode_L0+0, 0
	XORLW      51
	BTFSC      STATUS+0, 2
	GOTO       L__main128
	GOTO       L_main107
L__main128:
;PlacaDeTeste.c,345 :: 		if (comp_quantity[0] >= 0x30 && comp_quantity[0] <= 0x39) {
	MOVLW      48
	SUBWF      main_comp_quantity_L0+0, 0
	BTFSS      STATUS+0, 0
	GOTO       L_main110
	MOVF       main_comp_quantity_L0+0, 0
	SUBLW      57
	BTFSS      STATUS+0, 0
	GOTO       L_main110
L__main127:
;PlacaDeTeste.c,346 :: 		if (disp_pos == 0) disp_num = 0;
	MOVF       main_disp_pos_L0+0, 0
	XORLW      0
	BTFSS      STATUS+0, 2
	GOTO       L_main111
	CLRF       main_disp_num_L0+0
	GOTO       L_main112
L_main111:
;PlacaDeTeste.c,347 :: 		else if (disp_pos == 1) disp_num = 0;
	MOVF       main_disp_pos_L0+0, 0
	XORLW      1
	BTFSS      STATUS+0, 2
	GOTO       L_main113
	CLRF       main_disp_num_L0+0
	GOTO       L_main114
L_main113:
;PlacaDeTeste.c,348 :: 		else if (disp_pos == 2) disp_num = comp_quantity[0] - 0x30;
	MOVF       main_disp_pos_L0+0, 0
	XORLW      2
	BTFSS      STATUS+0, 2
	GOTO       L_main115
	MOVLW      48
	SUBWF      main_comp_quantity_L0+0, 0
	MOVWF      main_disp_num_L0+0
	GOTO       L_main116
L_main115:
;PlacaDeTeste.c,349 :: 		else if (disp_pos == 3) disp_num = comp_quantity[1] - 0x30;
	MOVF       main_disp_pos_L0+0, 0
	XORLW      3
	BTFSS      STATUS+0, 2
	GOTO       L_main117
	MOVLW      48
	SUBWF      main_comp_quantity_L0+1, 0
	MOVWF      main_disp_num_L0+0
L_main117:
L_main116:
L_main114:
L_main112:
;PlacaDeTeste.c,350 :: 		}
L_main110:
;PlacaDeTeste.c,351 :: 		}
	GOTO       L_main118
L_main107:
;PlacaDeTeste.c,353 :: 		if (time_array[disp_pos] >= 0x30 && time_array[disp_pos] <= 0x39) disp_num = time_array[disp_pos] - 0x30;
	MOVF       main_disp_pos_L0+0, 0
	ADDLW      main_time_array_L0+0
	MOVWF      FSR
	MOVLW      48
	SUBWF      INDF+0, 0
	BTFSS      STATUS+0, 0
	GOTO       L_main121
	MOVF       main_disp_pos_L0+0, 0
	ADDLW      main_time_array_L0+0
	MOVWF      FSR
	MOVF       INDF+0, 0
	SUBLW      57
	BTFSS      STATUS+0, 0
	GOTO       L_main121
L__main126:
	MOVF       main_disp_pos_L0+0, 0
	ADDLW      main_time_array_L0+0
	MOVWF      FSR
	MOVLW      48
	SUBWF      INDF+0, 0
	MOVWF      main_disp_num_L0+0
L_main121:
;PlacaDeTeste.c,354 :: 		}
L_main118:
;PlacaDeTeste.c,355 :: 		TRISA = TRISA | 0b00111111;
	MOVLW      63
	IORWF      TRISA+0, 1
;PlacaDeTeste.c,356 :: 		TRISC = TRISC | 0b11100000;
	MOVLW      224
	IORWF      TRISC+0, 1
;PlacaDeTeste.c,357 :: 		PORTB = segments[disp_num];
	MOVF       main_disp_num_L0+0, 0
	ADDLW      main_segments_L0+0
	MOVWF      R0+0
	MOVLW      hi_addr(main_segments_L0+0)
	BTFSC      STATUS+0, 0
	ADDLW      1
	MOVWF      R0+1
	MOVF       R0+0, 0
	MOVWF      ___DoICPAddr+0
	MOVF       R0+1, 0
	MOVWF      ___DoICPAddr+1
	CALL       _____DoICP+0
	MOVWF      PORTB+0
;PlacaDeTeste.c,358 :: 		if(disp_pos == 1) PORTB = PORTB | 0b00100000;
	MOVF       main_disp_pos_L0+0, 0
	XORLW      1
	BTFSS      STATUS+0, 2
	GOTO       L_main122
	BSF        PORTB+0, 5
L_main122:
;PlacaDeTeste.c,361 :: 		delay_us(15); //
	MOVLW      24
	MOVWF      R13+0
L_main123:
	DECFSZ     R13+0, 1
	GOTO       L_main123
	NOP
	NOP
;PlacaDeTeste.c,362 :: 		TRISA = TRISA & DISP_COM[disp_pos];
	MOVF       main_disp_pos_L0+0, 0
	ADDLW      main_DISP_COM_L0+0
	MOVWF      R0+0
	MOVF       R0+0, 0
	MOVWF      FSR
	MOVF       INDF+0, 0
	ANDWF      TRISA+0, 1
;PlacaDeTeste.c,363 :: 		PORTA = PORTA & DISP_COM[disp_pos];
	MOVF       R0+0, 0
	MOVWF      FSR
	MOVF       INDF+0, 0
	ANDWF      PORTA+0, 1
;PlacaDeTeste.c,364 :: 		disp_pos++;
	INCF       main_disp_pos_L0+0, 1
;PlacaDeTeste.c,365 :: 		if (disp_pos > 3) disp_pos = 0;
	MOVF       main_disp_pos_L0+0, 0
	SUBLW      3
	BTFSC      STATUS+0, 0
	GOTO       L_main124
	CLRF       main_disp_pos_L0+0
L_main124:
;PlacaDeTeste.c,368 :: 		delay_us(5); //2
	MOVLW      8
	MOVWF      R13+0
L_main125:
	DECFSZ     R13+0, 1
	GOTO       L_main125
;PlacaDeTeste.c,369 :: 		PORTB = 0x00;
	CLRF       PORTB+0
;PlacaDeTeste.c,372 :: 		}
	GOTO       L_main39
;PlacaDeTeste.c,373 :: 		}
L_end_main:
	GOTO       $+0
; end of _main
