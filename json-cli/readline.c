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

#define ORDER 1
#define MAP 2

static int IS_R = 0;
static int IS_END = 1;
static int MAP_IS_END = 1;
static int IN_MAP = 0;
static int IS_ENTER = 0;
static int IS_VAL = 0;
static char headstr[25] = "CLI: ";

extern char *xmalloc ();

char tokenlist[TAB_TOKEN_LEN][TAB_TOKEN_LEN];
int tl = 0;

static char name[CMD_L];
static char NAME[CMD_L];
static char *dc;
static int s_num,dci;
static int max_back_token_num;

cJSON *mapjson;
cJSON *orderjson;
cJSON *bigmapjson;
cJSON *thismap;

static int regex_back(cJSON  *cj, char *name,char *token);
static void print_error_msg(char *rl,int num);
static char *command_generator_help (char *text,int state);
static int  check_json(char*,int,cJSON*);
int list_next_fun(int,int);

void free_readline()
{
    cJSON_Delete(rootjson);
}

static void init_main(char *filename)
{
    FILE *f=fopen(filename,"rb");
    if(!f){
        printf("no json file in %s.\n",filename);
        exit(1) ;
    }
    char *data;
    cJSON *root;
    fseek(f,0,SEEK_END);
    long len=ftell(f);
    fseek(f,0,SEEK_SET);
    data=malloc(len+1);
    fread(data,1,len,f);
    fclose(f);
    rootjson = cJSON_Parse(data);
    free(data);
    if (!rootjson) {
        printf("Error before: [%s]\n",cJSON_GetErrorPtr());
        printf("NO THIS MODULE OR JSON ERROR.\n");
        exit(1);
    }
    mapjsonlist=cJSON_GetObjectItem(rootjson,"mapjson");
}

static int get_json(char *modulename)
{
    cJSON *root;
    root = cJSON_GetObjectItem(rootjson,modulename);
    if (!root) {
        printf("Error before: [%s]\n",cJSON_GetErrorPtr());
        printf("NO THIS MODULE OR JSON ERROR.\n");
        return -1;
    }
    json= cJSON_GetObjectItem(root,"cmdstree");
    datas=cJSON_GetObjectItem(root,"data");
    help=cJSON_GetObjectItem(root,"help");
    module=cJSON_GetObjectItem(root,"module");
    if (!json || !datas) {
        printf("Error before: [%s]\n",cJSON_GetErrorPtr());
        return -1;
    }
    return 0;
}

char *stripwhite ();

char *progname;

int done;

char *dupstr (s)
    char *s;
{
    char *r;
    r = xmalloc (strlen (s) + 1);
    strcpy (r, s);
    return (r);
}
static void list_fun()
{
    int k,helplen;
    char *string;
    helplen = cJSON_GetArraySize(help);

    for(k = 0;k < helplen;k++){
        printf("\t%s\n",cJSON_GetObjectItem(cJSON_GetArrayItem(help,k),"res")->valuestring);
    }

}

static void print_big_module_help(){
    int i,j;
    j = cJSON_GetArraySize(module);
    i = 0;
    char *module_str;
    printf("\tTo get more specific help use one of the following\n\thelp commands below.:\n\n");
    while(i<j){
        module_str = cJSON_GetArrayItem(module,i)->valuestring;
        printf("\thelp %s\n",module_str);
        i++;
    }
    printf("\n");
}

static void print_module_help(char *mdname){
    int i,j;
    j = cJSON_GetArraySize(help);
    i = 0;
    char *string;
    while(i<j){
        string = cJSON_GetObjectItem(cJSON_GetArrayItem(help,i),"module")->valuestring;
        if(strcmp(mdname,string)==0){
            string = cJSON_GetObjectItem(cJSON_GetArrayItem(help,i),"res")->valuestring;
            printf("\t%s\n",string);
            string = cJSON_GetObjectItem(cJSON_GetArrayItem(help,i),"help")->valuestring;
            printf("\t\thelp->:%s\n",string);
        }
        i++;
    }
    free(mdname);
}

static int check_tokenlist(char *string)
{
    int i = 0;
    for(i;i<tl;i++){
        if(strcmp(tokenlist[i],string) == 0)
            return 0;
    }
    return 1;
}

