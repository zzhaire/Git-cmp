#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
#include "class_aat.h"
//如有必要，可以加入其它头文件
using namespace std;

#if !ENABLE_LIB_COMMON_TOOLS //不使用lib才有效

/* ---------------------------------------------------------------
	 允许加入其它需要static函数（内部工具用）
   ---------------------------------------------------------------- */
   // ip字符串转int
static bool cheakIpSeg(const char* seg)
{
	int len = strlen(seg);
	if (len < 1 || len > 3)
		return false;

	for (int i = 0; i < len; i++)
		if (!isdigit(seg[i]))
			return false;

	int num = atoi(seg);

	if (num < 0 || num > 255)
		return false;

	if (len > 1 && seg[0] == '0')
		return false;

	return true;
}
static bool cheakIp(const char* const IP)
{
	char ip[128], seg[4][64];
	strcpy(ip, IP);
	int len = strlen(IP), l = 0, cnt = 0;
	ip[len] = '.', ip[len + 1] = '\0';
	for (int r = 0; r < len + 1; r++)
	{
		if (ip[r] == '.')
		{
			strncpy(seg[cnt], ip + l, r - l);
			seg[cnt][r - l] = '\0';
			if (!cheakIpSeg(seg[cnt]))
				return false;
			l = r + 1;
			cnt++;
		}
		if (cnt > 4)
			return false;
	}
	if (cnt != 4)
		return false;
	return true;
}

