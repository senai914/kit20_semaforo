/*
 * SEMAFORO S3250 Para situação de aprendizagem do módulo MANUTENÇÃO DE SISTEMAS ELETRÔNICOS - CURSO TÉCNICO ELETROELETRÔNICA EAD
 * Trata-se do mesmo programa utilizado na unidade curricular de Instalação de Sistemas Eletrônicos, porém serão introduzidas falhas
 * para exercitar a pesquisa de defeitos.
 *
 * Requer o KIT Didatico - Módulo Eletrônica
 *
 * File:   main.c
 * Autor Original: Carlos José Pereira Ferreira - CFP 1.06 -  SENAI SP/DN
 * Criado em 15 de Maio de 2012, 22:53
 *
 * Modificações (06/05/2022): Kleber Lima da Silva CFP 9.14 - SENAI SP
 */

/* Bibliotecas --------------------------------------------------------------*/
#include <xc.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "lcd.h"


/* Definições e Constantes --------------------------------------------------*/
#define _XTAL_FREQ 4000000

/* Botões */
#define BOTAO_CH2   PORTCbits.RC4
#define BOTAO_CH3   PORTCbits.RC2
#define BOTAO_CH4   PORTBbits.RB0
#define BOTAO_CH5   PORTBbits.RB1

/* LEDS do Semáforo */
#define FASE_1_VERMELHO PORTDbits.RD2
#define FASE_1_AMARELO  PORTDbits.RD1
#define FASE_1_VERDE    PORTDbits.RD0

#define FASE_2_VERMELHO PORTDbits.RD5
#define FASE_2_AMARELO  PORTDbits.RD4
#define FASE_2_VERDE    PORTDbits.RD3

#define FASE_3_VERMELHO PORTCbits.RC1
#define FASE_3_AMARELO  PORTDbits.RD7
#define FASE_3_VERDE    PORTDbits.RD6

/* Potenciômetro Digital */
#define POTENCIOMETRO_CS    PORTEbits.RE2
#define POTENCIOMETRO_INC   PORTEbits.RE0
#define POTENCIOMETRO_UD    PORTEbits.RE1
#define POTENCIOMETRO_INCREMENTO 1
#define POTENCIOMETRO_DECREMENTO 0

/* Definição de status */
#define VERDE       0
#define AMARELO     1
#define VERMELHO    2
#define ATRASO      3

/* Definição de status de configuracao do semáforo */
#define SEMAFORO            0
#define ALERTAPISCANTE      1
#define TESTELEDS           2
#define AJUSTEFREQUENCIA    3

/* Constantes */
#define cTempo_Timer    3036    /* 4Mhz; 16 bits; prescale 1:4    ->   0.25s */
#define cTempo_Amarelo  3       /* Tempo, em segundos, para o status AMARELO */
#define cVersaoFirmware "2.0"   
#define cIdxMenuMax     11      /* Indica a quantidade de menus */

/* CONFIG Fuses */
#pragma config FOSC = HS    /* HS oscillator */
#pragma config WDTE = OFF   /* Watchdog Timer */
#pragma config PWRTE = OFF  /* Power-up Timer */
#pragma config BOREN = ON   /* Brown-out Reset */
#pragma config LVP = OFF    /* Low-Voltage */
#pragma config CPD = OFF    /* Data EEPROM Memory Code Protection */
#pragma config WRT = OFF    /* Flash Program Memory Write Enable */
#pragma config CP = OFF     /* Flash Program Memory Code Protection */


/* Definições de Tipos ------------------------------------------------------*/
typedef struct
{
    const char* TituloMenu;
    uint8_t ValorMinimo;
    uint8_t ValorMaximo;
    uint8_t TempoFase;
    uint8_t Status;
    uint8_t LEDVERDE:1;
    uint8_t LEDAMARELO:1;
    uint8_t LEDVERMELHO:1;
} sTempoFases;

typedef struct
{
    const char* TituloMenu;
    uint8_t Tempoatraso;
    uint8_t ValorMinimo;
    uint8_t ValorMaximo;
} sTempoAtraso;