static void print_other_help(char **line_list,int len){

    int i,j,k;
    int helplen;
    int tokenlen;
    cJSON *token_help;
    int flag = 0;
    char *string;
    helplen = cJSON_GetArraySize(help);

    for(k = 0;k < helplen;k++){
        tokenlen = cJSON_GetArraySize(cJSON_GetObjectItem(cJSON_GetArrayItem(help,k),"cmd"));
        flag = 0;
        for(i = 0;i<tokenlen;i++){
            token_help = cJSON_GetArrayItem(cJSON_GetObjectItem(cJSON_GetArrayItem(help,k),"cmd"),i);
            string = token_help->valuestring;
            for(j = 1;j<len;j++)
            {
                if(strcmp(line_list[j],string)==0){
                    flag++;
                }
            }
        }
        if(flag >= (len-1)){
            string = cJSON_GetObjectItem(cJSON_GetArrayItem(help,k),"res")->valuestring;
            printf("\t%s\n",string);		
            string = cJSON_GetObjectItem(cJSON_GetArrayItem(help,k),"res")->valuestring;
            printf("\t\thelp->:%s\n",string);		
        }
    }

}

static void back_help_res(char **line_list,int len)
{
    int i,j;
    i = 0;
    int flag = 0;
    char *string;
    if (len == 2 && module){
        j = cJSON_GetArraySize(module);
        while(i<j){
            string = cJSON_GetArrayItem(module,i)->valuestring;
            if(strcmp(line_list[1],string)==0){
                flag++;
                break;
            }
            i++;
        }
        if(flag == 1){
            print_module_help(dupstr(string));
        }else{
            print_other_help(line_list,len);
        }
    }else if(module && len == 1){
        print_big_module_help();
    }else if(len == 1){
        list_fun();
    }
    else{
        print_other_help(line_list,len);
    }

}

