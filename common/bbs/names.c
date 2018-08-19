
#include "cmbbs.h"
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include "pttstruct.h"

// SLMT: 檢查是否是合法 id
// 檢查規則：長度在 2~IDLEN, 第一個字是英文, 後面都是英文或數字
int is_validuserid(const char *id)
{
    int len, i;
    if(id==NULL)
	    return 0;
    len = strlen(id);

    if (len < 2 || len>IDLEN)
	    return 0;

    if (!isalpha(id[0]))
	    return 0;
    for (i = 1; i < len; i++)
	    if (!isalnum(id[i]))
	        return 0;
    return 1;
}

