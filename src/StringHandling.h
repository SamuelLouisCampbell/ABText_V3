#pragma once
#include <string>
#include <vector>



class StringHandling
{
public:
	StringHandling(std::string str, const size_t lineBreak)
		:
		message(str),
		maxChars(lineBreak)
	{
		//TODO Line Wrapping
		auto tempstringies = Break(message);
		for (auto& str : tempstringies)
		{
			if (str.size() > lineBreak)
			{
				auto wrapped = wrap(str.c_str(), lineBreak);
				auto substrings = Break(wrapped);
				for (auto& s : substrings)
				{
					stringies.push_back(s);
				}
			}
			else
			{
				stringies.push_back(str);
			}
		}
	}

	std::vector<std::string> GetStringies() const
	{
		return stringies;
	}

	std::vector<std::string> Break(std::string str)
	{
		std::string message = str;
		std::vector<std::string> vecc;
		if (message.size() >= 1)
		{
			while (message.find('\n') != std::string::npos)
			{
				std::string temp = message.substr(0, message.find('\n'));
				vecc.push_back(temp);
				message.erase(0, message.find('\n') + 1);
			}
			vecc.push_back(message);
		}
		return vecc;
	}

	std::string wrap(const char* text, size_t line_length = 72)
	{
		std::istringstream words(text);
		std::ostringstream wrapped;
		std::string word;

		if (words >> word) {
			wrapped << word;
			size_t space_left = line_length - word.length();
			while (words >> word) {
				if (space_left < word.length() + 1) {
					wrapped << '\n' << word;
					space_left = line_length - word.length();
				}
				else {
					wrapped << ' ' << word;
					space_left -= word.length() + 1;
				}
			}
		}
		return wrapped.str();
	}

private:
	std::string message;
	std::vector<std::string> stringies;
	size_t maxChars;


	
};
