#include <iostream>

void *pointers[4000];

int main()
{
	std::cout << "Cast vs Union... Part 1: casts!" << std::endl
		<< "We have a 4000-elements array of void pointers... we cycle through it filling it with values." << std::endl
		<< "Every four pointers we have this sequence: int(1) float(3.14) bool(false) char('D')." << std::endl
		<< "After the filling we start the output" << std::endl
		<< "At the end we delete all the pointers" << std::endl;
	
	std::cout << "Phase 1: filling" << std::endl;
	for(register int i = 0; i < 4000; i+=4)
	{
		pointers[i] = new int; *(static_cast<int*>((pointers[i]))) = 1;
		pointers[i+1] = new float; *(static_cast<float*>((pointers[i+1]))) = 3.14;
		pointers[i+2] = new bool; *(static_cast<bool*>((pointers[i+2]))) = false;
		pointers[i+3] = new char; *(static_cast<char*>((pointers[i+3]))) = 'D';
	}

	std::cout << "Phase 2: output" << std::endl;
	for(register int i = 0; i < 4000; i+=4)
	{
		std::cout << *(static_cast<int*>((pointers[i]))) << '\t'
			<< *(static_cast<float*>((pointers[i+1]))) << '\t'
			<< *(static_cast<bool*>((pointers[i+2]))) << '\t'
			<< *(static_cast<char*>((pointers[i+3]))) << std::endl;
	}

	std::cout << "Phase 3: delete" << std::endl;
	for(register int i = 0; i < 4000; i+=4)
	{
		delete (static_cast<int*>((pointers[i])));
		delete (static_cast<float*>((pointers[i+1])));
		delete (static_cast<bool*>((pointers[i+2])));
		delete (static_cast<char*>((pointers[i+3])));
	}
	
	std::cout << "Part 1 complete" << std::endl;
}
