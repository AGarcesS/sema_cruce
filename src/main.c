#include <sys/time.h>
#include "model.c"

void timeval_add (struct timeval *res, struct timeval *a, struct timeval *b)
{
  res->tv_sec = a->tv_sec + b->tv_sec + a->tv_usec / 1000000 + b->tv_usec / 1000000; 
  res->tv_usec = a->tv_usec % 1000000 + b->tv_usec % 1000000;
}

void timeval_sub (struct timeval *res, struct timeval *a, struct timeval *b)
{

  res->tv_sec = a->tv_sec - b->tv_sec;
  res->tv_usec = a->tv_usec - b->tv_usec;
  if (res->tv_usec < 0) {
    --res->tv_sec;
    res->tv_usec += 1000000;
  }
}

// wait until next_activation (absolute time)
void delay_until (struct timeval* next_activation)
{
  struct timeval now, timeout;
  gettimeofday (&now, NULL);
  timeval_sub (&timeout, next_activation, &now);
  select (0, NULL, NULL, NULL, &timeout);
}

int main(){

    struct timeval clk_period = { 0, 250 * 1000 };
    struct timeval next_activation;

    fsm_t* fsm_cruce = fsm_new(cruce_tt);

    gettimeofday (&next_activation, NULL);   

    printf("Principal en verde, secundaria en rojo\n"); 

    while(scanf("%d %d %d", &peaton1, &peaton2, &espira) == 3){
        fsm_fire(fsm_cruce);
        gettimeofday (&next_activation, NULL);
        timeval_add (&next_activation, &next_activation, &clk_period);
        delay_until (&next_activation);        
    }

    return 1;
}