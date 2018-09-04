var rpctest = {};

rpctest.task_req =
  [new JSON_RPC.Request('getTask',    ['testTask35']),
   new JSON_RPC.Request('getTask',    [{'task':'testTask35'}]),
   new JSON_RPC.Request('getTask',    [{'task':'*'}]),
   new JSON_RPC.Request('addTask',    [{'task':              'Task_Markus',
                                        'command':           '/home/markus/cmd.sh',
                                        'task_parameters':   '-e -o -n root',
                                        'command_parameters':'-sleep 15',
                                        'utgid':             'Markus_0',
                                        'description':       'Markus is responsible for this one'}]),
   new JSON_RPC.Request('getTask',    ['Task_Markus']),
   new JSON_RPC.Request('modifyTask', [{'original_task':'Task_Markus','utgid': 'Another_0'}]),
   new JSON_RPC.Request('getTask',    ['Task_Markus']),
   new JSON_RPC.Request('deleteTask', [{'task':'Task_Markus'}]),
   new JSON_RPC.Request('getTask',    ['Task_Markus']),
   new JSON_RPC.Request('getTask',    ['Task_Markus1']),
   new JSON_RPC.Request('getTask',    ['Task_Markus2']),
   new JSON_RPC.Request('getTask',    ['Task_Markus3']),
   new JSON_RPC.Request('getTask',    ['Task_Markus4']),
   new JSON_RPC.Request('getTask',    ['Task_Markus5']),
   ];
rpctest.test_req =
  [
   new JSON_RPC.Request('baz',        ['Task_Markus','a1',{'a2':2}]),
   ];
rpctest.count = 0;
rpctest.all_req = rpctest.task_req;
//rpctest.all_req = [new JSON_RPC.Request('getTask',    ['Task_Markusssss'])]
rpctest.curr_request = 0;

function connectToWS() {
  if (rpctest.webSocket !== undefined) {
    rpctest.webSocket.close()
  }
  var host = window.location.hostname;
  var port = Number(window.location.port);
  var proto = window.location.protocol;
  proto = (proto == 'https:') ? 'wss://' : 'ws://';
	if ( port )
	  wsurl = [proto, host, ":", port].join("");
	else 
	  wsurl = [proto, host].join("");
  wsurl = wsurl + "/TDBDATA/JSONRPC";
  rpctest.webSocket = new WebSocket(wsurl);
  rpctest.webSocket.onmessage = function(event) {
    var leng;
    if (event.data.size === undefined) {
      leng = event.data.length;
    } else {
      leng = event.data.size
    }
    //console.log("onmessage. size: " + leng + ", content: " + event.data);
    rpctest.showdata(this, event.data);
  };
  rpctest.webSocket.onopen = function(evt) {
    console.log("onopen.");
  };
  rpctest.webSocket.onclose = function(evt) {
    console.log("onclose.");
  };
  rpctest.webSocket.onerror = function(evt) {
    console.log("Error!");
  };
}

rpctest.instanceDump = function(pre,obj)    {
  // copy the object by the constructor
  if ( !obj ) return 'No Object';
  
  const keys = Object.keys(obj);
  var s = pre+'Object: ['+typeof(obj)+']: ';
  if ( ''+typeof(obj) == 'string' )  {
    s = s + '  =  ' + obj + '\n';
  }
  else   {
    s = s + '\n';
    keys.forEach(key => {
        s = s + pre + '       -> Key:'+key+' ['+typeof(obj[key])+'] ';
        if ( typeof(obj[key]) == typeof({}) )  {
          s = s + pre + rpctest.instanceDump(pre+'   ',obj[key]);
        }
        else  {
          s = s + '  =  ' + obj[key] + '\n';
        }
      });
  }
  return s;
};

rpctest.showdata = function(self, str) {
  var response = str, result;
  rpctest.raw.innerHTML     = '<pre> Raw Response:   '+str+'</pre>';
  rpctest.request.innerHTML = '<pre> Request:  ['+self.url+']      ' + self.rq + '</pre>';
  if ( self.url == '/TDBDATA/JSONRPC' )   {  // <<<< JSON RPC api
    response = JSON_RPC.parse(str);
    result   = response.result;
  }
  else  {                                    // <<<< JSON REST api
    result = JSON.parse(str);
  }
  rpctest.response.innerHTML = '<pre> JSON Response:  '+response+'</pre>';
  str = '<pre> Return value:   '+result+'\n'+' Dump: '+
  rpctest.instanceDump('       ',result)+'</pre>'
  if ( response.error ) str += '<p><pre> Error:   '+rpctest.instanceDump('       ',response.error)+'</pre>';
  rpctest.result.innerHTML = str;
};
  
rpctest.call_jsonrpc = function(request,url) {  
  var xhttp = null;
  if (window.XMLHttpRequest)      // code for modern browsers
    xhttp = new XMLHttpRequest();
  else                            // code for IE6, IE5
    xhttp = new ActiveXObject('Microsoft.XMLHTTP');

  xhttp.url = url;
  xhttp.rq  = request;
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      // Success
      var str = this.responseText;
      rpctest.showdata(this, str);
    }
    else if (this.readyState == 4) {
      rpctest.raw.innerHTML = '----';
      rpctest.response.innerHTML = '----';
      rpctest.result.innerHTML = '<pre> Error: Status:' + this.status + '    StatusText:' + this.statusText + '</pre>';
      if ( this.status == 400 )   {
        //alert('Status: 400');
      }
      else if ( this.status == 404 )  {
        //alert('Status: 404');
      }
      else {
        //alert('Status: '+this.status);
      }
    }
  };
  xhttp.open('POST', url, true);
  xhttp.send(request);
};

rpctest.update_jsonrpc = function()   {
  var req = rpctest.all_req[rpctest.curr_request];
  var txt = req.toString(); // send to server
  rpctest.call_jsonrpc(txt, '/TDBDATA/JSONRPC');
};

rpctest.update_rest = function()   {
  var req = rpctest.all_req[rpctest.curr_request];
  var txt = JSON.stringify(req.params);
  rpctest.call_jsonrpc(txt, '/TDBDATA/REST/json/'+req.method);
};

rpctest.update_websocket = function()  {
  var req = rpctest.all_req[rpctest.curr_request];
  var txt = req.toString(); // send to server
  rpctest.webSocket.rq = txt;
  rpctest.webSocket.send(txt);
};

//   Update function
rpctest.update = function()   {
  ++rpctest.count;
  ++rpctest.curr_request;
  if ( rpctest.curr_request == rpctest.all_req.length )
    rpctest.curr_request = 0;
  
  var m = rpctest.count%3;
  if ( m == 0 )
    rpctest.update_jsonrpc();
  else if ( m == 1 )
    rpctest.update_rest();
  else if ( m == 2 )
    rpctest.update_websocket();
  window.setTimeout(rpctest.update, 2000);
};

rpctest.stop = function()  {
  console.info('Closing connection.....');
};

rpctest.start = function()  {
  rpctest.raw = document.getElementById("raw");
  rpctest.raw.innerHTML = '----';
  rpctest.response = document.getElementById("response");
  rpctest.response.innerHTML = '----';
  rpctest.result = document.getElementById("result");
  rpctest.result.innerHTML = '----';
  rpctest.request = document.getElementById("request");
  rpctest.request.innerHTML = 'Web page is now initialized!';
  window.setTimeout(rpctest.update, 1000);
  connectToWS();
};
