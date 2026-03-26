#include <iostream>
#include <string>

#include "engine.h"
#include "parser.h"
#include "tokenizer.h"

/*
Recieves a line from the console and executes it.
.tables - shows all table names in the database
else parses line as sql query
input: string from console
output: none
*/
static void executeConsoleCommand(const std::string& line, Engine& engine)
{
	if (line == ".tables")
	{
		printTables(engine.getTables());
		return;
	}

	// Done with meta commands, now we parse as if it is a sql query
	Tokenizer tokenizer = Tokenizer(line);
	const std::vector<Token> tokens = tokenizer.tokenize();

	Parser parser = Parser(tokens);
	std::unique_ptr<Statement> statement = parser.parse();

	engine.executeStatement(*statement);
}

int main(int argc, char* argv[])
{
	Engine engine = Engine("mySQL");

	while (true)
	{
		try 
		{
			std::string line;

			std::cout << "> ";

			if (!std::getline(std::cin, line)) {
				std::cout << "\nEOF received, exiting.\n";
				break;
			}

			if (line == ".quit")
			{
				break;
			}

			if (line.empty())
			{
				continue;
			}

			executeConsoleCommand(line, engine);
		}
		catch (const std::runtime_error& e)
		{
			std::cout << e.what() << std::endl;
		}

		std::cout << std::endl;
	}
	
	return 0;
}