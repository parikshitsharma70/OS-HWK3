#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
# define size 100
int main(){
    
    int l=0,h=0;
    int count=0;
    int n=0;
    int* final = (int*) malloc(size * sizeof(int));
    
    //reading number of zip and interval from pipe
    read(STDIN_FILENO,&n,sizeof(int));
    read(STDIN_FILENO,&l,sizeof(int));
    read(STDIN_FILENO,&h,sizeof(int));
    
    int *data= (int*) malloc(n * sizeof(int));;
    
    for(int i=0;i<=n;i++){
        read(STDIN_FILENO,&data[i],sizeof(int));
    }	

    for (int i = 0; i <=n; i++) {

        // checking for given interval
        if (data[i] >= l && data[i] <= h) {
            final[count]=data[i];
            count++;
        }
    }
    
    //writing the value to pipe
    write(STDOUT_FILENO,&count,sizeof(int));
    
    for(int i=0;i<count;i++){
		write(STDOUT_FILENO,&final[i],sizeof(int));
    }

    free(final);
    free(data);
    return(0);    
}
