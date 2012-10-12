#!/usr/bin/python
import yaml
import json
import sys 
import copy 
import string 
import os 
import stat 

pre = 0
mak = {}
sun = {}

res = {}
reghelp = {}
helps = {}
funnum = 0
mapnum = 0
maptrees ={'mapname1':None} 
mapname = 'mapname'

#------------------------------------------------------------------
# regex
#-------------------------------------------------------------------
def fill_res():
    bnffp = open("file/regex","r")
    allline = bnffp.readline()
    bnffp.seek(0)
    textlist_reg = bnffp.readlines()
    bnffp.close()

    keys = []
    i = 0
    for line in textlist_reg:
        if len(line.strip())>0 and line[0] != '#':
            key = line.split('=')[0].strip()
            reg = string.join(line.split('=')[1:],'=')[0:-1].strip()
            for j in range(0,i):
                if reg.find('('+str(keys[j])+')') > -1:
                    reg = reg.replace('('+str(keys[j])+')','('+res[keys[j]]+')')
            keys.append(key)
            res[key] = reg 
            i = i+1
        elif len(line.strip())>0 and line[0] == '#':
            reghelp[line.split(':')[0][1:].strip()] = ':'.join(line.split(':')[1:]).strip()
    for line in textlist_reg:
        if len(line.strip())>0 and line[0] == '@':
            if len(line.strip())>4:
                ke = line[1:].strip().split(':')[0]
                val = ':'.join(line[1:].strip().split(':')[1:])
                helps[ke] = val                            



def backonecom(line,a,flag):
        num = 0
        for char in line:
                num = num + 1
                if num == len(line):
                        return line

                if a == '{':
                        if line[num] == '{':
                                flag = flag + 1
                        if line[num] == '}':
                                flag = flag - 1
                        if flag == 0:
                                return line[:num + 1]
                elif a == '<':
                        if line[num] == '<':
                                flag = flag + 1
                        if line[num] == '>':
                                flag = flag - 1
                        if flag == 0:
                                return line[:num + 1]
                elif a == '(':
                        if line[num] == '(':
                                flag = flag + 1
                        if line[num] == ')':
                                flag = flag - 1
                        if flag == 0:
                                return line[:num + 1]
                elif a == '[':
                        if line[num] == '[':
                                flag = flag + 1
                        if line[num] == ']':
                                flag = flag - 1
                        if flag == 0:
                                return line[:num + 1]
                else:
                        if line[num] == ' ':
                                return line[:num + 1]
                                break


def  backonecoms(line):
        onecoms = [] 
        while len(line) > 0:
                backstr = backonecom(line,line[0],1)
                if line[0] == '[':
                        onecoms.append({"key": '[',"value": backonecoms(backstr[1:-1])})
                elif line[0] == '{':
                        valuestr = ""
                        valuelist =  backstr[1:-1].split(",")
                        for pp in valuelist[:-1]:
                            valuestr += "["+pp+"] "
                        valuestr += valuelist[-1] 
                        onecoms.append({'key':'{','value':backonecoms(valuestr)})
                elif line[0] == '<':
                        onecoms.append({'key':'<','value':backstr[1:-1]})
                elif line[0] == '(':
                        onecoms.append({'key':'(','value':backstr[1:-1]})
                else: 
                        onecoms.append({'key':'key','value':backstr})
                line = line[len(backstr):]
                line = line.strip()
        return onecoms

