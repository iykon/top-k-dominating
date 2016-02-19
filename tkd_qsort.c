#include "tkd_qsort.h"

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
int partition(int a[],int d, int l,int r){
	int pivot = dataset[a[l]].value[d], i = l-1, j = r+1;
	while (1){
		do j=j-1;
		//while (dataset[a[j]].value[d] > pivot);
		while(dominates(dataset[a[j]].value[d], pivot) == -1);
		do i=i+1;
		while(dominates(dataset[a[i]].value[d], pivot) == 1);
		if(i<j){
			a[i]^=a[j];
			a[j]^=a[i];
			a[i]^=a[j];
		}
		else return j;
	}
}		

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
void quicksort(int a[],int d, int l, int r){
	int p;
	if(l<r){
		p = partition(a,d,l,r);
		quicksort(a,d,l,p);
		quicksort(a,d,p+1,r);
	}
}


