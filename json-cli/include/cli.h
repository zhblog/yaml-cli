#ifndef _CLI_H_
#define _CLI_H_

#include "def.h"

#define errprint(fmt, args...) fprintf(stderr, fmt, ## args)

#define PROMPT(str)			"     " str
#define PROMPT2(prompt, format)		"     %-40s" format, prompt

#define MAX_MODULE_NUMBER		5
#define MAX_PORT_NUMBER		24


#define CLI_GE_PORT(X)          ((X) - 1)
#define CLI_XGE_PORT(X)         ((X) + 25)
#define CLI_CX4_MAP             CLI_XGE_PORT(2)

struct css
{
	char bytes[32][8];
	int num;
};

extern int ignore;
extern int console;
extern int sig_type;

extern struct board_info_t g_board_info;

extern char *cli_error(int errcode);

extern int cli_init(char*);
extern int cli_fini(void);
extern int start_cli(char *, FILE*);

extern int get_console(void);
extern void setalarm(int, int);

extern int banner_maker(char * );
extern int cli_print_logo(void);

extern int cli_show_board_info(void);
extern int cli_show_system(int verbose);

extern int cli_show_interface_counter(void);
extern int cli_show_port_counter(int, int);
extern int cli_show_ppe_counter(int);
extern int cli_show_port_info(unsigned int);
extern int cli_show_port_setting(unsigned int);
extern int cli_show_port_status(unsigned int);
extern int cli_port_info(int *, unsigned int *, unsigned int *);

extern int cli_set_port_autoneg(unsigned int port_map, int able);
extern int cli_set_port_pause(unsigned int port_map, int able);
extern int cli_set_port_speed(unsigned int port_map, int able);
extern int cli_set_port_phymode(unsigned int port_map, int phymode);
extern int cli_set_port_serial(unsigned int port_map, unsigned int ports, int flag);
extern int cli_set_port_switch(unsigned int port_map, int able);
extern int cli_set_port_framemax(unsigned int ports, int max_frame);
extern int cli_show_port_status(unsigned int ports);
extern int cli_show_port_setting(unsigned int port_map);
extern int cli_show_port_info(unsigned int ports);
extern int cli_show_interface_counter(void);
extern int cli_show_ppe_counter(int ppe);
extern int cli_show_port_counter(int ports, int verbose);
extern int cli_clear_port_counter(int ports);
extern int cli_clear_interface_counter(void);
extern int cli_clear_ppe_counter(int ppe);

extern int cli_set_cat_port(int cat_id, int mask, struct cat_port_t *cat);
extern int cli_show_cat_port(int cat_id);
extern int cli_set_isl(int able);
extern int cli_set_timestamp_mode(int mode);
extern int cli_set_timestamp(int date, int time);
extern int cli_show_timestamp(int verbose);
extern int cli_show_function(void);
extern int cli_set_ruleflag(int ruleflag);
extern int cli_show_ruleflag(void);
extern int cli_strip_vlan(int strip_mode);
extern int cli_show_stripvlan(void);

extern int cli_set_user_data(int id, int offset_type, int offset, int data, int mask);
extern int cli_set_stage3_user_data(int id, int offset_type, int offset, int data, int mask);
extern int cli_add_class_entry(char *rule_name, int rule_id, struct classentry_t *class_entry, int *index);
extern int cli_delete_class_entry(char *rule_name, int rule_id, int start, int end);
extern void printf_class_entry(struct classentry_t* class_entry_p);
extern void cli_print_rule( struct exp_rule_t *rl);
extern void print_ruleset(struct exp_ruleset_t *rst);
extern int cli_show_ruleset(char* ruleset_name);
extern int cli_show_rule(char* ruleset_name,int rule_id);
extern int cli_delete_rule_by_classentry(char *rule_name, int rule_id, struct classentry_t *class_entry, int *index);
extern int cli_check_class_entry(char *rule_name, struct classentry_t *class_entry);
extern int cli_set_rule_forward_action(char *rule_name, int rule_id, struct forward_t *forward);
extern int cli_adj_hashmap(char *rule_name, int rule_id, int src, int dst);
extern int cli_get_hashmap_ip(char* rule_name, int rule_id,int sip);
extern int cli_get_hashmap_ipport(char *rule_name, int rule_id, int sip, int dip, int sport, int dport);
extern int cli_get_entrynum(char *rule_name, int rule_id);
extern int cli_set_ruleset_hash_mask(char *rule_name, int sip_mask, int dip_mask, int sport_mask, int dport_mask);
extern int cli_set_ruleset_hash_ex_mask(char *rule_name, int ua_mask, int ub_mask,int uc_mask);
extern int cli_set_ruleset_hash_ex_offset(char* rule_name, unsigned int ua_offset, unsigned int ub_offset, unsigned int uc_offset);

extern int cli_reset_system(void);
extern int cli_restore_configuration(void);
extern int cli_save_config(void);
extern int cli_get_config_file(void);
extern int cli_get_config_file(void);
extern int cli_show_config_file(void);
extern int cli_op_httpd(int op);
extern int cli_get_httpd_status(void);
/*CQ add*/
extern int cli_op_snmp(int op);
extern int cli_get_snmp_status(void);
extern int cli_show_snmp_conf(void);
extern int cli_check_snmp_param(int param, char *param_value);
extern int cli_set_snmp_param(int param, char *param_value);

extern int cli_set_debug_flag(int );
extern int cli_download_fpga_image(char *);
extern int cli_init_framer(int );
extern int cli_r_int(unsigned int );
extern int cli_r_int_int(unsigned int , int );
extern int cli_rr_int(unsigned int);
extern int cli_rr_int_int(unsigned int, int);
extern int cli_write_reg16(unsigned int, unsigned int);
extern int cli_r_vsc(int, unsigned int);
extern int cli_write_7280_register(unsigned int, unsigned int, unsigned int);
extern int cli_write_reg32(unsigned int, unsigned int);
extern int cli_start_capture(int where);
extern int cli_stop_capture(void);
extern int cli_show_capture(int);
extern int cli_show_subtable(char*, int);
extern int cli_debug_add_class_entry(char *rule_name, struct classentry_t *class_entry, int n_repeat);
extern int cli_debug_probe_srv(void);

extern int cli_show_failover(int );
extern int cli_show_backup(int );
extern int cli_exit_shell(void );
extern int cli_set_port_phymaster(unsigned int ,int );
extern int cli_set_port_forcelink(int , int , int );
extern int cli_set_port_loop(int, int);
extern int cli_set_timestamp_format(int );
extern int cli_set_backup_interval(int );
extern int cli_set_failover_interval(int );

extern int cli_set_ruleset_hash_single_mask( char* , int , int );
extern int cli_set_ruleset_hash_single_offset( char* , unsigned int , int );
extern int cli_set_entry_unique_check(int );
extern int cli_show_udf(int );

#endif