static unsigned int ip_to_dec(string s)
{
	unsigned int ret = 0;
	s += '.';
	int n = s.length();
	for (int i = 0; i < n; i++)
	{
		int point = i;
		while (point < n && isdigit(s[point]))
			point++;
		int val = 0;
		for (int j = i; j < point; j++)
			val = val * 10 + s[j] - '0';
		ret <<= 8;
		ret |= val;
		i = point;
	}
	return ret;
}
static int get_wid(int num)
{
	string s = to_string(num);
	return s.length();
}
static int cheak_int(string s, int l = -INF, int r = INF)
{
	int sign = 1, ans = 0;
	for (int i = 0; i < (int)s.length(); i++)
	{
		if (i == 0 && s[i] == '-' && s.length() > 1)
			sign = -1;
		else if (!isdigit(s[i]))
			return NOT_NUM;
	}
	ans = stoi(s);
	if (ans<l || ans>r)
		return OUT_RANGE;
	return ans;
}
//空白
args_analyse_tools::args_analyse_tools()
{
	args_name.clear();
	extargs_type = ST_EXTARGS_TYPE::null;
	extargs_num = 0;
	def_wid = 0;
	set_len = 0;
	val_wid = 0;
	set_default_bool();
	set_default_int();
	set_default_int_set();
	set_default_string();
	set_default_string_set();
	set_default_ip();

}
// bool 型构造函数
args_analyse_tools::args_analyse_tools(const char* name, const ST_EXTARGS_TYPE type, const int ext_num, const bool def)
{
	if (type != ST_EXTARGS_TYPE::boolean)
	{
		cout << "构造函数错误,传入的参数" << TYPE_NAME[(int)type];
		cout << "不是bool" << endl;
		exit(-1);
	}
	set_default_int();
	set_default_int();
	set_default_int_set();
	set_default_string();
	set_default_string_set();
	set_default_ip();
	args_name = name;
	extargs_type = type;
	extargs_num = ext_num;
	extargs_bool_default = def;
	args_existed = 0;
	def_wid = extargs_bool_default ? 4 : 5;//true or false
	val_wid = extargs_bool_default ? 4 : 5;
}
// int_with_default、int_with_error
args_analyse_tools::args_analyse_tools(const char* name, const ST_EXTARGS_TYPE type, const int ext_num, const int def, const int _min, const int _max)
{
	if (type != ST_EXTARGS_TYPE::int_with_default && type != ST_EXTARGS_TYPE::int_with_error)
	{
		cout << "构造函数错误,传入的参数" << TYPE_NAME[(int)type];
		cout << "不是int_with_default或int_with_error" << endl;
		exit(-1);
	}

	set_default_bool();
	set_default_int_set();
	set_default_string();
	set_default_string_set();
	set_default_ip();

	args_name = name;
	extargs_type = type;
	extargs_num = ext_num;
	extargs_int_min = _min;
	extargs_int_max = _max;
	args_existed = 0;
	def_wid = 0;
	val_wid = 0;

	if (type == ST_EXTARGS_TYPE::int_with_default)
		extargs_int_default = def, extargs_int_value = def, def_wid = get_wid(def);

}
//int_with_set_default、int_with_set_error
args_analyse_tools::args_analyse_tools(const char* name, const ST_EXTARGS_TYPE type, const int ext_num, const int def_of_set_pos, const int* const set)
{
	if (type != ST_EXTARGS_TYPE::int_with_set_default && type != ST_EXTARGS_TYPE::int_with_set_error)
	{
		cout << "构造函数错误,传入的参数" << TYPE_NAME[(int)type];
		cout << "不是int_with_set_default或int_with_set_error" << endl;
		exit(-1);
	}

	set_default_bool();
	set_default_int();
	set_default_string();
	set_default_string_set();
	set_default_ip();


	args_name = name;
	extargs_type = type;
	extargs_num = ext_num;
	args_existed = 0;
	def_wid = 0;
	val_wid = 0;
	int n = 0;
	for (const int* p = set; *p != INVALID_INT_VALUE_OF_SET; p++)
		n++;
	extargs_int_set = new(nothrow) int[n + 1];
	if (extargs_int_set == NULL)
		exit(-1);
	memcpy(extargs_int_set, set, (n + 1) * sizeof(int));
	set_len = n;
	if (type == ST_EXTARGS_TYPE::int_with_set_default)
		extargs_int_default = set[def_of_set_pos], def_wid = get_wid(extargs_int_default);

}
//str、ipaddr_with_default、ipaddr_with_error
args_analyse_tools::args_analyse_tools(const char* name, const ST_EXTARGS_TYPE type, const int ext_num, const string def)
{
	if (type != ST_EXTARGS_TYPE::ipaddr_with_default && type != ST_EXTARGS_TYPE::ipaddr_with_error && type != ST_EXTARGS_TYPE::str)
	{
		cout << "构造函数错误,传入的参数" << TYPE_NAME[(int)type];
		cout << "不是str,ipaddr_with_default或ipaddr_with_error" << endl;
		exit(-1);
	}
	set_default_bool();
	set_default_int();
	set_default_int_set();
	set_default_string_set();

	args_name = name;
	extargs_type = type;
	extargs_num = ext_num;
	extargs_string_default = def;
	args_existed = 0;
	val_wid = 0;

	if (type == ST_EXTARGS_TYPE::ipaddr_with_default) {
		extargs_ipaddr_default = ip_to_dec(def);
	}
	def_wid = extargs_string_default.length();
	val_wid = extargs_string_value.length();

}
//str_with_set_default、str_with_set_error
args_analyse_tools::args_analyse_tools(const char* name, const ST_EXTARGS_TYPE type, const int ext_num, const int def_of_set_pos, const string* const set)
{
	if (type != ST_EXTARGS_TYPE::str_with_set_default && type != ST_EXTARGS_TYPE::str_with_set_error)
	{
		cout << "构造函数错误,传入的参数" << TYPE_NAME[(int)type];
		cout << "不是ipaddr_with_default或ipaddr_with_error" << endl;
		exit(-1);
	}
	set_default_bool();
	set_default_int();
	set_default_int_set();
	set_default_string();
	set_default_ip();

	args_name = name;
	extargs_type = type;
	extargs_num = ext_num;
	args_existed = 0;
	val_wid = 0;
	def_wid = 0;

	int n = 0;
	for (const string* p = set; *p != ""; p++)
		n++;
	extargs_string_set = new(nothrow)string[n + 1];
	if (extargs_string_set == NULL)
		exit(-1);

	for (int i = 0; i < n + 1; i++)
		extargs_string_set[i] = set[i];
	set_len = n;
	if (type == ST_EXTARGS_TYPE::str_with_set_default)
		extargs_string_default = set[def_of_set_pos], def_wid = extargs_string_default.length();
}

args_analyse_tools::~args_analyse_tools()
{
	if (extargs_int_set != NULL)
		delete[] extargs_int_set;
	if (extargs_string_set != NULL)
	{
		for (int i = 0; i < set_len + 1; i++)
			extargs_string_set[i].~basic_string();
		delete[] extargs_string_set;
	}
}

/* ---------------------------------------------------------------
	 允许AAT中自定义成员函数的实现（private）
   ---------------------------------------------------------------- */
