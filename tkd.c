
#include "postgres.h"
#include "fmgr.h"
#include "funcapi.h"
#include "executor/spi.h"
#include "utils/builtins.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MISS -2147483647

PG_MODULE_MAGIC;

/*
 * pre declaration, descriptions are displayed below
 */
int partition(int a[], int d, int l, int r);
void quicksort(int a[], int d, int l, int r);
void perculateUp(int a[], int index[], int pos);
int popqueue(int a[],int v[]);
int getscore(int obj,int tau,int missingnumber, int sc);
void tkd_exec(void);
Datum tkd_query(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1(tkd_query); // version 1 function set to postgresql server

typedef struct DATASET{
	int *missing;	//value 1 represents for missing data, 0 otherwise
	int *value;		//value of data
	int *T;			// the number of values it dominates, say less than it
}Dataset;

int N,D,K; // number of objects, number of dimentions, top K as in query
int *candidateset; // candidate set
Dataset *dataset; // date set of all objects

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
		while (dataset[a[j]].value[d] > pivot);
		do i=i+1;
		while(dataset[a[i]].value[d] < pivot);
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
		largest=pos;
		if(v[a[pos+pos]]>v[a[largest]])
			largest=pos+pos;
		if(pos+pos+1<=a[0] && v[a[pos+pos+1]]>v[a[largest]])
			largest = pos+pos+1;
		if(largest!=pos){
			a[pos]^=a[largest];
			a[largest]^=a[pos];
			a[pos]^=a[largest];
		}
		else break;
		pos<<=1;
	}
	return popqueue;
}

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
int getscore(int obj,int tau,int missingnumber, int sc){//parameter hasn't been finished,as well as where calls it
	double sigma; // σ, missing rate
	int i,j,k,l;
	int sum,lastu,average,bin;
	int pando;
	int ar; 
	int omiga;
	int *kesai; // ξ, the number of bins for each dimention
	int *ari; // all existing values in a dimention
	int *goods,*goodv; // number and value of distinguishing values
	int *lbound,*ubound; // lower value of a bin and upper value of a bin, respectively, included
	int *nonD; // as defined in paper
	int *whichbin; // store which bin does a value belong to
	int *incomparable,incomparablenumber; // set of incomparable values, number of incomparable values, respectively
	int *tagT; // 
	int **bitmap; // as define in paper, the bitmap of all objects
	int **Pi,**Qi,*Q,*P,Qc,Pc; // as defined in paper
	
	kesai = (int *)palloc(sizeof(int)*D);
	lbound = (int *)palloc(sizeof(int)*N);
	ubound = (int *)palloc(sizeof(int)*N);
	ari = (int *)palloc(sizeof(int)*N);
	goods = (int *)palloc(sizeof(int)*N);
	goodv = (int *)palloc(sizeof(int)*N);
	Q = (int *)palloc(sizeof(int)*N);
	P = (int *)palloc(sizeof(int)*N);
	nonD = (int *)palloc(sizeof(int)*N);
	whichbin = (int *)palloc(sizeof(int)*N);
	incomparable = (int *)palloc(sizeof(int)*N);
	tagT = (int *)palloc(sizeof(int)*N);
	bitmap = (int **)palloc(sizeof(int *)*N);
	Pi = (int **)palloc(sizeof(int *)*N);
	Qi = (int **)palloc(sizeof(int *)*N);
	for(i = 0; i < N; ++i){
		bitmap[i] = (int *)palloc(sizeof(int)*D);
		Pi[i] = (int *)palloc(sizeof(int)*D);
		Qi[i] = (int *)palloc(sizeof(int)*D);
	}
		
	/*
	 *calculate the incomparable set with obj O(N*D)
	 */
	incomparablenumber=0;
	for(i=0;i<N;++i){
		for(j=0;j<D;++j)
			if(dataset[obj].missing[j]+dataset[i].missing[j]!=1)
				break;
		incomparablenumber+=incomparable[i]=(j==D);
	}

	/*
	 * calculate the bin size of each bin as well as the domain of each bin
	 * use a method of greedy
	 * when the sum of all values approach average value, go to next bin
	 */
	sigma = (missingnumber+0.0)/(N*D);
	for(i=0;i<D;i++){
		ari[0]=0; // put all existing values in ari
		for(j=0;j<N;++j)
			if(!dataset[j].missing[i])
				ari[++ari[0]]=j;
		quicksort(ari,i,1,ari[0]);
		goods[0]=goods[1]=1; 
		goodv[1]=dataset[ari[1]].value[i];
		for(j=2;j<=ari[0];++j)//calculate the number of a certain value
			if(dataset[ari[j]].value[i]==dataset[ari[j-1]].value[i]) // calculate the number of an identical value
				++goods[goods[0]];
			else{
				goods[++goods[0]]=1;
				goodv[goods[0]]=dataset[ari[j]].value[i];
			}
		kesai[i]=(int)(sqrt(sigma*N/(log(sigma*N)-1)));
		average=(int)(ari[0]/kesai[i]);
		if(goods[0]<=kesai[i]){ //if goods are less than bins, each bin contains a single value
			kesai[i]=goods[0];
			bin=goods[0];
			for(j=1;j<=goods[0];++j)
				lbound[j]=ubound[j]=goodv[j];
		}
		else{
			sum=0,bin=0,lastu=1;
			for(j=1;j<=goods[0];++j)
				if(bin==kesai[i]-1){ // this is the last bin
					lbound[bin]=goodv[lastu];
					ubound[bin]=goodv[goods[0]];
					j=goods[0];
				}
				else if(sum>average){ // sum is over average, go to next bin
					sum=goods[j];
					lbound[bin]=goodv[lastu];
					ubound[bin]=goodv[j-1];
					lastu=j;
					++bin;
				}
				else if(sum+goods[j]<average)
					sum+=goods[j];
				else if((average-sum)<=(sum+goods[j]-average)){ // when putting a value into a bin exactly over the sum, calculate the difference to decide put in into current bin or next bin
						sum=goods[j];
						ubound[bin]=goodv[j-1];
						lbound[bin]=goodv[lastu];
						lastu=j;
						++bin;
				}
				else{
					sum = 0;
					ubound[bin]=goodv[j];
					lbound[bin]=goodv[lastu];
					lastu=j+1;
					++bin;
				}
		}
		
		/*
		 * get bitmap of the data
		 */
		for(j=0;j<N;++j){
			if(dataset[j].missing[i]){
				whichbin[j]=0;
				for(k=0;k<=kesai[i];++k)
					bitmap[j][k]=1;
			}
			else{
				l=0;
				while(dataset[j].value[i]>ubound[++l]);
				whichbin[j]=l;
				for(k=0;k<l;++k)
					bitmap[j][k]=1;
				for(;k<=kesai[i];++k)
					bitmap[j][k]=0;
			}
		}

		/*
		 * establish Qi,Pi
		 */
		if(dataset[obj].missing[i])
			for(j=0;j<N;++j)
				Pi[i][j]=Qi[i][j]=1;
		else
			for(j=0;j<N;++j)
				Qi[i][j]=bitmap[obj][whichbin[obj]];
				Pi[i][j]=bitmap[obj][whichbin[obj]+1];
	}

	/*
	 * to this step calculate Q and P using Pi and Qi
	 */
	Qc=-1;	
	for(i=0;i<N;++i){
		for(j=0;j<D;++j)
			if(Qi[j][i]==0)
				break;
		if(j==D){
			++Qc;
			Q[i]=1;	
		}
		else
			Q[i]=0;
	}
	Q[obj]=0;
	if(sc==K && Qc<=tau)
		return 0;
	else{
		Pc=0;
		for(i=0;i<N;++i){
			for(j=0;j<D;++j)
				if(Pi[j][i]==0)	
					break;
			if(j==D){
				++Pc;
				P[i]=1;
			}
			else
				P[i]=0;
		}
		/*
		 * to this step, P is attained
		 */
		omiga=0;
		for(i=0;i<N;++i)
			if(P[i]==1 && incomparable[i]==0)
				++omiga;
		nonD[0]=0;
		for(i=0;i<N;++i)
			if(P[i]==0 && Q[i]==1){
				tagT[i]=0;
				for(j=0;j<D;++j)
					if(!dataset[obj].missing[j]){
						if(dataset[i].value[j]==dataset[obj].value[j])
							tagT[i]++;
						else if(dataset[i].value[j]<dataset[obj].value[j]){
							nonD[++nonD[0]]=i;
							if(sc==K && nonD[0]>Qc-incomparablenumber-tau)
								return 0;
						}
					}
				pando=0;
				for(j=0;j<D;++j)
					pando+=(dataset[i].missing[j]==0 && dataset[obj].missing[j]==0);
				if(pando==tagT[i])
					nonD[++nonD[0]]=i;
			}
		/*
		 * calculate the cardinality of two sets and return
		 */
		ar=0;
		for(i=1;i<=nonD[0];++i)
			Q[nonD[i]]=0;
		for(i=0;i<N;++i)
			ar+=(Q[i]==1 && P[i]==0);
		return ar+omiga;
	}
}

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
void tkd_exec(){
	int i,j,k,s,t,tau;
	int *maxscore,*score, *maxbitscore; // as define in paper, pruning data
	int *queue; // priority queue, or heap
	int miss,*missd; // the number of missing value
	int *arr; // array

	maxscore = (int *)palloc(sizeof(int)*N);
	score = (int *)palloc(sizeof(int)*N);
	maxbitscore = (int *)palloc(sizeof(int)*N);
	queue = (int *)palloc(sizeof(int)*N);
	candidateset = (int *)palloc(sizeof(int)*N);
	arr = (int *)palloc(sizeof(int)*N);
	missd = (int *)palloc(sizeof(int)*D);

	miss = 0;
	/*
	 * calculate missing values O(N*D) and initialize T
	 */
	for(i = 0; i < N; ++i){
		for(j = 0; j < D; ++j){
			dataset[i].T[j] = N-1;//initialization
			miss += dataset[i].missing[j];
		}
	}

	/*
	 * calculate the number of objects a certain object dominates on a certain dimention O(D*N*logN)
	 */
	for(i = 0; i < D; ++i){
		missd[i] = 0;
		for(j = 0; j < N; ++j){
			arr[j] = j;
			if(dataset[j].missing[i])
				++missd[i];
		}
		quicksort(arr,i,0,N-1);
		j = N-1;
		while(j>=missd[i]){
			k = j;
			while(k>0 && dataset[arr[j]].value[i]==dataset[arr[--k]].value[i]);
			if(k==0)
				--k;
			for(s = k+1; s <= j; ++s)
				dataset[arr[s]].T[i] = missd[i]+N-k-2;
			j = k;
		}
	}

	/*
	 * calculate maxscore O(N*D)
	 */
	for(i = 0;i < N; ++i){
		maxscore[i]=dataset[i].T[0];
		for(j = 1; j < D; ++j)
			if(dataset[i].T[j]<maxscore[i])
				maxscore[i]=dataset[i].T[j];
	}

	/*
	 * maintain a priority queue O(N*logN)
	 */
	queue[0] = N;
	for(i = 1;i <= N; ++i)
		queue[i] = i-1;
	for(i = 1; i <= N/2; ++i)
		perculateUp(maxscore,queue,i);

	/*
	 * maintain a candidate set with max scores and using pruning
	 */
	tau = -1,candidateset[0]=0;
	while(queue[0]){
		t = popqueue(queue,maxscore);
		
		if(maxscore[t]<tau) // maxscore pruning
			break;
		else{
			score[t]=getscore(t,tau,miss,candidateset[0]); // bitscore pruning
			if(score[t]>tau || tau<0){
				if(candidateset[0]==K)
					for(i=1;i<=candidateset[0];++i){
						if(score[candidateset[i]]==tau){
							candidateset[i]=t;
							break;
						}
					}
				else 
					candidateset[++candidateset[0]]=t;
				
				if(candidateset[0]==K){ // candidate set full
					tau=score[candidateset[1]];
					for(i=2;i<=K;++i)
						if(score[candidateset[i]]<tau)
							tau=score[candidateset[i]];
				}
			}
		}
		
	}
	pfree(maxscore);
	pfree(score);
	pfree(queue);
	pfree(arr);
	pfree(missd);
	pfree(maxbitscore);
}

