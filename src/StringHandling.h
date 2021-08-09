#pragma once
#include <string>
#include <vector>



class StringHandling
{
public:
	StringHandling(std::wstring str, const size_t lineBreak)
		:
		message(str),
		maxChars(lineBreak)
	{
		//TODO Line Wrapping

		if (message.size() >= 1)
		{
			while (message.find('\n') != std::string::npos)
			{
				std::wstring temp = message.substr(0, message.find('\n'));
				stringies.push_back(temp);
				message.erase(0, message.find('\n') + 1);
			}
			stringies.push_back(message);
		}
	}

	std::vector<std::wstring> GetStringies() const
	{
		return stringies;
	}

private:
	std::wstring message;
	std::vector<std::wstring> stringies;
	size_t maxChars;


	
};