void args_analyse_tools::set_default_int()
{
	extargs_int_default = INVALID_INT_VALUE_OF_SET;
	extargs_int_min = 1;
	extargs_int_max = -1;
	extargs_int_value = INVALID_INT_VALUE_OF_SET;
}
void args_analyse_tools::set_default_int_set()
{
	set_len = 0;
	extargs_int_set = NULL;
	extargs_int_default = INVALID_INT_VALUE_OF_SET;
}
void args_analyse_tools::set_default_bool()
{
	extargs_bool_default = false;
}
void args_analyse_tools::set_default_string()
{
	extargs_string_default = "";
	extargs_string_value = "";
}
void args_analyse_tools::set_default_string_set()
{
	set_len = 0;
	extargs_string_set = NULL;
	set_default_string();
}
void args_analyse_tools::set_default_ip()
{
	extargs_ipaddr_default = 0;
	extargs_ipaddr_value = 0;
}
bool args_analyse_tools::is_int()const
{
	return
		(
			extargs_type == ST_EXTARGS_TYPE::int_with_default ||
			extargs_type == ST_EXTARGS_TYPE::int_with_error ||
			extargs_type == ST_EXTARGS_TYPE::int_with_set_default ||
			extargs_type == ST_EXTARGS_TYPE::int_with_set_error
			);
}
bool args_analyse_tools::is_string()const
{
	return
		(
			extargs_type == ST_EXTARGS_TYPE::str ||
			extargs_type == ST_EXTARGS_TYPE::str_with_set_default ||
			extargs_type == ST_EXTARGS_TYPE::str_with_set_error
			);
}
bool args_analyse_tools::is_ip()const
{
	return
		(
			extargs_type == ST_EXTARGS_TYPE::ipaddr_with_default ||
			extargs_type == ST_EXTARGS_TYPE::ipaddr_with_error
			);
}
string args_analyse_tools::print_range(bool tips, bool ret)const
{
	char buf[128];
	memset(buf, 0, sizeof buf);
	switch (extargs_type)
	{
	case ST_EXTARGS_TYPE::int_with_default: //int
	case ST_EXTARGS_TYPE::int_with_error:
		if (tips)
			cout << "范围";
		if (!ret)
		{
			cout << "[";
			cout << extargs_int_min << ".." << extargs_int_max;
			cout << "]";
		}
		else
			sprintf(buf, "[%d..%d]", extargs_int_min, extargs_int_max);
		break;
	case ST_EXTARGS_TYPE::int_with_set_default:
	case ST_EXTARGS_TYPE::int_with_set_error:
		if (tips)
			cout << "可取值";
		if (!ret)
		{
			cout << "[";
			for (int i = 0; i < set_len; i++)
			{
				cout << extargs_int_set[i];
				if (i != set_len - 1)
					cout << "/";
			}
			cout << "]";
		}
		else {
			int idx = 0;
			//idx += sprintf(buf + idx, "[");

			for (int i = 0; i < set_len; i++) {
				idx += std::sprintf(buf + idx, "%d", extargs_int_set[i]);

				if (i != set_len - 1) {
					idx += std::sprintf(buf + idx, "/");
				}
			}
			//idx += sprintf(buf + idx, "]");
		}
		break;
	case ST_EXTARGS_TYPE::str_with_set_default:
	case ST_EXTARGS_TYPE::str_with_set_error:
		if (tips)
			cout << "可取值";
		if (!ret)
		{
			cout << "[";
			for (int i = 0; i < set_len; i++)
			{
				cout << extargs_string_set[i];
				if (i != set_len - 1)
					cout << "/";
			}
			cout << "]";
		}
		else {
			int idx = 0;
			//idx += sprintf(buf + idx, "[");

			for (int i = 0; i < set_len; i++) {
				idx += std::sprintf(buf + idx, "%s", extargs_string_set[i].c_str());

				if (i != set_len - 1) {
					idx += std::sprintf(buf + idx, "/");
				}
			}
			//idx += sprintf(buf + idx, "]");
		}
		break;
	default:
		break;
	}
	return buf;
}
string args_analyse_tools::print_default(bool tips, bool ret)const
{
	char buf[128];
	memset(buf, 0, sizeof buf);
	if (tips)
		cout << "缺省:";
	switch (extargs_type)
	{
	case ST_EXTARGS_TYPE::int_with_default:
		//case ST_EXTARGS_TYPE::int_with_error:
	case ST_EXTARGS_TYPE::int_with_set_default:
		//case ST_EXTARGS_TYPE::int_with_set_error:
		if (!ret)
			cout << extargs_int_default;
		else
			sprintf(buf, "%d", extargs_int_default);
		break;
	case ST_EXTARGS_TYPE::str_with_set_default:
	case ST_EXTARGS_TYPE::str_with_set_error:
	case ST_EXTARGS_TYPE::str:
		if (!ret)
			cout << extargs_string_default;
		else
			sprintf(buf, "%s", extargs_string_default.c_str());
		break;
	case ST_EXTARGS_TYPE::boolean:
		if (!ret)
			cout << ((extargs_bool_default) ? "true" : "false");
		else
			sprintf(buf, "%s", ((extargs_bool_default) ? "true" : "false"));
		break;
	case ST_EXTARGS_TYPE::ipaddr_with_default:
		if (!ret)
			cout << extargs_string_default;
		else
			sprintf(buf, "%s", extargs_string_default.c_str());
		break;
	case ST_EXTARGS_TYPE::ipaddr_with_error:
		break;
	default:
		break;
	}
	return buf;
}
string args_analyse_tools::print_value(bool ret)const
{
	char buf[64];
	memset(buf, 0, sizeof buf);
	switch (extargs_type)
	{
	case ST_EXTARGS_TYPE::int_with_default:
	case ST_EXTARGS_TYPE::int_with_error:
	case ST_EXTARGS_TYPE::int_with_set_default:
	case ST_EXTARGS_TYPE::int_with_set_error:
		if (!ret)
			cout << extargs_int_value;
		else
			sprintf(buf, "%d", extargs_int_value);
		break;
	case ST_EXTARGS_TYPE::str_with_set_default:
	case ST_EXTARGS_TYPE::str_with_set_error:
	case ST_EXTARGS_TYPE::str:
		if (!ret)
			cout << extargs_string_value;
		else
			sprintf(buf, "%s", extargs_string_value.c_str());
		break;
	case ST_EXTARGS_TYPE::boolean:
		if (!ret)
			cout << "true";
		else
			sprintf(buf, "%s", "true");
		break;
	case ST_EXTARGS_TYPE::ipaddr_with_default:
	case ST_EXTARGS_TYPE::ipaddr_with_error:
		if (!ret)
			cout << extargs_string_value;
		else
			sprintf(buf, "%s", extargs_string_value.c_str());
		break;
	default:
		break;
	}
	return buf;
}
bool args_analyse_tools::has_range()const
{
	return (
		extargs_type == ST_EXTARGS_TYPE::int_with_default ||
		extargs_type == ST_EXTARGS_TYPE::int_with_error ||
		extargs_type == ST_EXTARGS_TYPE::int_with_set_default ||
		extargs_type == ST_EXTARGS_TYPE::int_with_set_error ||
		extargs_type == ST_EXTARGS_TYPE::str_with_set_default ||
		extargs_type == ST_EXTARGS_TYPE::str_with_set_error
		);
}
bool args_analyse_tools::has_default()const
{
	return !(
		extargs_type == ST_EXTARGS_TYPE::int_with_error ||
		extargs_type == ST_EXTARGS_TYPE::int_with_set_error ||
		extargs_type == ST_EXTARGS_TYPE::str_with_set_error ||
		extargs_type == ST_EXTARGS_TYPE::ipaddr_with_error ||
		(extargs_type == ST_EXTARGS_TYPE::str && extargs_string_default == "")
		);

}
bool  args_analyse_tools::in_set(const ST_EXTARGS_TYPE type)const
{
	if (type == ST_EXTARGS_TYPE::int_with_set_default || type == ST_EXTARGS_TYPE::int_with_set_error)
	{
		for (int i = 0; i < set_len; i++)
			if (extargs_int_value == extargs_int_set[i])
				return true;
	}
	else if (type == ST_EXTARGS_TYPE::str_with_set_default || type == ST_EXTARGS_TYPE::str_with_set_error)
	{
		for (int i = 0; i < set_len; i++)
			if (extargs_string_value == extargs_string_set[i])
				return true;
	}
	return false;
}
void args_analyse_tools::error_tips(bool show_range, bool show_default)const
{
	cout << "(类型:";
	switch (extargs_type)
	{
	case ST_EXTARGS_TYPE::null:
		break;
	case ST_EXTARGS_TYPE::boolean:
		break;
	case ST_EXTARGS_TYPE::int_with_default:
	case ST_EXTARGS_TYPE::int_with_error:
	case ST_EXTARGS_TYPE::int_with_set_default:
	case ST_EXTARGS_TYPE::int_with_set_error:
		cout << "int";
		break;
	case ST_EXTARGS_TYPE::str:
	case ST_EXTARGS_TYPE::str_with_set_default:
	case ST_EXTARGS_TYPE::str_with_set_error:
		cout << "string";
		break;
	case ST_EXTARGS_TYPE::ipaddr_with_default:
	case ST_EXTARGS_TYPE::ipaddr_with_error:
		cout << "IP地址";
		break;
	case ST_EXTARGS_TYPE::tmax:
		break;
	default:
		break;
	}
	if (show_range)
	{
		cout << ", ";
		this->print_range();
	}
	if (show_default)
	{
		cout << " ";
		this->print_default();
	}
	cout << ")\n";
}
int args_analyse_tools::get_range_wid()const
{
	int wid = 0;
	switch (this->extargs_type)
	{
	case ST_EXTARGS_TYPE::int_with_default:
	case ST_EXTARGS_TYPE::int_with_error:
		wid = 4;
		wid += get_wid(extargs_int_min);
		wid += get_wid(extargs_int_max);
		return wid;
	case ST_EXTARGS_TYPE::int_with_set_default:
	case ST_EXTARGS_TYPE::int_with_set_error:
		wid = 0;
		for (int i = 0; i < set_len; i++)
			wid += get_wid(extargs_int_set[i]);
		return wid + 4;
	case ST_EXTARGS_TYPE::str_with_set_default:
	case ST_EXTARGS_TYPE::str_with_set_error:
		wid = 0;
		for (int i = 0; i < set_len; i++)
			wid += extargs_string_set[i].length() + 1;
		return wid;
	default:
		break;
	}
	return -1;
}
/***************************************************************************
  函数名称：
  功    能：
  输入参数：
  返 回 值：
  说    明：已实现，不要动
 ***************************************************************************/
