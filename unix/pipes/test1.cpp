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

std::vector<std::pair<std::string, std::string>>
getCommands()
{
	std::vector<std::pair<std::string, std::string>> result;

	char buffer[1024];
	auto size = read(STDIN_FILENO, buffer, 1024);
	buffer[size] = '\0';
	std::istringstream ss(buffer);

	auto f = [](char c){return std::isspace(static_cast<unsigned char>(c));};
	std::vector<std::string> vec;
	std::string input;
	while(std::getline(ss, input, '|'))
	{
		input.erase(std::remove_if(input.begin(), input.end(), f), input.end());
		vec.push_back(input);
	}
	
	for (auto s : vec)
	{
		auto pos = s.find('-');
		if (pos != std::string::npos)
		{
			std::string param = s.substr(pos);
			std::string command = s.substr(0, pos);
			result.push_back({command, param});
		}
		else
		{
			result.push_back({s,""});
		}
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
