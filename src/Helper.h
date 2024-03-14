#pragma once
#include <string>
#include <vector>

namespace Helper
{
	std::string TransToLetterLower(const std::u16string& strText, bool capitalFirstChar);
	std::string TransToLetterUpper(const std::u16string& strText);
	std::string TransToFirstLetterLower(const std::u16string& sChinese);
	std::string TransToFirstLetterUpper(const std::u16string& sChinese);

	std::vector<std::string> ConvertFromFile(const std::string& fpath, bool acronym, bool capitalFirstChar);

	void WriteResult(
		const std::string& inputfile,
		const std::string& outputfile,
		bool onlyFirstChar,
		bool capitalFirstChar
	);
	void WriteResult(
		const std::string& leftFile,
		const std::string& rightFile,
		const std::string& outputfile,
		bool onlyFirstChar,
		bool capitalFirstChar
	);

	void WriteResult(
		const std::string& outfile, 
		const std::vector<std::string>& vec
	);
	void WriteResult(
		const std::string& outfile, 
		const std::vector<std::string>& vec1, 
		const std::vector<std::string>& vec2
	);

	std::u16string utf8toU16(const std::string& cstr);
	std::u16string gb2312ToU16(const std::string& str);
	std::string u16touUtf8(const std::u16string& ustr);
}