/* Variáveis Privadas -------------------------------------------------------*/
sTempoFases vTempoFases[3];
sTempoAtraso vAtrasoFase[3];
uint8_t vQtdFases = 2;
uint8_t vTempo_fase = 0;
uint8_t vTicksTimer = 0;
char szBuf[16];
uint8_t vBotao_pressionado[4];
uint8_t vIdxMenu = 0;
uint8_t vStatusConfig = SEMAFORO;

/* Funções Privadas (Protótipos) --------------------------------------------*/
void AjusteFrequencia (void);
void IniciaAjusteFrequencia (void);
void TesteLEDS (void);
void LuzAlerta (void);
void Semaforo (void);
void MontaTituloMenu (uint8_t IdxMenu);
void MontaValorMenu (uint8_t IdxMenu);
void InicializaPotenciometro (void);
void InicializaSemaforo (void);
void AlteraValorMenu (uint8_t IdxMenu, uint8_t Aumenta);
void InicializaVariaveis(void);
void ConfiguracoesPIC (void);


/******************************************************************************
*
*       PROGRAMA PRINCIPAL
*
******************************************************************************/
void main()
{
    InicializaVariaveis();  /* Carrega valores padrão do semáforo */

    ConfiguracoesPIC(); /* Configura o PIC conforme as necessidades do semáforo */

    MontaTituloMenu(vIdxMenu);  /* Escreve o menu no display */

    InicializaSemaforo();   /* Carrega condição inicial do semáforo */

    InicializaPotenciometro();  /* Inicializa o potenciômetro digital */

    /* LOOP PRINCIPAL - VARREDURA DO TECLADO */
    while (1)
    {
        /* Botão CH2: Equivale a seta para cima nas opções do menu */
        if (BOTAO_CH2 == 1)
        {
            if (vBotao_pressionado[0] == 0)
            {
                vBotao_pressionado[0] = 1;
                if (vIdxMenu == cIdxMenuMax)
                {
                    vIdxMenu = 0;
                }
                else
                {
                    vIdxMenu++;
                }

                MontaTituloMenu(vIdxMenu);
                MontaValorMenu(vIdxMenu);
            }
        }
        else
        {
            /* Flag para evitar que o botão fique "preso" */
            vBotao_pressionado[0] = 0;
        }
        /************************************************************/


        /* Botão CH3: Equivale a seta para baixo nas opções do menu */
        if (BOTAO_CH3 == 1)
        {
            if (vBotao_pressionado[1] == 0)
            {
                vBotao_pressionado[1] = 1;

                if (vIdxMenu == 0)
                {
                    vIdxMenu = cIdxMenuMax;
                }
                else
                {
                    vIdxMenu--;
                }

                MontaTituloMenu(vIdxMenu);
                MontaValorMenu(vIdxMenu);
            }
        }
        else
        {
            vBotao_pressionado[1] = 0;
        }
        /************************************************************/


        /* Botão CH4: Equivale a seta para cima nos valores do menu */
        if (BOTAO_CH4 == 1)
        {
            if (vBotao_pressionado[2] == 0)
            {
                vBotao_pressionado[2] = 1;
                AlteraValorMenu(vIdxMenu, 0);
            }
        }
        else
        {
            vBotao_pressionado[2] = 0;
        }
        /************************************************************/


        /* Botão CH5: Equivale a seta para baixo nos valores do menu */
        if (BOTAO_CH5 == 1)
        {
            if (vBotao_pressionado[3] == 0)
            {
                vBotao_pressionado[3] = 1;
                AlteraValorMenu(vIdxMenu, 1);
            }
        }
        else
        {
            vBotao_pressionado[3] = 0;
        }
        /************************************************************/

        /* Delay do loop principal, de acordo com o ajuste ou fixo (modo normal) */
        if (vStatusConfig == AJUSTEFREQUENCIA)
        {
            AjusteFrequencia();
        }
        else
        {
            __delay_ms(10);
        }
    }
}


