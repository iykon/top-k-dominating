#include "tkd_queue.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
 * name: perculateUp
 * author: Weida Pan
 * description: update upwards in a heap
 * implementation: find swap the largest among current node and its children to current index, do recursively
 * arguments: int a[] 	  -- keys of heap
 * 			  int index[] -- index of heap
 * 			  int pos 	  -- current index in heap
 * return value: none
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void perculateUp(int a[], int index[], int pos){
	int l=pos+pos, r=pos+pos+1;
	int largest = pos;
	if(l>N)
		return;
	// get the largest among three
	if(a[index[pos]]<a[index[l]])
		largest = l;
	if(r<=N && a[index[largest]]<a[index[r]])
		largest = r;
	if(largest!=pos){
		index[pos]^=index[largest];
		index[largest]^=index[pos];
		index[pos]^=index[largest];
		if(pos>1)
			perculateUp(a,index,pos>>1);
	}
	return;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
 * name: popqueue
 * author: Weida Pan
 * description: pop the head from a heap and maintain the property of heap
 * implementation: put an key to the head, and update downwards by find the largest among current position and its children
 * 				   execute recursively until hit the tail of heap
 * arguments: int a[] -- index of heap
 * 			  int v[] -- keys of heap
 * return value: index of the formar head key
 */
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int popqueue(int a[],int v[]){
	int popqueue=a[1];
	int pos=1,largest;
	a[1]=a[a[0]--];
	while(pos<=a[0]/2){
		//get the largest among three
		largest=pos;
		if(v[a[pos+pos]]>v[a[largest]])
			largest=pos+pos;
		if(pos+pos+1<=a[0] && v[a[pos+pos+1]]>v[a[largest]])
			largest = pos+pos+1;
		if(largest!=pos){ // elements exchange
			a[pos]^=a[largest];
			a[largest]^=a[pos];
			a[pos]^=a[largest];
		}
		else break;
		pos<<=1;
	}
	return popqueue;
}
