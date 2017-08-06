/*
 *  ritorna la media degli elementi di un array di <lunghezza> posizioni
*/
int media(int* ar) {
  int out = 0;

  for (i = lun-1; i != lunghezza-lun; i++) {
    out += ar[i];
  }
  return out >>= lun;
}

/*
 * acquisizione ADC circolare su array da <lunghezza> posizioni
 * durante l'acquisizione viene modificato il valore appena scritto
 * nel caso in cui fosse distante dal precedente di più di <distanza>
 * numeri
 * Es:
 * precedente=1980 attuale=2000 → attuale=1990
 * 
*/
void acquisizioneTPS1() {
  if (posArrTh1 == lunghezza)
  {
    precedente=ArrTh1[posArrTh1];
    posArrTh1 = 0;
  }
  else{
    precedente=ArrTh1[posArrTh1-1];
  }
  ArrTh1[posArrTh1] = analogRead(TPS1);

  //confronto con il valore precedente e modifica del valore attuale
  if(ArrTh1[posArrTh1]>precedente+distanza)
    ArrTh1[posArrTh1]=precedente+distanza;
   else
    if (ArrTh1[posArrTh1]<precedente-distanza)
    ArrTh1[posArrTh1]=precedente-distanza;

  posArrTh1++;
}

/*
 * acquisizione ADC circolare su array da <lunghezza> posizioni
 * durante l'acquisizione viene modificato il valore appena scritto
 * nel caso in cui fosse distante dal precedente di più di <distanza>
 * numeri
 * Es:
 * precedente=1980 attuale=2000 → attuale=1990
 * 
*/
void acquisizioneTPS2() {
  if (posArrTh2 == lunghezza)
  {
    precedente=ArrTh2[posArrTh2];
    posArrTh2 = 0;
  }
  else{
    precedente=ArrTh2[posArrTh2-1];
  }
  ArrTh1[posArrTh2] = analogRead(TPS2);

  //confronto con il valore precedente e modifica del valore attuale
  if(ArrTh2[posArrTh2]>precedente+distanza)
    ArrTh2[posArrTh2]=precedente+distanza;
   else
    if (ArrTh2[posArrTh2]<precedente-distanza)
    ArrTh2[posArrTh2]=precedente-distanza;

  posArrTh2++;
}

/*
 * acquisizione ADC circolare su array da <lunghezza> posizioni
 * durante l'acquisizione viene modificato il valore appena scritto
 * nel caso in cui fosse distante dal precedente di più di <distanza>
 * numeri
 * Es:
 * precedente=1980 attuale=2000 → attuale=1990
 * 
*/
void acquisizioneBSE() {
  if (posArrBk == lunghezza)
  {
    precedente=ArrBk[posArrBk];
    posArrBk = 0;
  }
  else{
    precedente=ArrBk[posArrBk-1];
  }
  ArrBk[posArrBk] = analogRead(BRAKEIN);

  //confronto con il valore precedente e modifica del valore attuale
  if(ArrBk[posArrBk]>precedente+distanza)
    ArrBk[posArrBk]=precedente+distanza;
   else
    if (ArrBk[posArrBk]<precedente-distanza)
    ArrBk[posArrBk]=precedente-distanza;

  posArrBk++;
}

