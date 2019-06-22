#ifndef UNIX_SPECIFIC_H
#define UNIX_SPECIFIC_H
#include <unistd.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#ifndef _MAX_PATH
#define _MAX_PATH 1024

#endif
void show_text_file(file_t *flie, void *param) {
}
void show_bin_file(file_t *file, void *param) {
}
void exec_file(file_t *file, void *param) {
}
void exec_file_with_name(file_t *file, void *parm) {
	assert(file && param);
	const char *exec_path = (const char *)param;
	const char *file_name = file->str();//转化成c风格字符串
	system(exec_path, file_name);
}
filelist_t* query_files(const char* dirname, int need_dir) {
	char cmd[2048];
	const char *fname;
	filelist_t *contain_file = new filelist_t; 
	if(need_dir) {
		snprintf(cmd, sizeof(cmd), "ls -1 \"%s\"", dirname);    
		FILE *fp = popen(cmd, "r");                     //1. 执行ls命令，获取文件列表
		if (!fp) {
			fprintf(stderr, "popen '%s' failed\n", dir);
			return NULL;
		}
		while (!feof(fp)) {
			string str( getline(fp) );
			contain_file->push_back(str);
		}
		pclose(fp);		
	}else {
		string str(dirname);
		contain_file->push_back(str);
	}
	return contain_file;
}

int get_file_count(filelist_t *files) {
	assert(files);
	return files->size();
}
file_t *get_file(filelist_t *files, int idx) {
	assert(idx <= files->size());
	return &files[idx - 1];
}