static int doit(char *s)
{
    int i,j,k=0;
    char *line_his_list[LINE_MAX_TOKEN];
    int is,saveup = 0;
    int comlen;
    char *string;
    cJSON *onecom;
    int is_map;
    cJSON *this_map;
    int fun_num;
    int error_flag = 0;
    int has_r = 0;
    char s_bak[1024] = "";
    cJSON *fmt;
    tl = 0;
    cJSON *strcjson;
    strcpy(s_bak,s);
    for(i=0;;i++,s = NULL){
        line_his_list[i] = strtok(s," \t\n");
        if (NULL == line_his_list[i]){
            break;
        }
    }
    if(strcmp(line_his_list[0],"login") == 0 && i == 2){
        if(get_json(line_his_list[1]) != -1){
            memset(&headstr,0,sizeof(headstr));
            strcpy(headstr, line_his_list[1]);
            strcat(headstr, ": ");
            return 0;
        }else
            return -1;
    }
    else if(strcmp(line_his_list[0],"quit") == 0 && i == 1){
        get_json("all");
        memset(&headstr,0,sizeof(headstr));
        strcpy(headstr, "CLI: ");
        return 0;
    }
    else if(strcmp(line_his_list[0],"list") == 0 && i == 1){
        list_fun();
    }
    else{

        if(line_his_list[0] && strcmp(line_his_list[0],"help") == 0)
        {
            back_help_res(line_his_list,i);
            return 0;
        }
        else{
            comlen = cJSON_GetArraySize(json);
            for(j=0;j<comlen;j++){
                //IS_END = 1;
                MAP_IS_END = 1;
                IN_MAP = 0;
                fmt=cJSON_CreateObject();	
                onecom = cJSON_GetObjectItem(cJSON_GetArrayItem(json,j),"cmd");
                mapjson = (cJSON *)NULL;
                orderjson = onecom;
                i =0;
                while(line_his_list[i]){
                    is = check_json(line_his_list[i],DOIT,fmt);
                    if(is == 1){
                        if(i >= error_flag){
                            error_flag = i+1;
                            k = i;
                        }
                    }
                    if(is != 1){
                        if(i >= error_flag){
                            error_flag = i;
                            k = i;
                        }
                    }

                    {
                        if(i == k) {
                            if(saveup < i ){
                                memset(tokenlist,0,sizeof(char)*LINE_MAX_TOKEN*LINE_MAX_TOKEN);
                                tl = 0;
                                saveup = i;
                            }
                            cJSON *map = mapjson;
                            cJSON *order = orderjson;
                            if(IN_MAP)
                                while(map && map->type != cJSON_NULL){
                                    strcjson = cJSON_GetObjectItem(map,"name");
                                    string = strcjson->valuestring;
                                    strcpy(tokenlist[tl],string);
                                    tl++;
                                    map = cJSON_GetObjectItem(map,"bro");
                                }
                            if(!IN_MAP || MAP_IS_END)
                                while(order && order->type != cJSON_NULL){
                                    strcjson = cJSON_GetObjectItem(order,"ismap");
                                    is_map = strcjson->valueint;
                                    strcjson = cJSON_GetObjectItem(order,"name");
                                    string = strcjson->valuestring;
                                    if(is_map)
                                    {
                                        strcjson = cJSON_GetObjectItem(order,"name");
                                        string = strcjson->valuestring;
                                        this_map = 	cJSON_GetObjectItem(mapjsonlist,string);
                                        while(this_map && this_map->type != cJSON_NULL){
                                            strcjson = cJSON_GetObjectItem(this_map,"name");
                                            string = strcjson->valuestring;
                                            if(check_tokenlist(string)){
                                                strcpy(tokenlist[tl],string);
                                                tl++;
                                            }
                                            this_map = cJSON_GetObjectItem(this_map,"bro");
                                        }
                                    }
                                    else
                                    {
                                        strcjson = cJSON_GetObjectItem(order,"name");
                                        string = strcjson->valuestring;
                                        if(check_tokenlist(string)){
                                            strcpy(tokenlist[tl],string);
                                            tl++;
                                        }
                                    }
                                    order = cJSON_GetObjectItem(order,"bro");
                                }
                        }
                        if(is != 1)
                            break;
                    }
                    i++;
                }
                if(is == 1 && IS_END == 1){
                    fun_num = cJSON_GetObjectItem(onecom,"funnum")->valueint;
                    dispatcher(fmt,fun_num-1);
                    has_r = 1;
                    break;
                }
                else{
                    cJSON_Delete(fmt);
                }

            }

            if(has_r == 0){
                print_error_msg(s_bak,error_flag);
                return -1;
            }
        }
        return 0;
    }
}

static void print_error_msg(char rl[],int num)
{
    int print = 0;
    int flag = 0;
    int thnum = 0;
    int i=0,j,k=0;
    printf("%s\n",rl);

    while(rl[i]){
        if(rl[i] == ' ' && flag == 1)
            thnum++;
        if(rl[i] == ' ')
            flag = 0;
        else
            flag = 1;
        if(thnum == num && flag == 1){
            break;
        }
        i++;
    }
    for(print = 1;print<=i;print++)
        printf("%c",' ');
    printf("%s\n","^error");

    printf("%s\n","this should be:");
    for(i = 0;i < tl;i++){
        for(j = 0;j<i;j++){
            if(strcmp(tokenlist[j],tokenlist[i]) == 0)
                break;
        }
        if(i <= j){ 
            printf("%-8s",tokenlist[i]);
            if((k+1) % 5==0)
                printf("\n");
            k++;
        }
    }
    printf("\n");
}

static void start_readline()
{
    char *line, *s;
    int rv = 0;
    for ( ; done == 0; )
    {
        line = readline (headstr);
        if (!line)
            break;
        s = stripwhite (line);
        if (*s)
        {
            add_history (s);
            IS_END = 1;
            rv = doit(line);
        }
        free (line);
    }

}

int start_cli (char *cmd, FILE *fp)
{
    char *line, *s;
    int rv = 0;
    init_main(JSON_DIR );
    get_json("all");
    initialize_readline ();
    if(json){
        if(cmd){
            rv = doit(cmd);
            return rv;
        }

        if (!rl_instream)
            rl_instream = fp;
        if (!rl_outstream)
            rl_outstream = stdout;
        start_readline();
    }
    free_readline();
    if(fp)
        fclose(fp);
    return 0;
}

