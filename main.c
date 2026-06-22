#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Entrada {
    char *codigo;
    char *tipo;
    int valor;
    char *estado;
};

struct Horario {
    int horaInicio;
    int minutosInicio;
    int horaFin;
    int minutosFin;
};

struct Visitante {
    struct Entrada entrada;
    char *nombre;
    int edad;
    float altura;
    struct Horario horarioVisita;
};

struct NodoVisitantes {
    struct NodoVisitantes *anterior, *siguiente;
    struct Visitante *datos;
};

struct NodoFila {
    struct Visitante *asignacion;
    struct NodoFila *siguiente;
};

struct Atraccion {
    char *nombre;
    int capacidad;
    int duracion;
    float alturaMinima;
    int edadMinima;
    char *restriccionSeguridad;
    char *estado;
    struct NodoFila *headFila;
    int contadorUso;
};

struct NodoAtracciones {
    struct NodoAtracciones *izquierdo, *derecho;
    struct Atraccion *datos;
};

struct ZonaTematica {
    char *nombre;
    char *codigo;
    char *tematica;
    int capacidadAproximada;
    struct Horario horarios;
    struct NodoAtracciones *raizAtracciones;
};

struct NodoZonaTematica {
    struct NodoZonaTematica *siguiente;
    struct ZonaTematica *datos;
};

struct Parque {
    struct NodoZonaTematica *headZonaTematica;
    struct NodoVisitantes *headVisitantes;
};

void liberarFilaCompleta(struct NodoFila *headFila);

/* Utilidades */
char *copiarCadena(char *cadena) {
    /* Esta funcion simplemente copia las cadenas */

    char *cadenaCopiada = NULL;
    size_t largo; /* Variable para guardar la cantidad char tiene la cadena a copiar, incluido el '\0' */

    /* Caso de entregarle un NULL a la función */
    if (cadena == NULL) return NULL;

    largo = strlen(cadena) + 1;
    /* asignamos memoria y copiamos */
    cadenaCopiada = (char *)malloc(largo);
    strcpy(cadenaCopiada, cadena);

    return cadenaCopiada;
}

void limpiarBuffer(void) {
    /* Limpia el buffer al hacer una transición entre las funciones scanf y fgets */
    while (getchar() != '\n');
}

char *pedirCadena(void) {

    size_t posicion;

    /* hacemos un buffer de 99 caracteres, suficientes para cualquier nombre que se ingrese en este programa */
    char buffer[100];
    /* Pedimos la cadena con fgets */
    fgets(buffer, 100, stdin);
    /* Reemplazamos en el buffer el salto de línea por el caracter nulo */
    posicion = strcspn(buffer, "\n");
    buffer[posicion] = '\0';

    return copiarCadena(buffer);
}

int leerEntero(char *mensaje){
    char entrada[100];
    int i;
    int numero = 0;
    int repetir = 1, esValido; /* Variable de control del ciclo */

    while(repetir){
        printf("%s", mensaje);
        scanf(" %99s", entrada);

        i = 0;
        if (entrada[i] == '\0'){
            printf("Entrada invalida. Debe ingresar un numero entero.\n");
        }else{
            esValido = 1;
            if (entrada[0] == '-' && entrada[1] != '\0'){
                i = 1;
            }else{
                i = 0;
            }

            while(entrada[i] != '\0' && esValido == 1){
                if (entrada[i] < '0' || entrada[i] > '9'){
                    printf("\nEntrada invalida. Debe ingresar solo numeros.\n\n");
                    esValido = 0; /* Esto hace que el ciclo interno termine naturalmente */
                }
                i++;
            }
            /* Si pasó la validación de caracteres, convertimos y terminamos el bucle principal */
            if (esValido == 1) {
                numero = (int)strtol(entrada, NULL, 10);
                repetir = 0; /* Salimos del ciclo limpiamente sin usar break */
            }
        }
    }
    return numero;
}

int compararCadenasIgnorandoMayusculas(const char *s1,const char *s2)
{
    while(*s1 && *s2 && tolower((unsigned char)*s1)==tolower((unsigned char )*s2))
    {
        s1++;
        s2++;
    }
    return tolower((unsigned char)*s1)-tolower((unsigned char)*s2);
}

void pausarPantalla(void)
{
    printf("\nPresione ENTER para continuar...");
    getchar();
}

/* Visitantes */

void liberarListaVisitantes(struct NodoVisitantes *headVisitantes) {
    if (headVisitantes == NULL) return;
    struct NodoVisitantes *rec = headVisitantes->siguiente;
    struct NodoVisitantes *aux;
    while (rec != NULL) {
        aux = rec;
        rec = rec->siguiente;

        if (aux->datos != NULL) {
            free(aux->datos->nombre);
            free(aux->datos->entrada.codigo);
            free(aux->datos->entrada.tipo);
            free(aux->datos->entrada.estado);
            free(aux->datos);
        }
        free(aux);
    }
    free(headVisitantes);
}

void insertarVisitante (struct NodoVisitantes *headVisitantes, struct Visitante *datos) {

    struct NodoVisitantes *nuevo = (struct NodoVisitantes *)malloc(sizeof(struct NodoVisitantes));
    struct NodoVisitantes *ultimo = headVisitantes;

    nuevo->datos = datos;
    nuevo->siguiente=NULL;

    while(ultimo->siguiente!=NULL)
    {
        ultimo=ultimo->siguiente;
    }

    ultimo->siguiente=nuevo;
    nuevo->anterior=ultimo;

}

void eliminarVisitanteDeLista(struct NodoVisitantes *headVisitantes, struct Visitante *visitanteEliminar) {

    struct NodoVisitantes *rec = headVisitantes->siguiente;

    while (rec != NULL) {

        if (rec->datos == visitanteEliminar) {

            rec->anterior->siguiente = rec->siguiente;

            if (rec->siguiente != NULL) {
                rec->siguiente->anterior = rec->anterior;
            }
            free(rec->datos->nombre);
            free(rec->datos->entrada.codigo);
            free(rec->datos->entrada.tipo);
            free(rec->datos->entrada.estado);

            free(rec->datos);
            free(rec);
            return;
        }
        rec = rec->siguiente;
    }
}

int contarVisitantesTotales (struct NodoVisitantes *headVisitantes) {
    struct NodoVisitantes *rec = headVisitantes->siguiente;
    int contador = 0;

    while (rec != NULL) {
        contador++;
        rec = rec->siguiente;
    }

    return contador;
}

struct Visitante *quitarVisitanteDeLista(struct NodoVisitantes **headVisitantes, struct Visitante *visitanteQuitar) {

    struct NodoVisitantes *rec = (*headVisitantes)->siguiente;
    struct Visitante *quitado = NULL;

    while (rec != NULL) {
        if (rec->datos == visitanteQuitar) {

            quitado = rec->datos;
            rec->anterior->siguiente = rec->siguiente;

            if (rec->siguiente != NULL) {
                rec->siguiente->anterior = rec->anterior;
            }
            free(rec);
            return quitado;
        }

        rec = rec->siguiente;
    }

    return NULL;
}

int contarVisitantesDentro (struct NodoVisitantes *headVisitantes) {
    int contador = 0;
    struct NodoVisitantes *rec = headVisitantes->siguiente;

    while (rec != NULL) {
        if (rec->datos->horarioVisita.horaFin == -1) {
            contador++;
        }
        rec = rec->siguiente;
    }
    return contador;
}

struct Visitante* crearVisitante(struct Entrada entrada, char *nombre, int edad, float altura, struct Horario horarioVisita) {

    struct Visitante *nuevoVisitante = (struct Visitante *)malloc(sizeof(struct Visitante));

    nuevoVisitante->entrada.codigo = copiarCadena(entrada.codigo);
    nuevoVisitante->entrada.tipo = copiarCadena(entrada.tipo);
    nuevoVisitante->entrada.estado = copiarCadena(entrada.estado);
    nuevoVisitante->entrada.valor = entrada.valor;

    nuevoVisitante->nombre = copiarCadena(nombre);
    nuevoVisitante->edad = edad;
    nuevoVisitante->altura = altura;
    nuevoVisitante->horarioVisita = horarioVisita;

    return nuevoVisitante;
}

void listarVisitantes (struct NodoVisitantes *headVisitantes) {
    struct NodoVisitantes *rec = headVisitantes->siguiente;


    printf("\n %-10s | %-22s | %-4s | %-15s | %-7s\n", "Codigo", "Nombre Visitante", "Edad", "Tipo Entrada", "Ingreso");
    printf("--------------------------------------------------------------------------\n");
    while (rec != NULL) {
        printf(" %-10.10s | %-22.22s | %-4d | %-15.15s | %02d:%02d ---> ", rec->datos->entrada.codigo, rec->datos->nombre, rec->datos->edad, rec->datos->entrada.tipo, rec->datos->horarioVisita.horaInicio, rec->datos->horarioVisita.minutosInicio);
        if (rec->datos->horarioVisita.horaFin == -1) {
            printf("Aún no ha salido del parque.\n");
        } else {
            printf("Salió a las: %d\n", rec->datos->horarioVisita.horaFin);
        }

        rec = rec->siguiente;
    }

}

void marcarSalidaVisitante (struct NodoVisitantes *headVisitantes, char *codigoEntrada, int horaFin, int minutosFin) {

    struct NodoVisitantes *rec = headVisitantes->siguiente;

    while (rec != NULL) {
        if (strcmp(rec->datos->entrada.codigo, codigoEntrada) == 0){
            rec->datos->horarioVisita.horaFin = horaFin;
            rec->datos->horarioVisita.minutosFin = minutosFin;
            return;
        }
        rec = rec->siguiente;
    }
}

int calcularRecaudacionDiaria(struct NodoVisitantes *headVisitantes) {

    struct NodoVisitantes *nodoActual = headVisitantes->siguiente;
    int totalRecaudado = 0;

    while (nodoActual) {
        totalRecaudado += nodoActual->datos->entrada.valor;
        nodoActual = nodoActual->siguiente;
    }
    return totalRecaudado;
}

struct Visitante *visitanteBuscar(struct NodoVisitantes *headVisitantes, char *codigoBuscado) {
    struct NodoVisitantes *rec = NULL;
    struct Visitante *visitanteBuscado = NULL;

    if (headVisitantes == NULL) {
        return NULL;
    }

    rec = headVisitantes->siguiente;
    while (rec != NULL) {
        if (compararCadenasIgnorandoMayusculas(rec->datos->entrada.codigo, codigoBuscado) == 0) {
            visitanteBuscado = rec->datos;
            return visitanteBuscado;
        }
        rec = rec->siguiente;
    }
    return visitanteBuscado;
}

int modificarVisitante(struct NodoVisitantes *headVisitantes, char *codigoBuscado, char *nuevoNombre, int nuevaEdad, float nuevaAltura, char *nuevoTipoEntrada, char *nuevoEstado) {
    struct Visitante *visitanteEncontrado = NULL;

    visitanteEncontrado = visitanteBuscar(headVisitantes, codigoBuscado);

    /*Si no se encuentra, retornamos 0 para que el menu sepa que fallo */
    if (visitanteEncontrado == NULL) {
        return 0;
    }

    free(visitanteEncontrado->nombre);
    free(visitanteEncontrado->entrada.tipo);
    free(visitanteEncontrado->entrada.estado);

    visitanteEncontrado->nombre = nuevoNombre;
    visitanteEncontrado->edad = nuevaEdad;
    visitanteEncontrado->altura = nuevaAltura;
    visitanteEncontrado->entrada.tipo = nuevoTipoEntrada;
    visitanteEncontrado->entrada.estado = nuevoEstado;

    if (strcmp(nuevoTipoEntrada, "Entrada General") == 0){
        visitanteEncontrado->entrada.valor = 5000;
    }
    else if(strcmp(nuevoTipoEntrada, "Pase Infantil") == 0){
        visitanteEncontrado->entrada.valor = 3000;
    }
    else if(strcmp(nuevoTipoEntrada, "Pase Familiar") == 0){
        visitanteEncontrado->entrada.valor = 8000;
    }
    else if(strcmp(nuevoTipoEntrada, "Pase Rapido") == 0){
        visitanteEncontrado->entrada.valor = 10000;
    }
    else{
        visitanteEncontrado->entrada.valor = 0;
    }
    /*Retornamos 1 indicando que la modificacion se realizó */
    return 1;
}

/* Filas */
/*Funcion para crear nodos privada, solo se conoce  aqui su contenido*/

struct NodoFila *crearNodoFilas(struct Visitante *Datos)
{
    struct NodoFila *PuestoFila = NULL;

    PuestoFila=(struct NodoFila*)malloc(sizeof(struct NodoFila));

    PuestoFila->asignacion=Datos;
    PuestoFila->siguiente=NULL;

    return PuestoFila;
}

/*Resto de funciones para las filas del parque*/

struct Visitante *buscarVisitanteEnFila(struct NodoFila *HeadFila, char *codigo) {
    struct NodoFila *rec = NULL;
    struct Visitante *Buscado = NULL;

    rec = HeadFila->siguiente;

    while(rec != NULL) {
        if(strcmp(rec->asignacion->entrada.codigo, codigo) == 0) {
            Buscado = rec->asignacion;
            return Buscado;
        }
        rec = rec->siguiente;
    }
    return NULL;
}

int buscarVisitanteEnArbolAtracciones(struct NodoAtracciones *raiz, char *codigo) {
    if (raiz == NULL) return 0;

    if (raiz->datos != NULL && raiz->datos->headFila != NULL) {
        if (buscarVisitanteEnFila(raiz->datos->headFila, codigo) != NULL) {
            return 1; /* Encontrado */
        }
    }
    if (buscarVisitanteEnArbolAtracciones(raiz->izquierdo, codigo)) {
        return 1;
    }
    return buscarVisitanteEnArbolAtracciones(raiz->derecho, codigo);
}

