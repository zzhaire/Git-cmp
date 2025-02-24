#define _CRT_SECURE_NO_WARNINGS
#include"my_common.h"
#include"cmd_console_tools.h"
using namespace std;
/***********************************获取字符/数字**************************************************/
int  getRandomInt(int low, int high, const bool new_seed, const unsigned int seed)
{
	if (new_seed)
		srand(seed);
	return low + rand() % (high - low) + 1;
}
char getInputChar(int low, int high, bool ignore_case, char* except_list)
{
	int ch, flag;
	while (1)
	{
		flag = 0;
		ch = _getche();
		if (isalpha(ch))
		{
			if (ignore_case)
				ch = upcase(ch);
			if (ch <= high && ch >= low)
				break;
			if (except_list != NULL)
			{
				for (char* c = except_list; *c != 0; c++)
					if (*c == ch)
						flag = 1;
			}
			if (flag)
				break;
		}
		cout << "输入错误请重新输入" << endl;
	}
	return ch;
}
int  getInputNum(int low, int high)
{
	int num;
	while (1)
	{
		cin >> num;
		if (cin.good() && num >= low && num <= high)
			break;
		else
		{
			cin.clear();
			cin.ignore(265, '\n');
			int now_x, now_y;
			cct_getxy(now_x, now_y);
			cout << "输入错误请重新输入" << endl;
			//sleepLevel(0);
			//cct_showstr(now_x, now_y, "                           ");
		}
	}
	return num;
}
void getHiddenInput(char* password, int maxLength, char cover_kind) {
	int i = 0;
	unsigned char ch;

	while ((ch = _getch()) != '\r' && i < maxLength - 1) {
		if (ch == '\b' && i > 0) {
			cout << "\b \b"; // 删除字符并退格
			i--;
		}
		else if (isprint(ch)) {
			password[i++] = ch;
			switch (cover_kind) {
			case NO_COVER:
				cout << ch;
				break;
			case HIDDEN:
				break;
			default:
				cout << cover_kind;
				break;
			}
		}
	}
	password[i] = '\0'; // 添加字符串终止符
}
bool validHex(string buf, int wid) {
	if (buf.length() != wid)
		return false;
	for (int i = 0; i < wid; i++)
	{
		if (!isxdigit(buf[i]))
			return false;
	}
	return true;
}
bool isInt(string s)
{
	istringstream ss(s);
	int test_value;
	ss >> test_value;
	return !ss.fail() && ss.eof();
}
bool isDouble(string s)
{
	istringstream ss(s);
	double testValue;
	ss >> testValue;
	return !ss.fail() && ss.eof();
}
void strncopy(char* d, const char* s, int n)
{
	int i, len = min(n, (int)strlen(s));
	if (len == 0)
		return;
	for (i = 0; i < min(n, (int)strlen(s)); i++)
		d[i] = s[i];
	d[len] = '\0';
}
/***********************************大小写转换 ****************************************************/
char* upcase(char* str)
{
	if (!str)
		return 0;
	char* p = str;
	while (*p)
	{
		if (*p >= 'a' && *p <= 'z')
			*p -= 32;
		p++;
	}

	return str;
}
char upcase(char ch)
{
	if (ch >= 'a' && ch <= 'z')
		ch -= 32;
	return ch;
}
char lowcase(char ch)
{
	if (ch >= 'A' && ch <= 'Z')
		ch += 32;
	return ch;
}
char* lowcase(char* str)
{
	if (!str)
		return 0;
	char* p = str;
	while (*p)
	{
		if (*p >= 'A' && *p <= 'Z')
			*p += 32;
		p++;
	}
	return str;
}
/*************************************输出类函数******************************************************/
int  menu(int (*opt_fun)(), int kind)
{
	int choose, opt;
	cout << "-----------------------------------" << endl;
	opt = opt_fun();
	if (kind == NUM_OPT)
		cout << "0.退出" << endl;
	else
		cout << "Q.退出" << endl;
	cout << "-----------------------------------" << endl;
	cout << "[请选择:] ";
	if (kind == NUM_OPT)
		choose = getInputNum(0, opt);
	else if (kind == CHAR_OPT)
	{
		char str[] = "Qq";
		choose = (int)getInputChar('A', opt, true, str);
	}

	cout << endl << endl;
	return choose;
}
void endTips(int coord_x, int coord_y)
{
	cct_gotoxy(coord_x, coord_y);
	cct_showstr(coord_x, coord_y, "本小题结束，请输入End继续:");
	while (1)
	{
		char tips[256];
		getHiddenInput(tips, 256, NO_COVER);
		upcase(tips);
		if (!strcmp(tips, "END"))
		{
			cct_cls();
			break;
		}
		else
			cct_showstr(coord_x, coord_y, "输入错误，请重新输入END:   ");
	}
}
void getPrefix(char* procname, const char* argv0)
{
	int i = strlen(argv0) - 1;
	string des_name = "";
	while (i >= 0) // 参数长度读完也会终止
	{
		if (argv0[i] == '\\' || argv0[i] == '/') // 读到分割符停止
			break;
		else
		{
			des_name += argv0[i];
			--i;
		}
	}
	reverse(des_name.begin(), des_name.end()); // 从后向前读，所以需要反转
	des_name.copy(procname, des_name.length());
	procname[des_name.length()] = '\0';
}
void cheakMalloc(void* ptr, const char* tips)
{
	if (ptr == NULL)
	{
		cout << tips << endl;
		exit(-1);
	}
}
/*************************************色块**********************************************************/
void drawBlock(char kind, int coord_x, int coord_y, int bg_color, int char_color)
{

	switch (kind)
	{
	case'x':
	case 'X':
		cct_showstr(coord_x - 3, coord_y - 1, "╔═╗", bg_color, char_color);
		cct_showstr(coord_x - 3, coord_y, "║XX║", bg_color, char_color);
		cct_showstr(coord_x - 3, coord_y + 1, "╚═╝", bg_color, char_color);
		break;
	case'0':
		cct_showstr(coord_x - 3, coord_y - 1, "      ", bg_color);
		cct_showstr(coord_x - 3, coord_y, "      ", bg_color);
		cct_showstr(coord_x - 3, coord_y + 1, "      ", bg_color);
		break;
	case'S':
	case's':
		cct_showstr(coord_x - 3, coord_y - 1, "╔═╗", bg_color, char_color);
		cct_showstr(coord_x - 3, coord_y, "║★║", bg_color, char_color);
		cct_showstr(coord_x - 3, coord_y + 1, "╚═╝", bg_color, char_color);
		break;
	case'c':
	case'C':
		cct_showstr(coord_x - 3, coord_y - 1, "╔═╗", bg_color, char_color);
		cct_showstr(coord_x - 3, coord_y, "║●║", bg_color, char_color);
		cct_showstr(coord_x - 3, coord_y + 1, "╚═╝", bg_color, char_color);
		break;
	case'm':
	case'M':
		cct_showstr(coord_x - 3, coord_y - 1, "╔═╗", bg_color, char_color);
		cct_showstr(coord_x - 3, coord_y, "║♀║", bg_color, char_color);
		cct_showstr(coord_x - 3, coord_y + 1, "╚═╝", bg_color, char_color);
		break;
	default:
		break;

	}

}
/*************************************文件对比*****************************************************/
bool cmpFiles(string file1_name, string file2_name)
{
	ifstream file1(file1_name, ios::in | ios::binary);
	ifstream file2(file2_name, ios::in | ios::binary);

	char byte1, byte2;
	int cnt = 0;
	while (file1.get(byte1) && file2.get(byte2)) {
		cnt++;
		if (byte1 != byte2) {
			cerr << "文件不同.第" << cnt << endl;
			return 0;
		}
	}
	if (file1.eof() && file2.eof())
		return 1;
	else
		return 0;
}
bool isBlank(char ch)
{
	return (ch == '\t' || ch == ' ');
}
bool isCR(char ch)
{
	return (ch == '\n');
}
int have(const char* buf, char ch)
{
	for (int i = 0; i < (int)strlen(buf); i++)
		if (buf[i] == ch)
			return 1;
	return 0;
}
int have(const vector<string>& items, const string& seg)
{
	return find(items.begin(), items.end(), seg) != items.end();
}
int split(const char* buf, char ch, char* buf1, char* buf2) {
	const char* ch_ptr = strchr(buf, ch);
	if (ch_ptr == NULL) {
		return 0;
	}
	int index = ch_ptr - buf;
	strncpy(buf1, buf, index);
	buf1[index] = '\0';
	strcpy(buf2, ch_ptr + 1);
	return 1;
}
void trimSpace(char* str) {
	// 去除两端空格
	int len = strlen(str);
	int start = 0;
	while (isBlank(str[start]) && start < len) {
		start++;
	}
	int end = len - 1;
	while (isBlank(str[end]) && end > start) {
		end--;
	}
	int i, j;
	for (i = start, j = 0; i <= end; i++, j++) {
		str[j] = str[i];
	}
	str[j] = '\0';
}
bool ignoreComment(char* str, const char* sign)
{
	char* comment_start = strstr(str, sign);
	if (comment_start != NULL) {
		*comment_start = '\0';
		return 1;
	}
	return 0;
}
bool cmpNocaseString(string s1, string s2)
{
	if (s1.length() != s2.length())
		return false;
	for (int i = 0; i < (int)s1.length(); i++)
	{
		if (upcase(s1[i]) != upcase(s2[i]))
			return false;
	}
	return true;
}
bool isUft8File(ifstream& infile)
{
	infile.clear();
	infile.seekg(0, ios::beg);
	unsigned char ch;
	bool allASCII = true;
	while (1) {
		ch = infile.get();
		if (infile.eof())
			break;
		if ((ch & 0x80) == 0) //1字节
			;
		else if ((ch & 0xe0) == 0xc0) { //2字节，110xxxxx
			ch = infile.get();
			if (infile.eof()) {
				return false;
			}
			if ((ch & 0xc0) != 0x80) {
				return false; //错误，第二个应为10xxxxxx
			}
			allASCII = false;
		}
		else if ((ch & 0xf0) == 0xe0) { //3字节，1110xxxx
			for (int i = 0; i < 2; i++) {
				ch = infile.get();
				if (infile.eof()) {
					return false;
				}
				if ((ch & 0xc0) != 0x80) {
					return false; //错误，第二个应为10xxxxxx
				}

			}
			allASCII = false;
		}
		else if ((ch & 0xf8) == 0xf0) { //4字节，11110xxx
			for (int i = 0; i < 3; i++) {
				ch = infile.get();
				if (infile.eof()) {
					return false;
				}
				if ((ch & 0xc0) != 0x80) {
					return false; //错误，第二个应为10xxxxxx
				}
			}
			allASCII = false;
		}
		else {
			return false;
		}
	}

	if (allASCII) //全都是ascii码字符也被认为不是utf8
		return false;
	else
		return true;
}
int isComment(const char* str)
{
	int len = strlen(str);
	if (len < 2)
		return 0;
	if (str[0] == '/' && str[1] == '/')
		return LCOMMENT; // 行注释
	if (str[0] == '/' && str[1] == '*' && str[len - 2] == '*' && str[len - 1] == '/')
		return BCOMMENT;
	else if (str[0] == '/' && str[1] == '*' && !(str[len - 2] == '*' && str[len - 1] == '/'))
		return WRONG_BCOMMENT;
	return 0;
}
int splitSpace(string str, vector<string>& ret)
{
	istringstream ss(str);
	string seg;
	while (ss >> seg) {
		ret.push_back(seg);
	}
	return ret.size();

}
void getBcommentVal(char* str)
{
	int len = strlen(str);
	string ret;
	ret.assign(str);
	ret = ret.substr(2, len - 4);
	ret += '\0';
	strcpy(str, ret.c_str());
}
void getLcommentVal(char* str)
{
	int len = strlen(str);
	char buf[1024];
	memset(buf, 0, sizeof buf);
	strcpy(buf, str + 2);
	memset(str, 0, sizeof len);
	strcpy(str, buf);
}
bool endsWith(const std::string& str, const std::string& suffix) {
	if (str.length() >= suffix.length()) {
		return (0 == str.compare(str.length() - suffix.length(), suffix.length(), suffix));
	}
	else {
		return false;
	}
}
int fileSize(ifstream& ff)
{
	ff.clear();
	ff.seekg(0, ios::end);
	int size = (int)ff.tellg();
	ff.seekg(0, ios::beg);
	return size;
}