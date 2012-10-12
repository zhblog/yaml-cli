#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "include/cJSON.h" 
#include "include/cli_def.h" 

#define do_scan(str1,str2) scan(str1, str2)
#define MAXLEN 100
#define MAKE_RULEID(stage, id)  (((stage) << 24) + (id))

Scanner scanner;
int ReplaceStr(char *sSrc, char *sMatchStr, char *sReplaceStr)
{
    int  StringLen;
    char caNewString[MAXLEN];
    char *FindPos = strstr(sSrc, sMatchStr);
    if( (!FindPos) || (!sMatchStr) )
        return -1;
    while( FindPos )
    {
        memset(caNewString, 0, sizeof(caNewString));
        StringLen = FindPos - sSrc;
        strncpy(caNewString, sSrc, StringLen);
        strcat(caNewString, sReplaceStr);
        strcat(caNewString, FindPos + strlen(sMatchStr));
        strcpy(sSrc, caNewString);
        FindPos = strstr(sSrc, sMatchStr);
    }
    return 0;
}


void  __print_scanner(Scanner sc)
{
    if(sc.sptr){
        printf("sptr:%s\n",sc.sptr);
    }
    if(sc.token){
        printf("token:%d\n",sc.token);
    }
    if(sc.mask){
        printf("mask:%d\n",sc.mask);
    }
    if(sc.ival1){
        printf("ival1:%ld\n",sc.ival1);
    }
    if(sc.ival){
        printf("ival:%ld\n",sc.ival);
    }
    if(sc.ival2){
        printf("ival2:%ld\n",sc.ival2);
    }
    if(sc.mac){
        printf("mac0:%d\n",sc.mac[0]);
        printf("mac1:%d\n",sc.mac[1]);
        printf("mac2:%d\n",sc.mac[2]);
        printf("mac3:%d\n",sc.mac[3]);
        printf("mac4:%d\n",sc.mac[4]);
        printf("mac5:%d\n",sc.mac[5]);
    }

}

void bin(int x)
{
    if (x<=0){
        return;
    }
    else
    {            
        bin(x/2);
        printf("%d",x%2);
    }
}


int interfaces_start_stop(int start, int stop)
{
    int i, result = 0;

    if (start > stop) {
        i = start;
        start = stop;
        stop = i;
    }

    for (i = start; i <= stop; i++)
    {
        result |= (1 << i);
    }

    return result;
}


int one_str(char *chs)
{
    char portstr[256];
    strcpy(portstr,chs);
    ReplaceStr(portstr, "xge1", "26");
    ReplaceStr(portstr, "xge2", "27");
    ReplaceStr(portstr, "cx4", "27");
    char start[512] = "";
    char stop[512] = "";
    sscanf(portstr,"%[^-]",start);
    sscanf(portstr,"%*[^-]-%s",stop);
    if(strlen(start)>0 && strlen(stop)>0)
        return interfaces_start_stop(atoi(start), atoi(stop));
    else{
        return (1 << atoi(portstr));
    }
}

static void fill_slot_port(char *portstr,int flag)
{
    int ins = 0;
    int i;
    int j;
    char *strtok_chs;
    char *free_chs;
    char *portchs[24];
    memset(portchs,0,24*sizeof(char*));
    for(i=0;;i++,portstr = NULL){
        portchs[i] = strtok(portstr,",");
        if (NULL == portchs[i]){
            break;
        }
    }
    for(j = 0;j<i;j++){
        ins |= one_str(portchs[j]);
    }
    //bin(ins >> 1);
    //printf("\n");
    if(flag == 1)
        scanner.ival = ins >> 1;
    if(flag == 2)	
        scanner.ival2 = ins >> 1;
}

static void fill_port_reg( char *chs)
{
    int ins = 0;
    int i;
    int j;
    char *strtok_chs;
    char *free_chs;
    char *portchs[24];
    memset(portchs,0,24*sizeof(char*));

    for(i=0;;i++,chs = NULL){
        portchs[i] = strtok(chs,",");
        if (NULL == portchs[i]){
            break;
        }
    }
    for(j = 0;j<i;j++){
        ins |= one_str(portchs[j]);
    }
    //bin(ins >> 1);
    //printf("\n");
    scanner.ival = ins >> 1;
    scanner.mask |= IVAL;
}