int estaVisitanteEnAlgunaFila(struct NodoZonaTematica *headZona, char *codigo) {

    struct NodoZonaTematica *recZona = headZona;

    if (headZona == NULL) return 0;
    do {
        if (recZona->datos != NULL && recZona->datos->raizAtracciones != NULL) {
            /* Llamamos a la función de búsqueda en el ABB de la zona*/
            if (buscarVisitanteEnArbolAtracciones(recZona->datos->raizAtracciones, codigo)) {
                return 1; /* El visitante está esperando en una fila de esta zona */
            }
        }
        recZona = recZona->siguiente;
    } while (recZona != headZona);

    return 0; /* Recorrió todo el parque y no está en ninguna fila */
}

struct NodoFila *crearFilaVacia(void) /*La siguiente funcion creara la fila con nodo fantasma para que puedan agregarse los visitantes*/
{
    return crearNodoFilas(NULL);
}


void agregarVisitanteAFila(struct NodoFila *HeadFila, struct Visitante *Cliente) /*La siguiente funcion agregara un visitante a la fila de una atraccion*/
{
    struct NodoFila *rec = NULL;
    struct NodoFila *NodoPrivado = NULL; /*Nodo que no se vera en ningun momento en el main y solo se sabe que existe en este archivo*/

    NodoPrivado=crearNodoFilas(Cliente);

    rec=HeadFila;

    if(strcmp(Cliente->entrada.tipo,"Pase Rapido")==0) /*Si el visitante tiene el pase rapido, sera el primero en la fila*/
    {
        while(rec->siguiente!=NULL && strcmp(rec->siguiente->asignacion->entrada.tipo,"Pase Rapido")==0) /*Primero verificamos si no habian otros visitantes con Pase Rapido*/
        {
            rec=rec->siguiente;
        }
    }
    else /*Si no tiene Pase Rapido, simplemente lo dejamos al final*/
    {
        while(rec->siguiente!=NULL)  /*Buscamos el ultimo*/
        {
            rec=rec->siguiente;
        }
    }
    NodoPrivado->siguiente=rec->siguiente;  /*Asignamos el siguiente del cliente hacia el siguiente del visitante actual*/
    rec->siguiente=NodoPrivado;             /*y por ultimo el visitante actual apuntara como siguiente al cliente, asi manteniendo el orden y bien conectada la lista*/

}

void eliminarVisitanteDeFila(char *codigo, struct NodoFila *HeadFila) {
    struct NodoFila *rec = NULL, *anterior = NULL;
    rec = HeadFila->siguiente;
    anterior = HeadFila;

    while(rec != NULL) {
        if(strcmp(rec->asignacion->entrada.codigo, codigo) == 0) {
            anterior->siguiente = rec->siguiente;
            free(rec);
            return;
        }
        anterior = rec;
        rec = rec->siguiente;
    }
}

int contarPersonasEnFila(struct NodoFila *HeadFila) /*La siguiente funcion contara la cantidad de personas que esten actualmente en la fila*/
{
    struct NodoFila *rec = NULL;
    int Contador=0;

    rec=HeadFila->siguiente;

    while(rec!=NULL) /*Mientras exista lo contaremos*/
    {
        Contador++;
        rec=rec->siguiente;
    }

    return Contador;
}

int estimarTiempoEspera(struct NodoFila *HeadFila,int capacidad,int duracion) /*La siguiente funcion estimara el tiempo que tomara en que la fila se vacie*/
{
    int TiempoEsperaMinutos=0;
    int CantidadPersonas;

    if(HeadFila!=NULL)
    {
        CantidadPersonas=contarPersonasEnFila(HeadFila); /*Usamos la funcion anterior para contar*/
        if(CantidadPersonas>0)
        {
            while(CantidadPersonas>0) /*Mientras tenga capacidad para iniciar el juego, sera la espera estimada*/
            {
                TiempoEsperaMinutos+=duracion;
                CantidadPersonas-=capacidad;
            }
            return TiempoEsperaMinutos;
        }

    }
    return TiempoEsperaMinutos;
}

struct Visitante **vaciarTodaLaFila( struct NodoFila *HeadFila) /*La siguiente funcion vaciara toda la fila, si la funcion esta en mantenimiento y no entra en suspension la fila*/
{
    struct Visitante **Personas = NULL;
    struct NodoFila *rec = NULL,*aux = NULL;
    int CantidadPersonas;
    int Contador=0;

    if(HeadFila!=NULL) /*Comprobamos que existe el nodo fantasma*/
    {
        if(HeadFila->siguiente!=NULL)
        {
            rec=HeadFila->siguiente;
            CantidadPersonas=contarPersonasEnFila(HeadFila);  /*Calculamos la cantidad de personas en la fila*/
            if(CantidadPersonas>0)
            {
                Personas=(struct Visitante**)malloc(sizeof(struct Visitante*)*CantidadPersonas); /*Damos memoria con el contador*/

                while(rec!=NULL) /*Mientras que encontremos un visitante lo dejamos en el arreglo*/
                {
                    Personas[Contador++]=rec->asignacion;

                    aux=rec;

                    rec=rec->siguiente;

                    free(aux);
                }
                HeadFila->siguiente=NULL; /*Desconectamos el primer visitante de el nodo fantasma*/
                return Personas;

            }
        return NULL;
        }
    }
    return NULL;
}

void mostrarVisitantesEnFila(struct NodoFila *HeadFila) /*La siguiente funcion mostrara toda la fila actual de la atraccion correspondiente*/
{
    struct NodoFila *rec = NULL;
    int posicion=1;

    if(HeadFila==NULL ||HeadFila->siguiente==NULL)
    {
        printf("Error! -> la fila se encuentra vacia");
        return;
    }

    rec=HeadFila->siguiente;

    printf("\nLista de los visitantes en la fila de la atraccion seleccionada:\n");
    printf(" %-4s | %-25s | %-4s | %-15s\n", "Pos", "Nombre del visitante", "Edad", "Tipo entrada");
    printf("------------------------------------------------------------\n");
    while(rec!=NULL)   /*Vamos mostrando el nombre, tipo de entrada y la hora de llegada de todos los visitante para obtener detalles importantes*/
    {
        printf(" %-4d | %-25.25s | %-4d | %-15.15s\n", posicion, rec->asignacion->nombre, rec->asignacion->edad, rec->asignacion->entrada.tipo);

        posicion++;
        rec=rec->siguiente;
    }
    printf("------------------------------------------------------------\n");
}

void atenderVisitantesEnAtraccion(struct NodoFila *HeadFila,int capacidad, int *usos) /*La siguiente funcion atendera a los visitante, los quitara de la fila y los subira a la atraccion*/
{
    struct NodoFila *rec = NULL;
    int ContarPersonas,i,PersonasASubir;

    if(HeadFila!=NULL) /*Si existe la fila*/
    {
        ContarPersonas=contarPersonasEnFila(HeadFila); /*Contamos las personas que tiene en su fila*/
        while(ContarPersonas>0)
        {
            if(ContarPersonas<capacidad) /*Calculamos si las personas que subiran son las personas que sobra*/
            {
                PersonasASubir=ContarPersonas;
            }
            else    /*o si la fila es mas larga, la atraccion empezara con su capacidad maxima*/
            {
                PersonasASubir=capacidad;
            }

            for(i=0;i<PersonasASubir;i++) /*Ahora quitamos a los visitantes de las filas*/
            {
                rec=HeadFila->siguiente;

                HeadFila->siguiente=rec->siguiente;

                free(rec);
            }

            ContarPersonas-=PersonasASubir; /*Disminuimos el contador total de personas para tener la cantidad en la fila actualmente*/

            if(usos!=NULL)
            {
                (*usos)++;
            }

        }


    }

}

void ordenarEvacuadosPorEdad(struct Visitante **Evacuados,int Cantidad)
{
    struct Visitante *Copia = NULL;    /*Ordenamiento bubble sort*/
    int i,j;

    for(i=0;i<Cantidad-1;i++)
    {
        for(j=0;j<Cantidad-i-1;j++)
        {
            if (Evacuados[j] != NULL && Evacuados[j+1] != NULL) {
                if (Evacuados[j]->edad > Evacuados[j+1]->edad) {
                    Copia = Evacuados[j];
                    Evacuados[j] = Evacuados[j+1];
                    Evacuados[j+1] = Copia;
                }
            }
        }
    }
}

void atenderFilaUnaVez(struct NodoFila *HeadFila,int capacidad,int *usos)
{
    struct NodoFila *rec = NULL;
    int CantidadPersonas,i,PersonasASubir;


    if(HeadFila!=NULL)
    {
        rec=HeadFila->siguiente;

        CantidadPersonas=contarPersonasEnFila(HeadFila);

        if(CantidadPersonas>capacidad)
        {
            PersonasASubir=capacidad;
        }
        else
        {
            PersonasASubir=CantidadPersonas;
        }

        for(i=0;i<PersonasASubir;i++)
        {
            HeadFila->siguiente=rec->siguiente;
            free(rec);
            rec=HeadFila->siguiente;

        }
        (*usos)++;

    }

}

/* Atracciones */
void imprimirAtraccionesEnOrden(struct NodoAtracciones *raizArbol)
{
    if(raizArbol)
    {
        imprimirAtraccionesEnOrden(raizArbol->izquierdo);

        printf(" %-22.22s | %-9d | %-8d | %-10.2f | %-4d | %-15.15s\n", raizArbol->datos->nombre, raizArbol->datos->capacidad, raizArbol->datos->duracion, raizArbol->datos->alturaMinima, raizArbol->datos->edadMinima, raizArbol->datos->estado);

        imprimirAtraccionesEnOrden(raizArbol->derecho);
    }
}

void listarAtracciones(struct NodoAtracciones *raizArbol) {
    /* Haremos la lista en in-orden, para que queden ordenadas alfabéticamente */

    if (raizArbol==NULL)
    {
        printf("Error! -> no hay atracciones registradas en esta zona actualmente.\n");
        return;
    }

    printf("\n-----------Lista de atracciones en la zona -----------\n");
    printf(" %-22s | %-9s | %-8s | %-10s | %-4s | %-15s\n", "Nombre de Atraccion", "Capacidad", "Minutos", "Altura min.", "Edad", "Estado");
    printf(" ------------------------------------------------------------------------------------");

    imprimirAtraccionesEnOrden(raizArbol);

    printf(" ------------------------------------------------------------------------------------");
}

struct Atraccion* crearAtraccion(char *nombre, int capacidad, int duracion, float alturaMinima, int edadMinima,
                                 char *restriccionSeguridad, char *estado) {

    struct Atraccion *nueva = (struct Atraccion *)malloc(sizeof(struct Atraccion));

    nueva->nombre = nombre;
    nueva->capacidad = capacidad;
    nueva->duracion = duracion;
    nueva->alturaMinima = alturaMinima;
    nueva->edadMinima = edadMinima;
    nueva->restriccionSeguridad = restriccionSeguridad;
    nueva->estado = estado;
    nueva->headFila = crearFilaVacia();
    nueva->contadorUso = 0;

    return nueva;
}

void insertarAtraccion(struct NodoAtracciones **raiz, struct Atraccion *nuevaAtraccion) {
    int comparacion;

    /* En el caso de que el arbol o rama esté vacío */
    if (!(*raiz)) {
        *raiz = (struct NodoAtracciones *)malloc(sizeof(struct NodoAtracciones));
        (*raiz)->datos = nuevaAtraccion;
        (*raiz)->izquierdo = NULL;
        (*raiz)->derecho = NULL;

        return;
    }

    /* si no está vacío, comparamos */
    comparacion = strcmp(nuevaAtraccion->nombre, (*raiz)->datos->nombre);

    if (comparacion < 0) insertarAtraccion(&((*raiz)->izquierdo), nuevaAtraccion);
    else if (comparacion > 0) insertarAtraccion(&((*raiz)->derecho), nuevaAtraccion);
    else printf("Error: La atracción '%s' ya existe en esta zona temática.\n", nuevaAtraccion->nombre);
}


struct NodoAtracciones *buscarAtraccion(struct NodoAtracciones *arbol, char *nombreBuscado){
    /* Función de búsqueda */

    int comparacion;
    struct NodoAtracciones *atraccionEncontrada = NULL;
    if(!arbol)
    {return NULL;}
    comparacion = strcmp(arbol->datos->nombre, nombreBuscado);
    if(comparacion < 0){
        atraccionEncontrada = buscarAtraccion(arbol->derecho, nombreBuscado);
    }else if(comparacion > 0){
        atraccionEncontrada = buscarAtraccion(arbol->izquierdo, nombreBuscado);
    }else{
        atraccionEncontrada = arbol;
    }
    return atraccionEncontrada;

}

 void reemplazarAtraccion(struct NodoAtracciones **abb, struct NodoAtracciones **aux) {
    /* Función Auxiliar para la funcion eliminarAtraccion
    * Busca el predecesor in-orden o el mayor de los menores y se queda con lo que tiene. */

    /* abb: Puntero doble que viaja por la rama izquierda buscando el final derecho.
     * aux: Puntero doble que apunta al nodo original que queremos eliminar. */

    if (!((*abb)->derecho)) {
        /* CASO BASE: Llegamos al nodo más a la derecha del subárbol izquierdo.
         * Este es el Predecesor In-Order */

        /* 1. Copiamos los datos del predecesor al nodo original */
        (*aux)->datos = (*abb)->datos;

        /* 2. Marcamos el nodo predecesor para que free lo elimine en lugar del nodo original */
        *aux = *abb;

        /* 3. El abuelo ahora apuntará al hijo izquierdo del predecesor (ya que sabemos que no tiene hijo derecho).
         * Esto desconecta al predecesor del árbol de forma segura. */
        *abb = (*abb)->izquierdo;
    }
    else {
        /* PASO RECURSIVO: Si hay un hijo derecho, seguimos bajando hacia la derecha.
         * Pasamos la direccion de memoria del puntero derecho. */
        reemplazarAtraccion(&(*abb)->derecho, aux);
    }
}


