#ifndef TKDALGORITHMS_H
#define TKDALGORITHMS_H

/////////////////////////////////////////////
/*
 * name: dominates
 * author: Weida Pan
 * description: check if x dominates y
 * arguments: int x, int y
 * return value: 1 represents x dominates y;
 * 				 0 represents x equals y;
 * 				 -1 otherwise
 * */
/////////////////////////////////////////////
int dominates(int x, int y);

////////////////////////////////////////////////////////////////////////////////
/*
 * name: getscore
 * author: Weida Pan
 * description: get score of an object in dataset
 * implementation: 
 * arguments: int obj -- index of object in dataset
 * 			  int tau -- 
 * 			  int missingnumber -- the number of missing values of all objects
 * 			  int sc -- number of candidates in current candidate set
 * return value: score of the object
 */
////////////////////////////////////////////////////////////////////////////////
int getscore(int obj,int tau,int missingnumber, int sc);

////////////////////////////////////////////////////////////////////////////////////////////////////
/*
 * name: tkd_exec
 * author: Weida Pan
 * description: execute top-k-dominating query and get the candidate
 * implementation: calculate score and update candidate set using maxscore and maxbitscore pruning
 * arguments: none
 * return value: none
 */
////////////////////////////////////////////////////////////////////////////////////////////////////
void tkd_exec();

#endif
