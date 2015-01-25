#include <linux/module.h>
#include <linux/init.h>
#include <wlioctl.h>
#include <linux/string.h>
#include <linux/errno.h>
#include <hw_wifi.h>

/************�޸ļ�¼************************
�汾:      	����:              		˵��:
V01			2014/07/02			���ӹ����������


**************************************************/
#define BLACK_LIST_MAX 3
char *black_list[BLACK_LIST_MAX]={"IR", "CU", "KP"};


/*
�ж��Ƿ������޵Ĺ���: 
����ֵ:
0: �������޹���
1: �����޹���
*/
int is_blacklist_country(char *country)
{
	int i = 0;

	if(country == NULL){
		return 0;
	}

	printk(KERN_ERR "input country code: %s.\n", country);
	for(i=0; i<BLACK_LIST_MAX; i++){
		 if(strncmp(black_list[i], country, strlen(black_list[i])) == 0){
			return 1;
		 }
	}

	return 0;
}