/******************************************************************************
*
*       INTERRUPÇÃO DO TIMER 1
*
******************************************************************************/
void __interrupt() isr(void)
{
    /* Testa o flag da interrupção do Timer 1 */
    if (TMR1IF == 1)
    {
        /* Reseta o Timer */
        TMR1H = cTempo_Timer >> 8;
        TMR1L = cTempo_Timer & 0x00FF;
        TMR1IF = 0;
        
        /* Aguarda 4 estouros do timer (4x 0,25s = 1,0s) */
        vTicksTimer++;
        if (vTicksTimer == 4)
        {
            vTicksTimer = 0;

            /* Executa a rotina selecionada */
            switch (vStatusConfig)
            {
                case SEMAFORO:
                {   /* Rotina que controla o acendimento das luzes do semáforo */
                    vTempo_fase--;
                    Semaforo();
                    break;
                }
                case ALERTAPISCANTE:
                {   /* Rotina que controla o alerta piscante amarelo */
                    vTempo_fase--;
                    LuzAlerta();
                    break;
                }
                case AJUSTEFREQUENCIA:
                {   /* A frequencia do ajuste é tratada na main() */
                    break;
                }
            }
        }
    }
}


/******************************************************************************
*
*       AJUSTA A FREQUENCIA CONFORME O CONVEROR A/D AN1
* É USADA APENAS NO MODO "AJUST FREQUENCIA" PARA TREINAR O USO DO OSCILOSCÓPIO
*
******************************************************************************/
void AjusteFrequencia (void)
{
    uint16_t vTempo = 0;

    FASE_3_VERMELHO = !FASE_3_VERMELHO;

    /* Inicializa a conversão do ADC */
    ADCON0bits.ADON = 1;
    ADCON0bits.CHS = 1;
    ADCON0bits.GO = 1;
    while (ADCON0bits.GO_DONE == 1);

    /* Valor do ADC de 0 a 1023 */
    vTempo = (ADRESH << 8) + (ADRESL);

    /* A frequencia pode ser ajustada entre 12Hz até 5000Hz, aproximadamente */
    for (uint16_t i = 0; i < vTempo; i++)
    {
        __delay_us(10);
    }
}


/******************************************************************************
*
*       CONFIGURA AS SAIDAS PARA O AJUSTE DE FREQUENCIA
*
******************************************************************************/
void IniciaAjusteFrequencia (void)
{
    FASE_1_VERDE = 0;
    FASE_1_VERMELHO = 0;
    FASE_1_AMARELO = 0;
    FASE_2_VERDE = 0;
    FASE_2_VERMELHO = 0;
    FASE_2_AMARELO = 0;
    FASE_3_VERDE = 0;
    FASE_3_VERMELHO = 0;
    FASE_3_AMARELO = 0;
}


/******************************************************************************
*
*       ACENDE TODOS OS LEDS PARA VERIFICAR SE ESTÃO FUNCIONANDO
*
******************************************************************************/
void TesteLEDS (void)
{
    FASE_1_VERDE = 1;
    FASE_1_AMARELO = 1;
    FASE_1_VERMELHO = 1;
    FASE_2_VERDE = 1;
    FASE_2_AMARELO = 1;
    FASE_2_VERMELHO = 1;
    FASE_3_VERDE = 1;
    FASE_3_AMARELO = 1;
    FASE_3_VERMELHO = 1;
}


/******************************************************************************
*
*       CONTROLA O ACENDIMENTO DO AMARELO PISCANTE
*
******************************************************************************/
void LuzAlerta (void)
{
    FASE_1_VERDE = 0;
    FASE_1_VERMELHO = 0;
    FASE_1_AMARELO = !FASE_1_AMARELO;
    FASE_2_VERDE = 0;
    FASE_2_VERMELHO = 0;
    FASE_2_AMARELO = FASE_1_AMARELO;

    FASE_3_VERDE = 0;
    FASE_3_VERMELHO = 0;
    if (vQtdFases == 3)
    {
        FASE_3_AMARELO = FASE_1_AMARELO;
    }
    else
    {
        FASE_3_AMARELO = 0;
    }

    if (vIdxMenu == 8)
    {
        ltoa(szBuf, vTempo_fase, 10);
        PosicaoLCD(0xC0);
        EnviaTextoLCD_ROM("ALERTA PISCANTE");
    }
}


