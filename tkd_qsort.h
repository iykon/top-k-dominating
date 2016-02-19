#ifndef TKDQSORT_H
#define TKDQSORT_H

///////////////////////////////////////////////////////////////////////
/*
 * name: partition
 * author : Weida Pan
 * description: partition for quick sort
 * implementation: find a pivot and sort elements smaller than it and larger than it
 * arguments: int a[] -- array being sorted
			  int d   -- dimention of dataset
			  int l   -- lower bound of array, included
			  int r   -- upper bound of array, included
 * return value: the index of pivot for quicksort
 */
///////////////////////////////////////////////////////////////////////
int partition(int a[],int d, int l,int r);

///////////////////////////////////////
/*
 * name: quicksort
 * author: Weida Pan
 * description: sort the array by 'quick sort'
 * implementation: same as how quick sort is implemented
 * arguments: int a[] -- array being sorted
 * 			  int d   -- dimention of dataset
 * 			  int l   -- lower bound of array, included
 * 			  int r   -- upper bound of array, included
 * return value: none
 */
///////////////////////////////////////
void quicksort(int a[],int d, int l, int r);

#endif