const string args_analyse_tools::get_name() const
{
	return this->args_name;
}
/***************************************************************************
  函数名称：
  功    能：
  输入参数：
  返 回 值：
  说    明：加!!后，只能是0/1
			已实现，不要动
 ***************************************************************************/
const int args_analyse_tools::existed() const
{
	return !!this->args_existed;
}
/***************************************************************************
  函数名称：
  功    能：
  输入参数：
  返 回 值：
  说    明：已实现，不要动
 ***************************************************************************/
const int args_analyse_tools::get_int() const
{
	return this->extargs_int_value;
}
/***************************************************************************
  函数名称：
  功    能：
  输入参数：
  返 回 值：
  说    明：已实现，不要动
 ***************************************************************************/
const string args_analyse_tools::get_string() const
{
	return this->extargs_string_value;
}
/***************************************************************************
  函数名称：
  功    能：
  输入参数：
  返 回 值：
  说    明：已实现，不要动
 ***************************************************************************/
const unsigned int args_analyse_tools::get_ipaddr() const
{
	return this->extargs_ipaddr_value;
}
/***************************************************************************
  函数名称：
  功    能：
  输入参数：
  返 回 值：
  说    明：将 extargs_ipaddr_value 的值从 0x7f000001 转为 "127.0.0.1"
 ***************************************************************************/