char *stripwhite (string)
    char *string;
{
    register char *s, *t;

    for (s = string; whitespace (*s); s++)
        ;

    if (*s == 0)
        return (s);

    t = s + strlen (s) - 1;
    while (t > s && whitespace (*t))
        t--;
    *++t = '\0';

    return s;
}


char *command_generator ();
char **fileman_completion ();

static int regex_back(cJSON  *cj, char *name,char *token)
{
    char regexstr[REG_STR_LEN];
    int status,i;
    int cflags = REG_EXTENDED;
    regmatch_t pmatch[1];
    regex_t reg;
    cJSON *strcjson;
    const size_t nmatch = 1;
    char *string;

    strcjson = cJSON_GetObjectItem(cj,"keyword");
    if((strcjson->valueint) == 1){
        if ((strlen(name) == strlen(token)) && (strncmp(name,token,strlen(token))) == 0){
            return 2;
        }
        else if((strncmp(name,token,strlen(token))) == 0){
            return 0;
        }
    }
    else{
        strcjson = cJSON_GetObjectItem(cj,"regex");
        string = strcjson->valuestring;
        regcomp(&reg,string,cflags);
        status = regexec(&reg,token,nmatch,pmatch,0);
        if(status == REG_NOMATCH){
            regfree(&reg);
            return 1;
        }
        else if(status == 0){
            memset(regexstr,0,sizeof(char)*REG_STR_LEN);
            strncpy(regexstr,token+pmatch[0].rm_so,pmatch[0].rm_eo - pmatch[0].rm_so);
            regfree(&reg);
            return (strncmp(regexstr,token,strlen(token)));
        }else
        {
            regfree(&reg);
            return 1;

        }
        regfree(&reg);
    }
    return 1;
}


static int check_while(cJSON *cjson,char *token,int level){
    int re = 2;
    int lens;
    int flag = 0;
    int i,isval;
    cJSON *strcjson;
    char *string;

    while(cjson && cjson->type != cJSON_NULL){
        strcjson = cJSON_GetObjectItem(cjson,"ismap");
        i = strcjson->valueint;
        strcjson = cJSON_GetObjectItem(cjson,"name");
        string = strcjson->valuestring;

        if (i)
        {
            IS_END =  cJSON_GetObjectItem(cjson,"end")->valueint;
            bigmapjson = cJSON_GetObjectItem(mapjsonlist,string);
            mapjson = bigmapjson;
            IN_MAP = 1;
            thismap = cjson;
            orderjson = cjson;
        }
        else{
            strcjson = cJSON_GetObjectItem(datas,string);
            re = regex_back(strcjson,string,token);
            strcjson = cJSON_GetObjectItem(cjson,"isval");
            isval = strcjson->valueint;

            if(re==0 || re == 2){
                memset(name,0,sizeof(char)*CMD_L);
                strcpy(name,string);
                IS_VAL = isval;
                strcjson = cJSON_GetObjectItem(cjson,"end");
                if(!IS_ENTER)
                    IS_ENTER = strcjson->valueint;
                flag ++;
                if(level == ORDER){
                    IS_END = strcjson->valueint;
                    orderjson = cJSON_GetObjectItem(cjson,"son");
                }
                if(level == MAP){
                    IS_VAL = 1;
                    MAP_IS_END = strcjson->valueint;
                    mapjson = cJSON_GetObjectItem(cjson,"son");
                    orderjson = cJSON_GetObjectItem(thismap,"son");
                    if(MAP_IS_END){
                        mapjson = bigmapjson;
                    }
                }
            }
            if (re == 2)
            {
                memset(name,0,sizeof(char)*CMD_L);
                if(level == ORDER)
                    IN_MAP  = 0;
                strcpy(name,string);
                IS_VAL = isval;
                if(level == MAP)
                    IS_VAL  = 1;
                flag = -1;
                break;
            }
        }
        cjson = cJSON_GetObjectItem(cjson,"bro");
    }
    return flag;
}

