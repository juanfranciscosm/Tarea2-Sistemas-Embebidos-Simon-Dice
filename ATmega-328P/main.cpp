#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

#define DIN     PB3
#define CS      PB2
#define CLK     PB5

#define STROBE  PD4

const uint16_t demora_lenta = 900;
const uint16_t demora_media = 600;
const uint16_t demora_rapida = 350;

uint8_t nivel = 1;
uint8_t longitud_secuencia = 3;
uint8_t secuencia[7];

const uint8_t flecha_arriba[8] = {0B00011000,0B00111100,0B01111110,0B00011000,0B00011000,0B00011000,0B00011000,0B00000000};
const uint8_t flecha_abajo[8]  = {0B00011000,0B00011000,0B00011000,0B00011000,0B01111110,0B00111100,0B00011000,0B00000000};
const uint8_t flecha_izquierda[8] = {0B00010000,0B00110000,0B01111110,0B11111110,0B01111110,0B00110000,0B00010000,0B00000000};
const uint8_t flecha_derecha[8]   = {0B00001000,0B00001100,0B01111110,0B01111111,0B01111110,0B00001100,0B00001000,0B00000000};
const uint8_t error_x[8] = {0B10000001,0B01000010,0B00100100,0B00011000,0B00011000,0B00100100,0B01000010,0B10000001};
const uint8_t correcto[8] = {0B00000001,0B00000010,0B00000100,0B10001000,0B01010000,0B00100000,0B00000000,0B00000000};
const uint8_t nivel_1[8] = {0B10000100,0B10001100,0B10000100,0B10000100,0B10000100,0B10000100,0B11101110,0B00000000};
const uint8_t nivel_2[8] = {0B10001110,0B10000010,0B10000010,0B10001110,0B10001000,0B10001000,0B11101110,0B00000000};
const uint8_t nivel_3[8] = {0B10001110,0B10000010,0B10000010,0B10000110,0B10000010,0B10000010,0B11101110,0B00000000};

void esperar_ms(uint16_t tiempo) {
    while (tiempo--) { _delay_ms(1); }
}

void max7219_shift(uint8_t dato) {
    for (uint8_t i = 0; i < 8; i++) {
        if (dato & 0B10000000) PORTB |= (1 << DIN);
        else PORTB &= ~(1 << DIN);
        PORTB |= (1 << CLK);
        _delay_us(1);
        PORTB &= ~(1 << CLK);
        dato <<= 1;
    }
}

void max7219_send(uint8_t direccion, uint8_t dato) {
    PORTB &= ~(1 << CS);
    max7219_shift(direccion);
    max7219_shift(dato);
    PORTB |= (1 << CS);
}

void max7219_init() {
    DDRB |= (1 << CS) | (1 << DIN) | (1 << CLK);
    PORTB |= (1 << CS);
    PORTB &= ~(1 << CLK);
    max7219_send(0x09, 0x00); 
    max7219_send(0x0A, 0x08); 
    max7219_send(0x0B, 0x07); 
    max7219_send(0x0C, 0x01); 
    max7219_send(0x0F, 0x00); 
}

void limpiar_matriz() {
    for (uint8_t i = 1; i <= 8; i++) max7219_send(i, 0x00);
}

void mostrar_figura(const uint8_t figura[8]) {
    for (uint8_t i = 0; i < 8; i++) max7219_send(i + 1, figura[i]);
}

void enviar_comando_pic(uint8_t comando) {
    PORTD = (PORTD & 0B11110000) | (comando & 0B00001111);
    PORTD |= (1 << STROBE);
    _delay_ms(50);
    PORTD &= ~(1 << STROBE);
    _delay_ms(50);
}

void configurar_puertos() {
    DDRD |= 0B00011111;
    PORTD &= 0B11100000;
    DDRC &= ~((1 << PC0) | (1 << PC1) | (1 << PC2) | (1 << PC3));
    PORTC |= (1 << PC0) | (1 << PC1) | (1 << PC2) | (1 << PC3);
}

uint8_t leer_boton() {
    if (!(PINC & (1 << PC0))) {
        _delay_ms(50);
        while (!(PINC & (1 << PC0)));
        _delay_ms(50);
        return 1;
    }
    if (!(PINC & (1 << PC1))) {
        _delay_ms(50);
        while (!(PINC & (1 << PC1)));
        _delay_ms(50);
        return 2;
    }
    if (!(PINC & (1 << PC2))) {
        _delay_ms(50);
        while (!(PINC & (1 << PC2)));
        _delay_ms(50);
        return 3;
    }
    if (!(PINC & (1 << PC3))) {
        _delay_ms(50);
        while (!(PINC & (1 << PC3)));
        _delay_ms(50);
        return 4;
    }
    return 0;
}

uint8_t numero_aleatorio() {
    static uint8_t semilla = 7;
    semilla = semilla * 17 + 43;
    return (semilla % 4) + 1;
}

void generar_secuencia_completa() {
    for (uint8_t i = 0; i < 7; i++) {
        secuencia[i] = numero_aleatorio();
    }
}

void mostrar_simbolo(uint8_t simbolo) {
    limpiar_matriz();
    if (simbolo == 1) mostrar_figura(flecha_arriba);
    else if (simbolo == 2) mostrar_figura(flecha_abajo);
    else if (simbolo == 3) mostrar_figura(flecha_izquierda);
    else if (simbolo == 4) mostrar_figura(flecha_derecha);
}

void mostrar_nivel() {
    limpiar_matriz();
    if (nivel == 1) { mostrar_figura(nivel_1); enviar_comando_pic(2); }
    else if (nivel == 2) { mostrar_figura(nivel_2); enviar_comando_pic(3); }
    else if (nivel == 3) { mostrar_figura(nivel_3); enviar_comando_pic(4); }
    esperar_ms(1200);
    limpiar_matriz();
}

void mostrar_secuencia() {
    uint16_t velocidad;
    if (nivel == 1) velocidad = demora_lenta;
    else if (nivel == 2) velocidad = demora_media;
    else velocidad = demora_rapida;

    for (uint8_t i = 0; i < longitud_secuencia; i++) {
        mostrar_simbolo(secuencia[i]);
        esperar_ms(velocidad);
        limpiar_matriz();
        esperar_ms(250);
    }
}

uint8_t verificar_respuesta_usuario() {
    uint8_t boton;
    for (uint8_t i = 0; i < longitud_secuencia; i++) {
        boton = 0;
        while (boton == 0) boton = leer_boton();

        if (boton == secuencia[i]) {
            enviar_comando_pic(5);
            mostrar_simbolo(boton);
            esperar_ms(300);
            limpiar_matriz();
        } else {
            enviar_comando_pic(6);
            mostrar_figura(error_x);
            esperar_ms(1500);
            limpiar_matriz();
            return 0;
        }
    }
    return 1;
}

int main() {
    configurar_puertos();
    max7219_init();
    limpiar_matriz();

    enviar_comando_pic(1);

    while (1) {
        if (nivel == 1) {
            longitud_secuencia = 3;
            generar_secuencia_completa(); 
        } else if (nivel == 2) {
            longitud_secuencia = 5;
        } else {
            longitud_secuencia = 7;
        }

        mostrar_nivel();
        mostrar_secuencia();

        if (verificar_respuesta_usuario()) {
            enviar_comando_pic(7);
            mostrar_figura(correcto);
            esperar_ms(1500);
            limpiar_matriz();

            nivel++;
            if (nivel > 3) nivel = 1;

        } else {
            nivel = 1;
        }

        esperar_ms(1000);
    }
}