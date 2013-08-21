#include<iostream>
#include<fstream>
#include<cstdlib>
#include<cstring>
#include<cmath>
#include <sstream>
using namespace std;

class wgsFile {
public:
	wgsFile(){moves="";result="";};
	void isfileopen(char* filepath);					//判断文件是否成功打开
	bool testfile(char* filepath);						//测试文件是否符合要求 wgs文件最短长度122字节
	long long getqq(char* filepath,int p);				//处理qq号
	string common_info();								//棋谱基本信息
	string getname(char* filepath,int p);				//处理昵称
	string printname(char* filepath,int p);				//打印棋手名称qq号信息
	string getdate(char* filepath);						//对局日期
	int gethandicap(char* filepath);					//处理让子数
	string getmoves(char* filepath);					//棋局
	long long getrank(char* filepath,int p);			//积分等级换算
	int getdan(char* filepath,int p);					//段位
	string printdan(char*filepath);						//打印段位
	string getcommon(char* filepath);					//贴目，棋盘大小，让字数等信息
	string getresult(char* filepath);					//对局结果
	string getsgf(char* filepath,int p);				//得到sgf
	
	void version();										//版本信息
	void help();										//帮助信息
	
	
private:
	
	//对局基本信息,非二进制读取
	string bname;										//黑棋昵称	第10字节,占用22字节
	string wname;										//白棋昵称  第58字节,占用22字节
	
	//对局基本信息,需二进制读取
	long long bqq;										//黑棋QQ号
	long long wqq;										//白棋QQ号
	int handicap;										//让子
	int	gobansize;										//棋盘大小 60h行，5列，第101字节
	float komi;											//贴目
	string result;										//结果
	string date;										//对局日期
	
	//对局详细信息
	string totalmoves;									//总手数
	string moves;										//对局
	ifstream read_wgs;									//读取wgs文件
};