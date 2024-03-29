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

