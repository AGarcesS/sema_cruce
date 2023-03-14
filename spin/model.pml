int estado;

int next1 = 0;
int next2 = 0;

int peaton1 = 0;
int peaton2 = 0;
int espira = 0;

int semaforo_principal = 1;
int semaforo_secundaria = 0;

active proctype fsm() {
	estado = 0;
	do
	::(estado == 0) -> atomic{ 
        if
		:: (peaton1 || espira) && !peaton2 -> 
			semaforo_principal = 0; semaforo_secundaria = 1; estado = 1 
			peaton1 = 0; espira = 0
            printf("principal en rojo, secundaria en verde\n")

		:: peaton2 -> 
			semaforo_principal = 1; semaforo_secundaria = 0; estado = 0 
			peaton2 = 0; next2 = 0
            printf("principal en verde, secundaria en rojo\n")
		fi
    }		    
    ::(estado == 1) -> atomic{
        if        
        :: !peaton2 && !peaton1 && next1 -> 
			semaforo_principal = 1; semaforo_secundaria = 0; estado = 0 
			next1 = 0;
            printf("principal en verde, secundaria en rojo\n")

        :: peaton2 && !peaton1 -> 
		    semaforo_principal = 0; semaforo_secundaria = 0; estado = 2
			peaton2 = 0;		
            printf("principal en rojo, secundaria en rojo\n")

		:: peaton1 -> 
		    semaforo_principal = 0; semaforo_secundaria = 1; estado = 1
			peaton1 = 0; 		
            printf("principal en rojo, secundaria en verde\n")       
        fi
    }	
	::(estado == 2) -> atomic{ 
        if        
        :: next1 -> 
			semaforo_principal = 1; semaforo_secundaria = 0; estado = 0 
			next1 = 0
            printf("principal en verde, secundaria en rojo\n")             
        fi
    }		
	od
}

active proctype entorno() {
	do
	:: peaton1 = 1; printf("peaton1 = 1\n")
	:: peaton2 = 1; printf("peaton2 = 1\n")
	:: espira = 1; printf("espira = 1\n")
    :: next1 = 1; printf("tiempo de evento1 = 1\n")
    :: next2 = 1; printf("tiempo de evento2 = 1\n")
	:: skip -> skip
	od
}

ltl peaton1_liveness {
	[]<> (peaton1) -> []<> (peaton1 == 0)
}

ltl peaton2_liveness {
	[]<> (peaton2) -> []<> (peaton2 == 0)
}

ltl espira_liveness {
	[]<> (espira) -> []<> (espira == 0)
}