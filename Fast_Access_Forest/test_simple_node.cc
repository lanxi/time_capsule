#include <iostream>

#include "simple_node.h"

using namespace std;

int main()
{
	simple_node sn;
	
	cin >> sn;
	
	cout << sn << endl;
	
	simple_node sn2 = sn;
	
	cout << sn2 << endl;
	
	return 0;
}