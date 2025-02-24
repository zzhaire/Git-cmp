#pragma once
#include <iostream>
#include <conio.h>
#include <Windows.h>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include <vector>
using namespace std;

const int NUM_OPT = 1;
const int CHAR_OPT = 0;
const int HIDDEN = 0;
const int NO_COVER = 1;
const int LCOMMENT = 1;
const int BCOMMENT = 2;
const int WRONG_BCOMMENT = 3;
int  getRandomInt(int low, int high, const bool new_seed = false, const unsigned int seed = 0);
int  getInputNum(int low, int high);
char getInputChar(int low, int high, bool ignore_case = true, char* except_list = NULL);
void getHiddenInput(char* password, int maxLength, char cover_kind);
bool validHex(string buf, int wid);
bool isInt(string num);
bool isDouble(string num);
void strncopy(char* d, const char* s, int n);
char* upcase(char* str);
char upcase(char ch);
char* lowcase(char* str);
char lowcase(char ch);
int  menu(int (*opt_fun)(), int kind = NUM_OPT);
void endTips(int coord_x, int coord_y);
void getPrefix(char* procname, const char* argv0);
void cheakMalloc(void* ptr, const char* tips);
void drawBlock(char kind, int coord_x, int coord_y, int bg_color = 0, int char_color = 7);
bool cmpFiles(string file1_name, string file2_name);
bool isBlank(char ch);
bool isCR(char ch);
int have(const char* buf, char ch);
int have(const vector<string>& items, const string& seg);
int split(const char* buf, char ch, char* buf1, char* buf2);
void trimSpace(char* str);
bool ignoreComment(char* str, const char* sign);
bool cmpNocaseString(string s1, string s2);
bool isUft8File(ifstream& infile);
int  isComment(const char* str);
int splitSpace(string str, vector<string>& ret);
void getBcommentVal(char* str);
void getLcommentVal(char* str);
bool endsWith(const std::string& str, const std::string& suffix);
int fileSize(ifstream& ff);