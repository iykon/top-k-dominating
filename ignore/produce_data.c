#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

int getnumber(char *s){
	int n = 0,i;
	for(i=0;i<strlen(s);++i){
		n = n*10 + s[i]-'0';
	}
	return n;
}

int main(int argc, char **argv){
	int N,D,K,R;
	int i,j,r;
	FILE *fp;
	if(argc!=6){
		printf("usage %s <N> <D> <K> <rate> <outpupt_file>\n",argv[0]);
		return 0;
	}
	srand(time(NULL));
	N = getnumber(argv[1]);
	D = getnumber(argv[2]);
	K = getnumber(argv[3]);
	R = getnumber(argv[4]);
	fp = fopen(argv[5],"w+");
	fprintf(fp,"%d %d %d\n",N,D,K);
	for(i=0;i<N;++i){
		for(j=0;j<D;++j){
			r = rand()%R;
			if(r == 0)
				fprintf(fp,"%d ",-1);
			else
				fprintf(fp,"%d ",rand()%N*D+1);
		}
		fprintf(fp,"\n");
	}
			
	
	return 0;
}
