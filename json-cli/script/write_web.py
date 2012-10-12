#!/usr/bin/env python
import os
import yaml
import json 
import sys
import string
from pprint import pprint

keywords = ['set','undo','delete','show','unset','get','import','restore','export','exit','reset','save','clear','create','destroy','add','check','enable','download']
keywords = list(set(keywords))

base_dir = os.path.abspath(os.path.dirname(sys.argv[0]))
configFile = "%s/%s" % (base_dir,"../file/json")
stream = file(configFile, 'r')   
data = json.load(stream)
dataall = data['all']
datahelp = dataall['help']
tokenlist = []
for onehelp in datahelp:
	  tokenlist.extend(onehelp["cmd"])

tokenlist = list(set(tokenlist))

for i in keywords:
	if (i in tokenlist):
		tokenlist.remove(i)
tokenlistjson =  json.dumps(tokenlist,indent=4)
keylistjson =  json.dumps(keywords,indent=4)
tokenstr = 'var actions = '+str(tokenlistjson)+';\n'
keystr = 'var keywords = '+str(keylistjson)+';'


  
bnffp = open("/root/all-json-cli/json-web/script/key.js","w")
bnffp.write(tokenstr + keystr)
bnffp.close()

bnffp = open("/root/all-json-cli/json-web/script/json.json","w")
bnffp.write(json.dumps(dataall,indent=4))
bnffp.close()


