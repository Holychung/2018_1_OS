#include <iostream>
#include <fstream>
#include <cstdlib>

using namespace std;

int main(void){
	fstream myfile;
	myfile.open("test.txt");
	if(!myfile){
		cout << "Cannot open file to read";
		exit(1);
	}
	int n = 10000;
	myfile << n << " " << endl;
	for(int i = n - 1; i > 0; i--){
		myfile << i << " ";
	}
	myfile << 0;
	myfile.close();
	return 0;
}