def addkey(token):
        isequal = 0
        regtype = ''
        keyword = 0
        regex = ''
        helpstrs = ''
        isval = 0
        keystr = '' 
        if len(token.strip())>0:
                token1s = token.split(':')
                token2s = token.split('=')
                token3s = token.split('{')
                
                if len(token2s) > 1 and len(token3s) > 1:
                        ke = str(token2s[0])
                        strval = token3s[1][:-1]
                        strvals = strval.split(',')
                        sreg = ''
                        for val in strvals:
                                if len(sreg) > 0:
                                        sreg = sreg+'|'+str(val)
                                else:
                                        sreg = str(val)
                        helpstr = str(ke)+'='+token3s[1][:-1]
                        sregs = '('+sreg+')|('+sreg+'),('+sreg+')(,('+sreg+'))*'
                        
                        isequal = 1
                        isval = 1
                        regtype = ke[:-1]
                        keyword = 0
                        regex = ke+'=('+str(sregs)+')'
                        helpstrs = helpstr
                        keystr = ke
                elif len(token1s) > 1:
                        keystr = str(str(token).split(':')[0].strip())
                        isequal = 0
                        isval = 1
                        regtype = str(token1s[1])
                        keyword = 0
                        regex = res[str(token1s[1])]
                        if helps.has_key((token.split(':')[0]).strip()):
                                helpstrs = helps[(token.split(':')[0]).strip()]

                        elif reghelp.has_key((token.split(':')[1]).strip()):
                                helpstrs = reghelp[str(token1s[1])] 
                        else:
                                helpstrs = token 
                elif len(token2s) > 1:
                        keystr = str(token.split('=')[0].strip())
                        isequal = 1
                        isval = 1
                        regtype = str(token2s[1])
                        keyword = 0
                        regex = token2s[0]+"=("+res[str(token2s[1])]+')'
                        if helps.has_key((token.split('=')[0]).strip()):
                                helpstrs = helps[(token.split('=')[0]).strip()] 
                        elif reghelp.has_key(str(token2s[1])):
                                helpstrs = token2s[0]+"="+reghelp[str(token2s[1])] 
                        else :
                                helpstrs = token 
                else:
                        keystr = token.strip()
                        isequal = 0
                        isval = 0
                        regtype = None
                        keyword = 1
                        regex = token.strip()
                        helpstrs = token.strip()
                if isequal == 1:
                    key[keystr+'='+regtype] = {'isequal':isequal,'isval':isval,'keyword':keyword,'regtype':regtype,"regex":regex,'help':helpstrs}
                else:
                    key[keystr] = {'isequal':isequal,'isval':isval,'keyword':keyword,'regtype':regtype,"regex":regex,'help':helpstrs}


def backnode(val):
        step = 0
        flag = 0 
        start = 0
        for ch in val:
                step += 1
                if val[0] == '(':
                        start = 1
                        if ch == '(':
                                flag += 1
                        if ch == ')':
                                flag -= 1
                        if flag == 0:
                                return  val[start:step-1]
                else:
                        if ch == '|':
                                return  val[start:step-1]
                        if step == len(val):
                                return val[start:step]

def backnodes(val):
        i = 0
        vals = []
        valss = [] 
        while len(val) >0:
                node = backnode(val)
                addkey(node)
                key = ''
                strs = ''
                if len(node.split(':')) > 1:
                        strs = node.split(':')[0]
                        key = 0 
                elif len(node.split('=')) > 1:
                        #strs = str(node.split('=')[0])
                        strs = str(node)
                        key = 0
                else:
                        strs =node 
                        key = 1 
                if val[0] == '(' and len(val)-len(node)>2:
                        val = val[len(node)+3:]
                elif val[0] != '(' and len(val)-len(node)>0:
                        val = val[len(node)+1:]
                else:
                        val = ''
                valss[i:i] = [{'str':strs,'key':key,'sstr':val}]
                i += 1
        return valss

def is_end(listline):
    listend = listline[1:]
    flag = 0
    for pp in listend:
        if pp['key'] != '[':
            flag += 1
    if flag == 0:
        return 1
    else:
        return 0

