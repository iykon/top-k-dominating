#include "postgres.h"
#include "fmgr.h"
#include "funcapi.h"
#include "executor/spi.h"
#include "utils/builtins.h"
#include "tkd_qsort.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MISS -2147483647

PG_MODULE_MAGIC;

/*
 * pre declaration, descriptions are displayed below
 */
int dominates(int x, int y);
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
int *maxscore,*score; // as define in paper, pruning data
int *queue; // priority queue, or heap
int miss,*missd; // the number of missing value
int *arr; // array
int *kesai; // ξ, the number of bins for each dimention
int *ari; // all existing values in a dimention
int *goods,*goodv; // number and value of distinguishing values
int *lbound,*ubound; // lower value of a bin and upper value of a bin, respectively, included
int *nonD; // as defined in paper
int *whichbin; // store which bin does a value belong to
int *incomparable,incomparablenumber; // set of incomparable values, number of incomparable values, respectively
int *tagT; // 
int **Pi,**Qi,*Q,*P,Qc,Pc; // as defined in paper
int dominating_type;
Dataset *dataset; // date set of all objects

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
		if(!PG_ARGISNULL(2))
            dominating_type = PG_GETARG_INT32(2);
        else
            dominating_type = 0;
	
		SPI_connect(); // open internal connection to database

		ret = SPI_exec(command, 0); // run the SQL command, 0 for no limit of returned row number
		
		N = SPI_processed; // save the number of rows
		
		dataset = (Dataset *)palloc(sizeof(Dataset)*(N+2));	
		candidateset = (int *)palloc(sizeof(int)*(N+2));
		if(dataset == NULL){
			exit(1);
		}
		
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
			 * and palloc all memory needed
			 * */
			kesai = (int *)palloc(sizeof(int)*(N+2));
			lbound = (int *)palloc(sizeof(int)*(N+2));
			ubound = (int *)palloc(sizeof(int)*(N+2));
			ari = (int *)palloc(sizeof(int)*(N+2));
			goods = (int *)palloc(sizeof(int)*(N+2));
			goodv = (int *)palloc(sizeof(int)*(N+2));
			Q = (int *)palloc(sizeof(int)*(N+2));
			P = (int *)palloc(sizeof(int)*(N+2));
			nonD = (int *)palloc(sizeof(int)*(N+2));
			whichbin = (int *)palloc(sizeof(int)*(N+2));
			incomparable = (int *)palloc(sizeof(int)*(N+2));
			tagT = (int *)palloc(sizeof(int)*(N+2));
			Pi = (int **)palloc(sizeof(int *)*(N+2));
			Qi = (int **)palloc(sizeof(int *)*(N+2));
			for(i = 0; i < N; ++i){
				Pi[i] = (int *)palloc(sizeof(int)*(N+2));
				Qi[i] = (int *)palloc(sizeof(int)*(N+2));
			}
			arr = (int *)palloc(sizeof(int)*(N+2));
			score = (int *)palloc(sizeof(int)*(N+2));
			queue = (int *)palloc(sizeof(int)*(N+2));
			missd = (int *)palloc(sizeof(int)*(D+2));
			maxscore = (int *)palloc(sizeof(int)*(N+2));
			for(i = 0; i < N; ++i){
				dataset[i].missing = (int *)palloc(sizeof(int)*(D+2));
				dataset[i].value = (int *)palloc(sizeof(int)*(D+2));
				dataset[i].T = (int *)palloc(sizeof(int)*(D+2));
				if(dataset[i].missing == NULL || dataset[i].value == NULL || dataset[i].T == NULL){
					exit(1);
				}
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
		retarr = (int *)palloc(sizeof(int)*(K+2));
		if(retarr == NULL){
			exit(1);
		}
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
		values = (char **)palloc((tupdesc->natts+2) * sizeof(char *));
		values = (char **)palloc((tupdesc->natts+2) * sizeof(char *));
		if(values == NULL){
			exit(1);
		}

		for(i = 0; i < tupdesc->natts; ++i ){
			tuple = tuptable->vals[retarr[call_cntr]];
			values[i] = (SPI_getvalue(tuple, tupdesc, i+1));
		}
		
        ret_tuple = BuildTupleFromCStrings(attinmeta, values); // build a return tuple 
		
        result = HeapTupleGetDatum(ret_tuple); // make the tuple into a datum
		
		SRF_RETURN_NEXT(funcctx,result);
	}
	else{
		SPI_finish();
		SRF_RETURN_DONE(funcctx);
	}
}
