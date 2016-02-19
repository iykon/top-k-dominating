#include "tkd_algorithms.h"

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
int dominates(int x, int y){
    if(dominating_type == 0){
        if(x<y)
            return 1;
        else if(x==y)
            return 0;
        return -1;
    }
    else{
        if(x>y)
            return 1;
        else if(x==y)
            return 0;
        return -1;
    }
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
int getscore(int obj,int tau,int missingnumber, int sc){
	double sigma; // σ, missing rate
	int i,j,l;
	int sum,lastu,average,bin;
	int pando;
	int ar; 
	int omiga;
	int retval;
	
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
		if(kesai[i]<=0)
			kesai[i] = goods[0];
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
					++bin;
					break;
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
		 * identify which bin is each object in
		 */
		for(j=0;j<N;++j){
			if(dataset[j].missing[i]){
				whichbin[j]=0;
			}
			else{
				l=0;
				while(dominates(dataset[j].value[i],ubound[++l]) == -1);
				whichbin[j]=l;
			}
		}

		/*
		 * establish Qi,Pi
		 */
		if(dataset[obj].missing[i]){
			for(j=0;j<N;++j)
				Pi[i][j]=Qi[i][j]=1;
		}
		else{
			for(j=0;j<N;++j){
				Qi[i][j]=whichbin[obj]<=whichbin[j];
				Pi[i][j]=whichbin[obj]<whichbin[j];
			}
		}
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
		retval = 0;
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
						else if(dominates(dataset[i].value[j],dataset[obj].value[j])==1){
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
		retval = ar+omiga;
	}
	return retval;
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
				if(candidateset[0]==K){
					for(i=1;i<=candidateset[0];++i){
						if(score[candidateset[i]]==tau){
							candidateset[i]=t;
							break;
						}
					}
				}
				else {
					candidateset[++candidateset[0]]=t;
				}
				
				if(candidateset[0]==K){ // candidate set full
					tau=score[candidateset[1]];
					for(i=2;i<=K;++i)
						if(score[candidateset[i]]<tau)
							tau=score[candidateset[i]];
				}
			}
		}
		
	}
}

