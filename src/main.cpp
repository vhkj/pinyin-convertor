#include <stdio.h>
#include <string>
#include <vector>
#include <fmt/core.h>
#include "Helper.h"
#ifdef __GNUC__
#include <getopt.h>
#include <unistd.h>
#else
#include "getopt_win.h"
#endif

#define VERSION "1.0"

/// <summary>
/// 工作模式
/// </summary>
/// 
enum class Mode {
	InvalidMode,

	/// <summary>
	/// 输出组合单词的全拼, -l 指定的文件中的单词在前
	/// </summary>
	CombineComplete,

	/// <summary>
	/// 将指定字典的每行中文转换为拼音
	/// </summary>
	ConvertMode,
};

static void usage(const char * bin)
{
	fmt::print(stdout, "\nusage: {} [option] \nOptions:\n", bin);
	fmt::print(stdout, "-1\tCombine two dictionary. Output PinYin of combined chinese words. Must set -l and -r option.\n");
	fmt::print(stdout, "-2\tConvert specified Chinese dictionary to PinYin dictionary. Must set -i option.\n\n");
	fmt::print(stdout, "-C\tCapital the first character of all words.\n");
	fmt::print(stdout, "-F\tOnly output the first character of all words.\n");
	fmt::print(stdout, "-w\tOnly Convert input Chinese text to PinYin like and then quit.\n");
	
	fmt::print(stdout, "-l\tSet the left side dictionary\n");
	fmt::print(stdout, "-r\tSet the right side dictionary\n");

	fmt::print(stdout, "-i\tSet input file to convert\n");
	fmt::print(stdout, "-o\tSet output file. Default is out.dict\n");
	fmt::print(stdout, "--help\tShow this message\n\n");
	//fmt::print(stdout, "--version\tShow version of this application\n\n");
}

static const option s_long_options[] = {
	{"help",			no_argument,		0, 'h'},
	{"version",			no_argument,		0, 'v'},
};

int main(int argc, char** argv)
{
	int opt, optidx;

	std::string foutput = "out.dict";
	std::string finput;
	std::string fleft;
	std::string fright;
	std::string text;
	
	Mode mode = Mode::InvalidMode;
	bool onlyFirstChar = false;
	bool capitalFirstChar = false;

	while ((opt = getopt_long(argc, argv, "12Chi:l:o:r:vw:", s_long_options, &optidx)) != -1) {
		switch (opt) {
		case '1': mode = Mode::CombineComplete; break;
		case '2': mode = Mode::ConvertMode; break;
		case 'C': capitalFirstChar = true; break;
		case 'F': onlyFirstChar = true; break;
		case 'h': usage(argv[0]); std::quick_exit(0);
		case 'i': finput = optarg; break;
		case 'l': fleft = optarg; break;
		case 'r': fright = optarg; break;
		case 'v': fmt::print(stdout, VERSION "\n"); std::quick_exit(0);
		case 'w': text = optarg; break;
		case 'o': foutput = optarg; break;
		default: usage(argv[0]); std::quick_exit(0);
		}
	}

	if (mode == Mode::InvalidMode && text.empty()) {
		fmt::print(stderr, "Error: you may need to set one of the modes(-1 or -2 or -w to test input words)\n");
		usage(argv[0]);
		std::quick_exit(-1);
	}

	/* convert Chinese characters to PinYin from files */

	try {

		if (!text.empty()) {
			std::u16string in = Helper::gb2312ToU16(text);
			std::string pinyin = onlyFirstChar? Helper::TransToFirstLetterLower(in): Helper::TransToLetterLower(in, capitalFirstChar);
			fmt::print(stdout, "{}: {}\n", text, pinyin);
			exit(pinyin.empty()? -1: 0);
		}

		switch (mode)
		{
		case Mode::CombineComplete:
			Helper::WriteResult(fleft, fright, foutput, onlyFirstChar, capitalFirstChar);
			break;
		case Mode::ConvertMode:
			if (finput.empty()) {
				fmt::print(stderr, "Error: You must set inputfile with option -i");
				exit(-1);
			}
			Helper::WriteResult(finput, foutput, onlyFirstChar, capitalFirstChar);
			break;
		default: break;
		}

	}
	catch (const std::exception& e) {
		fmt::print(stderr, "{}\n", e.what());
		std::exit(-1);
	}

	return 0;
}