/******************************************************************************
*
*       CONTROLA O ACENDIMENTO DAS LUZES DO SEMÁFORO
*
******************************************************************************/
void Semaforo (void)
{
    uint8_t vproxFase = 0;
    uint8_t vSairdoFOR = 0;

    if (vTempo_fase == 0)
    {
        /* Laço para percorrer o array de todas as fases */
        for (uint8_t i = 0; i < vQtdFases; i++)
        {
            /* Evita alterar a condição de uma fase que já foi tratada */
            if (vSairdoFOR == 1) break;
            
            switch (vTempoFases[i].Status)
            {
                /*
                 * Caso o status seja VERDE: muda para amarelo e carrega tempo amarelo
                 */
                case VERDE:
                {
                    vTempoFases[i].LEDVERDE = 0;
                    vTempoFases[i].LEDAMARELO = 1;
                    vTempo_fase = cTempo_Amarelo;
                    vTempoFases[i].Status = AMARELO;
                    vSairdoFOR = 1;
                    break;
                }

                /*
                 * Caso o status seja AMARELO existem duas situações:
                 * 1-COM ATRASO: muda para VERMELHO e marca a proxima fase com status de atraso, para aguardar o tempo antes de passar para verde
                 * 2-SEM ATRASO: muda para VERMELHO e passa a proxima fase imediatamente para VERDE
                 */
                case AMARELO:
                {

                    /* Passa a fase para vermelho */
                    vTempoFases[i].LEDAMARELO = 0;
                    vTempoFases[i].LEDVERMELHO = 1;
                    vTempoFases[i].Status = VERMELHO;

                    /* Pega a proxima fase */
                    if (i == (vQtdFases - 1))
                    {
                        vproxFase = 0;
                    }
                    else
                    {
                        vproxFase = i+1;
                    }

                    /* Verifica se tem atraso */
                    if (vAtrasoFase[i].Tempoatraso > 0)
                    {
                        vTempo_fase = vAtrasoFase[i].Tempoatraso;
                        vTempoFases[vproxFase].Status = ATRASO;
                    }
                    else
                    {
                        vTempoFases[vproxFase].Status = VERDE;
                        vTempoFases[vproxFase].LEDVERMELHO = 0;
                        vTempoFases[vproxFase].LEDVERDE = 1;
                        vTempo_fase = vTempoFases[vproxFase].TempoFase;
                    }
                    vSairdoFOR = 1;
                    break;
                }

                /*
                 * Caso o status seja ATRASO:  muda  para VERDE e carrega variavel de tempo
                 */
                case ATRASO:
                {
                    vTempoFases[i].Status = VERDE;
                    vTempoFases[i].LEDVERMELHO = 0;
                    vTempoFases[i].LEDVERDE = 1;
                    vTempo_fase = vTempoFases[i].TempoFase;
                    vSairdoFOR = 1;
                    break;
                }
            }
        }
    }

    FASE_1_VERMELHO = vTempoFases[0].LEDVERMELHO;
    FASE_1_AMARELO = vTempoFases[0].LEDAMARELO;
    FASE_1_VERDE   = vTempoFases[0].LEDVERDE;

    FASE_2_VERMELHO = vTempoFases[1].LEDVERMELHO;
    FASE_2_AMARELO = vTempoFases[1].LEDAMARELO;
    FASE_2_VERDE   = vTempoFases[1].LEDVERDE;

    FASE_3_VERMELHO = vTempoFases[2].LEDVERMELHO;
    FASE_3_AMARELO  = vTempoFases[2].LEDAMARELO;
    FASE_3_VERDE    = vTempoFases[2].LEDVERDE;

    if (vIdxMenu == 8)
    {
        ltoa(szBuf, vTempo_fase, 10);
        PosicaoLCD(0xC0);
        EnviaTextoLCD_RAM(szBuf);
        EnviaTextoLCD_ROM("             ");
    }
}


