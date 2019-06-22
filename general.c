#include<general.h>
 
/**
 *  @brief 分解路径字符串，从中取出文件名，及扩展名
 *  @param [in] path    路径字符串
 *  @param [out] dir    目录
 *  @param [out] fname  文件名
 *  @param [out] fext   扩展名
 */
void split_path(const char *path, char *dir, char *fname, char *fext)
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
const char *getline(FILE *fp)
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
const char *getext(const char *fname)
{
    static char ext[1024];
    split_path(fname, NULL, NULL, ext);
    return ext;
}

FileType find_filename_suffix(const char *name, file_suffix *type_table) {
	
	/****
	根据name到表中查询。
	返回类型。
	****/
}

/**这里应该不用这么写，将类型和TXT以及EXE建立一个结构体，注册的时候，注册该表。
在is_filename_suffix_exist中对该表进行查询，并返回类型。
**/
FileType detect_file_type(file_t *fname, file_suffix *type_table)
{
	assert(fname!=NULL && type_table);
	FileType type;
	const char *file_name = fname.str();
	
	type = find_filename_suffix(file_name, type_table);
	return type;
}
void register_opt(const char *path, file_op *p，int exist_num) {
	assert(path && p);
	/**
	解析json文件，然后存到p指定的表中。
	json格式
	{"pdf":"/xxx/xxx", "xml":"/xxx/xxx"};
	**/
}
void register_type(const char *type_config_path, file_suffix *type_table) {
	assert(type_config_path && type_table);
	/**
	解析ini文件，然后存到文件表中。
	ini格式
	
	[EXEC]
	 .sh
	 .exe
	 
	[TXT]
	 .md
	 .txt
	**/
}


