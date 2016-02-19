#ifndef TKDQUEUE_H
#define TKDQUEUE_H

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
void perculateUp(int a[], int index[], int pos);

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
int popqueue(int a[],int v[]);

#endif