/******************************************************************************
*
*       ESCREVE O TITULO DOS MENUS NA PRIMEIRA LINHA DO DISPLAY
*
******************************************************************************/
void MontaTituloMenu (unsigned char IdxMenu)
{
    PosicaoLCD(0x00);

    switch (IdxMenu)
    {
        /* MENU TEMPO FASES - Indices de 0 a 2 */
        case 0:
        case 1:
        case 2:
        {
            EnviaTextoLCD_ROM(vTempoFases[IdxMenu].TituloMenu);
            break;
        }

        /* MENU ATRASO FASES - Indide de 3 a 5 */
        case 3:
        case 4:
        case 5:
        {
            EnviaTextoLCD_ROM(vAtrasoFase[IdxMenu-3].TituloMenu);
            break;
        }

        /* MENU QUANTIDADE DE FASES - Indice 6 */
        case 6:
        {
            EnviaTextoLCD_ROM("Quantidade fases");
            break;
        }

        /* MENU ALTERAR O MODO DE OPERAÇÃO ENTRE SEMAFORO/ALERTA PISCANTE/TESTE DAS LUZES - Indice 7 */
        case 7:
        {
            EnviaTextoLCD_ROM("Modo operacao   ");
            break;
        }

        /* MENU INFORMATIVO: CONTAGEM DE TEMPO */
        case 8:
        {
            EnviaTextoLCD_ROM("Contagem tempo  ");
            //__delay_ms(6);
            PosicaoLCD(0xC0);
            EnviaTextoLCD_ROM("                ");
            break;
        }

        /* MENU INFORMATIVO: VERSÃO DO FIRMWARE */
        case 9:
        {
            EnviaTextoLCD_ROM("Versao firmware ");
            //__delay_ms(6);
            PosicaoLCD(0xC0);
            EnviaTextoLCD_ROM(cVersaoFirmware);
            EnviaTextoLCD_ROM("           ");
            break;
        }

        /* MENU INFORMATIVO: INFORMAÇÕES INICIAIS */
        case 10:
        {
            EnviaTextoLCD_ROM(" Semaforo S3250 ");
            //__delay_ms(6);
            PosicaoLCD(0xC0);
            EnviaTextoLCD_ROM("  SENAI  SP/DN  ");
            break;
        }

        /* POTENCIOMETRO DIGITAL */
        case 11:
        {
            EnviaTextoLCD_ROM("Ajuste Resist.  ");
            //__delay_ms(6);
            PosicaoLCD(0xC0);
            EnviaTextoLCD_ROM("                ");
            break;
        }
    }
    
    __delay_ms(6);
}


/******************************************************************************
*
*       ESCREVE OS VALORES DO MENU NA SEGUNDA LINHA DO DISPLAY
*
******************************************************************************/
void MontaValorMenu (unsigned char IdxMenu)
{
    PosicaoLCD(0xC0);

    switch (IdxMenu)
    {
        /* MENU TEMPO FASES - Indice de 0 a 2 */
        case 0:
        case 1:
        case 2:
        {
            itoa(szBuf, vTempoFases[IdxMenu].TempoFase, 10);
            EnviaTextoLCD_RAM(szBuf);
            EnviaTextoLCD_ROM(" segundos    ");
            break;
        }

        /* MENU ATRASO FASES - Indide de 3 a 5 */
        case 3:
        case 4:
        case 5:
        {
            uint8_t i = IdxMenu - 3;
            itoa(szBuf, vAtrasoFase[i].Tempoatraso, 10);
            EnviaTextoLCD_RAM(szBuf);
            EnviaTextoLCD_ROM(" segundos    ");
            break;
        }

        /* MENU QUANTIDADE DE FASES - Indice 6 */
        case 6:
        {
            if (vQtdFases == 2)
            {
                EnviaTextoLCD_ROM("2               ");
            }
            else
            {
                EnviaTextoLCD_ROM("3               ");
            }

            break;
        }

        /* MENU ALTERAR O MODO DE OPERAÇÃO - Indice 7 */
        case 7:
        {
            switch (vStatusConfig)
            {
                case SEMAFORO:
                {
                    EnviaTextoLCD_ROM("semaforo        ");
                    break;
                }
                case ALERTAPISCANTE:
                {
                    EnviaTextoLCD_ROM("alerta piscante ");
                    break;
                }
                case TESTELEDS:
                {
                    EnviaTextoLCD_ROM("teste das luzes ");
                    break;
                }
                case AJUSTEFREQUENCIA:
                {
                    EnviaTextoLCD_ROM("Ajust frequencia");
                    break;
                }
            }
        }

        /* Os menus 8, 9, 10 e 11 são apenas informativos, por isso os valores não são tratados
        case 8:
        case 9:
        case 10:
        case 11:
        */
    }
}


