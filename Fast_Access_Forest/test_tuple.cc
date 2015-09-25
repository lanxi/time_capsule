#include <iostream>
#include <tuple>

using namespace std;

int main()
{
	tuple<int,int> mytuple(make_tuple(1,2));
	cout << "my tuple: (" << get<0>(mytuple) << "," << get<1>(mytuple) << ")" << endl;
	get<0>(mytuple) = 10;
	
}