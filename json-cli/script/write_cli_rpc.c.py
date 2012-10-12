#!/usr/bin/python
import yaml
import json
import sys 
import copy 
import string 
import os 
import stat 

funstr = ''

headstr = '''#include <stdio.h>
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
'''
funstr += '''static print_dsp_json(cJSON *dsp)
{
		printf("call:%s\\n",__FILE__);
        char *out; 
		out = cJSON_Print(dsp);
		printf("%s\\n",out);
        free(out);
}

'''

def  printfun(funname):
		global funstr
		funstr += 'int '+funname+'(cJSON *dsp)\n{\n'
		funstr += '\tint rv = 0;\n'
		funstr += '\t//rv = to_rpc_'+funname+'(dsp);\n'
		funstr += '\tprintf("call:%s\\n",__FUNCTION__);\n'
		funstr += '\tchar *out; \n'
		funstr += '\tout = cJSON_Print(dsp);\n'
		funstr += '\tprintf("%s",out);\n'
		funstr += '\tfree(out);\n'
		funstr += '\treturn rv;\n'
		funstr += '\n}\n\n'

def back_yaml_str(filename):
    yamlstr = ''
    bnffp = open(filename)
    allline = bnffp.readline()
    bnffp.seek(0)
    textlist = bnffp.readlines()
    bnffp.close()
    for line in textlist:   
        if len(line)>8 and line[:8] == 'include:':
            line = string.join(line.split(':')[1:],':')[0:-1].strip()
            yamlstr += str(back_yaml_str("file/"+line))
        else:
            yamlstr = yamlstr+str(line)
    return yamlstr

yaml_str = back_yaml_str("file/root.yaml")
data = yaml.load(yaml_str)

i = 0
casestr = '''
static int rv;
int dispatcher(cJSON *dsp,int funnum)
{                                 
	int rv;   
	switch(funnum){
'''
funnamelist = []
for da in data:
    for pa in data[da]:
                casestr += '\tcase '+str(i)+':\n'
                casestr += '\t\trv = '+str(pa['rpc'])+'(dsp);\n'
                casestr += '\t\tbreak;\n'
                i = i+1
                funnamelist.append(str(pa['rpc']))


casestr += '''\tdefault:
\t\tprintf("dispatcher error.\\n");
\t\trv = -1;
\t}
\tcJSON_Delete(dsp);
\treturn rv;
\t} 
'''

for name in list(set(funnamelist)):
    printfun(name)

wp = open("dispatcher.c","w")
wp.write(headstr)
wp.write(casestr)
#wp.write(funstr)
wp.close()

wp = open("cli_rpc.c","w")
wp.write(headstr)
#wp.write(casestr)
wp.write(funstr)
wp.close()
