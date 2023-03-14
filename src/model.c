#include <stdio.h>
#include <time.h>
#include "fsm.h"

#define TIME_PEATON 200

//estados
enum cruce_state {
    VERDE1_ROJO2,
    ROJO1_VERDE2,
    ROJO1_ROJO2
};

//funciones de utilidad
static int timer = 0;
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

static int llegada_peaton2_1 (fsm_t* this) { return (peaton2); }
static int llegada_peaton1_espira (fsm_t* this) { return (peaton1 || espira) && !peaton2; }
static int fin_tiempo_peaton1_espira (fsm_t* this) { return (!peaton1 && !peaton2 && timer); }
static int llegada_peaton1 (fsm_t* this) { return (peaton1); }
static int llegada_peaton2_2 (fsm_t* this) { return (!peaton1 && peaton2); }
static int fin_tiempo_peaton1 (fsm_t* this) { return (timer); }

//funciones de salida
static int semaforo_principal = 1, semaforo_secundaria = 0;

static void mantener_verde1_rojo2 (fsm_t* this){
    semaforo_principal = 1;
    semaforo_secundaria = 0;
    peaton2 = 0;
    printf("Se mantiene principal en verde, secundaria en rojo\n");
}

static void cambio_rojo1_verde2 (fsm_t* this){
    semaforo_principal = 0;
    semaforo_secundaria = 1;
    peaton1 = 0;
    espira = 0;
    timer_start (TIME_PEATON);
    printf("Principal en rojo, secundaria en verde\n");
}

static void cambio_verde1_rojo2 (fsm_t* this){
    semaforo_principal = 1;
    semaforo_secundaria = 0;
    timer = 0;
    printf("Principal en verde, secundaria en rojo\n");
}

static void mantener_rojo1_verde2 (fsm_t* this){
    semaforo_principal = 0;
    semaforo_secundaria = 1;
    peaton1 = 0;
    printf("Se mantiene principal en rojo, secundaria en verde\n");
}

static void cambio_rojo1_rojo2 (fsm_t* this){
    semaforo_principal = 0;
    semaforo_secundaria = 0;
    peaton2 = 0;
    timer_start (TIME_PEATON);
    printf("Principal en rojo, secundaria en rojo\n");
}

//tabla de transiciones
static fsm_trans_t cruce_tt[] = {
    { VERDE1_ROJO2, llegada_peaton2_1, VERDE1_ROJO2, mantener_verde1_rojo2  },
    { VERDE1_ROJO2, llegada_peaton1_espira, ROJO1_VERDE2, cambio_rojo1_verde2  },
    { ROJO1_VERDE2, fin_tiempo_peaton1_espira, VERDE1_ROJO2,  cambio_verde1_rojo2 }, 
    { ROJO1_VERDE2, llegada_peaton1, ROJO1_VERDE2,  mantener_rojo1_verde2 },    
    { ROJO1_VERDE2, llegada_peaton2_2, ROJO1_ROJO2,  cambio_rojo1_rojo2 },
    { ROJO1_ROJO2, fin_tiempo_peaton1, VERDE1_ROJO2,  cambio_verde1_rojo2 },
    {-1, NULL, -1, NULL },
};