static void fill_smp(char *chs)
{
    //printf("fill_smp\n");
    char val0[512] = "";
    char val1[512] = "";
    char val2[512] = "";
    char val3[512] = "";
    sscanf(chs,"%[^/]",val1);
    sscanf(chs,"%*[^/]/%s",val0);
    sscanf(val0,"%[^/]",val2);
    sscanf(chs,"%*[^/]/%*[^/]/%s",val3);
    fill_slot_port(val1,1);	
    fill_slot_port(val3,2);
    if(strchr(val2,'r')){
        scanner.ival1 |= 1;
    }
    if(strchr(val2,'f')){
        scanner.ival1 |= 2;
    }
}

static void fill_integer(char *chs)
{
    //printf("fill_integer\n",chs);
    scanner.ival = atoi(chs);
    scanner.mask |= IVAL;
}

static void fill_inthex(char *chs)
{
    //printf("fill_hexdigit\n",chs);
    scanner.mask |= IVAL;
    scanner.ival = atoi(chs);
}

static void fill_hexdigit(char *chs)
{
    //printf("fill_hexdigit\n",chs);
    scanner.mask |= IVAL;
    scanner.ival = atoi(chs);
}

static void fill_ident(char *chs)
{
    //printf("fill_ident\n",chs);
    scanner.mask |= SPTR;
    scanner.sptr = chs;
}

static void fill_string(char *chs)
{
    //printf("fill_string\n",chs);
    scanner.sptr = chs;
    scanner.mask |= SPTR;
}

static void fill_char(char *chs)
{
    //printf(" fill_char\n",chs);
    scanner.sptr = chs;
    scanner.mask |= SPTR;
}

static void fill_ipaddr(char *chs)
{
    //printf(" fill_ipaddr\n",chs);
    scanner.ival = inet_network(chs);
    scanner.mask |= IVAL;
}

static void fill_ipmask_reg(char *chs)
{
    char val[512] = "";
    char mask[512] = "";
    sscanf(chs,"%[^/]",val);
    sscanf(chs,"%*[^/]/%s",mask);
    if(strlen(val)>0){
        scanner.mask |= IVAL1;
        scanner.ival1 = inet_network(val);
    }
    if(strlen(mask)>0){
        scanner.mask |= IVAL2;
        scanner.ival2 = inet_network(mask);
    }
}

static void fill_css(char *chs)
{
    int i, j, k, num;
    memset(&scanner, 0, sizeof(scanner));
    i = 0;
    j = 0;
    k = 0;
    num = 0;
    while(chs[i])
    {
        if(chs[i] == ':')
        {
            num ++;
            j ++;
            k = 0;
        }
        else if(j < 32 && k < 8)
        {
            scanner.css[j][k] = chs[i];
            k++;
        }
        i++;
    }
    scanner.ival = num +1;
}



static void fill_interfaces(char *chs)
{
    //printf(" fill_interfaces\n",chs);
    scanner.sptr = chs;
}

static void fill_protocolreg(char *chs)
{
    //printf("fill_protocolreg\n");
    if(strcmp("tcp",chs)==0){
        scanner.ival = 6;
    }
    else if(strcmp("udp",chs)==0){
        scanner.ival = 17;
    }
    else if(strcmp("icmp",chs)==0){
        scanner.ival = 1;
    }
    else{
        scanner.ival = atoi(chs);
    }
    scanner.mask |= IVAL;
}

static void fill_udreg(char *chs)
{
    //printf("fill_udreg");
    char val[512] = "";
    char mask[512] = "";
    sscanf(chs,"%[^/]",val);
    sscanf(chs,"%*[^/]/%s",mask);
    if(strlen(val)>0){
        scanner.ival1 = atoi(val);
        scanner.mask |= IVAL1;
    }
    if(strlen(mask)>0){
        scanner.ival2 = atoi(mask);
        scanner.mask |= IVAL2;
    }
}

