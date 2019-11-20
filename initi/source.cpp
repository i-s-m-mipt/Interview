#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>

namespace initi
{
	class Container // based on RB tree
	{
	public:

	private:

	};

} // namespace initi

int main(int argc, char * argv[])
{
	try
	{
		

		system("pause");

		return EXIT_SUCCESS;
	}
	catch (const std::exception & exception)
	{
		std::cerr << "Fatal error: " << exception.what() << std::endl;

		return EXIT_FAILURE;
	}
	catch (...)
	{
		std::cerr << "Fatal error: " << "unknown exception" << std::endl;

		return EXIT_FAILURE;
	}
}