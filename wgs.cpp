#include <sys/stat.h>
#include <time.h>
#include "wgs.h"

/*
 * 计算qq号 ，若某qq号16进制形式数组为：qq[6]={00 00 41 E4 03 E9}，则qq号计算过程： 
 * (4*16^7+1*16^6) + (14*16^5+4*16^4)+ ...  =(4*16+1)*16^6 + (14*16+4)*16^4 + ......
 * 而41表示的十进制数为 4*16+1 ,E4表示的数为 14*16+4，则可见规律为 ：
 * qq号= qq[0]*16^10 + qq[1]*16^8 + qq[2]*16^6 + qq[3]*16^4 + +qq[4]*16^2 + qq[5]*16^0;
 */
 void wgsFile::isfileopen(char* filepath)
 {
	if(!read_wgs)
	{
		cerr<<"wgs2sgf: cannot convert `"<<filepath<<"`: No such file or directory"<<endl;
		exit(0);
	}
 }
 //处理qq号
long long wgsFile::getqq(char* filepath,int p)
{
	read_wgs.open(filepath,ios::in|ios::binary);	//二进制方式打开
	isfileopen(filepath);

	read_wgs.seekg(p,ios::beg);						//定位到第4字节处读取黑棋qq号
	unsigned char b[6];
	for(int i=5;i>=0;i--)
	{
		read_wgs.read((char*)&b[i],1);				//用read读取二进制数据
	}
	read_wgs.close();
	
	unsigned int qq[6];
	for(int i=0;i<6;i++)
	{
		qq[i]=(unsigned int)b[i];				//无符号型防止QQ号转换后为负数
	}
	long long qqnumber;
	qqnumber=qq[0]*1099511627776+qq[1]*4294967296+qq[2]*16777216+qq[3]*65536+qq[4]*256+qq[5];
	return qqnumber;
}

string wgsFile::getname(char* filepath,int p)
{
	read_wgs.open(filepath);
	isfileopen(filepath);
	
	read_wgs.seekg(p,ios::beg);	//黑棋棋手姓名保存在距文件开始10字节处
	char n[22];					//棋手姓名
	string name;				//棋手姓名
	read_wgs.get(n,22);			//棋手姓名信息占用22字节
	stringstream stream;
	stream<<n;
	stream>>name;
	read_wgs.close();
	return name;
}

string wgsFile::printname(char* filepath,int p)
{
	bqq=getqq(filepath,4);			//黑棋QQ号
	wqq=getqq(filepath,52);			//白棋QQ号
	bname=getname(filepath,10);		//黑棋昵称
	wname=getname(filepath,58);		//白棋昵称
	string bqqstr="";				//黑棋QQ字符串
	string wqqstr="";				//白棋QQ字符串	
	stringstream stream;
	if(p==1)						//p=1 才显示qq号
	{
		stream<<"("<<bqq<<")";
		stream>>bqqstr;
		stream.clear();
		stream.str("");
		stream<<"("<<wqq<<")";
		stream>>wqqstr;
		stream.clear();
		stream.str("");
	}
	string str;
	stream<<"PB["<<bname<<bqqstr<<"]PW["<<wname<<wqqstr<<"]";
	stream>>str;
	stream.clear();
	stream.str("");
	return str;
}
int wgsFile::gethandicap(char* filepath)
{
	read_wgs.open(filepath);
	isfileopen(filepath);
	
	read_wgs.seekg(100,ios::beg);		//让子数在100字节处
	char ch;
	read_wgs.read(&ch,1);
	handicap=(unsigned int)ch;
	read_wgs.close();
	return handicap;
}

string wgsFile::getdate(char* filepath)
{
	read_wgs.open(filepath);
	isfileopen(filepath);
	
	struct stat statbuf;
	stat(filepath, &statbuf);		/* get information about the file */ 
	
	read_wgs.close();

	long *m_second=&statbuf.st_mtime;				//st_mtime 文件内容最后一次修改时间(Unix时间戳) 对于wgs棋谱来说基本就是初次创建时间，即对局时间
	struct tm *m_localtime=localtime(m_second);		//localtime将时间戳转换为本地时间

	stringstream stream;
	stream<<"DT["<<m_localtime->tm_year+1900<<"-"<<m_localtime->tm_mon+1<<"-"<<m_localtime->tm_mday<<"]";
	stream>>date;
	stream.clear();
	stream.str("");
	return date;
}

/*
 * 积分信息低位到高位，和QQ号类似
 * 假设有积分，c2 ff ff ff，倒序排是 ff ff ff c2，求补码，结果为80 00 00 3E，转换为十进制：-62，
 */
long long wgsFile::getrank(char* filepath,int p)
{
	read_wgs.open(filepath,ios::in|ios::binary);
	isfileopen(filepath);
	
	long long rank;		//积分
	read_wgs.seekg(p,ios::beg);
	unsigned char ch[4];
	for(int i=3;i>=0;i--)
	{
		read_wgs.read((char*)&ch[i],1);
	}
	unsigned int tmp[4];
	for(int i=0;i<4;i++)
	{
		tmp[i]=(unsigned int)ch[i];
	}
	if(tmp[0]<128)
	{
		rank=tmp[0]*16777216+tmp[1]*65536+tmp[2]*256+tmp[3];
	}
	else if(tmp[0]>=128)
	{
		//负积分要求反码,由于int可能是4个字节，即32位，ff取反后为ffffff00，不是希望的结果，这里只取后8位即可
		unsigned int mask=0x000000ff;
		for(int i=0;i<4;i++)
		{
			tmp[i] = (~tmp[i])&mask;
		}
		rank=tmp[0]*16777216+tmp[1]*65536+tmp[2]*256+tmp[3]+1;
		rank=-rank;
	}
	
	/*
	 * 9 级 =-800  8 级 =-700  7 级 =-600  6 级 =-500  5 级 =-400  4 级 =-300  3 级 =-200  2 级 =-100  1 级 =0 
	 * 1 段 =100   2 段 =200   3 段 =300   4 段 =400   5 段 =500   6 段 =600   7 段 =700   8 段 =800   9 段 =900 
     */
	rank=rank/100;
	if(rank<1)			//考虑0~100之间为1级的情况 如40/100=0,0-1 = -1为1级
		rank=rank-1;
	if(rank>9)
		rank=9;
	if(rank<-9)
		rank=-9;
		
	read_wgs.close();
	
	return rank;
}

