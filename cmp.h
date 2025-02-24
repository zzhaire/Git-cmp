#pragma once
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <sstream>
#include "cmd_console_tools.h"
#define _CRT_SECURE_NO_WARNINGS
using namespace std;
const int N = 200;//最大错误显示数量
const int buf = 256; //缓冲区大小
const int MAX_buf = 70000;
const string STATE[] = { "null","<EOF>","<CR>","NotEmpty" }; //结尾的状态
enum         state { null, eof, cr, not_empty };	//结尾的状态

class Cmp {
public:
	string trim_type;
	int lineskip;
	int lineoffset;
	bool ignore_blank;
	bool CR_CRLF_not_equal;
	int max_diff;
	int max_line;
	string display;
private:
	ifstream file1;
	ifstream file2;
	int same = 1;
	char line1_buf[MAX_buf];
	char line2_buf[MAX_buf];
	char line1_hex[MAX_buf];
	char line2_hex[MAX_buf];
	char line1_dec[MAX_buf];
	char line2_dec[MAX_buf];
	bool diff_pos[MAX_buf];
	int file1_now_line;
	int file2_now_line;
	state state1;
	state state2;
	int len1;
	int len2;
	bool color = 0;
	int diff_cnt = 0;
	int width = 0;
	int line_maxlen = 0;
	int line_diff_idx = -1;
public:
	Cmp();
	int  getCmp();
	void clean(char* buf, char ch);
	void openFile(string file1_name, string file2_name);
	void closeFile();
	void fileCmp();
	void lineToHex(const char* source, char* destnation, state s);
	void lineToDec(const char* source, char* destnation, state s);
	void printRuler(int n);
	bool emptyLine(const char* buf);
	bool emptyLine(const char ch); // 重载一个判断第一个是不是换行符
	int  fileLineOffset();
	int  fileLineSkip();
	bool isCR(char ch);
	bool isBlank(char ch);
	void trimGetline(ifstream& file, char* line_buf, state& s);
	void errorMsg();
	void printTips(int kind);
	void printHex();
	void printDec();
	int  getDiffer();
	void setColor(bool fill);
	void tipMsg(int limit = 0);
	int  cmpLine();
	bool isVisable(char ch);
	void updateWid(int n);
	void printSepLine(int tty = 0);
	void getWid();
	void getFileLine1();
	void getFileLine2();
};
