#include <set>
#include <iostream>

int main()
{
	std::set<int> pippo;
	pippo.insert(4);
	pippo.insert(8);
	pippo.insert(384614039);
	pippo.insert(28111985);
	
	for(std::set<int>::iterator it = pippo.begin(); it != pippo.end(); it++)
	{
		std::cout << *it << std::endl;
	}
}