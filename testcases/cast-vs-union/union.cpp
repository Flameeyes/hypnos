#include <iostream>

union weak {
	int *i;
	float *f;
	bool *b;
	char *c;
};

weak pointers[4000];

int main()
{
	std::cout << "Cast vs Union... Part 2: union!" << std::endl
		<< "We have a 4000-elements array of 'weak' union... we cycle through it filling it with values." << std::endl
		<< "Every four pointers we have this sequence: int(1) float(3.14) bool(false) char('D')." << std::endl
		<< "After the filling we start the output" << std::endl
		<< "At the end we delete all the pointers" << std::endl;
	
	std::cout << "Phase 1: filling" << std::endl;
	for(register int i = 0; i < 4000; i+=4)
	{
		pointers[i].i = new int; *(pointers[i].i) = 1;
		pointers[i+1].f = new float; *(pointers[i+1].f) = 3.14;
		pointers[i+2].b = new bool; *(pointers[i+2].b) = false;
		pointers[i+3].c = new char; *(pointers[i+3].c) = 'D';
	}

	std::cout << "Phase 2: output" << std::endl;
	for(register int i = 0; i < 4000; i+=4)
	{
		std::cout << *(pointers[i].i) << '\t'
			<< *(pointers[i+1].f) << '\t'
			<< *(pointers[i+2].b) << '\t'
			<< *(pointers[i+3].c) << std::endl;
	}

	std::cout << "Phase 3: delete" << std::endl;
	for(register int i = 0; i < 4000; i+=4)
	{
		delete (pointers[i].i);
		delete (pointers[i+1].f);
		delete (pointers[i+2].b);
		delete (pointers[i+3].c);
	}
	
	std::cout << "Part 2 complete" << std::endl;
}
