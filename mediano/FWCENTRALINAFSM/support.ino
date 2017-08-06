/*
  funzione HASH per il bucket sort
  (int)lunghezza*((chiave*((SQRT(5)-1)/2))-(int)(chiave*((SQRT(5)-1)/2)))
  questa funzione hash corrisponde al metodo della moltiplicazione
  e sfrutta il suggerimento di Knuth utilizzando la parte dopo la virgola
  della sezione aurea = 1.618034

*/
int HASH(int chiave) {
  return (int)(lunghezza * (chiave * irr - (int)(chiave * irr)));
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
    int bnum = HASH(arr[i]);
    lista_ptr bptr = Htable[bnum];
    lista_ptr belem = (lista_ptr)malloc(sizeof(lista));
    belem->chiave = arr[i];

    if (bptr == NULL)
    {
      Htable[bnum] = belem;
      belem->next = NULL;
    }
    else if (arr[i] <= bptr->chiave)
    {
      belem->next = bptr;
      Htable[bnum] = belem;
    }
    else
    {
      while (bptr != NULL)
      {
        if ((bptr->chiave <= arr[i]) && ((bptr->next == NULL) || (bptr->next->chiave > arr[i])))
        {
          belem->next = bptr->next;
          bptr->next = belem;
        }
        bptr = bptr->next;
      }
    }
  }//fine riempimento hash table

  //raccolta elementi hash table
  for (i = 0; i != lunghezza; i++)
  {
    lista_ptr bptr = Htable[i];
    for(j=0; bptr!=NULL; j++)
    {
      lista_ptr optr = bptr;
      out[j] = bptr->chiave;
      bptr = bptr->next;
      free(optr);
    }
  }
  return out;
}


/*
 *  ritorna l'elemento mediano di un array di <lunghezza> posizioni
*/
int mediano(int* ar) {
  int* mediano=buckSort(ar);
  return mediano[(lunghezza>>1)-1];
}

/*
 * acquisizione ADC circolare su array da <lunghezza> posizioni
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

