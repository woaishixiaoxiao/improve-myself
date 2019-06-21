#include <unistd.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#ifndef _MAX_PATH
#define _MAX_PATH 1024
#endif
/**
 *  @brief 分解路径字符串，从中取出文件名，及扩展名
 *  @param [in] path    路径字符串
 *  @param [out] dir    目录
 *  @param [out] fname  文件名
 *  @param [out] fext   扩展名
 */
static void split_path(const char *path, char *dir, char *fname, char *fext)
{
    unsigned int len = 0;
    unsigned int dirend = 0;         //目录名的结束位置
    unsigned int fnend = 0;          //文件名的结束位置

    assert(path);

    len = strlen(path);
    if (len >= _MAX_PATH)
        len = _MAX_PATH - 1;
    dirend = 0;

    //  从后往前找,找到'.'或目录分界符
    for (fnend = len; fnend > 0; --fnend) {
        char c = path[fnend - 1];
        if (c == '.') {
            --fnend;            //fnend指向'.'的位置
            dirend = fnend;     //dirend也指向点的位置
            break;
        } else if (c == '/' || c == '\\') { //说明文件名中没有'.'
			dirend = fnend;     //dirend指向'/'后面一个位置,目录名包含了最后一个'/'
            fnend = len;        //fnend指向路径末尾
            break;
        }
    }
    if (fnend == 0 && path[0] != '.') {   //什么分隔符都没有找到，纯粹文件名
        dirend = 0;
        fnend = len;
    } else {
        //  找到最后一个目录分界符,dirend指向的是分界符的后一个位置
        for (; dirend > 0; --dirend) {
            char c = path[dirend - 1];
            if (c == '/' || c == '\\')
                break;
        }
    }
    if (dir) {
        memcpy(dir, path, dirend);
        dir[dirend] = '\0';
    }
    if (fname) {
        memcpy(fname, path + dirend, fnend - dirend);
        fname[fnend - dirend] = '\0';
    }
    if (fext) {
        memcpy(fext, path + fnend, len - fnend);
        fext[len - fnend] = '\0';
    }
}
/**
 *  获取一行数据
 */
static const char *getline(FILE *fp)
{
    static char linebuf[1024];
    int len;
    char *p = fgets(linebuf, sizeof(linebuf), fp);
    if (!p)
        return p;
	len = strlen(p);
    if (len > 1 && p[len - 1] == '\n')
        p[len - 1] = '\0';
    return p;
}
/**
 * 从文件名中获取文件的后缀
 */
static const char *getext(const char *fname)
{
    static char ext[1024];
    split_path(fname, NULL, NULL, ext);
    return ext;
}
/**
 *  执行命令行
 */
static int shell_exec(const char *cmd, ...)
{
    char buf[1024];
    va_list ap;

    va_start(ap, cmd);
    vsnprintf(buf, sizeof(buf), cmd, ap);
    va_end(ap);

    int ret = system(buf);

    return ret;
}
/**
 *  执行批处理操作
 */
void batch(const char *dir, const char *opt)
{
    const char *fname;
    const char *fext;
    char cmd[2048];
    
    snprintf(cmd, sizeof(cmd), "ls -1 \"%s\"", dir);
    
    FILE *fp = popen(cmd, "r");                     //1. 执行ls命令，获取文件列表
    if (!fp) {
        fprintf(stderr, "popen '%s' failed\n", dir);
        return;
    }
    while (!feof(fp)) {
        fname = getline(fp);                        //2. 获取一行数据，即一个文件名
		if (!fname)
            break;
        fext = getext(fname);                       //3. 获取文件的后缀名

        if (strcmp(opt, "show") == 0) {             //4. 要求显示文本文件内容
            char *sub = strstr(".md;.txt;.log", fext);
            if (sub && sub[strlen(fext)] == ';') {  //5. 通过文件名后缀，判断是否文本文件
                shell_exec("cat %s", fname);        //6. 执行cat命令，显示文件内容
            }
        } else if (strcmp(opt, "exec") == 0) {      //7. 要求执行文件
            char *sub = strstr(".exe;*.sh;*.bat", fext);
            if (sub && sub[strlen(fext)] == ';') {  //8. 通过文件名后缀，判断是否可执行文件
                shell_exec("%s", fname);            //9. 通过shell，执行该文件
            }
        }
    }
    pclose(fp);
}

/***
分析该函数为生成函数，里面大量耦合了易变的部分。
1、管道命令(popen pclose)feof、geline以及shell_exec均是linux下的命令。该版本不能在win上面跑，需要需改代码。
2、opt也就是不同的命令选项，今后可能也会有扩展，包括类型文件。
3、判断文件的类型，这个也是不确定的技术，之后可能会有新的方案。
上述耦合带来的弊端
1、对生产函数做单元测试，不能准确确定好位置。
2、易变的部分耦合在一起，需要经常改动，并且改动的动作较大，代码不易被维护。上述三点任意的单个或组合发生了变化，修改很大。
3、代码不易被扩展。比如增加了opt选项，或者增加了文件类型，或者文件类型对应的函数，改动会很大。

另外，因为时间很紧，下面具体以展示重构代码，hpp文件及其中的函数并未实现
***/
typedef void (*do_file)(FILE *p_file);
void show_text_file_win(FILE *pf) {
}
void show_text_file_linux(FILE *pf) {
}
void show_bin_file_win(FILE *pf)
typedef enum {
    SHOW,  
    EXEC
}OPT;
typedef enum {
    TXT_FILE,
	MD_FILE,
    EXE_FILE,
	SH_FILE
}FileType;
struct file_op {
   OPT opt;
   FileType type;
   do_file oper;
};
static file_op s_ops = {
	{SHOW, TXT_FILE, show_text_file},
    {SHOW, EXE_FILE, show_bin_file},
    {EXEC, EXE_FILE, exec_file}
	
};
/*
这三个函数要考虑跨平台以及执行的函数(show_text_file等)，函数数量很多，最好将他们分别写到win32_specific.hpp以及linux_specific.hpp文件中去。并在此文件中:
ifdef WIN32
#include “win32_specific.hpp”
#endif

#ifdef UNIX
#include “unix_specific.hpp”
#endif

filelist_t* query_files(const char* dirname, int need_dir);
int get_file_count(filelist_t *files);
file_t *file = get_file(filelist_t *files, int idx);
*/
void batch(const char *dir, const char *opt)
{
    filelist_t *files = query_files(dirname, NEED_SUBDIR);
    int cnt = get_file_count(files);
    int i, j;
    for (i = 0; i < cnt; ++cnt) {
        file_t *file = get_file(files, i);
        int type = detect_file_type(file);

        for (j = 0; j < dimof(s_ops); ++j) {
            if (s_ops[j].opt == opt && (type & s_ops[j].type) != 0) {
                s_ops[j].do_file(file);
            }
        }
    }
}
int main(int argc, char *argv[])
{
    if (argc < 3) {
        fprintf(stderr, "usage: batch <dir-name> <command>\n");
        return -1;
    }
    
    batch(argv[1], argv[2]);

    return 0;
}