void eliminarAtraccion(struct NodoAtracciones **arbol, char *nombreBuscado) {

    struct NodoAtracciones *aux = NULL; /*variable que nos permite almacenar el nodo a eliminar una vez lo encontremos, para luego liberar memoria*/
    struct Atraccion *datosAEliminar = NULL;
    int comparacion; /*Variable para almacenar resultado de comparación*/

    /* Verificamos que el árbol tenga elementos, ya sea al inicio de la ejecución de la función, o tras el recorrido, llegando al último nodo */
    if (!(*arbol)) {
        return;
    }
    /* Realizamos la comparación entre el nodo actual y el nodo buscado*/
    comparacion = strcmp((*arbol)->datos->nombre, nombreBuscado);
    /*Si el valor de la comparación es menor a 0, vamos hacia el nodo de la derecha */
    /*El nodo actual es menor al valor buscado, por lo que buscamos un nodo con un valor mayor para la siguiente comparación*/
    if (comparacion < 0) {
        eliminarAtraccion(&(*arbol)->derecho, nombreBuscado);
    }
    /*Si el valor de la comparación es mayor a 0, vamos hacia el nodo de la izquierda*/
    /*El nodo actual es mayor al valor buscado, por lo que buscamos el nodo siguiente que sea menor*/
    else if (comparacion > 0) {
        eliminarAtraccion(&(*arbol)->izquierdo, nombreBuscado);
    }
    /* --- Si comparación es igual a 0 --- */
    else {
        /* Almacenamos el nodo en el que estamos (que es el que vamos a eliminar) en la variable aux */
        aux = *arbol;
        datosAEliminar = aux->datos;
        /* Verificación: El nodo a eliminar no tiene hijo izquierdo (O es nodo hoja) */
        if (!((*arbol)->izquierdo)) {
            /* Conectamos nuestro nodo padre al hijo derecho */
            *arbol = (*arbol)->derecho;
        }
        /* Si no tiene hijo derecho */
        else if (!((*arbol)->derecho)) {
            /* Conectamos el nodo del padre directo al hijo izquierdo */
            *arbol = (*arbol)->izquierdo;
        }
        /* Si tiene ambos hijos */
        else {
            /* Llamamos a la función que enlazará ambos nodos pasándole el subárbol izquierdo y
             * la dirección del nodo congelado */
            reemplazarAtraccion(&(*arbol)->izquierdo, &aux);
        }
        /* aux ahora apunta al nodo que queríamos borrar,
         * Liberamos ese bloque de memoria para evitar Memory Leaks. */
        free(aux);

        if (datosAEliminar != NULL) {
            free(datosAEliminar->nombre);
            free(datosAEliminar->restriccionSeguridad);
            free(datosAEliminar->estado);
            liberarFilaCompleta(datosAEliminar->headFila);
            free(datosAEliminar);
        }
    }
}

void atenderAtraccion(struct NodoAtracciones *Atraccion) /*Funcion para usar la funcion de atender sin compartir datos de la atraccion*/
{
    if(Atraccion!=NULL && Atraccion->datos!=NULL)
    {
        atenderVisitantesEnAtraccion(Atraccion->datos->headFila,Atraccion->datos->capacidad,&(Atraccion->datos->contadorUso));
    }
}

void mostrarEstadoFilaAtraccion(struct NodoAtracciones *Atraccion) /*Funcion que muestra el tiempo y los datos de la fila de una atraccion especifica, además usamos una funcion de ListaFila.h*/
{
    int TiempoEstimado;
    if(Atraccion!=NULL && Atraccion->datos!=NULL)
    {
        mostrarVisitantesEnFila(Atraccion->datos->headFila);

        TiempoEstimado=estimarTiempoEspera(Atraccion->datos->headFila,Atraccion->datos->capacidad,Atraccion->datos->duracion);

        if (TiempoEstimado > 0) printf("Tiempo estimado para que la fila este vacia : %d minutos \n",TiempoEstimado);
    }
}

void mostrarDatosAtraccion(struct NodoAtracciones *Atraccion) /*Funcion que muestra los datos de la atraccion y sus restricciones de seguridad*/
{
    if(Atraccion!=NULL && Atraccion->datos!=NULL)
    {

        printf("\n================================================================================\n");
        printf("||-----------------------------Ficha de la atraccion--------------------------||\n");
        printf("================================================================================\n");
        printf("|| Nombre           : %-55.55s ||\n",Atraccion->datos->nombre);
        printf("|| Estado           : %-55.55s ||\n",Atraccion->datos->estado);
        printf("|| Capacidad        : %-10d personas                              ||\n",Atraccion->datos->capacidad);
        printf("|| Duracion         : %-10d minutos                               ||\n",Atraccion->datos->duracion);
        printf("|| Altura min.      : %-10.2f metros                                ||\n",Atraccion->datos->alturaMinima);
        printf("|| Edad minima      : %-10d anios                                 ||\n",Atraccion->datos->edadMinima);
        printf("|| Veces utilizada  : %-55d ||\n",Atraccion->datos->contadorUso);
        printf("================================================================================\n");
        printf("|| Regla            : %-55.55s ||\n",Atraccion->datos->restriccionSeguridad);
        printf("================================================================================\n");
    }
    else
        {
            printf("Restricciones: Ninguna\n");
        }
}

void modificarAtraccion(struct NodoAtracciones **raiz, char *nombreViejo, char *nombreNuevo,
                        int capacidad, int duracion, float alturaMinima, int edadMinima,
                        char *restriccionSeguridad, char *estado) {

    struct NodoAtracciones *encontrado = NULL, *nombreDuplicado = NULL;
    struct Atraccion *nuevaAtraccion = NULL;

    /* Primero buscamos si la función a modificar existe en nuestro árbol */
    encontrado = buscarAtraccion(*raiz, nombreViejo);

    if (!encontrado) {
        printf("La atracción '%s' no existe en la base de datos!\n", nombreViejo);
        return;
    }

    /* Evalúo el caso en que no se quiere cambiar el nombre */
    if (strcmp(nombreViejo, nombreNuevo) == 0) {
        free(encontrado->datos->restriccionSeguridad);
        free(encontrado->datos->estado);

        encontrado->datos->capacidad = capacidad;
        encontrado->datos->duracion = duracion;
        encontrado->datos-> alturaMinima = alturaMinima;
        encontrado->datos->edadMinima = edadMinima;
        encontrado->datos->restriccionSeguridad = restriccionSeguridad;
        encontrado->datos->estado = estado;

        free(nombreNuevo);
        printf("La atracción '%s' se modificó exitosamente\n", nombreViejo);
    } else {
        /* Si el nombre cambia necesito retirar la atracción, modificarla y luego volver a insertar
           para que siga ordenado alfabéticamente */


        /* Si el nombre nuevo ya existía en la base de datos */
        nombreDuplicado = buscarAtraccion(*raiz, nombreNuevo);
        if (nombreDuplicado) {
            printf("ERROR: No se puede renombrar, la atracción '%s' ya existe.\n", nombreNuevo);
            return;
        }

        /* creamos la nueva atracción que reemplazará a la antigua */
        nuevaAtraccion = crearAtraccion(nombreNuevo, capacidad, duracion, alturaMinima, edadMinima, restriccionSeguridad, estado);

        /* Libero la fila de la atracción nueva para asignarle la fila de la atracción a modificar */
        /* si no liberara, cada vez que modifico el nombre de una atracción se crea una fila nueva a la cual se
         * le perderá la referencia y nunca será usada lo cual es peligroso */
        free(nuevaAtraccion->headFila);
        nuevaAtraccion->headFila = encontrado->datos->headFila;
        nuevaAtraccion->contadorUso = encontrado->datos->contadorUso;
        encontrado->datos->headFila = NULL;

        /* Eliminamos la atracción a modificar e insertamos la modificada */
        eliminarAtraccion(raiz, nombreViejo);
        insertarAtraccion(raiz, nuevaAtraccion);

        printf("La atracción '%s' fue modificada y reposicionada como '%s' exitosamente.\n", nombreViejo, nombreNuevo);
    }
}

int contarPersonasAtraccion(struct NodoAtracciones *Atraccion)
{
    if(Atraccion!=NULL && Atraccion->datos!=NULL)
    {
        return contarPersonasEnFila(Atraccion->datos->headFila);
    }
    return 0;
}

int estimarTiempoAtraccion(struct NodoAtracciones *Atraccion)
{
    if(Atraccion!=NULL && Atraccion->datos!=NULL)
    {
        return estimarTiempoEspera(Atraccion->datos->headFila,Atraccion->datos->capacidad,Atraccion->datos->duracion);
    }
    return 0;
}

void quitarVisitanteDeFilaAtraccion(struct NodoAtracciones *Atraccion, char *codigo)
{
    if(Atraccion!=NULL && Atraccion->datos!=NULL)
    {
        eliminarVisitanteDeFila(codigo,Atraccion->datos->headFila);
    }
}

struct Visitante *buscarVisitanteAtraccion(struct NodoAtracciones *Atraccion,char *codigo)
{
    if(Atraccion!=NULL && Atraccion->datos!=NULL)
    {
        return buscarVisitanteEnFila(Atraccion->datos->headFila,codigo);
    }
    return NULL;
}

struct Visitante **evacuarFilaAtraccion(struct NodoAtracciones *Atraccion)
{
    if(Atraccion!=NULL && Atraccion->datos!=NULL)
    {
        return vaciarTodaLaFila(Atraccion->datos->headFila);
    }
    return NULL;
}

void formarEnFilaAtraccion(struct NodoAtracciones *Atraccion,struct Visitante *Cliente)
{
    if(Atraccion!=NULL && Atraccion->datos!=NULL)
    {
        if(Cliente->edad<Atraccion->datos->edadMinima)
        {
            printf("Error! -> El visitante no cumple con la edad minima para subirse a la atraccion.\n");

        }
        else
        {
            if(Cliente->altura<Atraccion->datos->alturaMinima)
            {
                printf("Error! -> El visitante no cumple con la altura minima para subirse a la atraccion.\n");

            }
            else
            {
                agregarVisitanteAFila(Atraccion->datos->headFila,Cliente);
                printf("El visitante ha sido formado exitosamente en la fila\n");
            }
        }
    }
}

int contarAtracciones(struct NodoAtracciones *raizArbol) {
    /* Haremos el recorrido recursivo del arbol y contamos cada nodo */
    if (raizArbol) return(1 + contarAtracciones(raizArbol->izquierdo) + contarAtracciones(raizArbol->derecho));
    return 0;
}

int contarVisitantesAtraccionesOperativas(struct NodoAtracciones *raiz) {
    int cantidadVisitantesAtraccion = 0;

    /* Acá al igual que en la funcion de contar atracciones, tomamos en cuenta solo los nodos != NULL */
    if (raiz) {
        if (strcmp(raiz->datos->estado, "Operativa") == 0)
            cantidadVisitantesAtraccion = contarPersonasAtraccion(raiz);
        return (cantidadVisitantesAtraccion + contarVisitantesAtraccionesOperativas(raiz->izquierdo) + contarVisitantesAtraccionesOperativas(raiz->derecho));
    }
    return cantidadVisitantesAtraccion;
}

void atenderAtraccionUnaVez(struct NodoAtracciones *Atraccion)
{
    if(Atraccion!=NULL && Atraccion->datos!=NULL)
    {
        atenderFilaUnaVez(Atraccion->datos->headFila,Atraccion->datos->capacidad,&(Atraccion->datos->contadorUso));
    }
}

void listarAtraccionesNoOperativas(struct NodoAtracciones *raizArbol) {

    if (raizArbol) {
        listarAtraccionesNoOperativas(raizArbol->izquierdo);
        if (strcmp(raizArbol->datos->estado, "Operativa") != 0) printf("%s: [%s]\n", raizArbol->datos->nombre, raizArbol->datos->estado);
        listarAtraccionesNoOperativas(raizArbol->derecho);
    }
}

int sumarUsosAtracciones(struct NodoAtracciones *raiz) {
    int usosActual = 0;

    if (raiz == NULL) {
        return 0;
    }

    if (raiz->datos != NULL) {
        usosActual = raiz->datos->contadorUso;
    }

    return usosActual + sumarUsosAtracciones(raiz->izquierdo) + sumarUsosAtracciones(raiz->derecho);
}

int sumarCapacidadAtracciones(struct NodoAtracciones *nodo) {
    if (nodo == NULL) {
        return 0;
    }
    return nodo->datos->capacidad + sumarCapacidadAtracciones(nodo->izquierdo) + sumarCapacidadAtracciones(nodo->derecho);
}

/* Zonas tematicas */
/*Funciones Publicas de zona tematica*/

/*Busca una zona por su nombre y retorna su struct ZonaTematica*/
struct ZonaTematica * buscarZonaTematica (struct NodoZonaTematica * headZona, char *nombre) {
    struct NodoZonaTematica * rec = NULL;

    if (headZona != NULL) {
        rec = headZona;
        do {
            /*Asumimos que solo puede buscar una atraccion a la vez*/
            if (compararCadenasIgnorandoMayusculas (rec->datos -> nombre, nombre) == 0)
                return rec -> datos;

            rec= rec -> siguiente;
        }while (rec != headZona);
    }
    return NULL;
}



struct ZonaTematica *quitarZona(struct NodoZonaTematica **headZona, char *nombre)
{
    struct ZonaTematica *quitado=NULL;
    struct NodoZonaTematica *rec = NULL;
    struct NodoZonaTematica *aux = NULL;

    if(*headZona != NULL)
    {
        rec=*headZona;
        while(rec->siguiente != *headZona)
        {
            rec = rec->siguiente;
        }

        /* Si el que queremos quitar es el head*/
        if(compararCadenasIgnorandoMayusculas((*headZona)->datos->nombre, nombre) == 0)
        {
            quitado = (*headZona)->datos;
            aux = *headZona;

            /* Si la lista tiene más de un nodo*/
            if((*headZona)->siguiente != *headZona)
            {
                rec->siguiente = (*headZona)->siguiente; /* El ultimo apunta al nuevo head */
                (*headZona)=(*headZona)->siguiente;      /* Se mueve el head */
            }
            else /* Si es el único elemento */
            {
                *headZona=NULL;
            }
            free(aux);
            return quitado;
        }
        else
        {
            rec = *headZona;

            do {
                /* Buscamos mirando un nodo hacia adelante */
                if (compararCadenasIgnorandoMayusculas(rec->siguiente->datos->nombre, nombre) == 0)
                {
                    aux = rec->siguiente;
                    quitado=aux->datos;
                    rec->siguiente = aux->siguiente; /* Lo sacamos y conectamos los de alrededor */
                    free(aux);
                    return quitado;
                }
                rec=rec->siguiente;
            } while(rec->siguiente != *headZona);
        }
    }
    return quitado;
}


