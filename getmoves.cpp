#include "wgs.h"

string wgsFile::getmoves(char* filepath)
{
	gethandicap(filepath);
	int start=0;				//标志是否是让子棋 让子棋白棋先行，否则黑棋先行
	moves="";
	if(handicap>0)
	{
		start=1;
		switch(handicap)
		{
		case 2:
			moves="AB[pd][dp]\n";							//让2子(位置在 右上和左下)
			break;
		case 3:
			moves="AB[dd][pd][dp]\n";						//让3子
			break;
		case 4:
			moves="AB[dd][pd][dp][pp]\n";					//让4子
			break;
		case 5:
			moves="AB[dd][pd][dp][pp][jj]\n";				//让5子
			break;
		case 6:
			moves="AB[dd][pd][dp][pp][dj][pj]\n";			//让6子
			break;
		case 7:
			moves="AB[dd][pd][dp][pp][dj][pj][jj]\n";		//让7子
			break;
		case 8:
			moves="AB[dd][pd][dp][pp][jd][dj][pj][jp]\n";	//让8子
			break;
		case 9:
			moves="AB[dd][pd][dp][pp][jd][dj][pj][jp][jj]\n";	//让9子
			break;
		default:
			moves="";
			break;
		}
	}
	
	read_wgs.open(filepath,ios::in|ios::binary);
	isfileopen(filepath);
	int filesize;
	read_wgs.seekg(0,ios::end);
	filesize=read_wgs.tellg();		//文件大小
	read_wgs.seekg(122,ios::beg);	//指针定位到据文件首部122字节处(对局开始处)
	int* tmp=new int[filesize-122];		//手数信息
	
	char* b=new char[filesize];		//读取每一手棋 临时变量
	for(int i=122;i<filesize;i++)
	{
		read_wgs.read(&b[i],1);
		tmp[i-122]=(unsigned int)b[i];
	}
	
	
	
	int newline=0;	//sgf文件换行
	stringstream stream;				//类型转换
	string tmpstr="";					//用于类型转换的临时变量
	
	/*
	 * 97~122对应字母a~z, 列：32对应i,0对应a,4对应b,8对应c,....;97+t[i]/4可以实现0->97,4->98,8->99...的映射，
	 * 行：0->a,1->b,2->c... t[j]+97实现映射；
	 */
	
	for(int j=0;j<filesize-122;j++)
	{
		if((j/2+start)%2==0)		//判断黑白，奇数手为白，偶数手为黑 start标志让子棋
		{
			if(tmp[j]==1&&tmp[j+1]==0)		//pass一手，在wgs对应16进制 01 00,在sgf中对应B[]或者W[]
			{
				tmpstr=";B[]";
				moves+=tmpstr;
				tmpstr="";
				j++;
			}
			else
			{
				stream<<";B["<<((char)(97+tmp[j]/4))<<((char)(97+tmp[j+1]))<<"]";
				stream>>tmpstr;
				moves+=tmpstr;
				stream.clear();			//清空stream内容 否则每次读取的都一样
				stream.str("");
				j++;			//读取了两个字节，需加1
			}
		}
		else
		{
			if(tmp[j]==1&&tmp[j+1]==0)			//pass一手，在wgs对应16进制 01 00,在sgf中对应B[]或者W[]
			{
				tmpstr=";W[]";
				moves+=tmpstr;
				tmpstr="";
				j++;
			}
			else
			{
				stream<<";W["<<((unsigned char)(97+tmp[j]/4))<<((unsigned char)(97+tmp[j+1]))<<"]";
				stream>>tmpstr;
				moves+=tmpstr;
				stream.clear();
				stream.str("");
				j++;
			}
		}
		
		newline++;
		if(newline%14==0)		//14手换一行
			moves+="\n";
		
	}
	moves+=")";
	delete [] b;
	delete [] tmp;
	read_wgs.close();
	return moves;
}