/******************************************************************************
*
*       INICIALIZA O POTENCIOMETRO DIGITAL
*
******************************************************************************/
void InicializaPotenciometro (void)
{
    /* Habilita o potenciômetro para alterar a resistência */
    POTENCIOMETRO_INC = 0;
    POTENCIOMETRO_CS = 0;

    /* Configura o potenciômetro para diminuir a resistência */
    POTENCIOMETRO_UD = POTENCIOMETRO_DECREMENTO;
    __delay_ms(10);

    /* Envia 100 pulsos para o potenciômetro, para garantir que esteja no 0 */
    for (uint8_t i = 0; i < 100; i++)
    {
        POTENCIOMETRO_INC = 1;
        __delay_ms(10);
        POTENCIOMETRO_INC = 0;
        __delay_ms(10);
    }

    /* Configura o potenciômetro para aumetar a resistência */
    POTENCIOMETRO_UD = POTENCIOMETRO_INCREMENTO; 
    __delay_ms(10);

    /* Envia 50 pulsos para o potenciômetro para deixar em 50% da resistência */
    for (uint8_t i = 0; i < 50; i++)
    {
        POTENCIOMETRO_INC = 1;
        __delay_ms(10);
        POTENCIOMETRO_INC = 0;
        __delay_ms(10);
    }

    /* Desabilita o potenciômetro */
    POTENCIOMETRO_CS = 1;
}


/******************************************************************************
*
*       CONDICÃO INICIAL DO SEMAFORO: LEDS e VARIAVEIS DE TEMPO
*
******************************************************************************/
void InicializaSemaforo (void)
{
    vTempo_fase = vTempoFases[0].TempoFase;

    vTempoFases[0].Status = VERDE;
    vTempoFases[1].Status = VERMELHO;
    vTempoFases[2].Status = VERMELHO;

    vTempoFases[0].LEDVERMELHO = 0;
    vTempoFases[0].LEDAMARELO = 0;
    vTempoFases[0].LEDVERDE = 1;

    vTempoFases[1].LEDVERMELHO = 1;
    vTempoFases[1].LEDAMARELO = 0;
    vTempoFases[1].LEDVERDE = 0;

    vTempoFases[2].LEDAMARELO = 0;
    vTempoFases[2].LEDVERDE = 0;
    if (vQtdFases == 3)
    {
        vTempoFases[2].LEDVERMELHO = 1;
    }
    else
    {
        vTempoFases[2].LEDVERMELHO = 0;
    }

    FASE_1_VERMELHO = vTempoFases[0].LEDVERMELHO;
    FASE_1_AMARELO  = vTempoFases[0].LEDAMARELO;
    FASE_1_VERDE    = vTempoFases[0].LEDVERDE;

    FASE_2_VERMELHO = vTempoFases[1].LEDVERMELHO;
    FASE_2_AMARELO  = vTempoFases[1].LEDAMARELO;
    FASE_2_VERDE    = vTempoFases[1].LEDVERDE;

    FASE_3_VERMELHO = vTempoFases[2].LEDVERMELHO;
    FASE_3_AMARELO  = vTempoFases[2].LEDAMARELO;
    FASE_3_VERDE    = vTempoFases[2].LEDVERDE;
}


