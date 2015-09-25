#include <iostream>

using namespace std;


int main()
{
	unsigned int x = 246319;
	unsigned int y = 8768667;
	
	double c = 0.43;
	
	cout << x << "," << c * y << "," << (x >= c * y) << endl;
	return 0;
}