def backmapjson(listline):
    global mapnum 
    if len(listline) == 1 :
        if listline[0]['key'] == '[':
            while listline[0]['key'] == '[':
                kbak = copy.deepcopy(listline[0])
                listline.pop(0)
                for ii in range(len(kbak['value'])-1,-1,-1):
                    listline.insert(0,kbak['value'][ii])
            if len(listline) == 1:
                if listline[0]['key'] == '<':
                    brolist = backnodes(listline[0]['value'])
                    bstr = brolist[0]['sstr']
                    if len(bstr) > 0:
                        listline[0]['value'] = bstr
                        return {"name":brolist[0]['str'].strip(),'son':None,'bro':backmapjson(listline),'end':1 ,'isval':1,'iskey':brolist[0]['key'],'ismap':0,'funnum':funnum}
                    else:
                        return {"name":brolist[0]['str'].strip(),'son':None,'bro':None,'end':1,'isval':1,'iskey':brolist[0]['key'],'ismap':0,'funnum':funnum}
                elif listline[0]['key'] == '{':
                    end = is_end(listline)
                    mapnum += 1
                    thisname = mapname +str(mapnum) 
                    maptrees.append({thisname:backonecomjsons(listline[0]['value'])})
                    return {'name':thisname,'son':backonecomjsons(listline[1:]), 'bro':None,'end':end,'isval':1,'iskey':0,'ismap':1,'funnum':funnum}
                else:
                    brolist = backnodes(listline[0]['value'])
                    return {"name":listline[0]['value'].strip(),'son':None,'bro':None,'end':1,'isval':1,'iskey':brolist[0]['key'],'ismap':0,'funnum':funnum}
            else:
                brolist = backnodes(listline[0]['value'])
                return {"name":listline[0]['value'].strip(),'son':backonecomjsons(listline[1:]),'bro':None,'end':is_end(listline),'isval':1,'iskey':1,'ismap':0,'funnum':funnum}

        elif listline[0]['key'] == '<':
            brolist = backnodes(listline[0]['value'])
            bstr = brolist[0]['sstr'] 
            if len(bstr) > 0:
                listline[0]['value'] = bstr
                return {"name":brolist[0]['str'].strip(),'son':None,'bro':backmapjson(listline),'end':1,'isval':1,'iskey':brolist[0]['key'],'ismap':0,'funnum':funnum}
            else:
                return {"name":brolist[0]['str'].strip(),'son':None,'bro':None,'end':1,'isval':1,'iskey':brolist[0]['key'],'ismap':0,'funnum':funnum}
        elif listline[0]['key'] == '{':
            end = is_end(listline)
            mapnum += 1
            thisname = mapname +str(mapnum) 
            maptrees.append({thisname:backonecomjsons(listline[0]['value'])})
            return {'name':'}','son':backonecomjsons(listline[1:]), 'bro':None,'end':end,'isval':1,'iskey':0,'ismap':1,'funnum':funnum}
        else:
            brolist = backnodes(listline[0]['value'])
            return {"name":listline[0]['value'].strip(),'son':None,'bro':None,'end':1,'isval':1,'iskey':brolist[0]['key'],'ismap':0,'funnum':funnum}
    else:
        if listline[0]['key'] == '[':
            sonbak = copy.deepcopy(listline[0]['value'])
            brobak = copy.deepcopy(listline[1:])

            if sonbak[0]['key'] == '<':
                brolist = backnodes(sonbak[0]['value'])
                bstr = brolist[0]['sstr'] 
                if len(bstr) > 0:
                    sonbak[0]['value'] = bstr
                    list_val = list_bak
                    while list_val[0]['key'] == '[':
                        list_val = list_val[0]['value']
                    list_val[0]['value'] = bstr
                    end = is_end(sonbak)
                    return {"name":brolist[0]['str'].strip(),'son':backonecomjsons(sonbak[1:]),'bro':backmapjson(brobak),'end':end,'isval':1,'iskey':brolist[0]['key'],'ismap':0,'funnum':funnum}
                else:
                    end = is_end(sonbak)
                    if len(sonbak) >1:
                        return {"name":brolist[0]['str'].strip(),'son':backonecomjsons(sonbak[1:]),'bro':backmapjson(brobak),'end':end,'isval':1,'iskey':brolist[0]['key'],'ismap':0,'funnum':funnum}
                    else:
                        return {"name":brolist[0]['str'].strip(),'son':None,'bro':backmapjson(brobak),'end':end,'isval':1,'iskey':brolist[0]['key'],'ismap':0,'funnum':funnum}
            elif sonbak[0]['key'] == '{':
                end = is_end(listline)
                mapnum += 1
                thisname = mapname +str(mapnum)
                maptrees.append({thisname:backonecomjsons(listline[0]['value'])})
                return {'name':thisname,'son':backonecomjsons(listline[1:]), 'bro':None,'end':end,'isval':1,'iskey':0,'ismap':1,'funnum':funnum}
            else:
                brolist = backnodes(sonbak[0]['value'])
                if len(sonbak) > 1:
                    end = is_end(sonbak)
                    return {'name':sonbak[0]['value'].strip(),'son':backonecomjsons(sonbak[1:]), 'bro':backmapjson(brobak),'end':end,'isval':1,'iskey':brolist[0]['key'],'ismap':0,'funnum':funnum}
                else:
                    return {'name':sonbak[0]['value'].strip(),'son':None, 'bro':backmapjson(brobak),'end':1,'isval':1,'iskey':brolist[0]['key'],'ismap':0,'funnum':funnum}
        
        elif listline[0]['key'] == '<':
            brolist = backnodes(listline[0]['value'])
            bstr = brolist[0]['sstr']
            end = is_end(listline)
            if len(bstr) > 0:
                listline[0]['value'] = bstr
                return {"name":brolist[0]['str'].strip(),'son':backonecomjsons(listline[1:]),'bro':backmapjson(listline),'end':end,'isval':1,'iskey':brolist[0]['key'],'ismap':0,'funnum':funnum}
            else:
                return {"name":brolist[0]['str'].strip(),'son':backonecomjsons(listline[1:]),'bro':None,'end':end,'isval':1,'iskey':brolist[0]['key'],'ismap':0,'funnum':funnum}
        elif listline[0]['key'] == '{':
            end = is_end(listline)
            mapnum += 1
            thisname = mapname + str(mapnum) 
            maptrees.append({thisname:backmapjson(listline[0]['value'])})
            return {'name':thisname,'son':backonecomjsons(listline[1:]), 'bro':None,'end':end,'isval':1,'iskey':0,'ismap':1,'funnum':funnum}
        else:
            end = is_end(listline)
            brolist = backnodes(listline[0]['value'])
            return {'name':brolist[0]['str'].strip(),'son':backonecomjsons(listline[1:]), 'bro':None,'end':end,'isval':1,'iskey':brolist[0]['key'],'ismap':0,'funnum':funnum}


