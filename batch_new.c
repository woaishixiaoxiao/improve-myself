/***
分析该函数为生成函数，里面大量耦合了易变的部分。
1、管道命令(popen pclose)feof、geline以及shell_exec均是linux下的命令。该版本不能在win上面跑，需要需改代码。
2、opt也就是不同的命令选项，今后可能也会有扩展，包括类型文件。
3、判断文件的类型，这个也是不确定的技术，之后可能会有新的方案。

上述耦合带来的弊端
1、对生产函数做单元测试，不能准确确定好位置。
2、易变的部分耦合在一起，需要经常改动，并且改动的动作较大，代码不易被维护。上述三点任意的单个或组合发生了变化，修改很大。
3、代码不易被扩展。比如增加了opt选项，或者增加了文件类型，或者文件类型对应的函数，改动会很大。

filelist_t* query_files(const char* dirname, int need_dir);
int get_file_count(filelist_t *files);
file_t *file = get_file(filelist_t *files, int idx);
这三个函数要考虑跨平台以及执行的函数(show_text_file等)，函数数量很多，最好将他们分别写到win32_specific.hpp以及linux_specific.hpp文件中去。并在此文件中:
ifdef WIN32
#include “win32_specific.hpp”
#endif

#ifdef UNIX
#include “unix_specific.hpp”
#endif
采用hpp文件的原因是:编译的时候跨平台的函数不需要链接其他跨平台源文件中同名函数，从而防止编译报错。
另外，因为时间很紧，下面具体以展示重构代码，WIN版的hpp文件及其中的函数并未实现 仅实现Linux版本的，并且
代码并没有编译过，甚至还存在各种小的不规范。

***/
#ifdef WIN32
#include “win32_specific.hpp”
#endif

#ifdef UNIX
#include “unix_specific.hpp”
#endif
#inlcude<general.h>

const char *type_config_path = "/home/file_type.ini";
const char *opt_config_path = "/home/opt_config.json";
static file_op s_ops[20] = {
	{SHOW, TXT_FILE, {NULL,show_text_file} },
    {SHOW, EXE_FILE, {NULL,show_bin_file} },
    {EXEC, EXE_FILE, {NULL,exec_file} }
};
static file_suffix s_file_suffix[100];
static int s_ops_num = 3;
static void batch(const char *dir, OPT opt)
{
    filelist_t *files = query_files(dirname, NEED_SUBDIR);
    int cnt = get_file_count(files);
    int i, j;
    for (i = 0; i < cnt; ++cnt) {
        file_t *file = get_file(files, i);
        FileType type = detect_file_type(file);

        for (j = 0; j < dimof(s_ops); ++j) {
            if (s_ops[j].opt == opt && (type & s_ops[j].type) != 0) {
				void *exec_path = s_ops[j].oper.param;
                s_ops[j].oper.op(file, exec_path);
            }
        }
    }
	delete files;
}
/**
满足拓展的三个需求 需求中说配置文件中含有文件类型以及该类型的指定程序。
我自己的理解是，指定的程序是一个字符串路径，因为存在指定程序了，所以不需要main函数指定动作了，动作默认均是执行。
新创一个函数，根据文件名字函数指针赋予的值就是exec_file函数。
一开始需要根据文件对表s_ops进行注册。
**/
int main(int argc, char *argv[])
{
	/***
    if (argc < 3) {
        fprintf(stderr, "usage: batch <dir-name> <command>\n");
        return -1;
    }
    ***/
	if (argc < 2) {
        fprintf(stderr, "usage: batch <dir-name> <command>\n");
        return -1;
    }
	OPT op;
	if( argc <= 2) {
		op = EXEC;
	}
	register_opt(opt_config_path, s_ops, s_ops_num);//对文件类型机及其对应的操作进行注册
	register_type(type_config_path, s_exec_file_suffix);//对可执行文件后缀名类型注册。
	register_type(type_config_path, s_txt_file_suffix);//对文本文件后缀名类型注册
	batch()
    return 0;
}
