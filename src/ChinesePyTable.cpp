#include "ChinesePyTable.h"
#include <SQLLiteWrapper.h>
#include <stdexcept>
#include <fmt/core.h>

SQLiteDB g_db;

ChinesePyTable::ChinesePyTable(const std::string & dbFile, const std::string & dbName)
{
	if (!g_db.isConnected())
	{
		if (!g_db.OpenConnection(dbFile, "./"))
			throw std::runtime_error(fmt::format("failed to open {}:{}", dbFile, g_db.GetLastError()));
	}
}


ChinesePyTable::~ChinesePyTable()
{
	g_db.CloseConnection();
}


ChinesePyTable & ChinesePyTable::instance()
{
	static ChinesePyTable inst("hanzi.db", "hanzi");
	return inst;
}

std::string ChinesePyTable::getPinYin(uint16_t ucode, bool isCommonUsedChar)
{
	std::string py;

	std::string query = fmt::format("select pinyin from hanzi where unicode = {};", ucode);

	IResult* res = g_db.ExcuteSelect(query.c_str());
	
	do {

		if (!res) {
			fmt::print(stderr, "No item found of unicode {}", ucode);
			return "";
		}

		if (!res->Next()) break;

		py = res->ColomnData(0);
		
		if (py.empty()) break;

		// 可能存在多个读音, 只取第一个

		const char* pos = strchr(py.c_str(), ',');

		if (pos) py = py.substr(0, pos - py.c_str());

	} while (0);

	res->Release();

	return py;
}
