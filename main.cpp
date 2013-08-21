#include "wgs.h"

/* 
 * File:   main.cpp
 * Author: annhe
 *
 * Created on 201年月日, :
 */

int main(int argc, char* argv[])
{
	wgsFile aWgs;
	ofstream oSgf;
	char* newfilepath=new char[200];
	
	int display_qq=0;	//默认不显示qq号
	int flags_qq=1;		//显示qq号处理辅助
	if(argc==1)
	{
		aWgs.help();
		exit(0);
	}
	for(int i=1;i<argc;i++)
	{
		if(strcmp(argv[i],"-v")==0)
		{
			aWgs.version();
			exit(0);
		}
		if(strcmp(argv[i],"-help")==0)
		{
			aWgs.help();
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
		if(!aWgs.testfile(argv[i]))				//测试文件是否符合要求 wgs文件最短长度为122字节
		{
			continue;
		}
		strcpy(newfilepath,argv[i]);
		strcat(newfilepath,".sgf");
		oSgf.open(newfilepath,ios::out);
		oSgf<<aWgs.getsgf(argv[i],display_qq);
		oSgf.close();
		cout<<"wgs2sgf: "<<argv[i]<<" convert successfully."<<endl;
	}
	delete [] newfilepath;
    return 0;
}
