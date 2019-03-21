#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

int main(int argc, char *argv[]){
    
    int low_zip,high_zip,child=0;
    int fd1[2], fd2[2],fd3[2], fd4[2];
    int data,temp,n=0,c[1000],check,check1;
	int i,data1,data2,found,found2;
    FILE *fp;
    pid_t pid;

  // Error checking 
    if(argc != 4) {
        fprintf (stderr, "Usage: %s <data-filename> <low-zip> <high-zip>\n", argv[0]);
        exit (1);
    }

    low_zip=atoi(argv[2]);
    high_zip=atoi(argv[3]);
    
    if(low_zip>high_zip){
        fprintf(stderr,"<low-zip> <= <high-zip> required.\n");
        exit(1);
    }

	// Creating 4 pipes
    
    if(pipe(fd1)<0){
        perror("pipe creation failed");
        exit(1);
    }
    if(pipe(fd2)<0){
        close(fd1[0]);
        close(fd1[1]);
        perror("pipe creation failed");
        exit(1);
    }
    if(pipe(fd3)<0){
        close(fd1[0]);
        close(fd1[1]);
        close(fd2[0]);
        close(fd2[1]);
        perror("pipe creation failed");
        exit(1);
    }
    if(pipe(fd4)<0){
        close(fd1[0]);
        close(fd1[1]);
        close(fd2[0]);
        close(fd2[1]);
        close(fd3[0]);
        close(fd3[1]);
        perror("pipe creation failed");
        exit(1);
    }
    
    // opening the input file
    if ((fp = fopen(argv[1], "r")) == NULL)
    {
        fprintf(stderr,"Error opening file %s",argv[1]);
        exit(1);
    }
    
	// Creating fan processes
	for (int i = 1; i <= 2; i++){
		child++;
      if ((pid = fork()) == 0){	 // child process 
            if(child == 1){               //client 1
			    dup2(fd1[0], STDIN_FILENO);
                dup2(fd2[1], STDOUT_FILENO);
			    close(fd1[1]);
			    close(fd2[0]);
			    if((check=execl("./client","client",NULL)) < 0) {
                    fprintf(stderr,"exec failed for client 1");
                    exit(1);   
                }
			}

            if(child == 2){       // client 2
			    dup2(fd3[0], STDIN_FILENO);
         	    dup2(fd4[1], STDOUT_FILENO);
			    close(fd3[1]);
			    close(fd4[0]);
			    if((check1=execl("./client","client",NULL)) < 0) {
                    fprintf(stderr,"exec failed for client 2 ");
                    exit(1);   
                }
		    }
            break;
        }

        if(pid == -1) {
            fprintf(stderr,"Error creating process");
            exit(1);
        }
    }

	if( pid > 0){ // parent process
		fscanf(fp,"%d", &n);
        int f=n/2;
        int o=n-f;
	    fprintf(stderr,"client 1 was sent first %d zipcodes\n",f);
	    fprintf(stderr,"client 2 was sent the remaining %d zipcodes\n",o);
		
        //sendng number of zip codes, low-zip and high-zip to client 1
        write(fd1[1],&f,sizeof(int));
        write(fd1[1],&low_zip,sizeof(int));
        write(fd1[1],&high_zip,sizeof(int));
        
        //sendng number of zip codes, low-zip and high-zip to client 2
        write(fd3[1],&o,sizeof(int));
        write(fd3[1],&low_zip,sizeof(int));
        write(fd3[1],&high_zip,sizeof(int));
            
        //writing zipcodes to client 1
        for(int i = 0; i <= f; i++) {
            fscanf(fp,"%d", &data);
            write(fd1[1],&data,sizeof(int));
        }

        //writing zipcodes to client 2
        while((fscanf(fp,"%d", &data) != EOF)) {
            write(fd3[1],&data,sizeof(int));
        }

        close(fd3[1]);
        close(fd1[1]);
        
        //reading data from client 1
        read(fd2[0],&found,sizeof(int));
        fprintf(stderr,"client 1 found %d\n",found);
        
        for(int i=0;i<found;i++){
            read(fd2[0],&data1,sizeof(int));
            fprintf(stderr,"%d\t",data1);
        }	
        
        printf("\n");
        
        //reading data from client 2
        read(fd4[0],&found2,sizeof(int));
        fprintf(stderr,"client 2 found %d\n",found2);
        
        for(int i=0;i<found2;i++){
            read(fd4[0],&data2,sizeof(int));
            fprintf(stderr,"%d\t",data2);
        }	

        printf("\n");
        close(fd2[0]);
        close(fd4[0]);

    }
    return(0);   
}