const string args_analyse_tools::get_str_ipaddr() const
{
	string ip = "";
	unsigned int ip_value = extargs_ipaddr_value;
	char seg[5];
	for (int i = 3; i >= 0; i--)
	{
		memset(seg, 0, sizeof seg);
		sprintf(seg, "%u", (ip_value >> (i * 8)) & 255);
		ip += seg;
		if (i != 0)
			ip += '.';
	}
	return ip; //此句根据需要修改
}

/***************************************************************************
  函数名称：
  功    能：
  输入参数：follow_up_args：是否有后续参数
			0  ：无后续参数
			1  ：有后续参数
  返 回 值：
  说    明：友元函数
***************************************************************************/
int args_analyse_process(const int argc, const char* const* const argv, args_analyse_tools* const args, const int follow_up_args)
{
	bool is_parm[128];
	// 分割argv
	for (int i = 1; i < argc; ++i) {
		is_parm[i] = (strlen(argv[i]) >= 2 && argv[i][0] == '-' && argv[i][1] == '-');
	}

	for (int arg_pos = 1; arg_pos < argc; ++arg_pos)
	{
		if (!is_parm[arg_pos]) {
			if (follow_up_args) {
				return arg_pos;
			}
			else {
				cout << "参数[" << argv[arg_pos] << "]格式非法(不是--开头的有效内容)." << endl;
				return -1;
			}
		}
		bool match = 0;
		for (args_analyse_tools* arg = args; arg->extargs_type != ST_EXTARGS_TYPE::null; arg++)
		{
			if (arg->args_name != argv[arg_pos])
				continue;
			if (arg->args_existed)
			{
				cout << "参数[" << arg->args_name << "]重复." << endl;
				return -1;
			}
			arg->args_existed = 1;
			match = 1;
			if (arg->extargs_type == ST_EXTARGS_TYPE::boolean)
			{
				arg->val_wid = 4;
				break;
			}
			if (arg_pos + 1 == argc || is_parm[arg_pos + 1]) {
				cout << "参数[" << arg->args_name << "]缺少附加参数.(类型:";
				cout << TYPE_NAME[(int)arg->extargs_type];
				arg->error_tips(arg->has_range(), arg->has_default());
				return -1;
			}
			ST_EXTARGS_TYPE type = arg->extargs_type;
			switch (type) {
			case ST_EXTARGS_TYPE::int_with_default:
			case ST_EXTARGS_TYPE::int_with_error:
			{
				int extarg = cheak_int(argv[arg_pos + 1], arg->extargs_int_min, arg->extargs_int_max);
				if (extarg == NOT_NUM)
				{
					cout << "参数[" << arg->args_name << "]的附加参数不是整数. ";
					arg->error_tips(1, type == ST_EXTARGS_TYPE::int_with_default);
					return -1;
				}
				else if (extarg == OUT_RANGE)
				{
					if (type == ST_EXTARGS_TYPE::int_with_error)
					{
						cout << "参数[" << arg->args_name << "]的附加参数值(" << argv[arg_pos + 1] << ")非法. ";
						arg->error_tips(1, 0);
						return -1;
					}
					arg->extargs_int_value = arg->extargs_int_default;
					arg->val_wid = get_wid(arg->extargs_int_default);
				}
				else
				{
					arg->extargs_int_value = extarg;
					arg->val_wid = get_wid(extarg);
				}
				break;
			}
			case ST_EXTARGS_TYPE::int_with_set_default:
			case ST_EXTARGS_TYPE::int_with_set_error:
			{
				int extarg = cheak_int(argv[arg_pos + 1]);
				if (extarg == NOT_NUM)
				{
					cout << "参数[" << arg->args_name << "]的附加参数值(" << argv[arg_pos + 1] << ")不是整数.";
					return -1;
				}
				else {
					arg->extargs_int_value = extarg;
					if (!arg->in_set(ST_EXTARGS_TYPE::int_with_set_default)) {
						if (type == ST_EXTARGS_TYPE::int_with_set_error)
						{
							cout << "参数[" << arg->args_name << "]的附加参数值(" << argv[arg_pos + 1] << ")非法. ";
							arg->error_tips(1, 0);
							return -1;
						}
						arg->extargs_int_value = arg->extargs_int_default;
					}
					else
						arg->extargs_int_value = extarg;
					arg->val_wid = get_wid(extarg);
				}
				break;
			}
			case ST_EXTARGS_TYPE::str:
			{
				arg->extargs_string_value = argv[arg_pos + 1];
				arg->val_wid = strlen(argv[arg_pos + 1]);
				break;
			}
			case ST_EXTARGS_TYPE::str_with_set_default:
			case ST_EXTARGS_TYPE::str_with_set_error:
			{
				arg->extargs_string_value = argv[arg_pos + 1];
				if (!arg->in_set(ST_EXTARGS_TYPE::str_with_set_default))
				{
					if (type == ST_EXTARGS_TYPE::str_with_set_error)
					{
						cout << "参数[" << arg->args_name << "]的附加参数值(" << argv[arg_pos + 1] << ")非法. ";
						arg->error_tips(1, 0);
						return -1;
					}
					arg->extargs_string_value = arg->extargs_string_default;
				}
				arg->val_wid = arg->extargs_string_value.length();
				break;
			}
			case ST_EXTARGS_TYPE::ipaddr_with_default:
			case ST_EXTARGS_TYPE::ipaddr_with_error:
			{
				bool valid = cheakIp(argv[arg_pos + 1]);
				if (!valid)
				{
					if (type == ST_EXTARGS_TYPE::ipaddr_with_error)
					{
						cout << "参数[" << arg->args_name << "]的附加参数值(" << argv[arg_pos + 1] << ")非法. ";
						arg->error_tips(0, 0);
						return -1;
					}
					else
					{
						arg->extargs_ipaddr_value = arg->extargs_ipaddr_default;
						arg->extargs_string_value = arg->extargs_string_default;
						arg->val_wid = arg->extargs_string_value.length();
					}
				}
				else
				{
					arg->extargs_ipaddr_value = ip_to_dec(argv[arg_pos + 1]);
					arg->extargs_string_value = argv[arg_pos + 1];
					arg->val_wid = arg->extargs_string_value.length();
				}
				break;
			}
			default:
				;
			}
			arg_pos++;
			break;
		}
		if (!match)
		{
			cout << "参数[" << argv[arg_pos] << "]非法." << endl;
			return -1;
		}
	}
	return argc;
}