/*REGISTRAR ZONA TEMATICA*/
void agregarZonaTematica(struct NodoZonaTematica **headZona, char *nombre, char *codigo, char *tematica, int capacidad, struct Horario horarios) {

    struct NodoZonaTematica *nuevo = NULL, *rec = NULL;
    struct ZonaTematica *zona = NULL;

    zona = (struct ZonaTematica *) malloc(sizeof(struct ZonaTematica));
    /*Solo se copia porque se piden en el main*/
    zona->nombre = copiarCadena(nombre);
    zona->codigo = copiarCadena(codigo);
    zona->tematica = copiarCadena(tematica);
    zona->capacidadAproximada = capacidad;
    zona->horarios = horarios;
    zona->raizAtracciones = NULL;

    nuevo = (struct NodoZonaTematica *) malloc(sizeof(struct NodoZonaTematica));

    nuevo->datos = zona;

    /*Lista vacía*/
    if (*headZona == NULL) {
        *headZona = nuevo;
        nuevo->siguiente = nuevo;
    }
    else {
        rec = *headZona;

        while (rec->siguiente != *headZona)
            rec = rec->siguiente;

        rec->siguiente = nuevo;
        nuevo->siguiente = *headZona;
    }
}

/*LISTAR ZONAS TEMATICAS*/
void listarZonasTematicas(struct NodoZonaTematica *headZona)
{
    struct NodoZonaTematica *rec = NULL;

    if (headZona != NULL) {

        rec = headZona;

        do {

            printf("\n--- ZONA TEMATICA ---\n");
            printf("Nombre: %s\n", rec->datos->nombre);
            printf("Codigo: %s\n", rec->datos->codigo);
            printf("Tematica: %s\n", rec->datos->tematica);
            printf("Capacidad: %d\n", rec->datos->capacidadAproximada);
            if (rec->datos->horarios.minutosInicio <= 9) printf("Hora de apertura %d:0%d\n", rec->datos->horarios.horaInicio, rec->datos->horarios.minutosInicio);
            else printf("Hora de apertura %d:%d\n", rec->datos->horarios.horaInicio, rec->datos->horarios.minutosInicio);
            if (rec->datos->horarios.minutosFin <= 9) printf("Hora de cierre %d:0%d\n", rec->datos->horarios.horaFin, rec->datos->horarios.minutosFin);
            else printf("Hora de cierre %d:%d\n", rec->datos->horarios.horaFin, rec->datos->horarios.minutosFin);

            rec = rec->siguiente;

        } while (rec != headZona);
    }
    else {
        printf("\nNo hay zonas registradas\n");
    }
}



/*MODIFICAR ZONA TEMATICA */
int modificarZonaTematica(struct NodoZonaTematica *headZona, char *codigoBuscar, char *nuevoNombre, char *nuevaTematica, int nuevaCapacidad)
{
    struct NodoZonaTematica *rec = NULL;

    if (headZona != NULL) {

        rec = headZona;

        do {

            if (strcmp(rec->datos->codigo, codigoBuscar) == 0) {

                rec->datos->nombre = nuevoNombre;
                rec->datos->tematica = nuevaTematica;
                rec->datos->capacidadAproximada = nuevaCapacidad;

                return 1;
            }

            rec = rec->siguiente;

        } while (rec != headZona);
    }

    return 0;
}



void listarZonasConAltaCapacidad(struct NodoZonaTematica *headZona, int limite)
{
    struct NodoZonaTematica *rec = NULL;
    int encontrado = 0;

    if (headZona != NULL) {

        rec = headZona;

    printf("\n %-18s | %-10s | %-15s | %-9s\n", "Nombre Zona", "Codigo", "Tematica", "Capacidad");
    printf("--------------------------------------------------------------------------\n");

        do {
            if (rec->datos->capacidadAproximada >= limite) {
                printf(" %-18.18s | %-10.10s | %-15.15s | %-9d\n", rec->datos->nombre, rec->datos->codigo, rec->datos->tematica, rec->datos->capacidadAproximada);

                encontrado = 1;
            }

            rec = rec->siguiente;

        } while (rec != headZona);

    printf("--------------------------------------------------------------------------\n");

        if (encontrado == 0)
            printf("\nNo hay zonas con capacidad mayor o igual a %d\n", limite);
    }
    else {
        printf("\nNo hay zonas registradas\n");
    }
}

int contarZonasTematicas(struct NodoZonaTematica *headZona)
{
    struct NodoZonaTematica *rec = NULL;
    int contador = 0;

    if (headZona != NULL) {

        rec = headZona;

        do {

            contador++;

            rec = rec->siguiente;

        } while (rec != headZona);
    }

    return contador;
}

int capacidadSuficienteZona(struct ZonaTematica *zona) {

    /* Esta función devolvera 1 si aún quedan más visitantes por ingresar a la zona temática */
    if (zona->capacidadAproximada > contarVisitantesAtraccionesOperativas(zona->raizAtracciones)) return 1;
    /* Devuelve 0 si ya no cabe mas gente */
    else return 0;
}

int contarVisitantesEnZona(struct NodoZonaTematica *zona) {
    return contarVisitantesAtraccionesOperativas(zona->datos->raizAtracciones);
}

void formarEnFilaAtraccionZona(struct ZonaTematica *zona, struct NodoAtracciones *Atraccion, struct Visitante *Cliente) {
    /* Si el cliente aún está en el parque */
    if (Cliente->horarioVisita.horaFin == -1) {
        if (Cliente->horarioVisita.horaInicio == zona->horarios.horaFin ) {
            if (Cliente->horarioVisita.minutosInicio >= zona->horarios.minutosFin) {
                printf("No se pudo agregar visitante a la fila, la zona '%s' está cerrada!\n", zona->nombre);
                return;
            } else {
                formarEnFilaAtraccion(Atraccion, Cliente);
                printf("Exito! -> Visitante %s agregado a la fila.\n", Cliente->nombre);
                return;
            }
        } else if (Cliente->horarioVisita.horaInicio < zona->horarios.horaFin ) {
            formarEnFilaAtraccion(Atraccion, Cliente);
            printf("Exito! -> Visitante %s agregado a la fila.\n", Cliente->nombre);
            return;
        } else {
            printf("No se pudo agregar visitante a la fila, la zona '%s' está cerrada!\n", zona->nombre);
            return;
        }

        /* Si el cliente llega antes de la hora de inicio de una zona, necesitamos saber la hora actual para poder comprobar */
    }
    printf("El visitante '%s' no se encuentra en el parque!\n", Cliente->nombre);
}

/*Funciones del Menu*/

void menuAgregarAtraccion(struct NodoAtracciones **raiz)
{

    struct Atraccion *nuevaAtraccion = NULL;
    char *restriccionSeguridad = NULL, *estado = NULL, *nombre = NULL, *entradaTexto = NULL;
    int capacidad, duracion, edadMinima, seleccionEstado, seleccionRestriccion;
    float alturaMinima;


    if(raiz!=NULL)
    {

        /* Hacemos todos los ingresos por teclado */
        printf("Ingrese el nombre de la atraccion: ");
        nombre = pedirCadena();
        capacidad = leerEntero("Ingrese la capacidad de la atraccion: ");
        if(capacidad<=0)
        {
            printf("Error! -> la capacidad ingresada no es valida!\n");
            free(nombre);
            return;
        }

        duracion = leerEntero("Ingrese la duracion de la atraccion (en minutos): ");
        if(duracion<=0)
        {
            printf("Error! -> la duracion ingresada no es valida!\n");
            free(nombre);
            return;
        }

        edadMinima = leerEntero("Ingrese la edad minima para la atraccion: ");
        if(edadMinima<=0)
        {
            printf("Error! -> la edad minima ingresada no es valida!\n");
            free(nombre);
            return;
        }

        limpiarBuffer();
        printf("Ingrese la altura minima para la atracción (en metros con decimal): ");
        entradaTexto = pedirCadena();
        alturaMinima = (float)strtod(entradaTexto, NULL);
        free(entradaTexto);
        if(alturaMinima<=0)
        {
            printf("Error! -> la altura minima ingresada no es valida!");
            free(nombre);
            return;
        }

        limpiarBuffer();
        printf("Elija una de las siguientes opciones para establecer las restriccion de seguridad de su atraccion:\n");

        do {
            printf("Presione 1 para establecer la restriccion de la atraccion como 'No apto para personas con problemas cardiacos'\n");
            printf("Presione 2 para establecer la restriccion de la atraccion como 'No apto para mujeres embarazadas'\n");
            printf("Presione 3 para establecer la restriccion de la atraccion como 'Advertencia -> La atraccion tiene luces que pueden afectar a personas con epilepsia'\n");
            printf("Presione 4 si la atraccion no tiene ninguna restriccion 'Ninguna'\n");
            seleccionRestriccion = leerEntero("Seleccione una opcion: ");
            limpiarBuffer();

            switch (seleccionRestriccion) {
                case 1:
                    restriccionSeguridad = copiarCadena("No apto para personas con problemas cardiacos");
                    break;
                case 2:
                    restriccionSeguridad = copiarCadena("No apto para mujeres embarazadas");
                    break;
                case 3:
                    restriccionSeguridad = copiarCadena("Advertencia -> La atraccion tiene luces que pueden afectar a personas con epilepsia");
                    break;
                case 4:
                    restriccionSeguridad = copiarCadena("Ninguna");
                    break;
                default:
                    printf("Error! Ingrese una opción valida: \n");
                    break;
            }
        } while (seleccionRestriccion > 4 || seleccionRestriccion < 1);

        printf("Elija una de las siguientes opciones para establecer el estado de su atraccion:\n");
        do {
            printf("Presione 1 para establecer la atraccion como 'Operativa'\n");
            printf("Presione 2 para establecer la atraccion como 'En mantenimiento'\n");
            printf("Presione 3 para establecer la atraccion como 'Fuera de servicio'\n");
            printf("Presione 4 si la atraccion está 'Cerrada por horario'\n");
            seleccionEstado = leerEntero("Seleccione una opcion: ");
            limpiarBuffer();

            switch (seleccionEstado) {
                case 1:
                    estado = copiarCadena("Operativa");
                    break;
                case 2:
                    estado = copiarCadena("En mantenimiento");
                    break;
                case 3:
                    estado = copiarCadena("Fuera de servicio");
                    break;
                case 4:
                    estado = copiarCadena("Cerrada por horario");
                    break;
                default:
                    printf("Error! Ingrese una opción válida: \n");
                    break;
        }
        } while (seleccionEstado > 4 || seleccionEstado < 1);

        /* Asignamos los valores ingresados a nuestra nueva atracción, luego la insertamos en el árbol */
        nuevaAtraccion = crearAtraccion(nombre, capacidad, duracion, alturaMinima, edadMinima, restriccionSeguridad, estado);
        insertarAtraccion(raiz, nuevaAtraccion);
    }
    else
    {
        printf("Error! -> La zona tematica no existe o no es posible aniadirle atracciones!\n");
    }
}

void menuEliminarAtraccion(struct NodoAtracciones **raiz)   /*Funcion para eliminar una atraccion especifica*/
{
    char *NombreBuscado = NULL;
    struct NodoAtracciones *Encontrada = NULL;

    printf("Ingrese el nombre de la atraccion que desea eliminar : ");
    NombreBuscado=pedirCadena(); /*Pedimos el nombre al usuario*/

    Encontrada=buscarAtraccion(*raiz,NombreBuscado); /*Ahora buscamos si existe la atraccion que se quiere eliminar*/
    if(Encontrada==NULL)
    {
        printf("\nLa atraccion que se ingreso no existe en el parque!\n");
    }
    else
    {
        eliminarAtraccion(raiz,NombreBuscado);
        printf("\nLa atraccion ha sido eliminada con exito\n");
    }
    free(NombreBuscado);
}

void menuVerEstadoFila(struct NodoAtracciones *raiz)
{
    struct NodoAtracciones *Buscada = NULL;
    char *Nombre = NULL;
    printf("Ingrese la atraccion que desea ver los datos de la fila : "); /*Pedimos el nombre al usuario*/

    Nombre=pedirCadena();

    Buscada=buscarAtraccion(raiz,Nombre);

    if(Buscada==NULL) /*Comprobamos que exista*/
    {
        printf("\nLa atraccion y su fila que desea ver no existe\n");
    }
    else
    {   printf("\n");
        mostrarEstadoFilaAtraccion(Buscada); /*Mostramos los datos de los visitantes*/
    }
    free(Nombre);
}

void menuAtenderFila(struct NodoAtracciones *raiz)
{
    struct NodoAtracciones *Buscada = NULL;
    char *Nombre = NULL;
    printf("Ingrese la atraccion que desea atender : "); /*Pedimos la atraccion al usuario*/

    Nombre=pedirCadena();

    Buscada=buscarAtraccion(raiz,Nombre);

    if(Buscada==NULL) /*Comprobamos que exista*/
    {
        printf("\nLa atraccion que desea atender no existe\n");
    }
    else
    {
        printf("\n");
        atenderAtraccion(Buscada); /*Ponemos en funcionamiento la atraccion y subimos el contador de usos*/
        printf("\nLa atraccion ha sido atendida con exito\n");
    }
    free(Nombre);
}

void menuMostrarAtraccion(struct NodoAtracciones *raiz)
{
    struct NodoAtracciones *Buscada = NULL;
    char *Nombre = NULL;
    printf("Ingrese la atraccion que desea ver : "); /*Pedimos el nombre al usuario*/

    Nombre=pedirCadena();

    Buscada=buscarAtraccion(raiz,Nombre);

    if(Buscada==NULL) /*Comprobamos que exista*/
    {
        printf("\nLa atraccion que desea mostrar no existe\n");
    }
    else
    {
        printf("\n");
        mostrarDatosAtraccion(Buscada); /*Mostramos todos los datos de la atraccion correspondiente*/
    }
    free(Nombre);
}

