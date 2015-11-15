#include <stdio.h>
#include <math.h>
#include <time.h>
#define AMOUNT 20000
#define MAXDIMENTION 50

int N,K,D;
struct DATASET{
	int missing[MAXDIMENTION];//1 represents for missing data
	int value[MAXDIMENTION];//value of data
	int T[MAXDIMENTION];// the number of values it dominates, say less than it
}dataset[AMOUNT];

/*
 * function: partition
 * partition for quick sort
 * find a pivot and sort elements smaller than it and larger than it
 * a[] is the index, d is dimention  l is lower bound, r is upper bound
 */
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

/*
 * function: quicksort
 * quick sort the array
 * a[] is the index, d is dimention  l is lower bound, r is upper bound
 */
void quicksort(int a[],int d, int l, int r){
	int p;
	if(l<r){
		p = partition(a,d,l,r);
		quicksort(a,d,l,p);
		quicksort(a,d,p+1,r);
	}
}

/*
 * function: perculateUp
 * a[] stores the keys, index[] is the index being sorted, pos is current position
 */
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

int getscore(int obj,int tau,int missingnumber, int sc){//parameter hasn't been finished,as well as where calls it
	int i,j,k,l;
	int kesai[MAXDIMENTION];//the number of bins
	double sigma;
	int sum,lastu,average,bin;
	int lbound[AMOUNT],ubound[AMOUNT];
	int ari[AMOUNT],goods[AMOUNT],goodv[AMOUNT];
	int bitmap[AMOUNT][MAXDIMENTION];
	int Pi[MAXDIMENTION][AMOUNT],Qi[MAXDIMENTION][AMOUNT],Q[AMOUNT],P[AMOUNT],Qc,Pc;
	int nonD[AMOUNT];
	int whichbin[AMOUNT];
	int incomparable[AMOUNT],incomparablenumber;
	int tagT[AMOUNT];
	int pando;
	int ar,omiga;
	//calculate the incomparable set with obj O(N*D)
	//incomparable=1 means it is incomparable with obj
	//incomparablenumber is the total number of incomparable data
	incomparablenumber=0;
	//printf("incompa: ");
	for(i=0;i<N;++i){
		for(j=0;j<D;++j)
			if(dataset[obj].missing[j]+dataset[i].missing[j]!=1)
				break;
		incomparablenumber+=incomparable[i]=(j==D);
		//printf("%d ",incomparable[i]);
	}
	//printf("\n");
	//calculate the bin size of each bin as well as the domain of each bin
	sigma = (missingnumber+0.0)/(N*D);
	//printf("missing: %d, sigma: %e\n",missingnumber,sigma);
	for(i=0;i<D;i++){
		ari[0]=0;//for sorting
		for(j=0;j<N;++j)
			if(!dataset[j].missing[i])
				ari[++ari[0]]=j;
		quicksort(ari,i,1,ari[0]);
		//printf("print the result of sort %d numbers.\n",ari[0]);
		for(l=1;l<=ari[0];++l)
			;//printf("%d ",dataset[ari[l]].value[i]);
		//printf("\n");
		goods[0]=goods[1]=1;
		goodv[1]=dataset[ari[1]].value[i];
		for(j=2;j<=ari[0];++j)//calculate the number of a certain value
			if(dataset[ari[j]].value[i]==dataset[ari[j-1]].value[i])
				++goods[goods[0]];
			else{
				goods[++goods[0]]=1;
				goodv[goods[0]]=dataset[ari[j]].value[i];
			}
		//printf("goods and goodv: %d\n",goods[0]);
		for(j=1;j<=goods[0];++j)
			;//printf("%d ",goods[j]);
		//printf("\n");
		for(j=1;j<=goods[0];++j)
			;//printf("%d ",goodv[j]);
		//printf("\n");
		//printf("%e %d\n",sigma,N);
		kesai[i]=(int)(sqrt(sigma*N/(log(sigma*N)-1)));
		//printf("kesai: %d\n",kesai[i]);
		average=(int)(ari[0]/kesai[i]);
		if(goods[0]<=kesai[i]){//if goods are less than bins
			kesai[i]=goods[0];
			bin=goods[0];
			for(j=1;j<=goods[0];++j)
				lbound[j]=ubound[j]=goodv[j];
		}
		else{
			sum=0,bin=0,lastu=1;
			for(j=1;j<=goods[0];++j)
				if(bin==kesai[i]-1){
					lbound[bin]=goodv[lastu];
					ubound[bin]=goodv[goods[0]];
					j=goods[0];
				}
				else if(sum>average){
					sum=goods[j];
					lbound[bin]=goodv[lastu];
					ubound[bin]=goodv[j-1];
					lastu=j;
					++bin;
				}
				else if(sum+goods[j]<average)
					sum+=goods[j];
				else if((average-sum)<=(sum+goods[j]-average)){
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
		//printf("lbound and ubound:\n");
		for(j=1;j<=bin;++j)
			;//printf("%d ",lbound[j]);
		for(j=1;j<=bin;++j)
			;//printf("%d ",ubound[j]);
		//printf("\n");
		//cause core dump
		//get bitmap of the data
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
		//establish Qi,Pi
		//return 0;
		if(dataset[obj].missing[i])
			for(j=0;j<N;++j)
				Pi[i][j]=Qi[i][j]=1;
		else
			for(j=0;j<N;++j)
				Qi[i][j]=bitmap[obj][whichbin[obj]];
				Pi[i][j]=bitmap[obj][whichbin[obj]+1];
	}
	// to this step , bitmap index is finished
	//and we calculate Q and P using Pi and Qi
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
	if(sc==k && Qc<=tau)
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
		}//to this step, P is attained
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
							if(sc==K && nonD[0]>Qc-incomparablenumber-tau)//need a parameter here
								return 0;
						}
					}
				pando=0;
				for(j=0;j<D;++j)
					pando+=(dataset[i].missing[j]==0 && dataset[obj].missing[j]==0);
				if(pando==tagT[i])
					nonD[++nonD[0]]=i;
			}
		//calculate the cardinality of two sets and return
		ar=0;
		for(i=1;i<=nonD[0];++i)
			Q[nonD[i]]=0;
		for(i=0;i<N;++i)
			ar+=(Q[i]==1 && P[i]==0);
		return ar+omiga;
	}
}

