#ifndef GENERAL_H
#define GENERAL_H
#endif

#include <string.h>
#include <assert.h>

#ifndef _MAX_PATH
#define _MAX_PATH 1024
#endif
typedef vector<string> filelist_t;  //作业均是用c实现，c语言中什么库都没有提供，而且作业的重点是练习解耦，即使这里编译不过也无所谓了
typedef string file_t;

typedef void (*file_opt)(file_t *p_file);
typedef enum {
    SHOW,  
    EXEC
}OPT;
typedef enum {
    TXT_FILE,
    EXE_FILE,
	/***事先将所有的文件类型补充完整****/
	.........
}FileType;
struct do_file {
    void *param;
	file_opt op;
};
struct file_op {
    OPT opt;
    FileType type;
    do_file oper;
};
struct file_suffix {
	FileType type;
	string filename;
};

FileType find_filename_suffix(const char *name, file_suffix *type_table);
void register_type(const char *type_config_path, const char *file_suffix_table);
void register_opt(const char *path, file_op *p);
void split_path(const char *path, char *dir, char *fname, char *fext);
const char *getline(FILE *fp);
const char *getext(const char *fname);
FileType detect_file_type(file_t *fp);

