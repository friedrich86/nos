/*
nos app
-------------------------------------------------
网络管理OS
基础组件
-------------------------------------------------
杨晓君
*/

#include <stdio.h>
#include <stddef.h>
#include <time.h>
#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

/*
文件夹操作
*/
#include <dirent.h>

/*
ti osa头文件
*/
#include "osa_thr.h"

/*
mqtt头文件
*/
#include <mosquitto.h>
#include <mqtt_protocol.h>

/*
json-c头文件
*/
#include "json.h"

/*
github开源链表操作库头文件
*/
#include "list.h"

/*
cron表达式解析模块
*/
#include "ccronexpr.h"


/*
连接内外部的mosquitto对象实例
*/
struct mosquitto * nos_ext_mosq = NULL;
struct mosquitto * nos_in_mosq = NULL;

/*
连接外部broker的mqtt线程
*/
static void nos_ext_br_thread(void)
{
	int my_mos_err;
    OSA_printf("nos app: enter nos_ext_br_thread.\n");
/*
初始化mosquitto库
*/
	mosquitto_lib_init();

/*
新建一个mqtt对象
*/
	nos_ext_mosq = mosquitto_new(NULL, TRUE, NULL);

	if(!nos_ext_mosq)
	{
		/*
		MQTT初始化失败，打印报错，写入日志，直接退出；
		*/
        OSA_printf("CRITICAL ERROR! \n");
        system("logger in nos app,mosquitto_new for external broker failed!");
        return;
	}
	
	/*
	设置订阅回调函数
	*/
	mosquitto_subscribe_callback_set(nos_ext_mosq, ssched_subs_callback);
	/*
	设置连接回调函数
	*/
	mosquitto_connect_v5_callback_set(nos_ext_mosq, ssched_con_callback);
	/*
	设置收到消息回调函数
	*/
	mosquitto_message_v5_callback_set(nos_ext_mosq, ssched_mes_callback);

	/*
	进行实际连接，连接到内部broker;
	*/
	my_mos_err = mosquitto_connect(nos_ext_mosq, SSCHED_BROKER_HOST, SSCHED_BROKER_PORT, 60);
	if (my_mos_err == MOSQ_ERR_SUCCESS)
	{
		OSA_printf("nos app : connect External broker done.\n");
	}
	else
	{
		OSA_printf("nos app : connect External broker failed,return %d!\n", my_mos_err);
	}

	mosquitto_loop_forever(nos_ext_mosq, -1, 1);

	mosquitto_destroy(nos_ext_mosq);
	mosquitto_lib_cleanup();

}


int main(void)
{
	#ifdef SSCHED_DEBUG_EN
		OSA_printf("nos app : RUN!\n");
	#endif
}