static void fill_sizereg(char *chs)
{
    //printf(" fill_sizereg\n");
    char val[512] = "";
    char mask[512] = "";
    sscanf(chs,"%[^-]",val,mask);
    sscanf(chs,"%*[^-]-%s",mask);
    if(strlen(val)){
        scanner.ival1 = atoi(val);
        scanner.mask |= IVAL1;
    }
    if(strlen(mask)>0){
        scanner.ival2 = atoi(mask);
        scanner.mask |= IVAL2;
    }
}

static void fill_rulenum_reg(char *chs)
{
    int stage = 0, index = 0;
    char val[512] = "";
    char mask[512] = "";
    sscanf(chs,"%[^:]",val,mask);
    sscanf(chs,"%*[^:]:%s",mask);
    stage = atoi(val);
    index = atoi(mask);
    scanner.ival = MAKE_RULEID(stage,index);
    scanner.mask |= IVAL;

}

static void fill_interfaceclass_reg(char *chs)
{
    char val[128] = "";
    char mask[128] = "";
    sscanf(chs,"%[^=]",val);
    sscanf(chs,"%*[^=]=%s",mask);
    char portstr[256];
    strcpy(portstr,mask);
    /*
       if(strstr(portstr,"xge1")){
       if( port_all_map !=0xcffffff ) 
       {
    //errprint("%s have not XGE ports.\n",optiway_get_board_part_number());
    scanner.error = 1;
    return 0;
    }
    ReplaceStr(portstr, "xge1", "27");
    }
    if(strstr(portstr,"xge2")){
    if( port_all_map !=0xcffffff ) 
    {
    //errprint("%s have not XGE ports.\n",optiway_get_board_part_number());
    scanner.error = 1;
    return 0;
    }
    ReplaceStr(portstr, "xge2", "28");
    }
    if(strstr(portstr,"cx4")){
    if( port_all_map !=0x8ffffff ) 
    {
    //errprint("%s have not cx4 ports.\n",optiway_get_board_part_number());
    scanner.error = 1;
    return 0;
    }
    ReplaceStr(portstr, "cx4", "28");
    }*/
    scanner.ival = atoi(portstr);	
}


static void fill_stageclass_reg(char *chs)
{
    char val[512] = "";
    char mask[512] = "";
    sscanf(chs,"%[^=]",val,mask);
    sscanf(chs,"%*[^=]=%s",mask);
    fill_integer(mask);
}

static void fill_ethertype_reg(char *chs)
{

    int eth = 0x8864;
    if(atoi(chs))
    {
        eth = atoi(chs);
    }
    scanner.ival = eth;
    scanner.mask |= IVAL;
    //if (($3 <0x0 ) || ($3 > 0xffff))
}

static void fill_ipversion_reg(char *chs)
{
    int ipversion = 0;
    if(strcmp(chs,"ipv4") == 0)	
        ipversion = 1;
    scanner.ival = ipversion;
    scanner.mask |= IVAL;
}

static void fill_ruleid(char *chs)
{
    char val[512] = "";
    char mask[512] = "";
    sscanf(chs,"%[^-]",val,mask);
    sscanf(chs,"%*[^-]-%s",mask);
    printf("chs:%s\n",chs);
    printf("val:%s\n",val);
    printf("mask:%s\n",mask);
}

