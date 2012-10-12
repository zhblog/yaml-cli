--author Jack.ZH
--title A Yaml CLI
--date today
JSON CLI README

--newpage agenda
--heading Yaml-root
--beginoutput
#include test1.yaml
#include test2.yaml
--endoutput
test1 and test2 is the diff module

--newpage intro
--heading Yaml-one cmd
test1cmds:
--beginoutput
- cmd: show system info        #cmd
help: show some system message #help 
web: yes                       #is in the web
file: yes                      # cli -f  yes
debug: no                      #debug no
--endoutput

--newpage intro
--heading Yaml-CMD
this:
--beginoutput
- cmd: add ruleset <rule_name:string> <rr|hash> <aa:integer|bb:string> 
    <cc=string> {mm,nn,qq <ee:inthex>,thover} [isover]  over     
--endoutput
exp:
add rukeset abc rr 234 cc=bca nn mm qq 0xa12 isover over 
show:
<> one or more ,N choose one:e:<rr|hash> <aa:integer|bb:string> <cc=string> 
[] kesuan e:[isover] 
{} shunxu"," e:{mm,nn,qq <ee:inthex>,thover}

--newpage intro
--heading in cmd
Login :e:login system
Quiet:CLI ALL 
List:
Help:
?:
