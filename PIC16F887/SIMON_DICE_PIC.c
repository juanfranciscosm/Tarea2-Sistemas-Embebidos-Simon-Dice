#define BUZZER RC2_bit

unsigned short comando = 0;

void beep_300(unsigned int repeticiones) {
    unsigned int i;
    for(i = 0; i < repeticiones; i++) {
        BUZZER = 1;
        Delay_us(300);
        BUZZER = 0;
        Delay_us(300);
    }
}

void beep_400(unsigned int repeticiones) {
    unsigned int i;
    for(i = 0; i < repeticiones; i++) {
        BUZZER = 1;
        Delay_us(400);
        BUZZER = 0;
        Delay_us(400);
    }
}

void beep_500(unsigned int repeticiones) {
    unsigned int i;
    for(i = 0; i < repeticiones; i++) {
        BUZZER = 1;
        Delay_us(500);
        BUZZER = 0;
        Delay_us(500);
    }
}

void beep_700(unsigned int repeticiones) {
    unsigned int i;
    for(i = 0; i < repeticiones; i++) {
        BUZZER = 1;
        Delay_us(700);
        BUZZER = 0;
        Delay_us(700);
    }
}

void beep_900(unsigned int repeticiones) {
    unsigned int i;
    for(i = 0; i < repeticiones; i++) {
        BUZZER = 1;
        Delay_us(900);
        BUZZER = 0;
        Delay_us(900);
    }
}

void beep_1200(unsigned int repeticiones) {
    unsigned int i;
    for(i = 0; i < repeticiones; i++) {
        BUZZER = 1;
        Delay_us(1200);
        BUZZER = 0;
        Delay_us(1200);
    }
}

void sonido_inicio() {
    beep_700(150);
    Delay_ms(100);
    beep_500(150);
}

void sonido_nivel1() {
    beep_900(100);
}

void sonido_nivel2() {
    beep_900(100);
    Delay_ms(100);
    beep_700(100);
}

void sonido_nivel3() {
    beep_900(100);
    Delay_ms(80);
    beep_700(100);
    Delay_ms(80);
    beep_500(100);
}

void sonido_correcto() {
    beep_400(120);
}

void sonido_error() {
    beep_1200(350);
}

void sonido_victoria() {
    beep_700(150);
    Delay_ms(100);
    beep_500(150);
    Delay_ms(100);
    beep_300(200);
}

unsigned short leer_comando() {
    unsigned short valor = 0;

    if (RB0_bit == 1) valor = valor + 1;
    if (RB1_bit == 1) valor = valor + 2;
    if (RB2_bit == 1) valor = valor + 4;
    if (RB3_bit == 1) valor = valor + 8;

    return valor;
}

void ejecutar_comando(unsigned short dato) {
    if (dato == 1) {
        sonido_inicio();
    }

    if (dato == 2) {
        sonido_nivel1();
    }

    if (dato == 3) {
        sonido_nivel2();
    }

    if (dato == 4) {
        sonido_nivel3();
    }

    if (dato == 5) {
        sonido_correcto();
    }

    if (dato == 6) {
        sonido_error();
    }

    if (dato == 7) {
        sonido_victoria();
    }
}

void main() {
    OSCCON = 0x70;      // Oscilador interno a 8 MHz

    ANSEL = 0;
    ANSELH = 0;

    TRISB = 0x1F;       // RB0, RB1, RB2, RB3 y RB4 como entradas
    TRISC = 0x00;       // Puerto C como salida

    PORTB = 0;
    PORTC = 0;

    BUZZER = 0;

    while(1) {
        if (RB4_bit == 1) {
            comando = leer_comando();
            ejecutar_comando(comando);

            while(RB4_bit == 1) {
                // Espera hasta que el ATmega baje el STROBE
            }

            Delay_ms(50);
        }
    }
}