void menuModificarAtraccion(struct NodoAtracciones **raiz)
{
    char *nombreAtraccion = NULL,*nuevoNombre = NULL,*nuevaRestriccion = NULL,*nuevoEstado = NULL, *entradaTextoFloat = NULL;
    int nuevaCapacidad,nuevaDuracion,nuevaEdadMinima,opcionNuevaRestriccion,opcionNuevoEstado;
    float nuevaAlturaMinima;
    struct NodoAtracciones *objetivo = NULL;

    printf("=============================================\n");
    printf("          Modificacion de Atraccion          \n");
    printf("=============================================\n");

    printf("Ingrese el nombre de la atraccion que desea modificar :\n");
    nombreAtraccion=pedirCadena();

    if(raiz!=NULL)
    {
        objetivo=buscarAtraccion(*raiz,nombreAtraccion);

        if(objetivo!=NULL)
        {
            printf("\nIngrese el nuevo nombre de la atraccion : \n");
            nuevoNombre=pedirCadena();

            nuevaCapacidad = leerEntero("\nIngrese la nueva capacidad de la atraccion : \n");
            if(nuevaCapacidad<=0)
            {
                printf("Error! -> la nueva capacidad ingresada no es valida!\n");
                free(nombreAtraccion);
                free(nuevoNombre);
                return;
            }


            nuevaDuracion = leerEntero("\nIngrese la nueva duracion de la atraccion (en minutos):\n");
            if(nuevaDuracion<=0)
            {
                printf("\nError! -> la nueva duracion ingresada no es valida!\n");
                free(nombreAtraccion);
                free(nuevoNombre);
                return;
            }

            limpiarBuffer();
            printf("\nIngrese la nueva altura minima de la atraccion : \n");
            entradaTextoFloat = pedirCadena();
            nuevaAlturaMinima = (float)strtod(entradaTextoFloat, NULL);
            free(entradaTextoFloat);
            if(nuevaAlturaMinima<=0)
            {
                printf("Error! -> la nueva altura minima ingresada no es valida!\n");
                free(nombreAtraccion);
                free(nuevoNombre);
                return;
            }


            nuevaEdadMinima = leerEntero("\nIngrese la nueva edad minima de la atraccion :\n");
            if(nuevaEdadMinima<=0)
            {
                printf("\nError! -> la edad minima ingresada no es valida!\n");
                free(nombreAtraccion);
                free(nuevoNombre);
                return;
            }


        printf("\nElija una de las siguientes opciones para establecer la nueva restriccion de seguridad de su atraccion:\n\n");

        do {
        printf("Presione 1 para establecer la nueva restriccion de la atraccion como 'No apto para personas con problemas cardiacos'\n");
        printf("Presione 2 para establecer la nueva restriccion de la atraccion como 'No apto para mujeres embarazadas'\n");
        printf("Presione 3 para establecer la nueva restriccion de la atraccion como 'Advertencia -> La atraccion tiene luces que pueden afectar a personas con epilepsia'\n");
        printf("Presione 4 si el estado nuevo de la atraccion no tiene ninguna restriccion 'Ninguna'\n");
        opcionNuevaRestriccion = leerEntero("Seleccione una opcion: ");
        limpiarBuffer();

        switch (opcionNuevaRestriccion) {
            case 1:
                nuevaRestriccion = copiarCadena("No apto para personas con problemas cardiacos");
                break;
            case 2:
                nuevaRestriccion = copiarCadena("No apto para mujeres embarazadas");
                break;
            case 3:
                nuevaRestriccion = copiarCadena("Advertencia -> La atraccion tiene luces que pueden afectar a personas con epilepsia");
                break;
            case 4:
                nuevaRestriccion = copiarCadena("Ninguna");
                break;
            default:
                printf("Error! Ingrese una opción válida: \n");
                break;
        }
        } while (opcionNuevaRestriccion > 4 || opcionNuevaRestriccion < 1);

        printf("\nElija una de las siguientes opciones para establecer el nuevo estado de su atraccion:\n\n");
        do {
        printf("Presione 1 para establecer el nuevo estado de la atraccion como 'Operativa'\n");
        printf("Presione 2 para establecer el nuevo estado de la atraccion como 'En mantenimiento'\n");
        printf("Presione 3 para establecer el nuevo estado de la atraccion como 'Fuera de servicio'\n");
        printf("Presione 4 si el nuevo estado de la atraccion es 'Cerrada por horario'\n");
        opcionNuevoEstado = leerEntero("Seleccione una opcion: ");
        limpiarBuffer();

        switch (opcionNuevoEstado) {
            case 1:
                nuevoEstado = copiarCadena("Operativa");
                break;
            case 2:
                nuevoEstado = copiarCadena("En mantenimiento");
                break;
            case 3:
                nuevoEstado = copiarCadena("Fuera de servicio");
                break;
            case 4:
                nuevoEstado = copiarCadena("Cerrada por horario");
                break;
            default:
                printf("Error! Ingrese una opción válida: \n");
                break;
        }
        } while (opcionNuevoEstado > 4 || opcionNuevoEstado < 1);

        modificarAtraccion(raiz,nombreAtraccion,nuevoNombre,nuevaCapacidad,nuevaDuracion,nuevaAlturaMinima,nuevaEdadMinima,nuevaRestriccion,nuevoEstado);
        free(nombreAtraccion);
        }
    }
}

/*Funciones del menu Visitantes*/

void menuInsertarVisitante(struct NodoVisitantes *headVisitantes) {
    struct Entrada entrada;
    struct Horario horario;
    struct Visitante *nuevoVisitante = NULL;
    struct NodoVisitantes *ultimo=headVisitantes;
    int nuevoId=1;
    char codigo[30], *entradaTextoVisitante = NULL, *nombre = NULL;

    int edad, seleccionEntrada,seleccionEstado;
    float altura;

    printf("\n========================================\n");
    printf("        INGRESO DE VISITANTE        \n");
    printf("========================================\n");


    while(ultimo->siguiente!=NULL)
    {
        ultimo=ultimo->siguiente;
    }

    if(ultimo!=headVisitantes && ultimo->datos!=NULL)
    {
        nuevoId = (int)strtol(ultimo->datos->entrada.codigo, NULL, 10) + 1;
    }

    sprintf(codigo,"%d",nuevoId);

    entrada.codigo=copiarCadena(codigo);

    printf("\nEl codigo asignado automaticamente es: %s\n",entrada.codigo);
    entrada.tipo = NULL;
    entrada.estado = NULL;
    entrada.valor = 0;

    printf("\nIngrese el tipo de entrada: \n\n");
    printf("Seleccione 1 para establecer la entrada como 'Entrada General'\n");
    printf("Seleccione 2 para establecer la entrada como 'Pase Infantil\n");
    printf("Seleccione 3 para establecer la entrada como 'Pase Familiar\n");
    printf("Seleccione 4 para establecer la entrada como 'Pase Rapido\n");
    seleccionEntrada = leerEntero("Seleccione una opcion");
    limpiarBuffer();
    switch (seleccionEntrada) {
        case 1:
            entrada.tipo = copiarCadena("Entrada General");
            entrada.valor= 5000;
            printf("\nEl valor de la entrada se establece como $5000\n");
            break;
        case 2:
            entrada.tipo = copiarCadena("Pase Infantil");
            entrada.valor= 3000;
            printf("\nEl valor de la entrada se establece como $3000\n");
            break;
        case 3:
            entrada.tipo = copiarCadena("Pase Familiar");
            entrada.valor= 8000;
            printf("\nEl valor de la entrada se establece como $8000\n");
            break;
        case 4:
            entrada.tipo = copiarCadena("Pase Rapido");
            entrada.valor= 10000;
            printf("\nEl valor de la entrada se establece como $10000\n");
            break;
        default:
            printf("\nError! -> la opcion que ingreso no es valida\n");
            break;
    }


    do {
        printf("\n");
        printf("Presione 1 para establecer el estado de la entrada como 'Activa'\n");
        printf("Presione 2 para establecer el estado de la entrada como 'Utilizada'\n");
        printf("Presione 3 para establecer el estado de la entrada como 'Anulada'\n");
        printf("Presione 4 para establecer el estado de la entrada como 'Vencida'\n");
        seleccionEstado = leerEntero("Seleccione una opcion: ");
        limpiarBuffer();

        switch (seleccionEstado) {
            case 1:
                entrada.estado = copiarCadena("Activa");
                break;
            case 2:
                entrada.estado = copiarCadena("Utilizada");
                break;
            case 3:
                entrada.estado = copiarCadena("Anulada");
                break;
            case 4:
                entrada.estado = copiarCadena("Vencida");
                break;
            default:
                printf("\nError! Ingrese una opción válida: \n");
                break;
        }
    } while (seleccionEstado > 4 || seleccionEstado < 1);

    /* Validamos que la entrada no esté usada*/
    if (compararCadenasIgnorandoMayusculas(entrada.estado, "Activa") != 0) {
        printf("\nError: La entrada no es valida, o ya fue utlizada, esta anulada o vencida.\n");
        printf("Interrumpiendo el registro del visitante...\n");
        free(entrada.codigo);
        free(entrada.tipo);
        free(entrada.estado);
        return;
    }

    printf("\n--- Datos Personales ---\n");

    printf("\n");
    printf("Ingrese nombre del visitante: ");
    nombre = pedirCadena();

    edad = leerEntero("Ingrese la edad: ");
    if(edad<0)
    {
        printf("\nError! -> la edad ingresada no es valida!\n");
        free(nombre);
        free(entrada.codigo);
        free(entrada.tipo);
        free(entrada.estado);
        return;
    }
    limpiarBuffer();
    printf("Ingrese la altura (en metros, ej. 1.75): ");
    entradaTextoVisitante = pedirCadena();
    altura = (float)strtod(entradaTextoVisitante, NULL);
    free(entradaTextoVisitante);
    if(altura<0)
    {
        printf("\nError! -> la altura ingresada no es valida!\n");
        free(nombre);
        free(entrada.codigo);
        free(entrada.tipo);
        free(entrada.estado);
        return;
    }

    printf("\n--- Horario de Visita ---\n");
    horario.horaInicio = leerEntero("Ingrese la hora de inicio (En formato 24hrs): ");
    if(horario.horaInicio<0 || horario.horaInicio>23)
    {
        printf("\nError! -> la hora ingresada no es valida\n");
        free(nombre);
        free(entrada.codigo);
        free(entrada.tipo);
        free(entrada.estado);
        return;
    }
    horario.minutosInicio = leerEntero("Ingrese el minuto de inicio: ");
    if(horario.minutosInicio<0 || horario.minutosInicio>59)
    {
        printf("\nError! -> el minuto ingresado no es valido\n");
        free(nombre);
        free(entrada.codigo);
        free(entrada.tipo);
        free(entrada.estado);
        return;
    }
    limpiarBuffer();

    horario.horaFin = -1;
    horario.minutosFin = -1;

    nuevoVisitante = crearVisitante(entrada, nombre, edad, altura, horario);

    free(entrada.codigo);
    free(entrada.tipo);
    free(entrada.estado);

    if (nuevoVisitante != NULL)
    {
        insertarVisitante(headVisitantes, nuevoVisitante);
        printf("\n----------------------------------------\n");
        printf("El visitante '%s' fue ingresado a la lista.\n", nombre);
    }
    else
    {
        printf("\nError: No se pudo crear el visitante en memoria.\n");
        free(nombre);
    }
}

void menuEliminarVisitante(struct NodoVisitantes **headVisitantes, struct NodoZonaTematica *headZona){
    char *codigoBuscado = NULL;
    struct Visitante *visitanteEncontrado = NULL;

    struct NodoVisitantes *rec = (*headVisitantes)->siguiente;

    printf("\n========================================\n");
    printf("          ELIMINAR VISITANTE            \n");
    printf("========================================\n");

    if (rec == NULL) {
        printf("\nLa lista de visitantes esta vacia.\n");
        return;
    }

    printf("Ingrese el codigo de entrada del visitante a eliminar: ");
    codigoBuscado = pedirCadena();

    while (rec != NULL) {
        if (compararCadenasIgnorandoMayusculas(rec->datos->entrada.codigo, codigoBuscado) == 0) {
            visitanteEncontrado = rec->datos;
            break;
        }
        rec = rec->siguiente;
    }

    if (visitanteEncontrado != NULL) {

        if (estaVisitanteEnAlgunaFila(headZona, codigoBuscado) == 1) {
            printf("\nError: No se puede eliminar. El visitante con codigo '%s' esta actualmente registrado en la fila de una atraccion.\n", codigoBuscado);
            free(codigoBuscado);
            return;
        }
        eliminarVisitanteDeLista(*headVisitantes, visitanteEncontrado);
        printf("\nEl visitante con codigo '%s' ha sido eliminado de la lista.\n", codigoBuscado);
    } else {
        printf("\nError: No se encontro ningun visitante registrado con el codigo '%s'.\n", codigoBuscado);
    }
    free(codigoBuscado);
}

void menuContarTotalVisitantes(struct NodoVisitantes *headVisitantes) {
    int total;

    printf("\n========================================\n");
    printf("        TOTAL DE VISITANTES             \n");
    printf("========================================\n");

    total = contarVisitantesTotales(headVisitantes);

    printf("\n>> El numero total historico de visitantes registrados es: %d\n", total);
}

void menuContarVisitantesAdentro(struct NodoVisitantes *headVisitantes) {
    int adentro;

    printf("\n========================================\n");
    printf("      VISITANTES DENTRO DE IBCLANDIA    \n");
    printf("========================================\n");

    adentro = contarVisitantesDentro(headVisitantes);

    printf("\n>> Actualmente hay %d visitantes explorando el parque.\n", adentro);
}

