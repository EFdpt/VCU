/*
   ritorna la media di un array di <lunghezza> posizioni
   lunghezza deve essere 32 o 64
*/
int mediaPond(int* ar, int pos) {
  res = 0;

  //calcolo della media con 32 elementi
  switch (lunghezza){
	case 8:
    
    for (i = 0; i != 8; i++) {
      if(pos>i)  res += ar[i]*(8-((pos-1)-i));
      else  res+= ar[i]*(i-(pos-1));
    }
   res=res /36; //media ponderata 1-8
	break;	
case 16:
    
    for (i = 0; i != 16; i++) {
      if(pos>i)  res += ar[i]*(16-((pos-1)-i));
      else  res+= ar[i]*(i-(pos-1));
    }
    res=res /136; //media ponderata 1-16
  break;

  //calcolo della media con 32 elementi
  case 32:
    
    for (i = 0; i != 32; i++) {
      if(pos>i)  res += ar[i]*(32-((pos-1)-i));
      else  res+= ar[i]*(i-(pos-1));
    }
    res=res /528; //media ponderata 1-32
    break;
  //calcolo della media con 64 elementi
  case 64:
  
    for (i = 0; i != 64; i++) {
      if(pos>i) res += ar[i]*(64-((pos-1)-i));
      else  res+=ar[i]*(i-(pos-1));
    }

     res=res /2080; //media ponderata 1-64
    break;

	default: res= -1;
	break;
  }
  return res;
}


/*
 * acquisizione circolare su array che modifica il valore dell'acquisizione
 * se è distante dalla precendete più di un quarto della dinamica
 * 1230/4 = 307.5
 *
 * l'incremento della posizione di scrittura nell'array, è l'ultima istruzione.
 * Questo determina l'impossibilità di entrare nella funzione con posizione = 0
 * e la certezza che posizione assumerà il valore lunghezza
 */
int acquisizioneCutTPS1(int pos) {
//      //memorizzazione valore acquisizione precedente
//  if(pos==lunghezza)
//  pre1=ArrTh1[lunghezza-1];
//  else pre1=ArrTh1[pos-1];

      //aggiornamento posizione se fuori array
  if (pos== lunghezza ){
    pos= 0;
  }

  //acquisizione
  ArrTh1[pos] = analogRead(TPS1);

  //prevenzione dei picchi di segnale causati da disturbi
//  if(ArrTh1[pos] > pre1+307)  ArrTh1[pos]=pre1+307;
//  else if(ArrTh1[pos] < pre1-307)  ArrTh1[pos]=pre1-308;

   //preparazione posizione per prossima acquisizione
  return pos+=1;
}

/*
 * acquisizione circolare su array che modifica il valore dell'acquisizione
 * se è distante dalla precendete più di un quarto della dinamica
 * 1095/4 = 273.75
 *
 * l'incremento della posizione di scrittura nell'array, è l'ultima istruzione.
 * Questo determina l'impossibilità di entrare nella funzione con posizione = 0
 * e la certezza che posizione assumerà il valore lunghezza
 */
int acquisizioneCutTPS2(int pos) {
//    //memorizzazione valore acquisizione precedente
//    if(pos==lunghezza)
//  pre2=ArrTh2[lunghezza-1];
//  else pre2=ArrTh2[pos-1];

    //aggiornamento posizione se fuori array
  if (pos== lunghezza ){
    pos= 0;
  }

  //acquisizione
  ArrTh2[pos] = analogRead(TPS2);

  //prevenzione dei picchi di segnale causati da disturbi
//  if(ArrTh2[pos] > pre2+273)  ArrTh2[pos]=pre2+273;
//  else if(ArrTh2[pos] < pre2-273)  ArrTh2[pos]=pre2-274;

   //preparazione posizione per prossima acquisizione
  return pos+=1;
}


/*
 * acquisizione circolare su array che modifica il valore dell'acquisizione
 * se è distante dalla precendete più di un quarto della dinamica
 * 140/4 = 35
 * 
 * l'incremento della posizione di scrittura nell'array, è l'ultima istruzione.
 * Questo determina l'impossibilità di entrare nella funzione con posizione = 0
 * e la certezza che posizione assumerà il valore lunghezza
 */
int acquisizioneCutBSE(int pos) {
  //memorizzazione valore acquisizione precedente
//  if(pos==lunghezza)
//  preBk=ArrBk[lunghezza-1];
//  else preBk=ArrBk[pos-1];

  //aggiornamento posizione se fuori array
  if (pos== lunghezza ){
    pos= 0;
  }

  //acquisizione
  ArrBk[pos] = analogRead(BRAKEIN);
  
  //prevenzione dei picchi di segnale causati da disturbi
//  if(ArrBk[pos] > preBk+35)  ArrBk[pos]=preBk+35;
//  else if(ArrBk[pos] < preBk-35)  ArrBk[pos]=preBk-35;

  //preparazione posizione per prossima acquisizione
  return pos+=1;
}