def backonecomjsons(listline):
    global mapnum 
    if len(listline) == 1 :
        if listline[0]['key'] == '[':
            while listline[0]['key'] == '[':
                kbak = copy.deepcopy(listline[0])
                listline.pop(0)
                for ii in range(len(kbak['value'])-1,-1,-1):
                    listline.insert(0,kbak['value'][ii])
            if len(listline) == 1:
                if listline[0]['key'] == '<':
                    brolist = backnodes(listline[0]['value'])
                    bstr = brolist[0]['sstr']
                    if len(bstr) > 0:
                        listline[0]['value'] = bstr
                        return {"name":brolist[0]['str'].strip(),'son':None,'bro':backonecomjsons(listline),'end':1 ,'isval':1,'iskey':brolist[0]['key'],'ismap':0,'funnum':funnum}
                    else:
                        return {"name":brolist[0]['str'].strip(),'son':None,'bro':None,'end':1,'isval':1,'iskey':brolist[0]['key'],'ismap':0,'funnum':funnum}
                elif listline[0]['key'] == '{':
                    end = is_end(listline)
                    mapnum += 1
                    thisname = mapname +mapnum 
                    maptrees[thisname] = backmapjson(listline[0]['value'])
                    return {'name':thisname,'son':backonecomjsons(listline[1:]), 'bro':None,'end':end,'isval':0,'iskey':0,'ismap':1,'funnum':funnum}
                else:
                    brolist = backnodes(listline[0]['value'])
                    return {"name":listline[0]['value'].strip(),'son':None,'bro':None,'end':1,'isval':1,'iskey':brolist[0]['key'],'ismap':0,'funnum':funnum}
            else:
                brolist = backnodes(listline[0]['value'])
                return {"name":brolist[0]['str'].strip(),'son':backonecomjsons(listline[1:]),'bro':None,'end':is_end(listline),'isval':1,'iskey':1,'ismap':0,'funnum':funnum}

        elif listline[0]['key'] == '<':
            brolist = backnodes(listline[0]['value'])
            bstr = brolist[0]['sstr'] 
            if len(bstr) > 0:
                listline[0]['value'] = bstr
                return {"name":brolist[0]['str'].strip(),'son':None,'bro':backonecomjsons(listline),'end':1,'isval':1,'iskey':brolist[0]['key'],'ismap':0,'funnum':funnum}
            else:
                return {"name":brolist[0]['str'].strip(),'son':None,'bro':None,'end':1,'isval':1,'iskey':brolist[0]['key'],'ismap':0,'funnum':funnum}
        elif listline[0]['key'] == '{':
            end = is_end(listline)
            mapnum += 1
            thisname = mapname +str(mapnum)
            maptrees[thisname] = backmapjson(listline[0]['value'])
            if len(listline)>1:
                return {'name':thisname,'son':backonecomjsons(listline[1:]), 'bro':None,'end':end,'isval':0,'iskey':0,'ismap':1,'funnum':funnum}
            else:
                return {'name':thisname,'son':None, 'bro':None,'end':end,'isval':0,'iskey':0,'ismap':1,'funnum':funnum}
        else:
            brolist = backnodes(listline[0]['value'])
            return {"name":listline[0]['value'].strip(),'son':None,'bro':None,'end':1,'isval':0,'iskey':brolist[0]['key'],'ismap':0,'funnum':funnum}
    else:
        if listline[0]['key'] == '[':
            sun = {}
            if_kx = 0
            end = is_end(listline)
            list_bak = copy.deepcopy(listline)
            if listline[0]['key'] == '[':
                sun = copy.deepcopy(listline[1:])
                if_kx = 1
            while listline[0]['key'] == '[':
                kbak = copy.deepcopy(listline[0])
                listline.pop(0)
                for ii in range(len(kbak['value'])-1,-1,-1):
                    listline.insert(0,kbak['value'][ii])
            if if_kx == 1:
                if listline[0]['key'] == '<':
                    brolist = backnodes(listline[0]['value'])
                    bstr = brolist[0]['sstr'] 
                    if len(bstr) > 0:
                        listline[0]['value'] = bstr
                        list_val = list_bak
                        while list_val[0]['key'] == '[':
                            list_val = list_val[0]['value']
                        list_val[0]['value'] = bstr
                        return {"name":brolist[0]['str'].strip(),'son':backonecomjsons(listline[1:]),'bro':backonecomjsons(list_bak),'end':end,'isval':1,'iskey':brolist[0]['key'],'ismap':0,'funnum':funnum}
                    else:
                        return {"name":brolist[0]['str'].strip(),'son':backonecomjsons(listline[1:]),'bro':backonecomjsons(sun),'end':end,'isval':1,'iskey':brolist[0]['key'],'ismap':0,'funnum':funnum}
                elif listline[0]['key'] == '{':
                    end = is_end(listline)
                    mapnum += 1
                    thisname = mapname + str(mapnum)
                    maptrees[thisname] = backmapjson(listline[0]['value'])
                    return {'name':thisname,'son':backonecomjsons(listline[1:]), 'bro':backonecomjsons(sun),'end':end,'isval':0,'iskey':0,'ismap':1,'funnum':funnum}
                else:
                    brolist = backnodes(listline[0]['value'])
                    return {'name':brolist[0]['str'].strip(),'son':backonecomjsons(listline[1:]), 'bro':backonecomjsons(sun),'end':end,'isval':1,'iskey':brolist[0]['key'],'ismap':0,'funnum':funnum}
            else:
                brolist = backnodes(listline[0]['value'])
                return {'name':brolist[0]['str'].strip(),'son':backonecomjsons(listline[1:]), 'bro':None,'end':end,'isval':1,'iskey':1,'ismap':0,'funnum':funnum}
        elif listline[0]['key'] == '<':
            brolist = backnodes(listline[0]['value'])
            bstr = brolist[0]['sstr']
            end = is_end(listline)
            if len(bstr) > 0:
                listline[0]['value'] = bstr
                return {"name":brolist[0]['str'].strip(),'son':backonecomjsons(listline[1:]),'bro':backonecomjsons(listline),'end':end,'isval':1,'iskey':brolist[0]['key'],'ismap':0,'funnum':funnum}
            else:
                return {"name":brolist[0]['str'].strip(),'son':backonecomjsons(listline[1:]),'bro':None,'end':end,'isval':1,'iskey':brolist[0]['key'],'ismap':0,'funnum':funnum}
        elif listline[0]['key'] == '{':
            end = is_end(listline)
            mapnum += 1
            thisname = mapname + str(mapnum) 
            maptrees[thisname] = backmapjson(listline[0]['value'])
            return {'name':thisname,'son':backonecomjsons(listline[1:]), 'bro':None,'end':end,'isval':0,'iskey':0,'ismap':1,'funnum':funnum}
        else:
            end = is_end(listline)
            brolist = backnodes(listline[0]['value'])
            return {'name':brolist[0]['str'].strip(),'son':backonecomjsons(listline[1:]), 'bro':None,'end':end,'isval':0,'iskey':brolist[0]['key'],'ismap':0,'funnum':funnum}

            

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

