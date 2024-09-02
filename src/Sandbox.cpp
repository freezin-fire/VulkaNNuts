#include "Application.h"

#include <cstdlib>
#include <iostream>
#include <stdexcept>

int main() {
	NNuts::Application sandbox{};

	try
	{
		sandbox.run();
	}
	catch (const std::exception&e)
	{
		std::cerr << e.what() << '\n';
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}