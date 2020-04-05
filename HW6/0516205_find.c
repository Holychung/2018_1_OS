#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>

#define MB 1048576

int flag[4]={0};
char target_name[200];
unsigned long long int target_inode;
double target_min, target_max;

void SearchDir(char dir_path[200]);
int Check(char name[200], int inode, double file_size);


int main(int argc, char* argv[]){
	char start_path[100];
	strcpy(start_path, argv[1]);

	for(int i=2; i<argc; i+=2){
		if(strcmp(argv[i], "-inode")==0){
			flag[0]=1;
			target_inode=atoi(argv[i+1]);
		}
		else if(strcmp(argv[i], "-name")==0){
			flag[1]=1;
			strcpy(target_name, argv[i+1]);
		}
		else if(strcmp(argv[i], "-size_min")==0){
			flag[2]=1;
			target_min=atof(argv[i+1]);
		}
		else if(strcmp(argv[i], "-size_max")==0){
			flag[3]=1;
			target_max=atof(argv[i+1]);
		}
	}

	SearchDir(start_path);

	return 0;
}

void SearchDir(char dir_path[200]){
	DIR *ptr_dir;
	struct dirent *ptr_dirent;
	char file_path[200];
	struct stat buf;

	ptr_dir=opendir(dir_path);
	
    while((ptr_dirent=readdir(ptr_dir))!=NULL){
    	// concate
    	sprintf(file_path,"%s/%s", dir_path, ptr_dirent->d_name);
    	stat(file_path, &buf);

    	if(Check(ptr_dirent->d_name, buf.st_ino, (double)buf.st_size/MB))
    		printf("%s %ld %lu %.1f MB %s", file_path, buf.st_ino, buf.st_nlink, (double)buf.st_size/MB, ctime(&buf.st_ctime));
    	
    	if(!strcmp(ptr_dirent->d_name, ".")||!strcmp(ptr_dirent->d_name, ".."))
            continue;

		// it is directory
    	if(S_ISDIR(buf.st_mode))
        	SearchDir(file_path);
    }
    closedir(ptr_dir);
}

int Check(char name[200], int inode, double file_size){
	if(flag[0]&&inode!=target_inode)
		return 0;
	if(flag[1]&&strcmp(name, target_name))
		return 0;
	if(flag[2]&&file_size<target_min)
		return 0;
	if(flag[3]&&file_size>target_max)
		return 0;
	return 1;
}