/******************************************************************************
*
*       CONTROLA A ALTERAÇÃO DOS VALORES MODIFICADOS POR MEIO DO MENU
*
******************************************************************************/
void AlteraValorMenu (unsigned char IdxMenu, unsigned char Aumenta)
{
    switch (IdxMenu)
    {
        /* MENU TEMPO FASES - Indice de 0 a 2 */
        case 0:
        case 1:
        case 2:
        {
            if (Aumenta == 0)
            {
                if (vTempoFases[IdxMenu].TempoFase == vTempoFases[IdxMenu].ValorMinimo)
                {
                    vTempoFases[IdxMenu].TempoFase = vTempoFases[IdxMenu].ValorMaximo;
                }
                else
                {
                    vTempoFases[IdxMenu].TempoFase--;
                }
            }
            else
            {
                if (vTempoFases[IdxMenu].TempoFase == vTempoFases[IdxMenu].ValorMaximo)
                {
                    vTempoFases[IdxMenu].TempoFase = vTempoFases[IdxMenu].ValorMinimo;
                }
                else
                {
                    vTempoFases[IdxMenu].TempoFase++;
                }
            }

            break;
        }

        /* MENU ATRASO FASES - Indide de 3 a 5 */
        case 3:
        case 4:
        case 5:
        {
            /* O indice é subtraído de três para ficar dentro dos limites da variável */
            uint8_t i = IdxMenu - 3;
            if (Aumenta == 0)
            {
                if(vAtrasoFase[i].Tempoatraso == vAtrasoFase[i].ValorMinimo)
                {
                    vAtrasoFase[i].Tempoatraso = vAtrasoFase[i].ValorMaximo;
                }
                else
                {
                    vAtrasoFase[i].Tempoatraso--;
                }
            }
            else
            {
                if(vAtrasoFase[i].Tempoatraso == vAtrasoFase[i].ValorMaximo)
                {
                    vAtrasoFase[i].Tempoatraso = vAtrasoFase[i].ValorMinimo;
                }
                else
                {
                    vAtrasoFase[i].Tempoatraso++;
                }
            }

            break;
        }

        /* MENU QUANTIDADE DE FASES - Indice 6 */
        case 6:
        {
            if (vQtdFases == 2)
            {
                vQtdFases = 3;
            }
            else
            {
                vQtdFases = 2;
            }

            if (vStatusConfig==SEMAFORO)
            {
                InicializaSemaforo();
            }
            break;
        }

        /* MENU CONFIGURAR O MODO DE OPERAÇÃO - Indice 7 */
        case 7:
        {
            if (Aumenta == 0)
            {
                if (vStatusConfig == 0)
                {
                    vStatusConfig = 3;
                }
                else
                {
                    vStatusConfig--;
                }
            }
            else
            {
                if (vStatusConfig == 3)
                {
                    vStatusConfig = 0;
                }
                else
                {
                    vStatusConfig++;
                }
            }

            switch (vStatusConfig)
            {
                case SEMAFORO:
                {
                    InicializaSemaforo();
                    break;
                }
                case ALERTAPISCANTE:
                {
                    break;
                }
                case TESTELEDS:
                {
                    TesteLEDS();
                    break;
                }
                case AJUSTEFREQUENCIA:
                {
                    IniciaAjusteFrequencia();
                    break;
                }
            }
            break;
        }

        /*Os menus 8, 9 e 10 são apenas informativos, por isso os valores não são tratados
        case 8:
        case 9:
        case 10:
        */

        /* MENU PARA INCREMENTO OU DECREMENTO DA RESISTENCIA - POTENCIOMETRO DIGITAL */
        case 11:
        {
            /* Habilita o potenciômetro para alterar a resistência */
            POTENCIOMETRO_CS = 0;

            if (Aumenta == 0)
            {
                POTENCIOMETRO_UD = POTENCIOMETRO_DECREMENTO;
            }
            else
            {
                POTENCIOMETRO_UD = POTENCIOMETRO_INCREMENTO;
            }

            __delay_ms(1);
            POTENCIOMETRO_INC = 1;

            __delay_ms(1);
            POTENCIOMETRO_INC = 0;

            __delay_ms(1);

            POTENCIOMETRO_CS  =1;
            break;
        }
    }

    MontaValorMenu(IdxMenu);
}


