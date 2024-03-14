#include "Helper.h"
#include <stdint.h>
#include <algorithm>
#include <fstream>
#include <exception>
#include <unordered_map>
#include <fmt/core.h>
#include <iconv.h>
#include "ChinesePyTable.h"

std::string Helper::TransToLetterUpper(const std::u16string& strText)
{
	std::string ret;
	uint16_t ucode;

	for (int i = 0; i < strText.length(); i++)
	{
		ucode = *(uint16_t*)&strText[i];
		std::string py = ChinesePyTable::instance().getPinYin(ucode);

		if (!py.empty()) ret += py;
	}

	std::for_each(ret.begin(), ret.end(), [](char& ch) { ch -= 32; });

	return ret;
}


std::string Helper::TransToLetterLower(const std::u16string& strText, bool capitalFirstChar)
{
	std::string ret;
	uint16_t ucode;

	for (int i = 0; i < strText.length(); i++) {
		ucode = *(uint16_t*)&strText[i];
		std::string py = ChinesePyTable::instance().getPinYin(ucode);

		if (!py.empty()) {
			if (!capitalFirstChar) {
				ret += py;
			}
			else {
				py[0] -= 32;
				ret += py;
			}
		}
	}

	return ret;
}


std::string Helper::TransToFirstLetterLower(const std::u16string& sChinese)
{
	uint16_t ucode = 0;
	int nCount = sChinese.size();
	std::string letter(nCount, 0);
	
	int i = 0;
	std::for_each(sChinese.begin(), sChinese.end(), [&](char16_t ch) {
		ucode = *(uint16_t*)& ch;

		std::string py = ChinesePyTable::instance().getPinYin(ucode);

		if (!py.empty())
			letter[i++] = py[0];
	});

	//for (auto it = letter.begin(); it != letter.end();) {
	//	if (*it == '\0') it = letter.erase(it);
	//	else ++it;
	//}

	return letter;
}


std::string Helper::TransToFirstLetterUpper(const std::u16string& sChinese)
{
	uint16_t ucode = 0;
	int nCount = sChinese.size();
	std::string letter(nCount, 0);

	int i = 0;
	std::for_each(sChinese.begin(), sChinese.end(), [&](char16_t ch) {
		ucode = *(uint16_t*)&ch;
		std::string py = ChinesePyTable::instance().getPinYin(ucode);
		if(!py.empty())
			letter[i++] = py[0] - 32;
		});

	//for (auto it = letter.begin(); it != letter.end();) {
	//	if (*it == 0) it = letter.erase(it);
	//	else ++it;
	//}

	return letter;
}


std::vector<std::string> Helper::ConvertFromFile(const std::string& fpath, bool acronym, bool capitalFirstChar)
{
#ifdef _MSC_VER
	FILE* fp = NULL;
	fopen_s(&fp, fpath.c_str(), "r");
#else
	FILE* fp = fopen(fpath.c_str(), "r");
#endif

	if (!fp)
		throw std::invalid_argument(fmt::format("{} not found", fpath));

	std::vector<std::string> vec;
	char buf[128];

	while (!feof(fp)) {
		memset(buf, 0, sizeof buf);
		if (fgets(buf, sizeof(buf) - 1, fp)) {
			std::string cstr(buf);
			std::u16string ustr(utf8toU16(cstr));
			while (*(--ustr.end()) == u'\n' || *(--ustr.end()) == u'\0') ustr.erase(--ustr.end());
			std::string pinyin = acronym? TransToFirstLetterLower(ustr): TransToLetterLower(ustr, capitalFirstChar);
			if (!pinyin.empty()) vec.emplace_back(std::move(pinyin));
		}
	}

	fclose(fp);

	if (vec.empty())
		throw std::runtime_error(fmt::format("nothing read from {}", fpath));

	return vec;
}


void Helper::WriteResult(
	const std::string& inputfile,
	const std::string& outputfile,
	bool onlyFirstChar,
	bool capitalFirstChar
)
{
	std::ifstream ifs(inputfile, std::ios_base::in);
	if (!ifs.is_open())
		throw std::invalid_argument(fmt::format("failed to open {} to read", inputfile));

	std::ofstream ofs(outputfile, std::ios_base::out | std::ios_base::trunc);
	if (!ofs.is_open())
		throw std::invalid_argument(fmt::format("failed to open {} to write", outputfile));

	char buf[64];
	std::unordered_map<std::string, char> * maps = new std::unordered_map<std::string, char>();

	while (!ifs.eof()) {
		memset(buf, 0, sizeof buf);
		ifs.getline(buf, sizeof(buf) - 1);

		std::string cstr(buf);

		if (!cstr.empty()) {
			
			std::u16string ustr(utf8toU16(cstr));
		
			ustr.erase(std::find(ustr.begin(), ustr.end(), u'\n'), ustr.end());
			ustr.erase(std::find(ustr.begin(), ustr.end(), u'\0'), ustr.end());
			
			std::string pinyin = onlyFirstChar? TransToFirstLetterLower(ustr): TransToLetterLower(ustr, capitalFirstChar);
			pinyin.erase(std::find(pinyin.begin(), pinyin.end(), '\0'), pinyin.end());

			if (!pinyin.empty() && maps->find(pinyin) == maps->end()) {
				
				// Write to output file

				ofs.write(pinyin.c_str(), pinyin.size());
				ofs.write("\n", 1);

				maps->insert(maps->end(), { pinyin, 0 });
			}
		}

		// 我认为此时前 9 W 个中不大可能出现重复的了, 假如字典内容是比较规律的话, 比如按拼音首字母排序

		if (maps->size() >= 100000) {
			auto it = maps->begin();
			std::advance(it, maps->size() - 10000);
			maps->erase(maps->begin(), it);
		}
	}

	ofs.close();
	ifs.close();

	delete maps;
}


