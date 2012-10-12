#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/errno.h>
#include <stdlib.h>

#include <readline/readline.h>
#include <readline/history.h>

#include <regex.h>

#include "include/cJSON.h"
#include "include/cli_def.h"
                  
extern cJSON *datas;

static int rv;
int dispatcher(cJSON *dsp,int funnum)
{                                 
	int rv;   
	switch(funnum){
	case 0:
		rv = cli_set_user_data(dsp);
		break;
	case 1:
		rv = cli_show_udf(dsp);
		break;
	case 2:
		rv = cli_show_ruleset(dsp);
		break;
	case 3:
		rv = cli_add_class_entry(dsp);
		break;
	case 4:
		rv = cli_reboot_system(dsp);
		break;
	case 5:
		rv = cli_reset_system(dsp);
		break;
	case 6:
		rv = cli_restore_configuration(dsp);
		break;
	case 7:
		rv = cli_save_config(dsp);
		break;
	case 8:
		rv = cli_show_config_file(dsp);
		break;
	case 9:
		rv = cli_show_system(dsp);
		break;
	case 10:
		rv = cli_show_system_verbose(dsp);
		break;
	case 11:
		rv = cli_show_board_info(dsp);
		break;
	case 12:
		rv = cli_httpd_start_restrat(dsp);
		break;
	case 13:
		rv = cli_httpd_stop(dsp);
		break;
	case 14:
		rv = cli_httpd_restore_configuration(dsp);
		break;
	case 15:
		rv = cli_get_httpd_status(dsp);
		break;
	case 16:
		rv = cli_show_failover(dsp);
		break;
	case 17:
		rv = cli_show_backup(dsp);
		break;
	case 18:
		rv = cli_exit_shell(dsp);
		break;
	case 19:
		rv = cli_set_is(dsp);
		break;
	case 20:
		rv = cli_set_am(dsp);
		break;
	case 21:
		rv = cli_set_debug_flag(dsp);
		break;
	case 22:
		rv = cli_r_int(dsp);
		break;
	case 23:
		rv = cli_r_int_int(dsp);
		break;
	case 24:
		rv = cli_r_vsc(dsp);
		break;
	case 25:
		rv = cli_show_subtable(dsp);
		break;
	case 26:
		rv = cli_start_capture(dsp);
		break;
	default:
		printf("dispatcher error.\n");
		rv = -1;
	}
	cJSON_Delete(dsp);
	return rv;
	} 
