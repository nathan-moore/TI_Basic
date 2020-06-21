#include <cstdio>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cstdlib>
#include <unistd.h>
#include <cstring>

#include "TestCode.hpp"

void TestCode(const std::string& code, const std::string& expectedOutput)
{
	driver d;
	d.parseString(code);
	d.Compile();

	int fd = open("tmpFile", O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
	REQUIRE(fd >= 0);

	pid_t pid = fork();
	REQUIRE(pid >= 0);

	if (pid == 0)
	{
		//child
		dup2(fd, STDOUT_FILENO);
		close(fd);

		voidFunc f = d.EmitCode();
		f();

		exit(0);
	}

	//int status;
	wait(NULL);

	//seek to beginning
	long error = lseek(fd, 0, 0);
	REQUIRE(error != -1);

	std::vector<char> buffer(expectedOutput.length() + 1);
	ssize_t count = read(fd, buffer.data(), buffer.size());

	close(fd);

	REQUIRE(count > 0);
	REQUIRE((strncmp(buffer.data(), expectedOutput.c_str(), buffer.size()) == 0));
}