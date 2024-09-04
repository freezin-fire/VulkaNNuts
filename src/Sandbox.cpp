#include "Application.h"

#include <cstdlib>
#include <iostream>
#include <stdexcept>

int main() {
	NNuts::NNApplication sandbox{};

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