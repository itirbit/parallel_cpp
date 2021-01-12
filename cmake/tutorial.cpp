#include <iostream>
#include "TutorialConfig.h"

int main(int argc, char** argv)
{
	std::cout << "Hello from cmake";
	if (argc < 2)
	{
		std::cout << argv[0] << "Version " << Tutorial_VERSION_MAJOR << "." << Tutorial_VERSION_MINOR << "/n";
		std::cout << "Usage: " << argv[0] << " number" << "/n";
		return 1;
	}
	const double inputValue = std::stod(argv[1]);
	return 0;
}