static void fill_tcpflag_reg(char *chs)
{
    int key_mask = 0;
    int tcp_flag = 0;
    char tcpflagstr[256];
    strcpy(tcpflagstr,chs);
    ReplaceStr(tcpflagstr, "!fin", "1");
    ReplaceStr(tcpflagstr, "!syn", "2");
    ReplaceStr(tcpflagstr, "!rst", "3");
    ReplaceStr(tcpflagstr, "!psh", "4");
    ReplaceStr(tcpflagstr, "!ack", "5");
    ReplaceStr(tcpflagstr, "!urg", "6");
    ReplaceStr(tcpflagstr, "fin", "a");
    ReplaceStr(tcpflagstr, "syn", "b");
    ReplaceStr(tcpflagstr, "rst", "c");
    ReplaceStr(tcpflagstr, "psh", "d");
    ReplaceStr(tcpflagstr, "ack", "e");
    ReplaceStr(tcpflagstr, "urg", "f");

    if(strstr(tcpflagstr,"a"))
    {
        key_mask |= 0x0040;
        tcp_flag |= 0x01;
    }
    if(strstr(tcpflagstr,"b"))
    {
        key_mask |= 0x0080;
        tcp_flag |= 0x02;
    }
    if(strstr(tcpflagstr,"c"))
    {
        key_mask |= 0x0100;
        tcp_flag |= 0x04;
    }
    if(strstr(tcpflagstr,"d"))
    {
        key_mask |= 0x0200;
        tcp_flag |= 0x08;
    }
    if(strstr(tcpflagstr,"e"))
    {
        key_mask |= 0x0400;
        tcp_flag |= 0x10;
    }
    if(strstr(tcpflagstr,"f"))
    {
        key_mask |= 0x0800;
        tcp_flag |= 0x20;
    }
    if(strstr(tcpflagstr,"1"))
    {
        key_mask |= 0x0040;
        tcp_flag &= (~0x01);
    }
    if(strstr(tcpflagstr,"2"))
    {
        key_mask |= 0x0080;
        tcp_flag &= (~0x02);
    }
    if(strstr(tcpflagstr,"3"))
    {
        key_mask |= 0x0100;
        tcp_flag &= (~0x04);
    }
    if(strstr(tcpflagstr,"4"))
    {
        key_mask |= 0x0200;
        tcp_flag &= (~0x08);
    }
    if(strstr(tcpflagstr,"5"))
    {
        key_mask |= 0x0400;
        tcp_flag &= (~0x10);
    }
    if(strstr(tcpflagstr,"6"))
    {
        key_mask |= 0x0800;
        tcp_flag &= (~0x20);
    }
    scanner.ival1 = tcp_flag;
    scanner.ival2 = key_mask;
    scanner.mask |= IVAL1;
    scanner.mask |= IVAL2;
}

static void fill_hash_mode(char *chs)
{
    if(strcmp(chs,"s")==0)
    {
        scanner.ival = 1;
        scanner.mask |= IVAL;
    }
    if(strcmp(chs,"d")==0)
    {
        scanner.ival = 2;
        scanner.mask |= IVAL;
    }
    if(strcmp(chs,"m")==0)
    {
        scanner.ival = 3;
        scanner.mask |= IVAL;
    }
    if(strcmp(chs,"a")==0)
    {
        scanner.ival = 5;
        scanner.mask |= IVAL;
    }
    if(strcmp(chs,"b")==0)
    {
        scanner.ival = 6;
        scanner.mask |= IVAL;
    }
    if(strcmp(chs,"ab")==0)
    {
        scanner.ival = 7;
        scanner.mask |= IVAL;
    }
    if(strcmp(chs,"c")==0)
    {
        scanner.ival = 8;
        scanner.mask |= IVAL;
    }
    if(strcmp(chs,"abc")==0)
    {
        scanner.ival = 9;
        scanner.mask |= IVAL;
    }
}

static fill_copylist(char *chs)
{
    int copy_ports[32] = {0};
    char *strtok_chs;
    char *free_chs;
    char *portchs[32];
    char start[512] = ""; 
    char stop[512] = ""; 
    int i,j;
    int maps = 0;
    int k = 0;
    int mapnum = 0;
    memset(portchs,0,32*sizeof(char*));
    printf("%s\n",chs);

    for(i=0;;i++,chs = NULL){
        portchs[i] = strtok(chs,";");
        if (NULL == portchs[i]){
            break;
        }
    }

    for(j = 0;j<i;j++){
        printf("%d:%s\n",j,portchs[j]);
        memset(start,0,512*sizeof(char));
        memset(stop,0,512*sizeof(char));
        sscanf(portchs[j],"%[^>]",start);
        sscanf(portchs[j],"%*[^>]>%s",stop);
        k = atoi((char*)start);

        fill_port_reg((char*)stop);
        printf("mapnum = %d",scanner.ival);
        printf("%d:\n",j);
        mapnum = scanner.ival;
        copy_ports[k] |= mapnum;
        maps |=(0x1 << k);
    }
    /*
       scanner.ival = maps;
       scanner.mask |= IVAL;
       for(i = 0;i<32;i++){
       scanner.copy_ports[i] = copy_ports[i];
       printf("ports = %d",copy_ports[i]);
       } 
       */
}