void TKD(){
	int i,j,k,s,t,tau;
	int maxscore[AMOUNT],score[AMOUNT];//pruning data
	int maxbitscore[AMOUNT];//pruning data
	int queue[AMOUNT];
	int sc[AMOUNT];//candidate set
	int miss,missd[MAXDIMENTION];//the number of missing value
	int arr[AMOUNT];
	scanf("%d %d %d",&N,&D,&K);
	//printf("initial: %d %d %d\n",N,D,K);
	miss = 0;
	//input data set and calculate missing values O(N*D) 
	for(i = 0; i < N; ++i){
		for(j = 0; j < D; j++){
			scanf("%d",&dataset[i].value[j]);
			dataset[i].T[j] = N-1;//initialization
			//here the value smaller than 0 donates missing, but need further update in application
			if(dataset[i].value[j] < 0)
				miss += dataset[i].missing[j] = 1;
			else 
				dataset[i].missing[j] = 0;
		}
	}
	//calculate the number of objects a certain object dominates on a certain dimention O(D*N*N)
	//printf("missing number: %d\n",miss);
	//-----------low efficiency---!!!!!!!!!!!!!!!!!!!!!!!!!!!
	/*for(i = 0; i < D; ++i)
		for(j = 0; j < N-1; j++)
			if(!dataset[j].missing[i])
				for(s = j+1; s < N; ++s)
					if(!dataset[s].missing[i]){
						dataset[j].T[i] -= dataset[j].value[i]>dataset[s].value[i];
						dataset[s].T[i] -= dataset[j].value[i]<dataset[s].value[i];
					}
	printf("low eff:\n");
	for(i=0;i<D;++i){
		for(j=0;j<N;++j)
			printf("%d ",dataset[j].T[i]);
		printf("\n");
	}*/
	for(i = 0; i < D; ++i){
		missd[i] = 0;
		for(j = 0; j < N; ++j){
			arr[j] = j;
			if(dataset[j].value[i]<0)
				++missd[i];
		}
		quicksort(arr,i,0,N-1);
		j = N-1;
		//printf("miss: %d\n",missd[i]);
		while(j>=missd[i]){
			k = j;
			while(k>0 && dataset[arr[j]].value[i]==dataset[arr[--k]].value[i]);
			if(k==0)
				--k;
			//printf("j:%d k: %d\n",j,k);
			for(s = k+1; s <= j; ++s)
				dataset[arr[s]].T[i] = missd[i]+N-k-2;
			j = k;
		}
		/*printf("%dth dimention\n",i);
		for(j = 0; j < N; ++j)
			printf("%d ",arr[j]);
		printf("\n");
		for(j = 0;j<N;++j)
			printf("%d ",dataset[arr[j]].value[i]);
		printf("\n");
		*/
	}
	/*printf("high eff:\n");
	for(i = 0; i<D;++i){
		for(j = 0;j<N;++j)
			printf("%d ",dataset[j].T[i]);
		printf("\n");
	}
	return;
	*/
	//dataset[i].T[j] is correct
	//printf("dominants:\n");
	for(i = 0; i < N; ++i){
		//printf("%d: ",i);
		for(j = 0; j < D; ++j)
			;//printf("%d ", dataset[i].T[j]);
		//printf("\n");
	}
	//printf("\n");
	//calculate maxscore O(N*D)
	for(i = 0;i < N; ++i){
		maxscore[i]=dataset[i].T[0];
		for(j = 1; j < D; ++j)
			if(dataset[i].T[j]<maxscore[i])
				maxscore[i]=dataset[i].T[j];
	}
	//printf("maxscore:\n");
	for(i = 0; i < N; ++i)
		;//printf("%d ",maxscore[i]);
	//printf("\n");
	//maintain a priority queue O(N*logN)
	queue[0] = N;
	for(i = 1;i <= N; ++i)
		queue[i] = i-1;
	for(i = 1; i <= N/2; ++i)
		perculateUp(maxscore,queue,i);
	//printf("--------------For priority queue----------------\n");
	for(i = 1; i <= N; ++i)
		;//printf("%d ",queue[i]);
	//printf("\n");
	tau = -1,sc[0]=0;
	while(queue[0]){
		t = popqueue(queue,maxscore);
		//printf("dequeue: %d\n",t);//correct dequeue
		
		if(maxscore[t]<tau)
			break;
		else{
			score[t]=getscore(t,tau,miss,sc[0]);// the parameter in the function getscore is not completed yet
			//printf("score[%d]: %d\n",t,score[t]);
			if(score[t]>tau || tau<0){
				if(sc[0]==K)
					for(i=1;i<=sc[0];++i){
						if(score[sc[i]]==tau){
							sc[i]=t;
							break;
						}
					}
				else 
					sc[++sc[0]]=t;
				if(sc[0]==K){
					tau=score[sc[1]];
					for(i=2;i<=K;++i)
						if(score[sc[i]]<tau)
							tau=score[sc[i]];
				}
			}
		}
		
	}
	//printf("\n");
	//to this step, the index remained in array sc is the result of the tkd query
	for(i=0;i<K;++i)
		printf("%d ",sc[i+1]);
	printf("\n");
}

int main(int argc, char **argv){
	time_t ctm;
	clock_t start,stop;
	if(argc!=3){
		printf("usage %s <input_file> <output_file>\n",argv[0]);
		return 0;
	}
	freopen(argv[1],"r",stdin);
	//freopen(argv[2],"w",stdout);
	start = clock();
	TKD();
	stop = clock();
	printf("%6.3f\n",(double)(stop-start)/1000);
	return 0;
}

