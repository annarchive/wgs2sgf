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
		<<"-qq   转换qq号\n"
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
	string str=common_info()+printdan(filepath);
	str=str+"C[黑方:    "+getname(filepath,10)+"    ";
	string tmpstr="";
	stringstream stream;
	if(brank<0)
		stream<<abs(brank)<<"级";
	else
		stream<<brank<<"段";
	stream>>tmpstr;
	str+=tmpstr;
	stream.clear();
	stream.str("");
	
	if(blackdan>0)
	{
		stream<<"(业余"<<blackdan<<"段)";
		stream>>tmpstr;
	}
	str=str+tmpstr+"\n";
	stream.clear();
	stream.str("");
	
	str=str+"白方:    "+getname(filepath,58)+"    ";
	
	if(wrank<0)
		stream<<abs(wrank)<<"级";
	else
		stream<<wrank<<"段";
	stream>>tmpstr;
	str+=tmpstr;
	stream.clear();
	stream.str("");
	tmpstr="";
	
	if(whitedan>0)
	{
		stream<<"(业余"<<whitedan<<"段)";
		stream>>tmpstr;
	}
	str=str+tmpstr+"\n";
	stream.clear();
	stream.str();
	
	getresult(filepath);
	getdate(filepath);
	
	str=str+"结果:    "+result.substr(3,result.length()-4)+"\n";
	str=str+"日期:    "+date.substr(3,date.length()-4)+"]";
	
	str=str+getcommon(filepath)+printname(filepath,p)+date+result+"\n"+getmoves(filepath);
	return str;

}