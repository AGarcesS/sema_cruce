#include <stdio.h>
#include <time.h>
#include "fsm.h"

#define TIME_AMARILLO 2000
#define TIME_VERDE 8000

//estados
enum cruce_state {
    VERDE1_ROJO2,
    AMARILLO1_ROJO2,
    ROJO1_VERDE2,
    ROJO1_AMARILLO2
};

//funciones de utilidad
static int timer = 1;
static void timer_isr (union sigval arg) { timer = 1; }
static void timer_start (int ms){
    timer_t timerid;
    struct itimerspec spec;
    struct sigevent se;
    se.sigev_notify = SIGEV_THREAD;
    se.sigev_value.sival_ptr = &timerid;
    se.sigev_notify_function = timer_isr;
    se.sigev_notify_attributes = NULL;
    spec.it_value.tv_sec = ms / 1000;
    spec.it_value.tv_nsec = (ms % 1000) * 1000000;
    spec.it_interval.tv_sec = 0;
    spec.it_interval.tv_nsec = 0;
    timer_create (CLOCK_REALTIME, &se, &timerid);
    timer_settime (timerid, 0, &spec, NULL);
}

//funciones de guarda
static int peaton1 = 0, peaton2 = 0, espira = 0;

static int llegada_peaton1_espira_1 (fsm_t* this) { return (peaton1 || espira) && timer; }
static int llegada_peaton2 (fsm_t* this) { return peaton2; }
static int llegada_peaton1_espira_2 (fsm_t* this) { return (peaton1 || espira) && !timer; }
static int fin_tiempo (fsm_t* this) { return timer; }

//funciones de salida
enum semaforo_output {
    VERDE,
    AMARILLO,
    ROJO
};

static int semaforo_principal = VERDE, semaforo_secundaria = ROJO;

static void cambio_amarillo1_rojo2 (fsm_t* this){
    semaforo_principal = AMARILLO;
    semaforo_secundaria = ROJO;
    timer = 0;
    timer_start (TIME_AMARILLO);
    printf("Principal en amarillo, secundaria en rojo\n");
}

static void mantiene_verde1_rojo2 (fsm_t* this){
    semaforo_principal = VERDE;
    semaforo_secundaria = ROJO;
    peaton2 = 0;
    printf("Principal en verde, secundaria en rojo: cruza peaton2\n");
}

static void cambio_rojo1_verde2 (fsm_t* this){
    semaforo_principal = ROJO;
    semaforo_secundaria = VERDE;
    peaton1 = 0;
    espira = 0;
    timer = 0;
    timer_start (TIME_VERDE);
    printf("Principal en rojo, secundaria en verde: cruza peaton1 y/o espira\n");
}

static void cambio_rojo1_amarillo2 (fsm_t* this){
    semaforo_principal = ROJO;
    semaforo_secundaria = AMARILLO;
    timer = 0;
    timer_start (TIME_AMARILLO);
    printf("Principal en rojo, secundaria en amarillo\n");
}

static void mantiene_rojo1_verde2 (fsm_t* this){
    semaforo_principal = ROJO;
    semaforo_secundaria = VERDE;
    peaton1 = 0;
    espira = 0;
    printf("Principal en rojo, secundaria en verde: cruza peaton1 y/o espira\n");
}

static void cambio_verde1_rojo2 (fsm_t* this){
    semaforo_principal = VERDE;
    semaforo_secundaria = ROJO;
    peaton2 = 0;
    timer = 0;
    timer_start (TIME_VERDE);
    printf("Principal en verde, secundaria en rojo: cruza peaton2\n");
}

//tabla de transiciones
static fsm_trans_t cruce_tt[] = {
    { VERDE1_ROJO2, llegada_peaton1_espira_1, AMARILLO1_ROJO2, cambio_amarillo1_rojo2  },
    { VERDE1_ROJO2, llegada_peaton2, VERDE1_ROJO2, mantiene_verde1_rojo2  },
    { AMARILLO1_ROJO2, fin_tiempo, ROJO1_VERDE2, cambio_rojo1_verde2  },
    { ROJO1_VERDE2, fin_tiempo, ROJO1_AMARILLO2,  cambio_rojo1_amarillo2 }, 
    { ROJO1_VERDE2, llegada_peaton1_espira_2, ROJO1_VERDE2,  mantiene_rojo1_verde2 }, 
    { ROJO1_AMARILLO2, fin_tiempo, VERDE1_ROJO2,  cambio_verde1_rojo2 },
    {-1, NULL, -1, NULL },
};