void menuQuitarVisitante(struct NodoVisitantes **headVisitantes) {
    char *codigoBuscado = NULL;
    struct Visitante *visitanteEncontrado = NULL;
    struct Visitante *visitanteQuitado=NULL;

    struct NodoVisitantes *rec = (*headVisitantes)->siguiente;

    printf("\n========================================\n");
    printf("          QUITAR VISITANTE              \n");
    printf("========================================\n");

    if (rec == NULL) {
        printf("\n>> La lista de visitantes esta vacia.\n");
        return;
    }

    printf("Ingrese el codigo de entrada del visitante a quitar: ");
    codigoBuscado = pedirCadena();

    /* Buscar al visitante por su código*/
    while (rec != NULL) {
        if (compararCadenasIgnorandoMayusculas(rec->datos->entrada.codigo, codigoBuscado) == 0) {
            visitanteEncontrado = rec->datos;
            break;
        }
        rec = rec->siguiente;
    }

    if (visitanteEncontrado != NULL) {
        visitanteQuitado = quitarVisitanteDeLista(headVisitantes, visitanteEncontrado);

        if (visitanteQuitado != NULL) {
            printf("\nVisitante quitado de la lista.\n\n");
            printf("Nombre: %s\n", visitanteQuitado->nombre);
            printf("Edad: %d\n", visitanteQuitado->edad);
            printf("Codigo de Entrada: %s\n", visitanteQuitado->entrada.codigo);

            free(visitanteQuitado->nombre);
            free(visitanteQuitado->entrada.codigo);
            free(visitanteQuitado->entrada.tipo);
            free(visitanteQuitado->entrada.estado);
            free(visitanteQuitado);
        }
    } else {
        printf("\nError: No se encontro ningun visitante con el codigo '%s'.\n", codigoBuscado);
    }
    free(codigoBuscado);
}

void menuListarVisitantes(struct NodoVisitantes *headVisitantes) {
    printf("\n========================================\n");
    printf("          LISTADO DE VISITANTES         \n");
    printf("========================================\n");

    listarVisitantes(headVisitantes);

    printf("\n");
}

void menuRegistrarSalidaVisitante(struct NodoVisitantes *headVisitantes, struct NodoZonaTematica *headZona){
    char *codigoEntrada = NULL;
    int horaFin, minutosFin, minutosEntradaTotal, minutosSalidaTotal;
    struct Visitante *visitanteEncontrado = NULL;

    struct NodoVisitantes *rec = headVisitantes->siguiente;

    printf("\n========================================\n");
    printf("          SALIDA DE VISITANTE             \n");
    printf("========================================\n");

    if (rec == NULL) {
        printf("\nNo hay visitantes para marcar.\n");
        return;
    }

    printf("Ingrese el codigo del visitante a marcar: \n");
    codigoEntrada = pedirCadena();

    while (rec != NULL) {
        if (compararCadenasIgnorandoMayusculas(rec->datos->entrada.codigo, codigoEntrada) == 0) {
            visitanteEncontrado = rec->datos;
            break;
        }
        rec = rec->siguiente;
    }
    if (visitanteEncontrado != NULL) {

        if (visitanteEncontrado->horarioVisita.horaFin != -1) {
            printf("Error: El visitante ya registro su salida previamente.\n");
            free(codigoEntrada);
            return;
        }

        if (estaVisitanteEnAlgunaFila(headZona, codigoEntrada) == 1) {
            printf("Error: El visitante no puede salir del parque porque se encuentra esperando en una fila de atraccion.\n");
            free(codigoEntrada);
            return;
        }

        do {
            horaFin = leerEntero("Ingrese SOLO la hora de salida: \n");
            minutosFin = leerEntero("Ingrese SOLO los minutos de salida (0-59): \n");

            minutosEntradaTotal = (visitanteEncontrado->horarioVisita.horaInicio * 60) + visitanteEncontrado->horarioVisita.minutosInicio;
            minutosSalidaTotal = (horaFin * 60) + minutosFin;

            if (minutosSalidaTotal < minutosEntradaTotal) {
                printf("Error: La hora de salida no puede ser previa a la de entrada (%02d:%02d).\n",
                       visitanteEncontrado->horarioVisita.horaInicio, visitanteEncontrado->horarioVisita.minutosInicio);
                horaFin = -1; // se fuerza la repetición del bucle
            }
        }while(horaFin < 0 || horaFin > 23 || minutosFin < 0 || minutosFin > 59);

        marcarSalidaVisitante (headVisitantes, codigoEntrada, horaFin, minutosFin);
        printf("La salida ha sido marcada correctamente.\n");
        limpiarBuffer();
        free(codigoEntrada);
    }else{
        printf("Error: No se encontro ningun visitante registrado con el codigo '%s'.\n", codigoEntrada);
        free(codigoEntrada);
    }
}

void menuRecaudacionDiaria(struct NodoVisitantes *headVisitantes) {
    int totalRecaudado;

    printf("\n========================================\n");
    printf("          RECAUDACION DIARIA            \n");
    printf("========================================\n");

    if (headVisitantes == NULL) {
        printf("No hay visitantes registrados hoy.\n");
        printf("La recaudacion total es de: $0\n");
        return;
    }

    totalRecaudado = calcularRecaudacionDiaria(headVisitantes);

    printf("El calculo de ingresos por entradas se ha completado.\n");
    printf("----------------------------------------\n");
    printf("=> Recaudacion Total: $%d\n", totalRecaudado);
    printf("========================================\n");
}

void menuBuscarVisitante(struct NodoVisitantes *headVisitantes) {
    char *codigoBuscado = NULL;
    struct Visitante *visitanteEncontrado = NULL;

    printf("\n========================================\n");
    printf("            BUSCAR VISITANTE            \n");
    printf("========================================\n");

    /* 1. Verificamos que la lista tenga elementos */
    if (headVisitantes == NULL || headVisitantes->siguiente == NULL) {
        printf("No hay visitantes registrados en el parque actualmente.\n");
        return;
    }

    /* 2. Pedimos el codigo */
    printf("Ingrese el codigo de entrada del visitante a buscar: \n");
    codigoBuscado = pedirCadena();

    /* 3. Llamamos a tu funcion logica */
    visitanteEncontrado = visitanteBuscar(headVisitantes, codigoBuscado);

    /* 4. Mostramos el resultado */
    printf("\n----------------------------------------\n");
    if (visitanteEncontrado != NULL) {
        printf("Visitante encontrado con exito:\n");
        printf("- Nombre: %s\n", visitanteEncontrado->nombre);
        printf("- Edad: %d anos\n", visitanteEncontrado->edad);
        printf("- Altura: %.2f m\n", visitanteEncontrado->altura);
        printf("\n--- Datos de la Entrada ---\n");
        printf("- Codigo: %s\n", visitanteEncontrado->entrada.codigo);
        printf("- Tipo: %s\n", visitanteEncontrado->entrada.tipo);
        printf("- Estado: %s\n", visitanteEncontrado->entrada.estado);
        printf("- Valor Pagado: $%d\n", visitanteEncontrado->entrada.valor);
        printf("\n--- Horario de Visita ---\n");
        printf("- Llegada: %02d:%02d hrs\n", visitanteEncontrado->horarioVisita.horaInicio, visitanteEncontrado->horarioVisita.minutosInicio);

        /* Opcional: mostrar la salida si ya se retiro */
        if (visitanteEncontrado->horarioVisita.horaFin != -1) {
            printf("- Salida: %02d:%02d hrs\n", visitanteEncontrado->horarioVisita.horaFin, visitanteEncontrado->horarioVisita.minutosFin);
        } else {
            printf("- Salida: Aun en el parque\n");
        }
    } else {
        printf("Error: No se encontro ningun visitante con el codigo '%s'.\n", codigoBuscado);
    }
    free(codigoBuscado);
}

void menuModificarVisitante(struct NodoVisitantes *headVisitantes) {
    char *codigoBuscado = NULL, *entradaTextoModificarVisitante = NULL, *nuevoNombre = NULL, *nuevoTipoEntrada = NULL, *nuevoEstado = NULL;
    int nuevaEdad, seleccionEntrada, seleccionEstado;
    float nuevaAltura;
    int resultado;

    printf("\n========================================\n");
    printf("          MODIFICAR VISITANTE           \n");
    printf("========================================\n");

    if (headVisitantes == NULL) {
        printf("Error: No hay visitantes registrados en el parque.\n");
        return;
    }

    printf("Ingrese el codigo de entrada del visitante a modificar: \n");
    codigoBuscado = pedirCadena();

    if (visitanteBuscar(headVisitantes, codigoBuscado) == NULL) {
        printf("\nError: No se encontro ningun visitante con el codigo '%s'.\n", codigoBuscado);
        free(codigoBuscado);
        return;
    }

    /*Si existe, pedimos los nuevos datos */
    printf("\n--- Ingrese los nuevos datos para el visitante ---\n");

    printf("Ingrese el nuevo nombre: \n");
    nuevoNombre = pedirCadena();

    do
    {
        nuevaEdad = leerEntero("Ingrese la nueva edad: \n");
        if(nuevaEdad < 0) printf("Edad invalida.\n");
    }while(nuevaEdad < 0);
    limpiarBuffer();

    do {
        printf("Ingrese la nueva altura (en formato decimal, ej. 1.75): \n");
        entradaTextoModificarVisitante = pedirCadena();
        nuevaAltura = (float)strtod(entradaTextoModificarVisitante, NULL);
        free(entradaTextoModificarVisitante);
        if(nuevaAltura <= 0) printf("Altura invalida.\n");
    } while(nuevaAltura <= 0);

    do {
        printf("\nSeleccione el nuevo tipo de entrada:\n");
        printf("1. Entrada General\n2. Pase Infantil\n3. Pase Familiar\n4. Pase Rapido\n");
        seleccionEntrada = leerEntero("Ingrese una opcion: ");
        limpiarBuffer();
        if(seleccionEntrada == 1){
            nuevoTipoEntrada = copiarCadena("Entrada General");
        }
        else if(seleccionEntrada == 2){
            nuevoTipoEntrada = copiarCadena("Pase Infantil");
        }
        else if(seleccionEntrada == 3){
            nuevoTipoEntrada = copiarCadena("Pase Familiar");
        }
        else if(seleccionEntrada == 4){
            nuevoTipoEntrada = copiarCadena("Pase Rapido");
        }
    } while(seleccionEntrada < 1 || seleccionEntrada > 4);

    do {
        printf("\nSeleccione el nuevo estado de la entrada:\n");
        printf("1. Activa\n2. Utilizada\n3. Anulada\n4. Vencida\n");
        seleccionEstado = leerEntero("Ingrese una opcion: ");
        limpiarBuffer();
        if(seleccionEstado == 1){
            nuevoEstado = copiarCadena("Activa");
        }
        else if(seleccionEstado == 2){
            nuevoEstado = copiarCadena("Utilizada");
        }
        else if(seleccionEstado == 3){
            nuevoEstado = copiarCadena("Anulada");
        }
        else if(seleccionEstado == 4){
            nuevoEstado = copiarCadena("Vencida");
        }
    } while(seleccionEstado < 1 || seleccionEstado > 4);

    resultado = modificarVisitante(headVisitantes, codigoBuscado, nuevoNombre, nuevaEdad, nuevaAltura, nuevoTipoEntrada, nuevoEstado);

    printf("\n----------------------------------------\n");
    if (resultado == 1) {
        printf("Los datos del visitante con codigo '%s' fueron actualizados exitosamente.\n", codigoBuscado);
    } else {
        printf("Error: No se pudieron actualizar los datos del visitante.\n");
        free(nuevoNombre);
        free(nuevoTipoEntrada);
        free(nuevoEstado);
    }
    free(codigoBuscado);
}

/* ==========================================*/
/*            MENU VISITANTES                   */
/* ==========================================*/
void menuVisitantes(struct NodoVisitantes **headVisitantes, struct NodoZonaTematica *headZona) {
    char opcion;
    do {
        printf("\n===================================================\n");
        printf("||------------- MENU VISITANTES -----------------||\n");
        printf("===================================================\n");
        printf("||                                               ||\n");
        printf("|| [1] Insertar visitante                        ||\n");
        printf("|| [2] Eliminar visitante                        ||\n");
        printf("|| [3] Contar total de visitantes                ||\n");
        printf("|| [4] Contar visitantes adentro                 ||\n");
        printf("|| [5] Quitar visitante                          ||\n");
        printf("|| [6] Listar visitantes                         ||\n");
        printf("|| [7] Marcar la salida del visitante            ||\n");
        printf("|| [8] Modificar visitante                       ||\n");
        printf("|| [9] Buscar visitante                          ||\n");
        printf("||                                               ||\n");
        printf("|| [0] Volver al menu principal                  ||\n");
        printf("||                                               ||\n");
        printf("===================================================\n");
        printf("Seleccione una opcion: ");


        scanf(" %c",&opcion);

        limpiarBuffer();

        switch(opcion) {
            case '1':
                menuInsertarVisitante(*headVisitantes);
                pausarPantalla();
                break;

            case '2':
                menuEliminarVisitante(headVisitantes, headZona);
                pausarPantalla();
                break;

            case '3':
                menuContarTotalVisitantes(*headVisitantes);
                pausarPantalla();
                break;

            case '4':
                menuContarVisitantesAdentro(*headVisitantes);
                pausarPantalla();
                break;

            case '5':
                menuQuitarVisitante(headVisitantes);
                pausarPantalla();
                break;

            case '6':
                menuListarVisitantes(*headVisitantes);
                pausarPantalla();
                break;

            case '7':
                menuRegistrarSalidaVisitante(*headVisitantes, headZona);
                pausarPantalla();
                break;

            case '8':
                menuModificarVisitante(*headVisitantes);
                pausarPantalla();
                break;

            case '9':
                menuBuscarVisitante(*headVisitantes);
                pausarPantalla();
                break;

            case '0':
                printf("\nVolviendo al menu principal...\n");
                break;

            default:
                printf("\nError: Opcion invalida. Intente de nuevo.\n");
                pausarPantalla();
                break;
        }
    } while(opcion != '0');
}


/*Funciones del menu zona tematica*/

void liberarFilaCompleta(struct NodoFila *headFila) {
    struct NodoFila *rec = NULL, *aux = NULL;
    if (headFila == NULL) return;

    rec = headFila->siguiente;
    while (rec != NULL) {
        aux = rec;
        rec = rec->siguiente;
        free(aux);
    }
    free(headFila);
}

