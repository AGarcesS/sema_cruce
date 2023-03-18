int estado;

int next1 = 0;

int peaton1 = 0;
int peaton2 = 0;
int espira = 0;

int principal_verde = 1;
int principal_amarillo = 0;
int principal_rojo = 0;
int secundaria_verde = 0;
int secundaria_amarillo = 0;
int secundaria_rojo = 1;

active proctype fsm() {
	estado = 0;
	do
	::(estado == 0) -> atomic{ //principal en verde, secundaria en rojo
        if
		:: (peaton1 || espira) && next1 -> 
			principal_verde = 0; principal_amarillo = 1; principal_rojo = 0; 
			secundaria_verde = 0; secundaria_amarillo = 0; secundaria_rojo = 1; 
			estado = 1; 
			next1 = 0;
            printf("principal en amarillo, secundaria en rojo\n");
		fi
    }		    
    ::(estado == 1) -> atomic{ //principal en amarillo, secundaria en rojo
        if        
        :: next1 -> 
			principal_verde = 0; principal_amarillo = 0; principal_rojo = 1; 
			secundaria_verde = 1; secundaria_amarillo = 0; secundaria_rojo = 0;
			estado = 2; 
			peaton1 = 0; espira = 0; next1 = 0;
            printf("principal en rojo, secundaria en verde: cruza peaton1 y/o espira\n");   
        fi
    }	
	::(estado == 2) -> atomic{ //principal en rojo, secundaria en verde
        if        
        :: next1 -> 
			principal_verde = 0; principal_amarillo = 0; principal_rojo = 1; 
			secundaria_verde = 0; secundaria_amarillo = 1; secundaria_rojo = 0;
			estado = 3; 
			next1 = 0;
            printf("principal en rojo, secundaria en amarillo\n");             
        fi
    }		
	::(estado == 3) -> atomic{ //principal en rojo, secundaria en amarillo
        if        
        :: next1 -> 
			principal_verde = 1; principal_amarillo = 0; principal_rojo = 0; 
			secundaria_verde = 0; secundaria_amarillo = 0; secundaria_rojo = 1; 
			estado = 0;
			peaton2 = 0; next1 = 0;
            printf("principal en verde, secundaria en rojo: cruza peaton2\n");             
        fi
    }	
	od
}

active proctype entorno() {
	do
	:: peaton1 = 1; printf("peaton1 = 1\n")
	:: peaton2 = 1; printf("peaton2 = 1\n")
	:: espira = 1; printf("espira = 1\n")
    :: next1 = 1; printf("next1 = 1\n")
	:: skip -> skip
	od
}

ltl peaton1_liveness {
	[]<> (peaton1) -> []<> (!peaton1)
}

ltl peaton2_liveness {
	[]<> (peaton2) -> []<> (!peaton2)
}

ltl espira_liveness {
	[]<> (espira) -> []<> (!espira)
}