///////////
/*
 * name: tkd_query
 * author: Weida Pan
 * description: a function integrated with postgresql server
 * 				execute user query, read all tuples and execute tkd query
 * 				output the result tuples as in postgresql
 * arguments: PG_FUNCTION_ARGS -- postgres function arguments, infomation about tuples are included
 * return value: postgresql data type
 */
///////////
Datum tkd_query(PG_FUNCTION_ARGS){
	char *command;
	int i, j;
	int ret, curdm; // return value, current dimetion
	int call_cntr;
	int max_calls;
	int *retarr;
	//Dataset *retstruct;

	FuncCallContext *funcctx;	// context switch variable
	AttInMetadata *attinmeta;	// not known------------------------
	TupleDesc tupdesc;			// tuple descriptor, for getting data

	D = 0;
	/*
	 * this section will be executed only for the first call of function
	 * connect to postgres server and execute the first command and get data
	 */
	if(SRF_IS_FIRSTCALL()){
		MemoryContext oldcontext;
	
		/* 
		 * create a function context for cross-call persistence 
		 */
		funcctx = SRF_FIRSTCALL_INIT();

		/* 
		 * switch to memory context appropriate for multiple function calls 
		 * */
		oldcontext = MemoryContextSwitchTo(funcctx->multi_call_memory_ctx);

		/* 
		 * get arguments, convert given text object to a C string that can be used by server 
		 * */
		command = text_to_cstring(PG_GETARG_TEXT_P(0));

		/*
		 * arguments error
		 * */
		if(!PG_ARGISNULL(1))
			K = PG_GETARG_INT32(1);
		else
			K = 1;
	
		SPI_connect(); // open internal connection to database

		ret = SPI_exec(command, 0); // run the SQL command, 0 for no limit of returned row number
		
		N = SPI_processed; // save the number of rows

		dataset = (Dataset *)palloc(sizeof(Dataset)*N);	
		
		/*
		 * some rows are fetched
		 * */
		if(ret > 0 && SPI_tuptable != NULL){
			TupleDesc tupdesc;
			SPITupleTable *tuptable;
			HeapTuple tuple;
			char *type_name;

			/*
			 * get tuple description
			 * */
			tupdesc = SPI_tuptable->tupdesc;
			tuptable = SPI_tuptable;

			/* 
			 * for each colum, check type
			 * */
			for(i = 1; i <= tupdesc->natts; ++i){ 
				type_name = SPI_gettype(tupdesc, i);// get type of data
				if(strcmp(type_name,"int4") == 0 || strcmp(type_name,"int2") ==0 )//add float4 or flat8 types if want (2)
					++D;
			}
			
			/* 
			 * for each tuple
			 * */
			for(i = 0; i < N; ++i){
				dataset[i].missing = (int *)palloc(sizeof(int)*D);
				dataset[i].value = (int *)palloc(sizeof(int)*D);
				dataset[i].T = (int *)palloc(sizeof(int)*D);
				curdm = 0;
				tuple = tuptable->vals[i]; // get the ith tuple

				/* 
				 * for each dimention of a tuple 
				 * */
				for(j = 1; j <= tupdesc->natts; ++j) {
					type_name = SPI_gettype(tupdesc, j);
					if(strcmp(type_name,"int4") == 0 || strcmp(type_name,"int2") == 0 ){
						if(SPI_getvalue(tuple, tupdesc, j) == NULL) { // value is missing
							dataset[i].missing[curdm] = 1;
							dataset[i].value[curdm] = MISS;
						}
						else{ // value is not missing
							dataset[i].missing[curdm] = 0;
							dataset[i].value[curdm] = atof(SPI_getvalue(tuple, tupdesc, j));
						}
						++curdm;
					}
				}
			}
		}
		pfree(command);

		tkd_exec(); // call to execute tkd query
		
		funcctx->max_calls = K;
	
		/*
		 * allocate local variable retstruct and store the result tuple init
		 * */
		retarr = (int *)palloc(sizeof(int)*K);
		for(i = 0; i < K; ++i )
			retarr[i] = candidateset[i+1];
		funcctx->user_fctx = retarr;

		if(get_call_result_type(fcinfo, NULL, &tupdesc) != TYPEFUNC_COMPOSITE)
            ereport(ERROR, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED), errmsg("function returning record called in context that cannot accept type record")));
		
		/* Generate attribute metadata needed later to produce tuples from raw C strings */
        attinmeta = TupleDescGetAttInMetadata(tupdesc);
        funcctx->attinmeta = attinmeta;

        /* MemoryContext switch to old context */
        MemoryContextSwitchTo(oldcontext);
	}

	funcctx = SRF_PERCALL_SETUP();

	call_cntr = funcctx->call_cntr;
	max_calls = funcctx->max_calls;
	attinmeta = funcctx->attinmeta;
	retarr = funcctx->user_fctx;
	
	if(call_cntr < max_calls){
		char **values;
		HeapTuple tuple;
		HeapTuple ret_tuple;
		Datum result;
		TupleDesc tupdesc;
		SPITupleTable *tuptable;

		tupdesc = SPI_tuptable->tupdesc;
		tuptable = SPI_tuptable;
		
		/*
         * Prepare a values array for building the returned tuple.
         * This should be an array of C strings which will
         * be processed later by the type input functions.
         */
		values = (char **)palloc(tupdesc->natts * sizeof(char *));

		for(i = 0; i < tupdesc->natts; ++i ){
			tuple = tuptable->vals[retarr[call_cntr]];
			values[i] = (SPI_getvalue(tuple, tupdesc, i+1));
		}
		
        ret_tuple = BuildTupleFromCStrings(attinmeta, values); // build a return tuple 
		
        result = HeapTupleGetDatum(ret_tuple); // make the tuple into a datum
		
		SRF_RETURN_NEXT(funcctx,result);
	}
	else{
		pfree(candidateset);
		pfree(dataset);
		SPI_finish();
		SRF_RETURN_DONE(funcctx);
	}
}
