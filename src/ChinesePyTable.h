#pragma once
#include <string>
#include <cstdint>

class ChinesePyTable
{
	ChinesePyTable(const std::string& fdb, const std::string& dbName);

public:
	~ChinesePyTable();

	static ChinesePyTable& instance();

	std::string getPinYin(uint16_t ucode, bool isCommonUsedChar = true);
};

