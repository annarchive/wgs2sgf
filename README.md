用法：
1. 简单方式
	将wgs棋谱文件拖到 wgs2sgf.exe 上即可在wgs棋谱旁生成一个sgf棋谱文件。

2. 批处理方式
	点击 wgs2sgf.bat ，将会转换当前目录下的所有wgs棋谱，并生成两个目录 qqgo_sgf、qqgo_wgs分别保存wgs棋谱和转换后的sgf棋谱。

3. 命令行方式
	wgs2sgf 源文件
	or: wgs2sgf *.wgs(使用通配符)
	or: wgs2sgf [选项]

示例: wgs2sgf -qq qqgo.wgs

可用选项:
-v    显示版本信息
-qq   转换QQ号 (默认不转换QQ号，因此上述用法 1、2将不转换QQ号)
-help 显示此帮助信息

HomePages：http://www.annhe.net
Copyright @ annhe email:admin@annhe.net
