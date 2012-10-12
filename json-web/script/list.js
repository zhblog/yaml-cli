
function print_r(theObj){
    var retStr = '';
    if (typeof theObj == 'object') {
        retStr += '<div style="font-size:12px;">';
        for (var p in theObj) {
            if (typeof theObj[p] == 'object') {
                retStr += '<div><b>[' + p + '] => ' + typeof(theObj) + '</b></div>';
                retStr += '<div style="padding-left:25px;">' + print_r(theObj[p]) + '</div>';
            }
            else {
                retStr += '<div>[' + p + '] => <b>' + theObj[p] + '</b></div>';
            }
        }
        retStr += '</div>';
    }
    return retStr;
}

function check_target_one(token, name){

    var data = datas[name];
    if (data.keyword == 1) {
        return strncmp(name, token);
    }
    else {
        var regstr = data.regex;
        var reg = new RegExp(regstr, "gi");
        var back = token.match(reg);
        if (back == token) 
            return 0;
        else 
            return -1;
    }
}

function one_cmd(cmdlist, cmdtree){
    var cmd = cmdtree.cmd
    var target = 0;
    if (cmdtree.web == 0) 
        return 0;
    else {
        orderjson = cmd;
        mapjson = null;
        var i = 0;
        var cmdlen = cmdlist.length;
        var end;
        var flag = 0;
        for (i = 0; i < cmdlist.length; i++) {
            var order = orderjson;
            var map = mapjson;
            flag = 0;
            while (order) {
                target = check_target_one(cmdlist[i], order.name);
                if (target == 0 || target == 2) {
                    end = order.end
                    flag++;
                    if (order.ismap == 1) {
                        mapjson = orderjson;
                        orderjson = order.son
                    }
                    else {
                        mapjson = null;
                        orderjson = order.son;
                    }
                }
                if (target == 2) {
                    flag = 1;
                    break;
                }
                order = order.bro;
            }
            while (map) {
                target = check_target_one(cmdlist[i], map.name);
                if (target == 0 || target == 2) {
                    end = map.end
                    flag++;
                    if (map.ismap == 1) {
                        orderjson = map.son
                        mapjson = mapjson;
                    }
                    else {
                        mapjson = null;
                        orderjson = map.son;
                    }
                }
                if (target == 2) {
                    flag = 1;
                    break;
                }
                map = map.bro;
            }
            if (flag != 1) {
                end = 0;
                break;
            }
        }
        return end;
    }
}

function init(){
    $.getJSON("script/json.json", function(data){
        allcmdtree = data.cmdstree;
        datas = data.data;
    });
    var pattern = new RegExp("\s" + keyw[i] + "\s", "g");
    html = html.replace(pattern, "<font class='keyword'> " + keyw[i] + " </font>");
}

function strncmp(str1, str2){
    var n = str2.length;
    var m = str1.length;
    if (m == n && str1 == str2) 
        return 2;
    else {
        str1 = str1.substring(0, n);
        str2 = str2.substring(0, n);
        return ((str1 == str2) ? 0 : ((str1 > str2) ? 1 : -1));
    }
}

function test(){
    var cmdone = $("#cmdone").val();
    check_onecmd_from_all(cmdone);
}

function check_onecmd_from_all(cmdone){
    onecmdlist = cmdone.split(" ");
    var tlist = new Array();
    var j = 0;
    var back = 0;
    for (var i = 0; i < onecmdlist.length; i++) {
        if (onecmdlist[i].length > 0) {
            tlist[j] = onecmdlist[i];
            j++;
        }
    }
    for (var i = 0; i < allcmdtree.length; i++) {
        j = one_cmd(tlist, allcmdtree[i]);
        if (j == 1) {
            back = 1;
            break;
        }
    }
    return back;
}

function check_allcmd(){
    $("#res").html("");
    var allstr = $("#textid").val();
    var allstrlist = allstr.split("\n");
    var back = 0;
    for (var i = 0; i < allstrlist.length; i++) {
        if ($.trim(allstrlist[i].length > 0)) {
            back = check_onecmd_from_all($.trim(allstrlist[i]));
            show_cmd_res(back, $.trim(allstrlist[i]));
        }
    }
    $(".res").css("display", "block");
    alert($("pre").width());
}

function show_cmd_res(worr, cmdstr){
    var reg = /\s{2,}/g;
    cmdstr = cmdstr.replace(reg, " ");
    if (worr == 0) {
        if ($.trim(cmdstr).length > 1) 
            $("#res").append("  <font color='red'><strike>" + cmdstr + "</strike></font><br>");
    }
    else {
        var strs = highLightKeyWord(cmdstr);
        if ($.trim(cmdstr).length > 1) 
            $("#res").append("  " + strs + "<br>");
    }
}

function highLightKeyWord(strs){
    var action = actions;
    var keyw = keywords;
    var html = strs;
    for (var i = 0; i < keyw.length; i++) {
        var pattern = new RegExp("^" + keyw[i] + " ", "gi");
        html = html.replace(pattern, "<font class='keyword'>" + keyw[i] + "</font> ");
        var pattern = new RegExp(" " + keyw[i] + " ", "gi");
        html = html.replace(pattern, " <font class='keyword'>" + keyw[i] + "</font> ");
        var pattern = new RegExp("^" + keyw[i] + "$", "gi");
        html = html.replace(pattern, "<font class='keyword'>" + keyw[i] + "</font>");
        var pattern = new RegExp(" " + keyw[i] + "$", "gi");
        html = html.replace(pattern, " <font class='keyword'>" + keyw[i] + "</font>");
    }
    for (var i = 0; i < action.length; i++) {
        var pattern = new RegExp("^" + action[i] + " ", "gi");
        html = html.replace(pattern, "<font class='action'>" + action[i] + "</font> ");
        var pattern = new RegExp(" " + action[i] + " ", "gi");
        html = html.replace(pattern, " <font class='action'>" + action[i] + "</font> ");
        var pattern = new RegExp("^" + action[i] + "$", "gi");
        html = html.replace(pattern, "<font class='action'>" + action[i] + "</font>");
        var pattern = new RegExp(" " + action[i] + "$", "gi");
        html = html.replace(pattern, " <font class='action'>" + action[i] + "</font>");
    }
    return html;
}
