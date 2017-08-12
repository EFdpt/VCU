/*
 *
 */
void qSort(int *v, int first, int last){
  int a,b,pivot,tmp;
  if (first<last) {
    // Partenza: i parte dal primo elemento del vettore, j dall'ultimo
    a = first; b = last;

    // Il pivot è l'elemento medio del vettore
    pivot = v[(first + last)/2];
    do {
    // Finché l'elemento generico i-esimo a sinistra del pivot
    // è minore del pivot, incrementa i
    while (v[a] < pivot) a++;
    // Finché l'elemento generico j-esimo a destra del pivot
    // è maggiore del pivot, decrementa j
    while (v[b] > pivot) b--;
    // Altrimenti, scambia tra loro l'elemento i-esimo e quello j-esimo
    if (a <= b) {
      tmp=v[a];
      v[a]=v[b];
      v[b]=tmp;
      a++, b--;
    }
  } while (a <= b); // Cicla finché i e j non si incontrano
  // Richiama il quick sort sul primo sottovettore
  qSort(v, first, b);
  // Richiama il quick sort sul secondo sottovettore
  qSort(v, a, last);
  }
}


/*
 * ritorna la media di un array di <lunghezza> posizioni
 */
int media(int* ar) {
  
  for (i = 0; i != lunghezza; i++) {
    out[i] = ar[i];
  }

  //chiamare questa funzione con le posizioni estreme dell'arraty da ordniare
  qSort(out, primo, ultimo);

  res = 0;
  for (i = lun - 2; i != lunghezza - lun; i++) {
    res += out[i];
  }
  
  return res / (lunghezza - ((lun - 1) * 2)); //media degli elementi centrali
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
 * acquisizione ADC circolare su array da <lunghezza> posizioni
 */
void acquisizioneTPS2() {
  if (posArrTh2 > (lunghezza - 1) )    posArrTh2 = 0;
  ArrTh2[posArrTh2] = analogRead(TPS2);
  posArrTh2++;
}

/*
 * acquisizione ADC circolare su array da <lunghezza> posizioni
 */
void acquisizioneBSE() {
  if (posArrBk > (lunghezza - 1) )    posArrBk = 0;
  ArrBk[posArrBk] = analogRead(BRAKEIN);
  posArrBk++;
}