static int check_json(char *token ,int level,cJSON *fmt)
{
    int flag = 0;
    int order_flag = 0;
    int i = 0;
    cJSON *cjson1,*cjson2;
    cJSON *strcjson;
    cjson1 = orderjson;
    if(!IN_MAP || MAP_IS_END == 1){
        flag = check_while(cjson1,token,ORDER);
        order_flag = flag;
    }
    if(IN_MAP && flag != -1){
        cjson2 = mapjson;
        flag += check_while(cjson2,token,MAP);
    }
    if(flag == -1 || flag == 1)
        flag = 1;
    if(flag == 1 && order_flag == 1)
        IN_MAP = 0;

    if (flag == 1 && level ==DOIT && IS_VAL == 1){

        if(cJSON_GetObjectItem(fmt,name)){
            cJSON_DeleteItemFromObject(fmt,name);
        }
        strcjson = cJSON_GetObjectItem(datas,name); 
        strcjson = cJSON_GetObjectItem(strcjson,"isequal");
        if(strcjson->valueint){
            memset(NAME,0,sizeof(char)*CMD_L);
            for(i = 0;i<strlen(name);i++)
            {
                if(name[i] == '=')
                    break;
            }
            strncpy(NAME,token+i+1,strlen(token)-i-1);
            cJSON_AddStringToObject(fmt,name,NAME);
        }else{
            cJSON_AddStringToObject(fmt,name,token);
        }
    }
    return flag;
}



static int 	check_line(char **list,int len,cJSON *onejson){

    int i=0;
    int is;
    mapjson = (cJSON *)NULL;
    orderjson = onejson;
    MAP_IS_END = 1;
    //IS_END = 1;
    IN_MAP = 0;
    for(i = 0;i<len;i++){
        is = check_json(list[i],TAB_V,NULL);
        if(is != 1){
            return -1;
        }
    }
    return 0;
}

static void tab_init(char *text){

    char *line_his_list[LINE_MAX_TOKEN];
    char *rlstr;
    char *rlstr_bak;
    int i;
    int comlen;
    int rl_split_len;
    int is;
    int is_map;
    cJSON *onecom;
    cJSON *this_map;
    cJSON *strcjson;
    char *string;
    memset(tokenlist,0,sizeof(char)*LINE_MAX_TOKEN*LINE_MAX_TOKEN);
    tl = 0;
    rlstr = dupstr(rl_line_buffer);
    rlstr_bak = rlstr;
    for(i=0;;i++,rlstr = NULL){
        line_his_list[i] = strtok(rlstr," \t\n");
        if (NULL == line_his_list[i]){
            break;
        }
    }
    if(strlen(text)){
        i--;
    }
    rl_split_len = i;

    comlen = cJSON_GetArraySize(json);
    for(i=0;i<comlen;i++){
        if(cJSON_GetObjectItem(cJSON_GetArrayItem(json,i),"debug")->valueint)
            continue; 
        onecom = cJSON_GetObjectItem(cJSON_GetArrayItem(json,i),"cmd");
        is = check_line(line_his_list,rl_split_len,onecom);
        if(is == 0){
            if(IN_MAP){
                while(mapjson && mapjson->type != cJSON_NULL){
                    strcjson = cJSON_GetObjectItem(mapjson,"name");
                    string = strcjson->valuestring;
                    if(check_tokenlist(string)){
                        strcpy(tokenlist[tl],string);
                        tl++;
                    }
                    mapjson = cJSON_GetObjectItem(mapjson,"bro");
                }
            }
            if(!IN_MAP || MAP_IS_END){
                while(orderjson && orderjson->type != cJSON_NULL){
                    strcjson = cJSON_GetObjectItem(orderjson,"ismap");
                    is_map = strcjson->valueint;
                    if(is_map)
                    {
                        strcjson = cJSON_GetObjectItem(orderjson,"name");
                        string = strcjson->valuestring;
                        this_map = 	cJSON_GetObjectItem(mapjsonlist,string);
                        while(this_map && this_map->type != cJSON_NULL){
                            strcjson = cJSON_GetObjectItem(this_map,"name");
                            string = strcjson->valuestring;
                            if(check_tokenlist(string)){
                                strcpy(tokenlist[tl],string);
                                tl++;
                            }
                            this_map = cJSON_GetObjectItem(this_map,"bro");
                        }
                    }
                    else
                    {
                        strcjson = cJSON_GetObjectItem(orderjson,"name");
                        string = strcjson->valuestring;
                        if(check_tokenlist(string)){
                            strcpy(tokenlist[tl],string);
                            tl++;
                        }
                    }
                    orderjson = cJSON_GetObjectItem(orderjson,"bro");
                }
            }
        }
    }
    free(rlstr_bak);
}

