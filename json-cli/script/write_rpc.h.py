#!/usr/bin/python
import yaml
import json
import sys 
import copy 
import string 
import os 
import stat 

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

//#include "include/cJSON.h"
//#include "include/cli_def.h"

'''

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
for da in data:
    funnamelist = []
    openfile=open("include/rpc_"+da+".h","wa")
    openfile.write(headstr)			
    for pa in data[da]:
        funnamelist.append(str(pa['rpc']))
    for pa in list(set(funnamelist)):
        openfile.write('extern int ')			
        openfile.write(pa+'(cJSON *dsp);\n')			
    openfile.close()
#for filename in filenames:
#	openfile=open("include/rpc_"+filename+".h","wa")
#	openfile.write(headstr)			
#	for key in funmapmod:
#		if filename == funmapmod[key]:
#			openfile.write('extern int rpc_')			
#			openfile.write(key+'(cJSON *dsp);\n')			
#	openfile.close()