void liberarArbolAtracciones(struct NodoAtracciones *raiz) {
    if (raiz == NULL) return;

    /* Primero se libera a los sub arboles izq y der */
    liberarArbolAtracciones(raiz->izquierdo);
    liberarArbolAtracciones(raiz->derecho);

    if (raiz->datos != NULL) {
        liberarFilaCompleta(raiz->datos->headFila);
        free(raiz->datos);
    }
    free(raiz);
}

void liberarZonaTematicaCompleta(struct ZonaTematica *zona) {
    if (zona == NULL) return;

    /* 1. Liberamos las cadenas dinámicas */
    if (zona->nombre != NULL) free(zona->nombre);
    if (zona->codigo != NULL) free(zona->codigo);
    if (zona->tematica != NULL) free(zona->tematica);

    /* 2. Liberamos su árbol de atracciones */
    liberarArbolAtracciones(zona->raizAtracciones);

    /* 3. Liberamos la estructura en sí */
    free(zona);
}

void menuAgregarZonaTematica(struct NodoZonaTematica **headZonaTematica) {
    char *nombre = NULL, *codigo = NULL, *tematica = NULL;
    int capacidad;
    struct Horario horarioZona;

    printf("\n========================================\n");
    printf("        AGREGAR ZONA TEMÁTICA           \n");
    printf("========================================\n");

    /*Pedir datos de texto */
    printf("Ingrese el nombre de la zona tematica: \n");
    nombre = pedirCadena();

    printf("Ingrese el codigo identificador de la zona: \n");
    codigo = pedirCadena();

    printf("Ingrese la tematica principal (ej. Aventura, Terror): \n");
    tematica = pedirCadena();

    /*Pedir datos numéricos */
    capacidad = leerEntero("Ingrese la capacidad aproximada de personas: \n");
    if(capacidad<=0)
    {
        printf("Error! -> la capacidad ingresada no es valida\n");
        free(nombre);
        free(codigo);
        free(tematica);
        return;
    }
    limpiarBuffer();

    /*Pedir horarios */
    printf("\n--- Horario de la Zona ---\n");

    horarioZona.horaInicio = leerEntero("Ingrese la hora de apertura (En formato 24hrs): \n");
    if(horarioZona.horaInicio<0 || horarioZona.horaInicio>23)
    {
        printf("Error! -> la hora de inicio ingresada no es valida");
        free(nombre);
        free(codigo);
        free(tematica);
        return;
    }

    horarioZona.minutosInicio = leerEntero("Ingrese los minutos de apertura: \n");
    if(horarioZona.minutosInicio<0 || horarioZona.minutosInicio>59)
    {
        printf("Error! -> el minuto de inicio ingresado no es valido");
        free(nombre);
        free(codigo);
        free(tematica);
        return;
    }

    horarioZona.horaFin = leerEntero("Ingrese la hora de cierre (En formato 24hrs): \n");
    if(horarioZona.horaFin<0 || horarioZona.horaFin>23)
    {
        printf("Error! -> la hora de fin ingresada no es valida");
        free(nombre);
        free(codigo);
        free(tematica);
        return;
    }

    horarioZona.minutosFin = leerEntero("Ingrese los minutos de cierre: \n");
    if(horarioZona.minutosFin<0 || horarioZona.minutosFin>59)
    {
        printf("Error! -> el minuto de fin ingresado no es valido");
        free(nombre);
        free(codigo);
        free(tematica);
        return;
    }
    limpiarBuffer();

    agregarZonaTematica(headZonaTematica, nombre, codigo, tematica, capacidad, horarioZona);

    printf("\n----------------------------------------\n");
    printf("La zona '%s' fue agregada exitosamente al parque.\n", nombre);

    free(nombre);
    free(codigo);
    free(tematica);
}

void menuBuscarZona(struct NodoZonaTematica *headZona) {
    char *nombre = NULL;
    struct ZonaTematica *zonaBuscada = NULL;

    printf("\n========================================\n");
    printf("              BUSCAR ZONA               \n");
    printf("========================================\n");

    if (headZona == NULL) {
        printf("No hay zonas tematicas registradas para buscar.\n");
        return;
    }

    printf("Ingrese el nombre de la zona a buscar: \n");
    nombre = pedirCadena();

    zonaBuscada = buscarZonaTematica(headZona, nombre);

    printf("\n----------------------------------------\n");
    if (zonaBuscada != NULL) {
        printf("\n================================================================================\n");
        printf("||-------------------------Ficha de la zona tematica--------------------------||\n");
        printf("================================================================================\n");
        printf("|| Nombre           : %-55.55s ||\n", zonaBuscada->nombre);
        printf("|| Codigo           : %-55.55s ||\n", zonaBuscada->codigo);
        printf("|| Tematica         : %-55.55s ||\n", zonaBuscada->tematica);
        printf("|| Capacidad        : %-4dpersonas                                            ||\n", zonaBuscada->capacidadAproximada);
        printf("|| Hora apertura    : %02d:%02d                                                   ||\n", zonaBuscada->horarios.horaInicio, zonaBuscada->horarios.minutosInicio);
        printf("|| Hora cierre      : %02d:%02d                                                   ||\n", zonaBuscada->horarios.horaFin, zonaBuscada->horarios.minutosFin);
        printf("================================================================================\n");
    }
    else
    {
        printf("Error: No se encontro ninguna zona con el nombre '%s'.\n", nombre);
    }
    free(nombre);
}
void menuQuitarZona(struct NodoZonaTematica **headZona) {
    char *nombre = NULL;
    struct ZonaTematica *zonaQuitada = NULL;

    printf("\n========================================\n");
    printf("              QUITAR ZONA               \n");
    printf("========================================\n");

    if (*headZona == NULL) {
        printf("La lista de zonas esta vacia. No hay nada que quitar.\n");
        return;
    }

    printf("Ingrese el nombre de la zona que desea eliminar: \n");
    nombre = pedirCadena();

    zonaQuitada = quitarZona(headZona, nombre);

    printf("\n----------------------------------------\n");
    if (zonaQuitada != NULL) {
        printf("La zona '%s' fue removida exitosamente del parque.\n", zonaQuitada->nombre);
        liberarZonaTematicaCompleta(zonaQuitada);
    } else {
        printf("Error: No se pudo quitar. La zona '%s' no existe.\n", nombre);
    }
    free(nombre);
}

void menuListarZonasAltaCapacidad(struct NodoZonaTematica *headZona) {
    int limite;

    printf("\n========================================\n");
    printf("     ZONAS DE ALTA CAPACIDAD            \n");
    printf("========================================\n");

    if (headZona == NULL) {
        printf("No hay zonas registradas en el parque.\n");
        return;
    }

    limite = leerEntero("Ingrese el limite de capacidad minima deseada a buscar: \n");
    limpiarBuffer();

    printf("\n--- Resultados ---\n");
    listarZonasConAltaCapacidad(headZona, limite);
}

void menuContarZonasTematicas(struct NodoZonaTematica *headZona) {
    int total;

    printf("\n========================================\n");
    printf("        CONTEO DE ZONAS TEMATICAS       \n");
    printf("========================================\n");

    total = contarZonasTematicas(headZona);

    if (total == 0) {
        printf("Actualmente no hay zonas tematicas registradas en el parque.\n");
    } else {
        printf("El parque cuenta con un total de %d zonas tematicas.\n", total);
    }
}

void menuVerificarCapacidadZona(struct NodoZonaTematica *headZona) {
    char *nombre = NULL;
    struct ZonaTematica *zonaBuscada = NULL;

    printf("\n========================================\n");
    printf("    CONSULTAR DISPONIBILIDAD DE ZONA    \n");
    printf("========================================\n");

    if (headZona == NULL) {
        printf("No hay zonas tematicas registradas en el parque.\n");
        return;
    }

    printf("Ingrese el nombre de la zona a consultar: \n");
    nombre = pedirCadena();

    zonaBuscada = buscarZonaTematica(headZona, nombre);

    printf("\n----------------------------------------\n");

    if (zonaBuscada != NULL) {
        if (capacidadSuficienteZona(zonaBuscada) == 1) {
            printf("Estado: DISPONIBLE\n");
            printf("Aun hay espacio para que ingresen mas visitantes a la zona '%s'.\n", zonaBuscada->nombre);
        } else {
            printf("Estado: LLENO\n");
            printf("La zona '%s' ha alcanzado su limite de %d visitantes en las atracciones.\n",
                   zonaBuscada->nombre, zonaBuscada->capacidadAproximada);
        }
    } else {
        printf("Error: No se encontro ninguna zona con el nombre '%s'.\n", nombre);
    }
    free(nombre);
}

void menuFormarEnFilaAtraccionZona(struct NodoZonaTematica *headZona, struct NodoVisitantes *headVisitantes) {
    char *codigoVisitante = NULL, *nombreZona = NULL, *nombreAtraccion = NULL;
    struct Visitante *visitanteEncontrado = NULL;
    struct ZonaTematica *zonaBuscada = NULL;
    struct NodoAtracciones *atraccionBuscada = NULL;

    printf("\n========================================\n");
    printf("     FORMAR VISITANTE EN ATRACCION      \n");
    printf("========================================\n");

    printf("Ingrese el codigo de entrada del visitante: \n");
    codigoVisitante = pedirCadena();

    visitanteEncontrado = visitanteBuscar(headVisitantes, codigoVisitante);
    if (visitanteEncontrado == NULL) {
        printf("Error: No se encontro ningun visitante con el codigo '%s'.\n", codigoVisitante);
        free(codigoVisitante);
        return;
    }

    printf("Ingrese el nombre de la zona tematica: \n");
    nombreZona = pedirCadena();

    zonaBuscada = buscarZonaTematica(headZona, nombreZona);
    if (zonaBuscada == NULL) {
        printf("Error: La zona '%s' no existe en el parque.\n", nombreZona);
        free(codigoVisitante);
        free(nombreZona);
        return;
    }

    printf("Ingrese el nombre de la atraccion: \n");
    nombreAtraccion = pedirCadena();

    atraccionBuscada = buscarAtraccion(zonaBuscada->raizAtracciones, nombreAtraccion);
    if (atraccionBuscada == NULL) {
        printf("Error: La atraccion '%s' no se encuentra en la zona.\n", nombreAtraccion);
        free(codigoVisitante);
        free(nombreZona);
        free(nombreAtraccion);
        return;
    }

    printf("\n----------------------------------------\n");
    printf("El visitante");
    formarEnFilaAtraccionZona(zonaBuscada, atraccionBuscada, visitanteEncontrado);
    free(codigoVisitante);
    free(nombreZona);
    free(nombreAtraccion);
}

/*==========================================*/
            /* MENU ZONA TEMATICA*/
/*==========================================*/
void menuZonasTematicas(struct NodoZonaTematica **headZona)
{
    char opcion;

    do {
        printf("===================================================\n");
        printf("||------------- MENU ZONA TEMATICA --------------||\n");
        printf("===================================================\n");
        printf("||                                               ||\n");
        printf("|| [1] Agregar zona                              ||\n");
        printf("|| [2] Buscar zona                               ||\n");
        printf("|| [3] Quitar zona                               ||\n");
        printf("|| [4] Listar zonas con alta capacidad           ||\n");
        printf("|| [5] Contar zonas tematicas                    ||\n");
        printf("||                                               ||\n");
        printf("|| [0] Volver al menu principal                  ||\n");
        printf("||                                               ||\n");
        printf("===================================================\n");
        printf("Seleccione una opcion: ");
        scanf(" %c", &opcion);
        limpiarBuffer();

        switch(opcion) {
            case '1':
                menuAgregarZonaTematica(headZona);
                pausarPantalla();
                break;
            case '2':
                menuBuscarZona(*headZona); /* Pasa solo el puntero*/
                pausarPantalla();
                break;
            case '3':
                menuQuitarZona(headZona); /* Pasa el doble puntero*/
                pausarPantalla();
                break;
            case '4':
                menuListarZonasAltaCapacidad(*headZona); /* Pasa solo el puntero*/
                pausarPantalla();
                break;
            case '5':
                menuContarZonasTematicas(*headZona); /* Pasa solo el puntero*/
                pausarPantalla();
                break;
            case '0':
                printf("\nVolviendo al menu principal...\n");
                break;
            default:
                printf("\nOpcion invalida. Intente nuevamente.\n");
                pausarPantalla();
                break;
        }
    }while(opcion != '0');
}
/* ==========================================*/
/*            MENU ATRACCIONES                 */
/* ==========================================*/
void menuAtracciones(struct NodoZonaTematica **headZona) {

    struct ZonaTematica *Buscar = NULL;
    char *ZonaObjetivo = NULL;
    char opcion;

    printf("Primero indique el nombre de la zona tematica que desea operar: \n");
    ZonaObjetivo=pedirCadena();

    Buscar=buscarZonaTematica(*headZona,ZonaObjetivo);

    if(Buscar!=NULL)
    {

        do {
            printf("===================================================\n");
            printf("||----------- MENU ATRACCIONES (ABB) ------------||\n");
            printf("===================================================\n");
            printf("||                                               ||\n");
            printf("|| [1] Crear e insertar atraccion                ||\n");
            printf("|| [2] Buscar atraccion                          ||\n");
            printf("|| [3] Modificar atraccion                       ||\n");
            printf("|| [4] Eliminar atraccion                        ||\n");
            printf("|| [5] Listar atracciones                        ||\n");
            printf("||                                               ||\n");
            printf("|| [0] Volver al menu principal                  ||\n");
            printf("||                                               ||\n");
            printf("===================================================\n");
            printf("Seleccione una opcion: ");
            scanf(" %c", &opcion);
            limpiarBuffer();


            switch(opcion) {
                case '1':
                    menuAgregarAtraccion(&Buscar->raizAtracciones);
                    pausarPantalla();
                    break;
                case '2':
                    menuMostrarAtraccion(Buscar->raizAtracciones);
                    pausarPantalla();
                    break;
                case '3':
                    menuModificarAtraccion(&Buscar->raizAtracciones);
                    pausarPantalla();
                    break;
                case '4':
                    menuEliminarAtraccion(&Buscar->raizAtracciones);
                    pausarPantalla();
                    break;
                case '5':
                    listarAtracciones(Buscar->raizAtracciones);
                    pausarPantalla();
                    break;
                case '0':
                    break;
                default:
                    printf("Opcion invalida.\n");
                    pausarPantalla();
                    break;
                }


        }while(opcion != '0');
    }
    else
    {
        printf("La zona ingresada no existe en el parque");
        pausarPantalla();
    }
    free(ZonaObjetivo);
}