int list_next_fun(int start,int end) 
{
    int j,i,bf_len;
    int keyword;
    char *s;
    cJSON *strcjson = NULL;
    char *string;
    bf_len = strlen(rl_line_buffer);
    IS_ENTER = 0;
    while(bf_len >=1 && rl_line_buffer && rl_line_buffer[bf_len-1] != 32 ){
        bf_len--;
    }
    s = (rl_line_buffer + bf_len);

    tab_init(s);
    printf("\n");
    for(i = 0;i<tl;i++){
        for(j = 0;j<i;j++){
            if(strcmp(tokenlist[j],tokenlist[i]) == 0)
                break;
        }
        if(i <= j){ 
            strcjson = cJSON_GetObjectItem(datas,tokenlist[i]);
            keyword = cJSON_GetObjectItem(strcjson,"keyword")->valueint;
            if(keyword == 1){
                memset(name,0,sizeof(char)*CMD_L);
                strncpy(name,tokenlist[i],strlen(tokenlist[i]));
                if((strncmp(name,s,strlen(s))) == 0)
                    printf("%s\n",name);

            }
            else{
                memset(name,0,sizeof(char)*CMD_L);
                string = cJSON_GetObjectItem(strcjson,"help")->valuestring;
                if((strncmp(string,s,strlen(s))) == 0){
                    printf("name: %-20stype: %-20s\texample: %-20s\n",tokenlist[i],cJSON_GetObjectItem(strcjson,"regtype")->valuestring,string);
                }
            }
        }
    }
    if(IS_END || IS_ENTER)
        printf("<cr>\n");

    printf("%s",rl_prompt);
    printf("%s",rl_line_buffer);
    return 0;
}

initialize_readline ()
{
    rl_add_defun ("?",list_next_fun,'?');
    rl_readline_name = "JSON";
    rl_completion_entry_function = command_generator;
    rl_attempted_completion_function = (CPPFunction *)fileman_completion;
}


char **fileman_completion (text, start, end)
    char *text;
    int start, end;
{


    char **matches;
    matches = (char **)NULL;
    char *ishelp;
    char *ishelp_bak;
    char i;

    ishelp = dupstr(rl_line_buffer);
    if (strtok(ishelp," \n\t") && strcmp("help",strtok(ishelp," \n\t")) == 0){
        free(ishelp);
        matches = (char **) completion_matches (text, command_generator_help);
        return matches;
    }
    else{
        free(ishelp);
        matches =  rl_completion_matches (text, command_generator);
        return (matches);
    }

}

char *command_generator_help (text, state)
    char *text;
    int state;
{
    int i ;
    int j ;
    char *string;
    if (!state)
    {
        j = cJSON_GetArraySize(module);
        i = 0;
    }
    while(i<j){
        i++;
        memset(name,0,sizeof(char)*CMD_L);
        string = cJSON_GetArrayItem(module,i-1)->valuestring;
        if((strncmp(string,text,strlen(text))) == 0)
            return dupstr(string);
    }
    return NULL;
}

char *command_generator (text, state)
    char *text;
    int state;
{
    int keyword;
    cJSON *strcjson = NULL;
    char *string;
    if(!state){
        tab_init(text);
        max_back_token_num = 0;
    }
    while(max_back_token_num<tl){
        max_back_token_num++;
        strcjson = cJSON_GetObjectItem(datas,tokenlist[max_back_token_num-1]);
        keyword = cJSON_GetObjectItem(strcjson,"keyword")->valueint;
        if(keyword == 1){
            memset(name,0,sizeof(char)*CMD_L);
            strncpy(name,tokenlist[max_back_token_num-1],strlen(tokenlist[max_back_token_num-1]));
            if((strncmp(name,text,strlen(text))) == 0)
                return dupstr(name);
        }
    }
    return (char *)NULL;
}

