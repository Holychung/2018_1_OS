/*
Student No.: 0516205
Student Name: 鍾禾翊
Email: mickey94378@gmail.com
SE tag: xnxcxtxuxoxsx
Statement: I am fully aware that this program is not
supposed to be posted to a public server, such as a
public GitHub repository or a public web page.
*/

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <map>
#include <list>
#include <set>
#include <ctime>

using namespace std;

class Node{
	public:
		int request;
		int ref_count;
		int seq_num;
		bool operator<(const Node& b) const
		{
			return ref_count < b.ref_count || (ref_count == b.ref_count) && seq_num < b.seq_num;
		}
};

void LFU(void);
void LRU(void);
char *filename[100];
// string filename;
int main(int argc, char *argv[]){
	if(argv[1]){
		*filename=argv[1];
		LFU();
		LRU();
	}
	else
		cout<<"missing arguement 1"<<endl;

	return 0;
}

void LFU(void){
	cout<<"LFU policy:"<<endl;
	cout<<"frame\thit\t\tmiss\t\tpage fault ratio"<<endl;
	int i, frame_size, now_frame, seq_num;
	int hit_count, miss_count;
	int request;
	double start_time, end_time, total_time=0;
	float fault_ratio;

	FILE *fin;
	set<Node> ListLFU;
	set<Node>::iterator iter_set;
	map<int, set<Node>::iterator> MapHash;
	map<int, set<Node>::iterator>::iterator iter_map;
	pair<set<Node>::iterator, bool> ret;

	for(i=128;i<1025;i=i*2){
		frame_size=i;

		if ((fin=fopen(*filename, "r"))==NULL){
	        printf("Error open file\n");
	        exit(1);
	    }
	    hit_count=0;
		miss_count=0;
		fault_ratio=0;
		now_frame=0;
		seq_num=0;

		start_time=clock();
	    while(fscanf(fin, "%d", &request)!=EOF){
	    	seq_num++;
	    	iter_map=MapHash.find(request);
	    	if(iter_map==MapHash.end()){
	    		// hash table miss
	    		miss_count++;
	    		Node tmp;
				tmp.request=request;
				tmp.seq_num=seq_num;
				tmp.ref_count=0;

	    		if(now_frame<frame_size){
					ret=ListLFU.insert(tmp);
					MapHash[request]=ret.first;
					now_frame++;
	    		}
	    		else{
	    			int del=ListLFU.begin()->request;
	    			MapHash.erase(del);
	    			ListLFU.erase(ListLFU.begin());
	    			ret=ListLFU.insert(tmp);
	    			if(ret.second==true)
		    			MapHash[request]=ret.first;
		    		else
		    			printf("error\n");
	    		}
	    	}
	    	else{
	    		// hash table hit
	    		hit_count++;
	    		Node tmp;
				tmp.request=request;
				tmp.seq_num=iter_map->second->seq_num;
				tmp.ref_count=iter_map->second->ref_count;
				tmp.ref_count++;

				ListLFU.erase(iter_map->second);
	    		ret=ListLFU.insert(tmp);
	    		if(ret.second==true)
	    			MapHash[request]=ret.first;
	    		else
	    			printf("error\n");
	    	}
	    }
	    
	    fault_ratio=float(miss_count)/float(miss_count+hit_count);
		printf("%d\t%d\t\t%d\t\t%.10f\n", frame_size, hit_count, miss_count, fault_ratio);
		
		MapHash.clear();
		ListLFU.clear();
		fclose(fin);
	}

	end_time=clock();
	total_time+=(end_time-start_time)/(double)(CLOCKS_PER_SEC);
	// cout<<"Total elapsed time: "<<total_time<<"sec"<<endl;
	printf("Total elapsed time: %.4f sec\n", total_time);
}

void LRU(void){
	cout<<"LRU policy:"<<endl;
	cout<<"frame\thit\t\tmiss\t\tpage fault ratio"<<endl;
	int i, frame_size, now_frame;
	int hit_count, miss_count;
	int request;
	double start_time, end_time, total_time=0;
	float fault_ratio;
	list<int> ListLRU;
	map<int, list<int>::iterator> MapHash;
	map<int, list<int>::iterator>::iterator iter;
	FILE *fin;

	for(i=128;i<1025;i=i*2){
		frame_size=i;
		if ((fin=fopen(*filename, "r"))==NULL){
	        printf("Error open file\n");
	        exit(1);
	    }

	    hit_count=0;
		miss_count=0;
		fault_ratio=0;
		now_frame=0;

	    start_time=clock();
	    while(fscanf(fin, "%d", &request)!=EOF){
	    	iter=MapHash.find(request);
	    	if(iter==MapHash.end()){
	    		// hash table miss
	    		miss_count++;
	    		if(now_frame<frame_size){
					ListLRU.push_front(request);
					MapHash[request]=ListLRU.begin();
					now_frame++;
	    		}
	    		else{
	    			MapHash.erase(ListLRU.back());
	    			ListLRU.pop_back();
	    			ListLRU.push_front(request);
	    			MapHash[request]=ListLRU.begin();
	    		}
	    		
	    	}
	    	else{
	    		// hash table hit
	    		hit_count++;
	    		ListLRU.erase(iter->second);
	    		ListLRU.push_front(request);
	    		MapHash[request]=ListLRU.begin();
	    	}
	    }
	    
	    fault_ratio=float(miss_count)/(float(miss_count+hit_count));
		printf("%d\t%d\t\t%d\t\t%.10f\n", frame_size, hit_count, miss_count, fault_ratio);
		MapHash.clear();
		ListLRU.clear();
		fclose(fin);
	}

	end_time=clock();
	total_time+=(end_time-start_time)/(double)(CLOCKS_PER_SEC);
	printf("Total elapsed time: %.4f sec\n", total_time);
}