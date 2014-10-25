/*
	Copyright (C) 2014 Chaos Software

	Distributed under the Boost Software License, Version 1.0.
	See accompanying file LICENSE_1_0.txt or copy at
	http://www.boost.org/LICENSE_1_0.txt.
*/

#include "cell_compiler.h"
#include "string_utils.h"

#include <cstdlib>

using namespace std;
using namespace chaos::cell;

int main(int argc, char* argv[])
{
	try
	{
		if (argc > 1)
		{

			auto baseModule = loadModule("D:\\projects\\cell\\cell_compiler\\base.bc");
			CellCompiler compiler;
			compiler.run(baseModule, argv[1], "custom_cell_ai_0");
		}
	}
	catch (exception& e)
	{
		cout << e.what() << endl;
		return EXIT_FAILURE;
	}
	catch (...)
	{
		cout << "unknown error\n";
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
