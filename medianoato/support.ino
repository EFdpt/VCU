/*
  funzione HASH per il bucket sort
  (int)lunghezza*((chiave*((SQRT(5)-1)/2))-(int)(chiave*((SQRT(5)-1)/2)))
  questa funzione hash corrisponde al metodo della moltiplicazione
  e sfrutta il suggerimento di Knuth utilizzando la parte dopo la virgola
  della sezione aurea = 1.618034

*/
int HASH(int chiave) {
  //  return (int)(lunghezza * (chiave * irr - (int)(chiave * irr)));
  return ((double)(chiave - min) / ((max + 1) - min)) * 100;
}

/*
   Questa funzione inserisce in una hash table gli elementi dell'array
   delle acquisizioni dei sensori, tutta la complessità aggiuntiva
   viene creata da degli if-else che permettono di avere le liste
   di trabocco dell'hash table già ordinate, risparmiando il passo successivo
   del bucket sort, quello di ordinare le suddette liste.

*/
int* buckSort(int* arr)
{
  //ciclo di riempimento dell'hash table
  for (i = 0; i != lunghezza; i++)
  {
    //printf("Se è l'ultima cosa che leggi, allora ho crashato\n");
    int bnum = HASH(arr[i]);
    printf("HASH(%d): %d\n", arr[i], bnum);
    lista_ptr bptr = Htable[bnum];
    lista_ptr belem = (lista_ptr)malloc(sizeof(lista_t));
    belem->chiave = arr[i];

    //inizio inserimento ordinato
    if (!bptr)
    { //la lista è vuota, inserisci in testa
      Htable[bnum] = belem;
      belem->next = NULL;
    }
    else if (arr[i] <= bptr->chiave)
    { //il nuovo elemento è più piccolo del primo della lista, inserisci in testa
      belem->next = bptr;
      Htable[bnum] = belem;
    }
    else
    {
      while (bptr != 0)
      { //
        if ((bptr->chiave <= arr[i]) && ((bptr->next == NULL) || (bptr->next->chiave > arr[i])))
        { //il nuovo elemento è più grande di bprt->chiave && più piccolo del prossimo, che magari non esiste, inserisci
          belem->next = bptr->next;
          bptr->next = belem;
          break;
        }
        bptr = bptr->next;
      }
    }
    //fine inserimento ordinato

    //raccolta elementi dalla hash table
    j = 0;

    for (i = 0; i != 100; i++)
    {
      lista_ptr bptr = Htable[i];
      while (bptr != NULL)
      {
        lista_ptr optr = bptr;
        out[j] = bptr->chiave;
        printf("out[%d]: %d\n", j, out[j]);
        j++;
        bptr = bptr->next;
        free(optr);
      }
    }
    return out;
  }
}

/*
  modifica le variabili globali "max", "min" con i valori
  degli elementi più grande e più piccolo dell'array parametro
*/
void MaxMin(int* array) {
  max = 0;
  min = 4095;
  for (i = 0; i != lunghezza; i++) {
    if (max < array[i])	max = array[i];
    if (min > array[i])	min = array[i];
  }
}

/*
    ritorna la media degli elementi centrali
    di un array di <lunghezza> posizioni
*/
int media(int* ar) {
  MaxMin(ar);	//prendo le distanze per l'HASH function
  int* media = buckSort(ar);
  int res = 0;

  for (i = lun - 2; i != lunghezza - lun; i++) {
    res += media[i];
  }
  return res / (lunghezza - ((lun - 1) * 2));	//media degli elementi centrali
}

/*
   acquisizione ADC circolare su array da <lunghezza> posizioni
*/
void acquisizioneTPS1() {
  if (posArrTh1 > (lunghezza - 1) )   posArrTh1 = 0;
  ArrTh1[posArrTh1] = analogRead(TPS1);
  posArrTh1++;
}

/*
  acquisizione ADC circolare su array da <lunghezza> posizioni
*/
void acquisizioneTPS2() {
  if (posArrTh2 > (lunghezza - 1) )    posArrTh2 = 0;
  ArrTh2[posArrTh2] = analogRead(TPS2);
  posArrTh2++;
}

/*
   acquisizione ADC circolare su array da <lunghezza> posizioni
*/
void acquisizioneBSE() {
  if (posArrBk > (lunghezza - 1) )    posArrBk = 0;
  ArrBk[posArrBk] = analogRead(BRAKEIN);
  posArrBk++;
}