def backothertoken(val):
        i = 0
        vals = []
        while len(val) >0:
                node = backnode(val)
                strs = ''
                if len(node.split(':')) > 1:
                        strs = '' 
                elif len(node.split('=')) > 1:
                        strs = ''
                else:
                        strs = node
                vals[i:i] = [str(strs)]
                if val[0] == '(' and len(val)-len(node)>2:
                        val = val[len(node)+3:]
                elif val[0] != '(' and len(val)-len(node)>0:
                        val = val[len(node)+1:]
                else:
                        val = ''
                i += 1
        return vals

def backhelpcomlist(listline):
        for lineone in listline:
                if lineone['key'] == 'key':
                        lists.append(lineone['value'].strip())
                if lineone['key'] == '[':
                        backhelpcomlist(lineone['value'])                
                if lineone['key'] == '<':
                        vals = backothertoken(lineone['value'])
                        for val in vals:
                                if len(val)>0:
                                        lists.append(val.strip())
                if lineone['key'] == '{':
                        backhelpcomlist(lineone['value'])                
        return lists

if __name__ == '__main__':
  yaml_str = back_yaml_str("file/root.yaml")
  data = yaml.load(yaml_str)
  fill_res()
  funnum = 0
  filejson = {}
  module=[]
  for da in data:
      module.append(da.strip())
      jsons = {} 
      pyjson = [] 
      pyjsons = [] 
      pyjsontree = {} 
      bighelps = [] 
      key = {}
      for pa in data[da]:
            pyjsontree = {} 
            pyjson = [] 
            lists = []
            line = pa['cmd'].strip()
            funnum += 1
            listline = backonecoms(line)
            line = pa['cmd'].strip()
            listline = backonecoms(line)
            back_r = {} 
            back_r = backonecomjsons(listline)
            pyjson.append(back_r)
            bighelp = {}
            if line and ((pa.has_key('debug') and (not pa['debug'])) or (not pa.has_key('debug'))):
                bighelp['module'] = da.strip()
                if pa.has_key('help'):
                    bighelp['help'] = pa['help'].strip()
                else:
                    bighelp['help'] = pa['cmd'].strip()
                bighelp['cmd'] = backhelpcomlist(backonecoms(pa['cmd'].strip()))
                bighelp['res'] = pa['cmd'].strip()
                bighelps.append(bighelp)
                bighelp = {}
            #if pa.has_key('web'):
             #   if pa['web']:
              #      pyjsontree['web'] = 1 
              #  else:
               #     pyjsontree['web'] = 0 
            else:
                pyjsontree['web'] = 1
            if pa.has_key('file'):
                if pa['file']:
                    pyjsontree['file'] = 1 
                else:
                    pyjsontree['file'] = 0 
            else:
                pyjsontree['file'] = 1
            if pa.has_key('debug') and pa['debug']:
                pyjsontree['debug'] = 1 
            else:
                pyjsontree['debug'] = 0
            pyjsontree['cmd'] = back_r 
            pyjsons.append(pyjsontree)
      jsons = {}
      jsons['cmdstree'] = pyjsons
      jsons['help'] = bighelps
      jsons['data'] = key
      filejson[da] = jsons
#for all
  
  allcmd = []
  allhelp = []
  alldata = {} 
  allmodule = {}
  for pp in filejson:
    for bb in filejson[pp]['cmdstree']:
      allcmd.append(bb) 
    for bb in filejson[pp]['help']:
      allhelp.append(bb) 
    for bb in filejson[pp]['data']:
      alldata[bb] = filejson[pp]['data'][bb]
  allmodule['cmdstree'] = allcmd
  allmodule['help'] = allhelp
  allmodule['data'] = alldata
  allmodule['module'] = module
  filejson['all'] = allmodule
  filejson['mapjson'] = maptrees
#for all over

#fill_file
  datas =  json.dumps(filejson)
  bnffp = open("file/json","w")
  bnffp.write(datas)
  bnffp.close()
