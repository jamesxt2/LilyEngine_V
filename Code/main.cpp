#include "Common.h"
#include "VulkanApp.h"

int main()
{
	LEV::VulkanApp myApp;
	try {
		myApp.run();
	}
	catch (const std::runtime_error& e) {
		std::cout << e.what() << std::endl;
	}

	return 0;
}