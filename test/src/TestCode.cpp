#include <cstdio>
#include <sys/types.h>
#include <sys/wait.h>
#include <cstdlib>
#include <unistd.h>
#include <cstring>

#include "TestCode.hpp"

void TestCode(const std::string& code, const std::string& expectedOutput)
{
	driver d;
	d.parseString(code);
	d.Compile();

	FILE* temp = tmpfile();

	pid_t pid = 0;// fork();
	if (pid == 0)
	{
		//child
		//int fd = fileno(temp);
		//dup2(fd, STDOUT_FILENO);

		voidFunc f = d.EmitCode();
		f();

		exit(0);
	}

	int status;
	waitpid(pid, &status, WUNTRACED | WCONTINUED);

	std::cout << "Exited with status" << WEXITSTATUS(status) << std::endl;
	char* line = nullptr;
	fscanf(temp, "%ms", &line);

	REQUIRE((strcmp(line, expectedOutput.c_str()) == 0));
}