void scan(char *key, char *val){

    memset(&scanner, 0, sizeof(scanner));
    if(strcmp("integer",key) == 0)
        fill_integer(val);
    else if(strcmp("string",key) == 0)
        fill_string(val);
    else if(strcmp("hexdigit",key) == 0)
        fill_hexdigit(val);
    else if(strcmp("inthex",key) == 0)
        fill_inthex(val);
    else if(strcmp("ident",key) == 0)
        fill_ident(val);
    else if(strcmp("ipaddr",key) == 0)
        fill_ipaddr(val);
    else if(strcmp("css_reg",key) == 0)
        fill_css(val);
    else if(strcmp("interfaces",key) == 0)
        fill_interfaces(val);
    else if(strcmp("protocol_reg",key) == 0)
        fill_protocolreg(val);
    else if(strcmp("ipmask_reg",key) == 0)
        fill_ipmask_reg(val);
    else if(strcmp("ud_reg",key) == 0)
        fill_udreg(val);
    else if(strcmp("size_reg",key) == 0)
        fill_sizereg(val);
    else if(strcmp("classport_reg",key) == 0)
        fill_sizereg(val);
    else if(strcmp("smp",key) == 0)
        fill_smp(val);
    else if(strcmp("ports_reg",key) == 0)
        fill_port_reg(val);
    else if(strcmp("rulenum_reg",key) == 0)
        fill_rulenum_reg(val);
    else if(strcmp("interfaceclass_reg",key) == 0)
        fill_interfaceclass_reg(val);
    else if(strcmp("stageclass_reg",key) == 0)
        fill_stageclass_reg(val);
    else if(strcmp("ethertype_reg",key) == 0)
        fill_ethertype_reg(val);
    else if(strcmp("ipversion_reg",key) == 0)
        fill_ipversion_reg(val);
    else if(strcmp("tcpflag_reg",key) == 0)
        fill_tcpflag_reg(val);
    else if(strcmp("ruleid",key) == 0)
        fill_ruleid(val);
    else if(strcmp("hashmode",key) == 0)
        fill_hash_mode(val);
    else if(strcmp("copylists",key) == 0)
        fill_copylist(val);
    else
        printf("no regex\n");;

}

Scanner for_cli_rpc(char *reg_str, char *val_str)
{
    memset(&scanner ,0,sizeof(scanner));
    do_scan(reg_str,val_str);
    __print_scanner(scanner);
    return scanner;
}

Scanner get_val(char *key,cJSON *dsp)
{

    cJSON *strjson;
    cJSON *isval;
    char *val;
    char *regname;
    memset(&scanner ,0,sizeof(scanner));
    strjson = cJSON_GetObjectItem(dsp,key);
    if(strjson && strjson->type != cJSON_NULL){
        isval = cJSON_GetObjectItem(datas,key);
        if(isval->type != cJSON_NULL){
            val = strjson->valuestring;
            if(cJSON_GetObjectItem(isval,"regtype")->type != cJSON_NULL){
                regname = cJSON_GetObjectItem(isval,"regtype")->valuestring;
                do_scan(regname,val);
                scanner.token = 1;
            }
            else{
                scanner.token = 1;
            }
        }
        else{
            scanner.token = -1;
        }
    }
    else{
        scanner.token = -1;
    }
    //	__print_scanner(scanner);
    return scanner;

} 