/***************************************************************************
  函数名称：
  功    能：
  输入参数：
  返 回 值：
  说    明：友元函数
***************************************************************************/
int args_analyse_print(const args_analyse_tools* const args)
{
	int name_wid = 4, type_wid = 4, default_wid = 7, exist_wid = 6, val_wid = 5, range_wid = 5;
	int n = 0;
	for (const args_analyse_tools* arg = args; arg->extargs_type != ST_EXTARGS_TYPE::null; arg++)
	{
		n++;
		name_wid = max(name_wid, (int)arg->args_name.length());
		type_wid = max(type_wid, (int)TYPE_NAME[(int)arg->extargs_type].length());
		default_wid = max(default_wid, arg->def_wid);
		val_wid = max(val_wid, arg->val_wid);
		range_wid = max(range_wid, arg->get_range_wid());
	}
	cout << setw(name_wid + 2
		+ type_wid + 1
		+ default_wid + 1
		+ exist_wid + 1
		+ val_wid + 1
		+ range_wid + 1)
		<< setfill('=') << '=' << endl;
	cout << setfill(' ');
	cout << left;
	cout << " " << setw(name_wid) << "name" << " ";
	cout << setw(type_wid) << "type" << " ";
	cout << setw(default_wid) << "default" << " ";
	cout << setw(exist_wid) << "exists" << ' ';
	cout << setw(val_wid) << "value" << " ";
	//cout << setw(range_wid) << "range/set" << " ";
	cout << "range/set";
	cout << endl;
	cout << setw(name_wid + 2
		+ type_wid + 1
		+ default_wid + 1
		+ exist_wid + 1
		+ val_wid + 1
		+ range_wid + 1)
		<< setfill('=') << '=' << endl;
	cout << setfill(' ');
	for (const args_analyse_tools* arg = args; arg->extargs_type != ST_EXTARGS_TYPE::null; arg++)
	{
		// arg_name
		cout << " " << setw(name_wid) << arg->args_name << " ";
		cout << setw(type_wid) << TYPE_NAME[(int)arg->extargs_type] << " ";

		string para;
		// arg_def
		bool flag_def = arg->has_default();
		if (flag_def)
			para = arg->print_default(0, 1);
		else
			para = "/";
		cout << setw(default_wid) << para << " ";

		// exist
		cout << setw(exist_wid) << arg->args_existed << " ";

		//val
		bool flag_val = arg->args_existed;
		if (flag_val)
			para = arg->print_value(1);
		else
			para = "/";
		cout << setw(val_wid) << para << " ";

		//range
		bool flag_range = arg->has_range();
		if (flag_range)
			para = arg->print_range(0, 1);
		else
			para = "/";
		//cout << setw(range_wid) << para << " ";
		cout << para;
		cout << endl;
	}
	//cout << endl;
	cout << setw(name_wid + 2
		+ type_wid + 1
		+ default_wid + 1
		+ exist_wid + 1
		+ val_wid + 1
		+ range_wid + 1)
		<< setfill('=') << '=' << endl << endl;
	cout << setfill(' ');
	cout << resetiosflags(ios::left);
	cout << setiosflags(ios::right);
	return 0;
}

#endif // !ENABLE_LIB_COMMON_TOOLS
