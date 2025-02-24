#define _CRT_SECURE_NO_WARNINGS
#include "cmp.h"
#include "cmd_console_tools.h"
using namespace std;
Cmp::Cmp()
{

	/* �ⲿָ�� ��ʼ��*/
	trim_type = "none";
	lineskip = 0;
	lineoffset = 0;
	ignore_blank = 0;
	CR_CRLF_not_equal = 0;
	max_diff = 0;
	max_line = 0;
	display = "none";
	/* �Լ������ȫ�ֱ�����ʼ�� */
	file1_now_line = 0;
	file2_now_line = 0;
	state1 = null;
	state2 = null;
	width = -1;
	memset(line1_buf, 0, sizeof line1_buf);
	memset(line2_buf, 0, sizeof line2_buf);
	memset(line1_hex, 0, sizeof line1_hex);
	memset(line2_hex, 0, sizeof line2_hex);
	memset(line1_dec, 0, sizeof line1_dec);
	memset(line2_dec, 0, sizeof line2_dec);
	memset(diff_pos, 1, sizeof diff_pos);
}
int  Cmp::getCmp()
{
	return same;
}
void Cmp::clean(char* buf, char ch)
{
	memset(buf, ch, MAX_buf);
}
void Cmp::openFile(string file1_name, string file2_name)
{
	file1.open(file1_name, ios::in | ios::binary);
	file2.open(file2_name, ios::in | ios::binary);
	if (!file1.is_open()) {
		cerr << "��һ���ļ�[" << file1_name << "]�޷���." << endl;
		exit(1);
	}
	if (!file2.is_open()) {
		cerr << "�ڶ����ļ�[" << file2_name << "]�޷���." << endl;
		exit(1);
	}
}
void Cmp::closeFile()
{
	file1.close();
	file2.close();
}
void Cmp::fileCmp()
{
	bool EOF_CR = 0;
	getWid();
	this->fileLineOffset();
	this->fileLineSkip();
	if (display != "none") {
		cout << "�ȽϽ�������" << endl;
		printSepLine();
	}
	int limited = 0, line_num = 0;
	while (!(state1 == eof && state2 == eof)) {

		getFileLine1(), getFileLine2();
		line_num++;
		int cmp_res = cmpLine();
		if (cmp_res == 0) {
			same = 0, diff_cnt++;
			errorMsg();
		}
		else  //��Ҫ���������ж�
		{
			if (max_line && line_num >= max_line)
				break;
			if (cmp_res == 2)
				continue;
		}
		if (max_diff && diff_cnt >= max_diff) {
			limited = 1;
			break;
		}

		if (state1 == eof || state2 == eof)
			break;
	}

	if (!same)
	{
		if (display == "none")
			cout << "�ļ���ͬ." << endl;
		else
			tipMsg(limited);
	}
	else
	{
		if (display == "none")
			cout << "�ļ���ͬ." << endl;
		else {
			cout << "��ָ�������������ȫһ��." << endl;
			printSepLine();
		}
	}
	//cct_gotoxy(begin_x, begin_y), printSepLine();
}
void Cmp::trimGetline(ifstream& file, char* line_buf, state& s)
{
	// 2���ƶ�,���з�\nһ��������ȥ,\0����,��λ��line_buf[len-1]=='\r'�Ƿ�Ҫ����ȥȡ����CR_CRLF_not_equal
	char byte = '\0';
	int idx = 0;
	// �����Ƿ��ǿ���, /r /n ���� 
	clean(line_buf, '\0');
	if (trim_type == "left" || trim_type == "all")
	{
		while (file.get(byte) && !file.eof() && isBlank(byte))
			;
		if (!isCR(byte) && !file.eof()) {
			line_buf[idx++] = byte;
		}
		else if (file.eof())
			s = eof;
		else if (isCR(byte))
			s = cr;
	}

	while (file.get(byte) && !isCR(byte) && !file.eof()) {
		line_buf[idx++] = byte;
	}
	if (file.eof())
		s = eof;
	else if (isCR(byte))
		s = cr;

	if (!CR_CRLF_not_equal)
	{
		// ���� /r /n �Ľ�β
		if (line_buf[idx - 1] == '\r' && s != eof)
			line_buf[--idx] = '\0';
	}

	if (trim_type == "right" || trim_type == "all")
	{
		while (idx > 0 && isBlank(line_buf[idx - 1]))
			line_buf[--idx] = '\0';
		idx++;
	}
	line_buf[idx] = '\0';
}
void Cmp::lineToHex(const char* source_buf, char* dis_buf, state stat) {
	int idx = 0;
	int pos = 0;
	char s[64] = { 0 };
	clean(dis_buf, '\0');
	if (!strlen(source_buf)) {
		string str = (stat == eof ? "<EOF>" : "<EMPTY>");
		sprintf(dis_buf, str.c_str());
		return;
	}

	// ʣ�µ���֮ǰ��16����ת��;
	while (1) {
		int ch = source_buf[idx++];
		if (ch == '\0')
			break;

		if (!(pos % 16)) {
			if (pos)
				dis_buf += sprintf(dis_buf, "  %s\n", s);
			dis_buf += sprintf(dis_buf, "%08x :", pos);
		}

		if (pos % 16 == 8)
			dis_buf += sprintf(dis_buf, " -");

		dis_buf += sprintf(dis_buf, " %02x", (unsigned char)ch);

		if (ch >= 32 && ch <= 126)
			s[pos % 16] = ch;
		else
			s[pos % 16] = '.';

		pos++;
	}

	if (pos % 16 == 0 || !pos) {
		if (pos)
			dis_buf += sprintf(dis_buf, "  %s", s);
		//dis_buf += sprintf(dis_buf, "\n");
		return;
	}

	while (pos % 16) {
		dis_buf += sprintf(dis_buf, "   ");
		if (pos % 16 == 8)
			dis_buf += sprintf(dis_buf, "  ");
		s[pos % 16] = '\0';
		pos++;
	}

	dis_buf += sprintf(dis_buf, "  %s", s);
}
void Cmp::lineToDec(const char* source_buf, char* dis_buf, state stat)
{
	/*if (!strlen(source_buf))
	{
		if (stat != eof) {
			sprintf(dis_buf, "<EMPTY>");
			return;
		}
	}*/
	clean(dis_buf, '\0');
	sprintf(dis_buf, source_buf);
}
bool Cmp::emptyLine(const char* buf)
{
	return buf[0] == '\0';
}
bool Cmp::emptyLine(const char ch)
{
	if (CR_CRLF_not_equal)
		return (ch == '\n');
	else
		return (ch == '\r'); //windows�¾��ǻ���
}
int  Cmp::fileLineOffset()
{
	// �����Ƿ񵽴��ļ�β,1:����,0:δ����
	char byte = '\0';
	bool is_eof = 0;
	if (lineoffset < 0) // ƫ���ļ�1
	{
		int linecnt = -lineoffset;
		for (int i = 0; i < linecnt && !is_eof; i++)
		{
			getFileLine1();
			if (state1 == eof) {
				if (file2.eof())
					same = 1;
				else
					same = 0;
				return 1;
			}
		}
	}
	else                // ƫ���ļ�2
	{
		int linecnt = lineoffset;
		for (int i = 0; i < linecnt && !is_eof; i++)
		{
			getFileLine2();
			if (state2 == eof) {
				if (file1.eof())
					same = 1;
				else
					same = 0;
				return 1;
			}
		}
	}
	return 0;
}
int  Cmp::fileLineSkip()
{
	bool is_eof = 0;
	//�����Ƿ񵽴��ļ�β 1:����  0: δ����
	for (int i = 0; i < lineskip && !is_eof; i++)
	{
		getFileLine1(), getFileLine2();
		if (state1 == eof && state2 == eof) {
			same = 1;
			return 1;
		}
		else if (state1 == eof || state2 == eof) // ��һ��������ͬ
		{
			same = 0;
			return 1;
		}
	}
	return 0;
}
bool Cmp::isCR(char ch)
{
	return (ch == '\n');
}
bool Cmp::isBlank(char ch)
{
	//if (CR_CRLF_not_equal) // ������ /r
	return (ch == ' ' || ch == '\t');
	//else
	//	return (ch == ' ' || ch == '\t');
}
void Cmp::errorMsg()
{
	if (display != "none")
	{
		lineToDec(line1_buf, line1_dec, state1);
		lineToDec(line2_buf, line2_dec, state2);
		int kind = getDiffer();
		cout << "��[" << file1_now_line << " / " << file2_now_line << "]�� - ";
		if (display == "detailed")
		{
			lineToHex(line1_buf, line1_hex, state1);
			lineToHex(line2_buf, line2_hex, state2);

			printTips(kind);
			printRuler(max(len1, len2));
			updateWid(max(len1, len2));
			printDec();
			printHex();
		}

		if (display == "normal")
		{
			updateWid(max(len1, len2));
			printTips(kind);
			printDec();
		}
	}
}
void Cmp::printTips(int kind)
{
	switch (kind)
	{
	case 1:
		cout << "�ļ�1��������/�ļ�2�ѽ�����" << endl;
		break;
	case -1:
		cout << "�ļ�1�ѽ���/�ļ�2�������ݣ�" << endl;
		break;
	case 2:
		cout << "�ļ�1��β���ж����ַ���" << endl;
		break;
	case -2:
		cout << "�ļ�2��β���ж����ַ���" << endl;
		break;
	case 0:
		cout << "��[" << line_diff_idx << "]���ַ���ʼ�в��죺" << endl;
		break;
	default:
		break;
	}
}
void Cmp::printRuler(int n)
{
	int wid = (n / 10 + 1);

	// =============================================....
	cout << setfill(' ') << setw(8) << ' ' << setw(wid * 10) << setfill('-') << '-' << setfill(' ');
	// 0         1         2         3              ....
	cout << endl << left << setw(8) << ' ';
	for (int i = 0; i < wid; i++)
		cout << setw(10) << i % 10;
	// 0123456789012345678901234567890123456789     ....
	cout << endl << setw(8) << ' ';
	for (int i = 0; i < wid; i++)
		cout << "0123456789";
	// =============================================....
	cout << endl << setw(8) << ' ';
	cout << setw(wid * 10) << setfill('-') << '-';
	cout << endl;
}
void Cmp::printHex()
{
	cout << "�ļ�1(HEX) : \n" << line1_hex << endl;
	cout << "�ļ�2(HEX) : \n" << line2_hex << endl;
	cout << endl;
}
void Cmp::printDec()
{
	//setCR();
	cout << "�ļ�1 : ";
	if (len1 == 0 && state1 != eof) {
		setColor(1);
		cout << "<EMPTY>";
	}
	for (int i = 0; i < len1; i++)
	{
		if (!diff_pos[i]) {
			setColor(1);
			if (isVisable(line1_dec[i]))
				cout << line1_dec[i];
			else
				cout << 'X';
		}
		else
		{
			setColor(0);
			if (isVisable(line1_dec[i]))
				cout << line1_dec[i];
			else
				cout << 'X';
		}
	}
	setColor(0);
	cout << STATE[(int)state1] << endl;
	cout << "�ļ�2 : ";
	if (len2 == 0 && state2 != eof) {
		setColor(1);
		cout << "<EMPTY>";
	}
	for (int i = 0; i < len2; i++)
	{
		if (!diff_pos[i]) {
			setColor(1);
			if (isVisable(line2_dec[i]))
				cout << line2_dec[i];
			else
				cout << 'X';
		}
		else
		{
			setColor(0);
			if (isVisable(line2_dec[i]))
				cout << line2_dec[i];
			else
				cout << 'X';
		}
	}
	setColor(0);
	cout << STATE[(int)state2] << endl;
	if (display == "normal")
		cout << endl;
}
int  Cmp::getDiffer()
{
	int ret = 5;// ��ö���� 1 -1 2 -2 0 ,����Щ��һ������
	int l1 = strlen(line1_dec), l2 = strlen(line2_dec);

	clean((char*)diff_pos, 1);
	int i;
	line_diff_idx = -1;
	for (i = 0; i < min(l1, l2); i++)
	{
		if (line1_dec[i] != line2_dec[i]) {
			ret = 0, diff_pos[i] = 0;
			if (line_diff_idx == -1)
				line_diff_idx = i;
		}
	}
	while (i < l1) {
		diff_pos[i++] = 0;
		if (ret != 0)
			ret = 2;
	}
	while (i < l2)
	{
		diff_pos[i++] = 0;
		if (ret != 0)
			ret = -2;
	}

	if (ret != 5)
		return ret;

	if (state1 == eof && state2 != eof)
		return -1;
	if (state1 != eof && state2 == eof)
		return 1;

	return ret;
}
void Cmp::setColor(bool fill)
{
	if (fill && !color) {
		cct_setcolor(COLOR_HYELLOW, COLOR_RED), color = 1;
		return;
	}
	else if (!fill && color)
	{
		cct_setcolor(), color = 0;
		return;
	}
}
void Cmp::tipMsg(int limit)
{

	printSepLine();
	cout << setw(0) << "��ָ����������¹�" << diff_cnt << "���в���";
	if (limit)
		cout << "[�ѵ��趨��������ֵ]";
	cout << "." << endl;
	cout << setw(0) << "�Ķ���ʾ��" << endl;
	cout << "\t" << "1��������<EMPTY>���" << endl;
	cout << "\t" << "2���ļ�������<EOF>���" << endl;
	cout << "\t" << "3��������ͬ��λ�õĲ����ַ�����ɫ���" << endl;
	cout << "\t" << "4��ÿ���е�CR/LF/VT/BS/BEL��X���(���㿴�������ַ�)" << endl;
	cout << "\t" << "5��ÿ��β�Ķ�����ַ�����ɫ�����CR/LF/VT/BS/BEL����ɫX���(���㿴�������ַ�)" << endl;
	cout << "\t" << "6��ÿ�������<CR>���(���㿴�������ַ�)" << endl;
	cout << "\t" << "7��������Ϊ�������⣬����λ�ÿ��ܱ��ں��������ϣ����������ֶ���ɫ���" << endl;
	if (display != "detailed")
		cout << "\t" << "8����--display detailed���Եõ�����ϸ����Ϣ" << endl;
	printSepLine();
}
int  Cmp::cmpLine()
{
	int i;
	bool ret = 1;
	len1 = strlen(line1_buf), len2 = strlen(line2_buf);
	if (len1 != len2)
		return 0;
	for (i = 0; i < max(len1, len2); i++)
	{
		if (line1_buf[i] != line2_buf[i]) {
			return 0;
		}
	}
	//if (!ignore_blank) 
	if ((int)state1 != (int)state2)
	{
		if (!ignore_blank || trim_type == "none")
			return 0; // ��һ��ֱ�ӷ���
		else	// ��Ϊ�����пո�Ҫ�����������
		{
			if (state1 == eof) {
				clean(line1_buf, '\0');// �´ζ������൱���ǿյ�
			}
			else if (state2 == eof) {
				clean(line2_buf, '\0');
			}
			return 2;
		}
	}
	return 1;

}
bool Cmp::isVisable(char ch)
{
	if (ch >= 32 && ch <= 126 || ch == '\t') // \t ��ȻҲ�ǿɼ���?
		return true;
	else if (ch <= -1) // ����
		return true;
	return false;
}
void Cmp::updateWid(int n)
{
	line_maxlen = max(n, line_maxlen);
	width = (this->line_maxlen / 10 + 1) * 10 + 8 + 2;
	width = (line_maxlen / 10 + 1) * 10 + 8 + 2; //��---�ı�߶�2����line_maxlen����ȡ10��������8��"�ļ�1 : "

	if (this->display == "detailed" && width < 80)
		width = 80;
}
void Cmp::printSepLine(int tty)
{
	cout << setw(width) << setfill('=') << "=" << endl << setfill(' ');
}
void Cmp::getWid()
{
	/*bool is_eof = 0;
	if (this->fileLineOffset())
		is_eof = 1;
	if (this->fileLineSkip())
		is_eof = 1;*/
	string line1, line2;

	while (getline(file1, line1), getline(file2, line2))
	{
		int l1 = line1.length(), l2 = line2.length();
		updateWid(max(l1, l2));
	}

	file1_now_line = 0;
	file2_now_line = 0;
	file1.clear();
	file2.clear();
	file1.seekg(0, ios::beg);
	file2.seekg(0, ios::beg);
	state1 = null;
	state2 = null;
}
void Cmp::getFileLine1()
{
	if (state1 != eof)
		trimGetline(file1, line1_buf, state1), file1_now_line++;
	if (ignore_blank)
	{
		while (emptyLine(line1_buf) && state1 != eof) {
			trimGetline(file1, line1_buf, state1), file1_now_line++;
		}
	}
}
void Cmp::getFileLine2()
{
	if (state2 != eof)
		trimGetline(file2, line2_buf, state2), file2_now_line++;
	if (ignore_blank)
	{
		while (emptyLine(line2_buf) && state2 != eof) {
			trimGetline(file2, line2_buf, state2), file2_now_line++;
		}
	}
}