/******************************************************************************
*
*       INICIALIZAÇÃO DOS VALORES PADRÂO PARA O FUNCIONAMENTO DO SEMÁFORO
*
******************************************************************************/
void InicializaVariaveis(void)
{
    /* Variavel de controle de tempo da fase 1 */
    vTempoFases[0].TituloMenu = "Tempo fase 1    ";
    vTempoFases[0].ValorMinimo = 3;
    vTempoFases[0].ValorMaximo = 1200;
    vTempoFases[0].TempoFase = 4;

    /* Variavel de controle de tempo da fase 2 */
    vTempoFases[1].TituloMenu = "Tempo fase 2    ";
    vTempoFases[1].ValorMinimo = 3;
    vTempoFases[1].ValorMaximo = 1200;
    vTempoFases[1].TempoFase = 10;

    /* Variavel de controle de tempo da fase 3 */
    vTempoFases[2].TituloMenu = "Tempo fase 3    ";
    vTempoFases[2].ValorMinimo = 3;
    vTempoFases[2].ValorMaximo = 255;
    vTempoFases[2].TempoFase = 8;

    /* Variavel de controle de atraso da fase 1 (Após a fase ficar vermelho, este o tempo para a proxima ficar verde) */
    vAtrasoFase[0].TituloMenu = "Atraso fase 1   ";
    vAtrasoFase[0].Tempoatraso = 0;
    vAtrasoFase[0].ValorMinimo = 0;
    vAtrasoFase[0].ValorMaximo = 5;

    /* Variavel de controle de atraso da fase 2 (Após a fase ficar vermelho, este o tempo para a proxima ficar verde) */
    vAtrasoFase[1].TituloMenu = "Atraso fase 2   ";
    vAtrasoFase[1].Tempoatraso = 1;
    vAtrasoFase[1].ValorMinimo = 0;
    vAtrasoFase[1].ValorMaximo = 5;

    /* Variavel de controle de atraso da fase 2 (Após a fase ficar vermelho, este o tempo para a proxima ficar verde) */
    vAtrasoFase[2].TituloMenu = "Atraso fase 3   ";
    vAtrasoFase[2].Tempoatraso = 2;
    vAtrasoFase[2].ValorMinimo = 0;
    vAtrasoFase[2].ValorMaximo = 5;

    /* Inicialização padrão: semáforo, duas fases, menu 10 */
    vStatusConfig = SEMAFORO;
    vQtdFases = 2;
    vIdxMenu = 10;
}


/******************************************************************************
*
*       CONFIGURAÇÕES DO PIC PARA O PROJETO
* entradas e saídas digitais, interrupções e timer
*
******************************************************************************/
void ConfiguracoesPIC (void)
{
    /* Configuração da direção dos pinos - 1: entrada | 0: saída */
    TRISAbits.TRISA1 = 1;
    TRISBbits.TRISB0 = 1;
    TRISBbits.TRISB1 = 1;
    TRISCbits.TRISC1 = 0;
    TRISCbits.TRISC2 = 1;
    TRISCbits.TRISC4 = 0;
    TRISD = 0x00;
    TRISEbits.TRISE0 = 0;
    TRISEbits.TRISE1 = 0;
    TRISEbits.TRISE2 = 0;

    /* Configuração do ADC: fosc/4; AN0 e AN1; restande digital */
    ADCON0bits.ADCS = 0b00;
    ADCON1bits.ADCS2 = 1;
    ADCON0bits.CHS = 0b000;
    ADCON0bits.ADON = 0;
    ADCON1bits.ADFM = 1;
    ADCON1bits.PCFG = 0b0100;

    /* Inicializa o LCD */
    InicializaLCD(LCD_FOUR_BIT & LCD_LINES_5X7);

    /* Inicializa o Timer 1 e habilita interrupção */
    T1CON = 0b00100100;     /* Clock interno; prescale 1:4 */
    PIR1bits.TMR1IF = 0;
    PIE1bits.TMR1IE = 1;
    T1CONbits.TMR1ON = 1;
    INTCONbits.PEIE = 1;
    INTCONbits.GIE = 1;
    TMR1H = cTempo_Timer >> 8;
    TMR1L = cTempo_Timer & 0x00FF;
    PIR1bits.TMR1IF = 0;
}
