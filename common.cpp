#include "wgs.h"

string wgsFile::common_info()
{
	stringstream stream;
	string str="(;CA[gb2312]PC[QQ围棋]US[wgs2sgf]SO[wgs2sgf]CP[power by wgs2sgf \ncopyright @ annhe]MULTIGOGM[1]";
	return str;
}
void wgsFile::help()
{
	cout<<"用法: wgs2sgf 源文件\n"
		<<"  or: wgs2sgf *.wgs(使用通配符)\n"
		<<"  or: wgs2sgf [选项]\n\n"
		<<"示例: wgs2sgf qqgo.wgs\n\n"
		<<"可用选项:\n"
		<<"-qq   显示qq号\n"
		<<"-v    显示版本信息\n"
		<<"-help 显示此帮助信息\n\n"
		<<"Copyright @ annhe email:admin@annhe.net\n";
}

void wgsFile::version()
{
	cout<<"\nQQ围棋wgs棋谱转sgf棋谱工具 V 1.4 Release20130821\n"
		<<"作者: annhe 联系方式:admin@annhe.net  主页: www.annhe.net\n";
}

bool wgsFile::testfile(char* filepath)
{
	//文件大小是否合法,qq围棋文件最小122字节
	read_wgs.open(filepath);
	isfileopen(filepath);
	read_wgs.seekg(0,ios::end);
	int tmpsize=read_wgs.tellg();
	read_wgs.close();
	if(tmpsize<123)
	{
		cerr<<"wgs2sgf: Error while laod file `"<<filepath<<"`"<<endl;
		return false;
	}
	return true;
}

string wgsFile::getsgf(char* filepath,int p)
{
	string str;
	str=common_info()+getcommon(filepath)+printname(filepath,p)+getdate(filepath)+printdan(filepath)+getresult(filepath)+"\n"+getmoves(filepath);
	return str;
}