/*
	qq棋谱文件(.wgs)分析(用UltraEdit打开,(xxh,x)表示位置)：
	(00h,a)~(10h,f)表示执黑棋手姓名，共22字节；
	(30h,a)~(40h,f)表示执白棋手姓名，共22字节；
	棋手姓名前面六个字节，保存的是qq号信息，从低位到高位，直接转换为十进制；
		比如黑棋qq号保存在(00h,4)~(00h,9)字节内，信息为
				E9 03 E4 41 00 00
		则表示的qq号是 00 00 41 E4 03 E9 所表示的十进制数，即 1105462249
	棋盘大小 在wgs文件中，棋盘路数信息保存在 60h行，5列，第101字节处
	手数保存在 (70h,8)~(70h,9);
	对局从(70h,a)开始;
	

*/

#include<iostream>
#include<fstream>
#include<cstdlib>
#include<cstring>
#include<cmath>
#include<regex>
using namespace std;

void help();				//帮助信息
void version();				//版本信息
string info();				//棋谱相关信息
bool is_date(char* date);	//判断日期是否合法
int main(int argc, char* argv[])
{
	ifstream inFile;
	ofstream sgffile;
	int lu;		//棋盘大小 在wgs文件中，棋盘路数信息保存在 60h行，5列，第101字节处
	int filesize;		//棋谱文件大小
	char* filepath;	//棋谱文件路径
	char* newfilepath;	//sgf棋谱文件路径
	filepath=new char[200];
	newfilepath=new char[200];
	int display_qq=0;	//默认不显示qq号
	int flags_qq=1;		//显示qq号处理辅助
	//int flags_qq2=1;	//显示qq号处理辅助
	if(argc==1)
	{
		help();
		exit(0);
	}
	for(int i=1;i<argc;i++)
	{
		if(strcmp(argv[i],"-v")==0)
		{
			version();
			exit(0);
		}
		if(strcmp(argv[i],"-help")==0)
		{
			help();
			exit(0);
		}
		if(strcmp(argv[i],"-qq")==0)
		{
			display_qq=1;
			flags_qq=2;
		}
	}
	
	for(int i=flags_qq;i<argc;i++)
	{
		
		/*本段代码用正则代替
		char *date;		//对局日期,在qq棋谱名未改动的情况下有效，目前尚未能从棋谱中读出对局时间信息，格式 20130430-2332 年月日-时分
		date=new char[14];
		//if(argv[i]
		memmove(date,argv[i],14);	//函数从argv[i]中复制13 个字符到date中，并返回date指针
		date[14]='\0';	//添加结束符
		if(!is_date(date))
		{
			strcpy(date,"未知时间");
		}*/
		
		//提取qq围棋自动保存棋谱中的段位信息
		regex pattern("([0-9]+)-([0-9]+)(.)(.+)(\\[[0-9]+\\])(.)([^\\s]+)(.)(.+)(\\[[0-9]+\\])");
		cmatch res;
		regex_search(argv[i],res,pattern);
		char *remarks;	//备注信息
		remarks=new char[200];
		strcpy(remarks,argv[i]);
		unsigned int len=strlen(remarks);	//文件名长度
		for(unsigned int k=0;k<len;k++)
		{
			if(remarks[k]=='[')
				remarks[k]='<';
			if(remarks[k]==']')
				remarks[k]='>';
		}
		
		filepath=strcpy(filepath,argv[i]);
		newfilepath=strcpy(newfilepath,filepath);
		newfilepath=strcat(newfilepath,".sgf");
		
		
		inFile.open(filepath);
		if(!inFile)
		{
			cerr<<"wgs2sgf: cannot convert `"<<filepath<<"`: No such file or directory"<<endl;
			exit(0);
		}
		
		//文件大小是否合法,qq围棋文件最小122字节
		inFile.seekg(0,ios::end);
		int tmpsize=inFile.tellg();
		if(tmpsize<123)
		{
			cerr<<"wgs2sgf: Error while laod file `"<<filepath<<"`"<<endl;
			exit(0);
		}
		
		inFile.seekg(10,ios::beg);	//黑棋棋手姓名保存在距文件开始10字节处
		char* pb;	//黑棋棋手姓名
		char* pw;	//白棋棋手姓名
		pb=new char[22];
		pw=new char[22];
		inFile.get(pb,22);	//棋手姓名信息占用22字节
		inFile.seekg(58,ios::beg);	//白棋棋手姓名保存在距文件开始58字节处
		inFile.get(pw,22);	//棋手姓名信息占用22字节
		
		inFile.close();		//第一次，非二进制方式打开，读取棋手姓名
		
		inFile.open(filepath,ios::in|ios::binary);	//打开棋谱
		if(!inFile)
		{
			cerr<<"wgs2sgf: cannot convert `"<<filepath<<"`: No such file or directory"<<endl;
			exit(0);
		}
		
		inFile.seekg(0,ios::end);
		filesize=inFile.tellg();

		unsigned char* b; 	//棋谱二进制文件缓冲区
		b=new unsigned char[filesize];
		
		//如：00 00 41 E4 03 E9，bqq[0]=00,bqq[1]=0,..,计算个位置后计算 bqq[]的和即为qq号
		int bqq[6];	//黑棋qq号
		int wqq[6];	//白棋qq号
		long long  blackqq;	//黑棋qq号,long long 类型以便保存10位数及以上的qq号
		long long whiteqq;	//白棋qq号
		inFile.seekg(4,ios::beg);	//定位到第4字节处读取黑棋qq号
		for(int i=0;i<6;i++)
		{
			inFile.read((char*)&b[i],sizeof(char));
		}
		int k=0;
		for(int i=5;i>=0;i--)
		{
			bqq[k]=(int)b[i];
			k++;
		}
		/*
			计算qq号 ，若某qq号16进制形式数组为：qq[6]={00 00 41 E4 03 E9}，则qq号计算过程： 
				(4*16^7+1*16^6) + (14*16^5+4*16^4)+ ...  =(4*16+1)*16^6 + (14*16+4)*16^4 + ......
			而41表示的十进制数为 4*16+1 ,E4表示的数为 14*16+4，则可见规律为 ：
			qq号= qq[0]*16^10 + qq[1]*16^8 + qq[2]*16^6 + qq[3]*16^4 + +qq[4]*16^2 + qq[5]*16^0;
		*/
		blackqq=bqq[0]*1099511627776+bqq[1]*4294967296+bqq[2]*16777216+bqq[3]*65536+bqq[4]*256+bqq[5];
		inFile.seekg(52,ios::beg);	//定位到第52字节处读取白棋qq号
		for(int i=0;i<6;i++)
		{
			inFile.read((char*)&b[i],sizeof(char));
		}
		k=0;
		for(int i=5;i>=0;i--)
		{
			wqq[k]=(int)b[i];
			k++;
		}
		whiteqq=wqq[0]*1099511627776+wqq[1]*4294967296+wqq[2]*16777216+wqq[3]*65536+wqq[4]*256+wqq[5];
		
		//如果没有-qq选项则默认不显示qq号
		if(!display_qq)
		{
			blackqq=whiteqq=0;
			strcpy(remarks,"qq棋谱");
		}
		
		inFile.seekg(101,ios::beg);	//定位到据文件开始101字节处读取棋盘大小信息
		inFile.read((char*)&b[101],sizeof(char));
		lu=(unsigned int)b[101];	//棋盘路数
		inFile.seekg(122,ios::beg);	//指针定位到据文件首部122字节处
		int shoushu=filesize-122;		//2倍手数

		int* t;		//保存手数信息
		t=new int[shoushu];
		for(int i=122;i<filesize;i++)
		{
			inFile.read((char*)&b[i],sizeof(char));
			t[i-122]=(unsigned int)b[i];
		}

		sgffile.open(newfilepath,ios::out);	//先写入打开，确保重新写入时先清空文件内容
		sgffile.close();					//需要先关闭才能再次以追加方式打开
		sgffile.open(newfilepath,ios::app);	//ios:app 添加输入

		sgffile<<"(;CA[gb2312]SZ["<<lu<<"]"<<"PB["<<pb<<blackqq<<"]PW["<<pw<<whiteqq<<"]AP[MultiGo]GC[原始棋谱:"<<remarks<<"]\n";
		sgffile<<"BR["<<res[4]<<"]WR["<<res[9]<<"]RE[黑"<<res[7]<<"]TM[15分钟]KM[7.5]GN[QQ围棋]";
		sgffile<<"DT["<<res[1]<<"]"<<info()<<endl;
		
		/*
		  97~122对应字母a~z, 列：32对应i,0对应a,4对应b,8对应c,....;97+t[i]/4可以实现0->97,4->98,8->99...的映射，
		  行：0->a,1->b,2->c... t[j]+97实现映射；
		*/
		
		int tmp=0;	//sgf文件换行

		for(int j=0;j<shoushu;j++)
		{
			if((j/2)%2==0)		//判断黑白，奇数手为黑，偶数手为白
			{
				if(t[j]==1&&t[j+1]==0)		//pass一手，在wgs对应16进制 01 00,在sgf中对应B[]或者W[]
				{
					sgffile<<";B[];";
					j++;
				}
				else
				{
					sgffile<<";B["<<((char)(97+t[j]/4))<<((char)(97+t[j+1]))<<"];";
					j++;			//读取了两个字节，需加1
				}
			}
			else
			{
				if(t[j]==1&&t[j+1]==0)			//pass一手，在wgs对应16进制 01 00,在sgf中对应B[]或者W[]
				{
					sgffile<<"W[]";
					j++;
				}
				else
				{
					sgffile<<"W["<<((unsigned char)(97+t[j]/4))<<((unsigned char)(97+t[j+1]))<<"]";
					j++;
				}
			}
			
			tmp++;
			if(tmp%14==0)		//14手换一行
				sgffile<<endl;
			
		}

		delete []b;
		delete []t;
		sgffile<<")";
		
		cout<<"转换成功，已保存为: "<<newfilepath<<endl;
		delete [] pb;
		delete [] pw;
		delete [] remarks;

		inFile.close();
		sgffile.close();
	}
	delete [] filepath;
	delete [] newfilepath;
	return 0;
}

void help()
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

void version()
{
	cout<<"\nQQ围棋wgs棋谱转sgf棋谱工具 V 1.2 Release20130501\n"
		<<"作者: annhe 联系方式:admin@annhe.net  主页: www.annhe.net\n";
}

string info()
{
	string str="PC[QQ围棋]US[wgs2sgf]SO[wgs2sgf]CP[power by wgs2sgf \ncopyright @ annhe]MULTIGOGM[1]";
	return str;
}
/*用正则代替
bool is_date(char* date)
{
	if(date[8]!='-')
		return false;
	for(int i=0;i<8;i++)
	{
		if(int(date[i])>47&&int(date[i])<58)
			return true;
		else
			return false;
	}
	return false;
}*/