void Helper::WriteResult(
	const std::string& leftFile,
	const std::string& rightFile,
	const std::string& outputfile,
	bool onlyFirstChar,
	bool capitalFirstChar
)
{
	auto left = ConvertFromFile(leftFile, onlyFirstChar, capitalFirstChar);
	auto right = ConvertFromFile(rightFile, onlyFirstChar, capitalFirstChar);

	WriteResult(outputfile, left, right);
}


void Helper::WriteResult(
	const std::string& outfile,
	const std::vector<std::string>& vec)
{
	std::ofstream ofs(outfile, std::ios_base::out | std::ios_base::trunc);
	if (!ofs.is_open())
		throw std::runtime_error(fmt::format("failed to open {} to write", outfile));

	uint64_t i = 0;
	uint64_t total = vec.size();
	auto* maps = new std::unordered_map<std::string, char>();

	for (const auto& item : vec) {
		if (maps->find(item) == maps->end()) {
			ofs.write(item.c_str(), item.size());
			if (++i != total)
				ofs.write("\n", 1);

			maps->insert(maps->end(), { item, 0 });
		}

		if (maps->size() >= 100000) {
			auto it = maps->begin();
			std::advance(it, maps->size() - 10000);
			maps->erase(maps->begin(), it);
		}
	}

	ofs.close();
	delete maps;
}


void Helper::WriteResult(
	const std::string& outfile,
	const std::vector<std::string>& vec1, 
	const std::vector<std::string>& vec2)
{
	std::ofstream ofs(outfile, std::ios_base::out | std::ios_base::trunc);
	if (!ofs.is_open())
		throw std::runtime_error(fmt::format("failed to open {} to write", outfile));

	uint64_t i = 0;
	uint64_t total = vec1.size() * vec2.size();
	auto* maps = new std::unordered_map<std::string, char>();

	for (const auto& item1 : vec1) {
		for (const auto& item2 : vec2) {
			
			std::string combined = fmt::format("{}{}", item1, item2);

			if (maps->find(combined) == maps->end()) {
				if(++i == total)
					ofs.write(combined.c_str(), combined.size());
				else
					ofs.write(fmt::format("{}\n", combined).c_str(), combined.size() + 1);

				maps->insert(maps->end(), { combined, 0 });
			}

			if (maps->size() >= 100000) {
				auto it = maps->begin();
				std::advance(it, maps->size() - 10000);
				maps->erase(maps->begin(), it);
			}
		}
	}

	ofs.close();
	delete maps;
}


std::u16string Helper::utf8toU16(const std::string& cstr)
{
	static iconv_t cd = iconv_open("UTF-16LE", "UTF-8");
	if (cd == (iconv_t)-1)
		throw std::invalid_argument("iconv_open");

	std::string tmpstr(cstr);
	char* pin = &tmpstr[0];
	size_t inlen = tmpstr.size();
	
	size_t olen = inlen * sizeof(char16_t);
	std::u16string ustr(inlen, 0);
	char* pout = (char*)&ustr[0];
	
	size_t rc = iconv(cd, &pin, &inlen, &pout, &olen);

	if (rc == (size_t)-1) throw std::runtime_error("failed to convert UTF8 to UTF-16LE");

	return ustr;
}


std::u16string Helper::gb2312ToU16(const std::string& str)
{
	static iconv_t cd = iconv_open("UTF-16LE", "GB2312");
	if (cd == (iconv_t)-1)
		throw std::invalid_argument("iconv_open");

	std::string tmpstr(str);
	char* pin = &tmpstr[0];
	size_t inlen = tmpstr.size();

	size_t olen = inlen;
	std::u16string ustr(olen, 0);
	char* pout = (char*)&ustr[0];

	size_t rc = iconv(cd, &pin, &inlen, &pout, &olen);

	if (rc == (size_t)-1) throw std::runtime_error("failed to convert GB2312 to U16");

	return ustr;
}


std::string Helper::u16touUtf8(const std::u16string& ustr)
{
	static iconv_t cd = iconv_open("UTF-8", "UTF-16LE");
	if (cd == (iconv_t)-1)
		throw std::runtime_error("iconv_open");

	std::u16string tmpstr(ustr);
	size_t inlen = tmpstr.size() * sizeof(char16_t);
	char* pin = (char*)&tmpstr[0];

	size_t olen = inlen * 2;
	std::string cstr(olen + 1, 0);
	char* pout = &cstr[0];

	if (iconv(cd, &pin, &inlen, &pout, &olen) == (size_t)-1)
		throw std::runtime_error("failed to convert UTF-16LE to UTF8");

	cstr.erase(std::find(cstr.begin(), cstr.end(), '\0'), cstr.end());

	return cstr;
}
