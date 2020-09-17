#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cctype>


void lastCommand(std::string command, std::string param)
{
	char data[1024];
	int f = open("/tmp/stepik_pipe", O_RDWR | O_CREAT | O_TRUNC, 0666);
	dup2(f, STDOUT_FILENO);
	if (param.empty())
		execlp(command.c_str(), command.c_str(), NULL);
	else
		execlp(command.c_str(), command.c_str(), param.c_str(), NULL);
	close(f);
}

void sendCommand(std::string command, std::string param)
{
	int pfd[2];
	pipe(pfd);
	if (!fork())
	{
		dup2(pfd[1], STDOUT_FILENO);
		close(pfd[1]);
		close(pfd[0]);
		if (param.empty())
			execlp(command.c_str(), command.c_str(), NULL);
		else
			execlp(command.c_str(), command.c_str(), param.c_str(), NULL);
		exit(0);
	}
	dup2(pfd[0], STDIN_FILENO);
	close(pfd[1]);
	close(pfd[0]);
}

std::string trim(std::string str)
{
	std::string result = str;
	const std::string spaces = " \t\f\v\n\r";
	std::string::size_type pos = result.find_first_not_of(spaces);
	if (pos != std::string::npos)
	{
		result.erase(0, pos);
	}
	
	pos = result.find_last_not_of(spaces);
	if (pos != std::string::npos)
	{
		result.erase(pos + 1);
	}

	return result;
}

std::vector<std::pair<std::string, std::string>>
getCommands()
{
	std::vector<std::pair<std::string, std::string>> result;

	char buffer[1024];
	auto size = read(STDIN_FILENO, buffer, 1024);
	buffer[size] = '\0';
	std::istringstream ss(buffer);

	std::string input;
	while(std::getline(ss, input, '|'))
	{
		input = trim(input);
		std::string command;
		std::string argc;
		auto pos = input.find_first_of(' ');
		if (pos != std::string::npos)
		{
			command = input.substr(0, pos);
			argc = input.substr(pos + 1, input.length() - pos - 1);
		}
		else
		{
			command = input;
		}
		result.push_back({command, argc});
	}	
	return result;
}

int main(int argc, char** argv)
{
	auto vec = getCommands();
	for (int i = 0; i < vec.size(); ++i)
	{
		if (i == vec.size() - 1)
			lastCommand(vec[i].first, vec[i].second);
		else
			sendCommand(vec[i].first, vec[i].second);
	}
	return 0;
}