int wgsFile::getdan(char* filepath,int p)
{
	read_wgs.open(filepath,ios::in|ios::binary);
	isfileopen(filepath);
	
	read_wgs.seekg(p,ios::beg);
	char d[1];
	read_wgs.read(&d[0],1);
	int dan=(unsigned int)d[0];
	read_wgs.close();
	return dan;
	
}

string wgsFile::printdan(char* filepath)
{
	int black=getrank(filepath,36);		//黑棋段位,36字节处
	int white=getrank(filepath,84);		//白棋段位,84字节处
	int blackdan=getdan(filepath,42);	//黑棋评测段位,42字节处
	int whitedan=getdan(filepath,90);	//白棋评测段位,90字节处
	read_wgs.open(filepath,ios::in|ios::binary);
	isfileopen(filepath);
	string str="BR[";
	string tmpstr="";
	stringstream stream;
	if(black<0)
	{
		stream<<abs(black)<<"级";
	}
	else
	{
		stream<<black<<"段";
	}
	stream>>tmpstr;
	stream.clear();
	stream.str("");
	str+=tmpstr;
	if(blackdan>0)
	{
		stream<<"(业余"<<blackdan<<"段)";
		stream>>tmpstr;
		stream.clear();
		stream.str("");
		str+=tmpstr;
	}
	str+="]WR[";
	
	if(white<0)
	{
		stream<<abs(white)<<"级";
	}
	else
	{
		stream<<white<<"段";
	}
	stream>>tmpstr;
	stream.clear();
	stream.str("");
	str+=tmpstr;
	if(whitedan>0)
	{
		stream<<"(业余"<<whitedan<<"段)";
		stream>>tmpstr;
		stream.clear();
		stream.str("");
		str+=tmpstr;
	}
	str+="]";
	
	read_wgs.close();
	return str;
}

string wgsFile::getcommon(char* filepath)
{
	gethandicap(filepath);
	read_wgs.open(filepath,ios::in|ios::binary);
	isfileopen(filepath);
	read_wgs.seekg(103,ios::beg);
	char tmp[4];
	for(int i=0;i<2;i++)
	{
		read_wgs.read(&tmp[i],1);
	}
	komi=(unsigned int)tmp[0]+0.1*(unsigned int)tmp[1];
	
	read_wgs.seekg(101,ios::beg);
	read_wgs.read(&tmp[2],1);
	gobansize=(unsigned int)tmp[2];
	
	stringstream stream;
	string str;
	stream<<"SZ["<<gobansize<<"]HA["<<handicap<<"]KM["<<komi<<"]";
	stream>>str;
	stream.clear();
	stream.str("");

	read_wgs.close();
	return str;
}

string wgsFile::getresult(char* filepath)
{
	read_wgs.open(filepath,ios::in|ios::binary);
	isfileopen(filepath);
	char r[4];
	read_wgs.seekg(105,ios::beg);
	read_wgs.read(&r[0],1);
	int flags=(unsigned int)r[0];
	
	//点目结果，108,109字节，倒序取补码，除2再减0.25
	read_wgs.seekg(108,ios::beg);
	read_wgs.read(&r[1],1);
	read_wgs.read(&r[2],1);

	unsigned int c=(unsigned int)r[1];
	unsigned int d=(unsigned int)r[2];
	unsigned int mask=0x000000ff;
	float point;
	if((d&mask)<128)			//高位没有1，为正数
	{
		point=(d*256+c)/2.0 - 0.25;
	}
	else if((d&mask)>128)
	{
		d=(~d)&mask;
		c=(~c)&mask;
		point=(d*256+c+1)/2.0;
		point=-point-0.25;
		point=-point;
	}	
	
	int rst;
	read_wgs.seekg(106,ios::beg);
	read_wgs.read(&r[3],1);
	rst=(unsigned int)r[3];
	string winner="";
	string loser="";
	if(rst==0)
	{
		winner="白胜";
		loser="黑";
	}
	else if(rst==1)
	{
		winner="黑胜";
		loser="白";
	}
	
	string pts;			//point的字符串形式
	stringstream stream;
	stream<<point;
	stream>>pts;
	stream.clear();
	stream.str("");
	
	switch(flags)
	{
	case 0:
		result="RE[对局正在进行中]";
		break;
	case 1:
		result="RE[步数超过上限和棋]";
		break;
	case 2:
		result="RE[和棋]";
		break;
	case 3:
		result="RE[出错]";
		break;
	case 4:
		result="RE[结束]";
		break;
	case 5:
		result="RE[游戏被解散无胜负]";
		break;
	case 6:
		result="RE["+loser+"逃跑/"+winner+"]";
		break;
	case 7:
		result="RE[在对方同意下退出未分出胜负]";
		break;
	case 8:
		result="RE["+loser+"超时/"+winner+"]";
		break;
	case 9:
		result="RE["+loser+"认输/"+winner+"]";
		break;
	case 10:
		result="RE["+winner+pts+"子]";
		break;
	default:
		result="RE[未分出胜负]";
		break;
	}
	
	read_wgs.close();
	return result;
}