/* ==========================================*/
/*                  MENU FILAS              */
/* ==========================================*/
void menuFilas(struct NodoZonaTematica **headZona, struct NodoVisitantes *head) {
    char opcion,subOpcion;
    char *nombreAtraccion = NULL,*nombreZona = NULL, *codigoVisitante = NULL;
    struct ZonaTematica *Buscada = NULL;
    struct NodoAtracciones *AtraccionActual=NULL;

    struct Visitante *Encontrado = NULL;
    struct Visitante **Evacuado = NULL;

    struct NodoVisitantes *rec = NULL;
    struct Visitante *VisitanteLista = NULL;


    int tiempo,cantidad,cantidadEnFila,i;



    printf("\n");
    printf("===================================================\n");
    printf("||-------------- Gestion de Filas ---------------||\n");
    printf("===================================================\n");
    printf(" Ingrese el nombre de la zona para operar la atraccion: ");

    nombreZona=pedirCadena();

    Buscada=buscarZonaTematica(*headZona,nombreZona);

    if(Buscada!=NULL)
    {

        printf("Ingrese el nombre de la atraccion que desea gestionar la fila: ");

        nombreAtraccion=pedirCadena();

        AtraccionActual=buscarAtraccion(Buscada->raizAtracciones,nombreAtraccion);

        if(AtraccionActual==NULL)
        {
            printf("La atraccion %s no existe en el parque\n",nombreAtraccion);
            free(nombreZona);
            free(nombreAtraccion);
            pausarPantalla();
            return;
        }

        do {
            printf("===================================================\n");
            printf("||------------------ MENU FILAS -----------------||\n");
            printf("===================================================\n");
            printf("||                                               ||\n");
            printf("|| [1] Agregar visitante a la fila               ||\n");
            printf("|| [2] Atender visitantes (Avanzar fila)         ||\n");
            printf("|| [3] Mostrar fila                              ||\n");
            printf("|| [4] Buscar visitante en fila                  ||\n");
            printf("|| [5] Eliminar visitante de la fila (Abandono)  ||\n");
            printf("|| [6] Estimar tiempo de espera                  ||\n");
            printf("|| [7] Contar cantidad de personas               ||\n");
            printf("|| [8] Vaciar toda la fila                       ||\n");
            printf("||                                               ||\n");
            printf("|| [0] Volver al menu principal                  ||\n");
            printf("||                                               ||\n");
            printf("===================================================\n");
            printf("Seleccione una opcion: ");
            scanf(" %c", &opcion);
            limpiarBuffer();

            switch(opcion) {

                case '1':
                {
                    struct Visitante *VisitanteEnFila = NULL;

                    /* Si no caben mas visitantes en la zona */
                    if (!capacidadSuficienteZona(Buscada)) {
                        printf("La zona '%s' esta llena, no se puede ingresar un nuevo visitante en este momento!\n", Buscada->nombre);
                        pausarPantalla();
                        break;
                    }

                    /* 1. Pedimos el CÓDIGO en lugar del nombre */
                    printf("Ingrese el codigo de entrada del visitante a formar: ");
                    codigoVisitante = pedirCadena();

                    /* 2. Buscamos en la lista de visitantes del parque por CÓDIGO */
                    rec = head->siguiente;
                    VisitanteLista = NULL;
                    while(rec != NULL)
                    {
                        /* Cambiamos ->nombre por ->entrada.codigo */
                        if(strcmp(rec->datos->entrada.codigo, codigoVisitante) == 0)
                        {
                            VisitanteLista = rec->datos;
                            break;
                        }
                        rec = rec->siguiente;
                    }

                    if(VisitanteLista != NULL)
                    {
                        /* 3. Comprobamos si el visitante YA está en la fila */
                        /* Usamos tu funcion de busqueda pasándole el nombre del visitante encontrado */
                        VisitanteEnFila = buscarVisitanteAtraccion(AtraccionActual, VisitanteLista->entrada.codigo);

                        if (VisitanteEnFila != NULL)
                        {
                            printf("Error! -> El visitante %s (Codigo: %s) YA se encuentra esperando en esta fila.\n", VisitanteLista->nombre, codigoVisitante);
                        }
                        else
                        {
                            /* 4. Si no está en la fila, lo formamos con éxito */
                            formarEnFilaAtraccionZona(Buscada, AtraccionActual, VisitanteLista);

                        }
                    }
                    else
                    {
                        printf("No hay ningun visitante registrado con el codigo '%s' en el parque.\n", codigoVisitante);
                    }

                    free(codigoVisitante); /* Liberamos la memoria del texto ingresado por teclado */
                    pausarPantalla();
                    break;
                }

                case '2':

                    printf("[1] Atender atraccion solo una vez\n");
                    printf("[2] Atender atraccion hasta que la fila este vacia\n");
                    printf("Seleccione de que forma se atendera la atraccion :\n");

                    scanf(" %c",&subOpcion);

                    if(subOpcion=='1')
                    {
                        atenderAtraccionUnaVez(AtraccionActual);
                        printf("Exito!-> La fila ha sido atendida correctamente\n");
                    }
                    else if(subOpcion=='2')
                    {
                        atenderAtraccion(AtraccionActual);
                        printf("Exito!-> La fila ha sido atendida correctamente\n");
                    }
                    else
                    {
                        printf("Error! -> La opcion que selecciono no existe\n");
                    }
                    pausarPantalla();
                    break;

                case '3':
                    mostrarEstadoFilaAtraccion(AtraccionActual);
                    pausarPantalla();
                    break;

                case '4':
                    printf("Ingrese el codigo del visitante a buscar en la fila: ");
                    codigoVisitante = pedirCadena();

                    Encontrado = buscarVisitanteAtraccion(AtraccionActual, codigoVisitante);

                    if(Encontrado != NULL) {

                        printf("El visitante %s (Codigo: %s) se encuentra en la fila de la atraccion %s.\n",
                                Encontrado->nombre, codigoVisitante, nombreAtraccion);
                    } else {
                        printf("El visitante con codigo %s no se encuentra en la fila.\n", codigoVisitante);
                    }
                    free(codigoVisitante);
                    pausarPantalla();
                    break;

                case '5':
                    printf("Ingrese el codigo del visitante que se retira de la fila: \n");
                    codigoVisitante = pedirCadena();

                    /* 1. Primero verificamos si el visitante realmente está en la fila */
                    if (buscarVisitanteAtraccion(AtraccionActual, codigoVisitante) != NULL) {

                        /* 2. Si existe, lo eliminamos usando tu función modularizada */
                        quitarVisitanteDeFilaAtraccion(AtraccionActual, codigoVisitante);

                        printf("El visitante con codigo %s ha sido retirado de la fila correctamente.\n", codigoVisitante);
                    } else {
                        /* 3. Si no se encuentra, avisamos al usuario */
                        printf("Error: No se encontro a ningun visitante con codigo %s en la fila.\n", codigoVisitante);
                    }
                    free(codigoVisitante);
                    pausarPantalla();
                    break;

                case '6':
                    tiempo=estimarTiempoAtraccion(AtraccionActual);
                    printf("Tiempo de espera estimado: %d minutos\n", tiempo);
                    pausarPantalla();
                    break;

                case '7':
                    cantidad=contarPersonasAtraccion(AtraccionActual);
                    printf("\nPersonas en fila: %d\n", cantidad);
                    pausarPantalla();
                    break;

                case '8':
                    cantidadEnFila=contarPersonasAtraccion(AtraccionActual);

                    if(cantidadEnFila>0)
                    {
                        Evacuado=evacuarFilaAtraccion(AtraccionActual);

                        ordenarEvacuadosPorEdad(Evacuado,cantidadEnFila);

                        printf("\n --- Reporte de evacuacion (Ordenada por edad) ---\n");
                        printf(" %-4s | %-25s | %-4s\n", "Nro", "Nombre del visitante", "Edad\n");
                        printf("  ----------------------------------------\n");


                        for(i=0;i<cantidadEnFila;i++)
                        {
                            printf(" %-4d | %-25.25s | %-4d\n", i+1, Evacuado[i]->nombre, Evacuado[i]->edad);

                        }

                        printf("  ----------------------------------------\n");

                        free(Evacuado);
                    }
                    else
                    {
                        printf("La lista ya esta vacia\n");
                    }
                    pausarPantalla();
                    break;

                case '0': break;

                default: printf("Opcion invalida.\n");
                pausarPantalla();
                break;
            }
        } while(opcion != '0');
        free(nombreZona);
        free(nombreAtraccion);
    }
    else
    {
        printf("Error! -> La zona tematica ingresada no existe!\n");
        pausarPantalla();
        free(nombreZona);
    }
}

void menuZonaMasPopular(struct NodoZonaTematica *headZona) {
    struct NodoZonaTematica *actual = NULL;
    struct ZonaTematica *zonaTop = NULL;
    int maxUsos = -1, usosActual = 0;

    printf("\n========================================\n");
    printf("       ZONA CON MAYOR TENDENCIA         \n");
    printf("========================================\n");

    if (headZona == NULL) {
        printf("No hay zonas tematicas registradas en el parque.\n");
        return;
    }

    actual = headZona;
    do {
        usosActual = sumarUsosAtracciones(actual->datos->raizAtracciones);

        if (usosActual > maxUsos) {
            maxUsos = usosActual;
            zonaTop = actual->datos;
        }

        actual = actual->siguiente;

    } while (actual != headZona);

    if (zonaTop != NULL && maxUsos > 0) {
        printf("La zona mas visitada del parque es:\n");
        printf("=> %s\n", zonaTop->nombre);
        printf("=> Total de usos en sus atracciones: %d\n", maxUsos);
    } else {
        printf("Aun no hay datos de uso registrados en las atracciones del parque.\n");
    }

    printf("========================================\n");
}

void menuCierre(struct NodoVisitantes *headVisitantes, struct NodoZonaTematica **headZona) {
    char opcion;

    do {
        printf("\n========================================\n");
        printf("||----------- MENU CIERRE ------------||\n");
        printf("========================================\n");
        printf("||                                    ||\n");
        printf("|| [1] Calcular recaudacion diaria    ||\n");
        printf("|| [2] Buscar la zona mas popular     ||\n");
        printf("|| [0] Volver al menu principal       ||\n");
        printf("||                                    ||\n");
        printf("========================================\n");
        printf("Opcion: ");
        scanf(" %c", &opcion);
        limpiarBuffer();

        switch (opcion) {
            case '1':
                menuRecaudacionDiaria(headVisitantes);
                pausarPantalla();
                break;
            case '2':
                menuZonaMasPopular(*headZona);
                pausarPantalla();
                break;
            case '0':
                printf("\nVolviendo al menu principal...\n");
                break;
            default:
                printf("\nError: Opcion no valida. Intente de nuevo.\n");
                pausarPantalla();
                break;
        }
    } while (opcion != '0');
}

int main(void) {

    /* 1. Inicialización de las estructuras principales */
    struct Parque *IBCLandia = NULL;
    char opcion;

    IBCLandia = (struct Parque *)malloc(sizeof(struct Parque));
    IBCLandia->headZonaTematica=NULL;

    IBCLandia->headVisitantes=(struct NodoVisitantes*)malloc(sizeof(struct NodoVisitantes));
    IBCLandia->headVisitantes->anterior=NULL;
    IBCLandia->headVisitantes->siguiente=NULL;
    IBCLandia->headVisitantes->datos=NULL;

    do {
        printf("\n===================================================\n");
        printf("||                                               ||\n");
        printf("||                  IBCLandia                    ||\n");
        printf("||                                               ||\n");
        printf("===================================================\n");
        printf("|| Bienvenido al sistema. Seleccione una opcion: ||\n");
        printf("||                                               ||\n");
        printf("|| [1] Menu Visitantes                           ||\n");
        printf("|| [2] Menu Zona Tematica                        ||\n");
        printf("|| [3] Menu Atracciones                          ||\n");
        printf("|| [4] Menu Filas                                ||\n");
        printf("|| [5] Menu Cierre                               ||\n");
        printf("||                                               ||\n");
        printf("|| [0] Salir del programa                        ||\n");
        printf("||                                               ||\n");
        printf("===================================================\n");
        printf("\nSeleccione una opcion: ");
        scanf(" %c", &opcion);
        limpiarBuffer();

        switch (opcion) {
            case '1': menuVisitantes(&(IBCLandia->headVisitantes), IBCLandia->headZonaTematica); break;

            case '2': menuZonasTematicas(&(IBCLandia->headZonaTematica)); break;

            case '3':
                if(IBCLandia->headZonaTematica!=NULL)
                menuAtracciones(&(IBCLandia->headZonaTematica));
                else
                {
                    printf("Error! No hay ninguna zona tematica creada!\n");
                    pausarPantalla();
                }
                break;
            case '4':
                if(IBCLandia->headZonaTematica!=NULL)
                {
                    menuFilas(&(IBCLandia->headZonaTematica),IBCLandia->headVisitantes);
                }
                else
                {
                    printf("Error! No hay ninguna zona tematica creada!\n");
                    pausarPantalla();
                }
                break;
            case '5': menuCierre(IBCLandia->headVisitantes, &IBCLandia->headZonaTematica); break;
            case '0':
                printf("\nSaliendo de IBCLandia. ¡Hasta pronto!\n");
                liberarListaVisitantes(IBCLandia->headVisitantes);
                free(IBCLandia);
                break;
            default:
                printf("\n>> Error: Opcion no valida.\n");
                break;
        }
    } while (opcion != '0');

    return 0;
}
