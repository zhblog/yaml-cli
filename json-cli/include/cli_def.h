//#include "def.h"

#define errprint(fmt,args...) fprintf(stderr,fmt,## args)
#define PROMPT(str)         "     " str
#define PROMPT2(prompt, format)     "     %-40s" format, prompt


#define MAX_UD_ID			15


#define CMD_L 32	
#define LINE_MAX_TOKEN 32	
#define ONE_COM_MAX_LEN 32*64	
#define TAB_V 	1
#define DOIT 	2
#define TAB_TOKEN_LEN 	100
#define REG_STR_LEN 	1024*8
#define JSON_DIR 	    "file/json"


#define SPTR  0x01
#define VPTR  0x02
#define IVAL  0x04
#define IVAL1 0x08
#define IVAL2 0x10
#define CSS   0x20
#define MAC   0x40
#define IPV6  0x80
#define IP    0x100
#define IPMASK  0x200



#define CLASSENTRY_IPVERSION_MASK 0x01
#define CLASSENTRY_PROTOCOL_MASK 0x02
#define CLASSENTRY_SIP_MASK 0x04
#define CLASSENTRY_DIP_MASK 0x08
#define CLASSENTRY_SPORT_MASK 0x010
#define CLASSENTRY_DPORT_MASK 0x20
#define CLASSENTRY_PORTS_MASK 0x8000
#define CLASSENTRY_ETHERTYPE_MASK 0x40000
#define CLASSENTRY_STAGE2_MASK 0x01000000

#define CLI_GE_PROT(X)  ((X) - 1)
#define CLI_XGE_PORT(X) ((X) + 25)
#define CLI_CX4_MAP     CLI_XGE_PORT(2)

cJSON *rootjson;
cJSON *json;
cJSON *datas;
cJSON *help;
cJSON *module;
cJSON *mapjsonlist;
typedef struct Scanner
{	
		char *sptr;
		void *vptr;
		int token;
		unsigned long ival;
		unsigned long ival1;
		unsigned long ival2;
		unsigned char css[32][8];     
		unsigned int mac[6];     
		unsigned int ipv6[8];
		unsigned int mask;
		/*
		 *	sptr 	0x01
		 *	vptr 	0x02
		 *	ival 	0x04
		 *	ival1	0x08
		 *	css		0x10
		 *	mac		0x20
		 *	ipv6	0x40
		 * */
}Scanner;

struct ud_t {
    int offset;
    unsigned int data;
    unsigned int mask;
};


struct classentry_t {
    unsigned int index;
    unsigned int key_mask;
    unsigned int ud_mask;
    
    int protocol;
    int ipversion;    
    unsigned int sip;
    unsigned int sip_mask;    
    unsigned int dip;
    unsigned int dip_mask;
    
    unsigned short min_sport;
    unsigned short max_sport;
    unsigned short sport_mask;
    unsigned short min_dport;
    unsigned short max_dport;
    unsigned short dport_mask;
    unsigned char tcp_flag;
    int min_size;
    int max_size;
    
    
    char if_name[256];
    
    int stage2_tag;
    int ipfragment;
    int inports;
    
    unsigned short ethertype;
    
    struct ud_t uds[(MAX_UD_ID + 1)/2];
};

extern void __print_scanner(Scanner sc);
extern Scanner for_cli_rpc(char *reg_str, char *val_str);
extern Scanner get_val(char *key,cJSON *dsp);
extern int dispatcher(cJSON *fmt,int fun_num);
extern void free_readline();
