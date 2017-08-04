


/* Function for bucket sort */


void buckSort(int array[], int n)
{
  int i, j;
  int count[n];
  for (i = 0; i < n; i++)
    count[i] = 0;

  for (i = 0; i < n; i++)
    (count[array[i]])++;

  for (i = 0, j = 0; i < n; i++)
    for (; count[i] > 0; (count[i])--)
      array[j++] = i;
}
/* End of Bucket_Sort() */

/*
  ritorna la media di un array di <lunghezza> posizioni
*/
int media(int* ar) {
  
  int out = 0;

  for (int i = 0; i < lunghezza; i++) {
    out += ar[i];
  }
  return out >>= lun;
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

