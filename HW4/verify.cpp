#include <iostream>
#include <fstream>
#include <cstdlib>

using namespace std;

int main(void){
	ifstream infile;
	int k = 0;
	int n = 500000;
	int array[500000];
	for(string x = "1"; x[0] <= '8'; x[0]++){
		string file_name = "output_" + x + ".txt";
		infile.open(file_name.c_str());
		if(!infile){
			cout << "Cannot open file to write";
			exit(1);
		}
		for(int i = 0; i < n; i++){
			infile >> array[i];
		}
		for(int i = 0; i < n; i++){
			if(i != array[i]){
				cout << i << " " << array[i] << endl;
				cout << file_name << " wrong" << endl;
				k = 1;
				break;
			}
		}
		infile.close();
	}
	if(k == 0){
		cout << "Congradulations" << endl;
	}
	return 0;
}