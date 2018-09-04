//==========================================================================
// LHCb Web Status displays
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For details please see       Online/Stomp/README
// For the licensing terms see  Online/Stomp/LICENSE
//
// Author     : M.Frank
//
//==========================================================================

_loadFile('lhcb.display.ext','css');
_loadFile('lhcb.display.ext.fsm','css');
_loadScript('lhcb.display.zoom.cpp');
_loadScript('lhcb.display.extWidgets.cpp');

var OPTS_PANEL_NUM_ROWS = 15000;

/// Main entry point to create the 'LHCb Page1' web page
/*
 *    \author  M.Frank
 *    \version 2.0
 */
var LbExt_PropertyEditor = function(id, messages) {

  lbExt.PropertyEditor = this;

  var rpc = {};
  rpc.sync = {};
  rpc.async = {};
  ///
  rpc.printMessage = function(str)   {
    var err_str = '';
    var i = 0;
    while(i <= str.length)   {
      err_str = err_str + str.substr(i, 80) + '||\n';
      i = i + 80;
    }
    err_str = err_str + "<|||";
    alert(err_str);
  };
  lbExt.PropertyEditor.editor = {};
  lbExt.PropertyEditor.explorer = {};
  ///
  var message_box = function(icon, buttons, title)   {
    var msg = '';
    for(var i=3; i < arguments.length; i++) {
      msg = msg + arguments[i];
    }
    //alert(title+'\n\n'+msg);
    msg = msg.replace(/\n/gi, "<br>");
    Ext.Msg.show({
      title: title,
          message: msg,
          icon:    icon,
          buttons: buttons,
          fn:      function()  {}
      });
    msg = null;
  };

  var instanceCopy = function(obj)    {
    // copy the object by the constructor
    const copy = new obj.constructor();
    const keys = Object.keys(obj);
    keys.forEach(key => {
        copy[key] = obj[key]
          });
    return copy;
  };

  var emptyDomainData = function()  {
    return {expanded: true, children: [{ text: 'DNS-Domains', id: 'DNS-Domains', expanded: true, children: []}]};
  };
  var emptyNodeData = function()  {
    return {expanded: true, children: [{ text: 'Nodes', id: 'DIM-Nodes', expanded: true, children: []}]};
  };
  var emptyServerData = function()  {
    return {expanded: true, children: [{ text: 'Servers', id: 'DIM-Servers', expanded: true, children: []}]};
  };


  var to_dns_tree_data = function(result, para)  {
    /// Now convert to json:
    var root   = result.children[0].children;
    var tree   = {};
    var d, sf, r, subfarm, rack, v;
    for(var i = 0; i<para.length; i++)    {
      d = para[i];
      if ( d.substr(0,3).toLowerCase() == 'hlt' && d.length >= 6 )  {
        rack = d.substr(0,4);
        subfarm   = d.substr(0,6);
        if ( !tree[rack] )   {
          v = { text: rack, id: rack, expanded: false, children: []};
          tree[rack] = v;
          root.push(v);
        }
        r = tree[rack];
        if ( !r[subfarm] )  {
          v = { text: subfarm, id: subfarm+'.lbdaq.cern.ch', qtip: subfarm+'.lbdaq.cern.ch', leaf: true};
          r[subfarm] = v;
          r.children.push(v);
        }
      }
      else   {
        subfarm = d;
        v = { text: subfarm, id: subfarm+'.lbdaq.cern.ch', qtip: subfarm+'.lbdaq.cern.ch', leaf: true};
        root.push(v);
      }
    }
    return result;
  };

  var to_tree_data = function(result, para)   {
    /// Now convert to json:
    var root   = result.children[0].children;
    var d, v;
    for(var i = 0; i<para.length; i++)    {
      d = para[i];
      v = { text: d, id: d, qtip: d, leaf: true};
      root.push(v);
    }
    return result;
  };

  var to_job_properties = function(result,para)   {
    if ( para == null )
      return result;
    for(var i = 0; i<para.length; i++)    {
      var o = para[i];
      o.changed = '';
      o.identifier = i;
      result.push(o);
    }
    return result;
  };

  rpc.async.pending = [];

  rpc.async.call_args = function(args)   {
    var xhttp;
    if (window.XMLHttpRequest) {
      // code for modern browsers
      xhttp = new XMLHttpRequest();
    } else {
      // code for IE6, IE5
      xhttp = new ActiveXObject('Microsoft.XMLHTTP');
    }
    for(var i=0; i<rpc.async.pending.length; ++i)  {
      var rq = rpc.async.pending.pop();
      delete rq;
    }
    xhttp.onreadystatechange = function() {
      if (this.readyState == 4 && this.status == 200) {
        var str = this.responseText;
        if (typeof DOMParser != 'undefined') {
          var parser = new DOMParser();
          var obj    = parser.parseFromString(str, 'text/xml');
          var resp   = new XmlRpcResponse(obj);
          var para   = resp.parseXML();
          if ( para )   {
            if ( para.faultCode )   {
              message_box(Ext.Msg.ERROR,
                          Ext.Msg.OK,
                          'XMLRPC Error',
                          '<h2>Serious XMLRPC Error:</h2>',
			  'Server:\t'+this.responseURL+'',
                          '<h2>Code:\t'+para.faultCode+'</h2>',
                          '<h2>Error:\t'+para.faultString+'</h2>');
              para = null;
            }
	    //rpc.printMessage(str);
            var call = this.call.obj;
            var fun  = this.call.func;
            call[fun](para);
          }
          else  {
            message_box(Ext.Msg.ERROR,
                        Ext.Msg.OK,
                        'XMLRPC Error',
                        '<h2>XMLRPC Error</h2><br>',
			'Server:\t'+this.responseURL,
                        '<h2>Invalid XML reponse:</h2><br>',str);
	    rpc.printMessage(str);
          }
        }
        else  {
          message_box(Ext.Msg.ERROR,
                      Ext.Msg.OK,
                      'Browser Error',
                      '<h2>No DOM parser present!</h2><br>',
                      'This application will simply not work.');
        }
        //document.body.style.cursor = 'default';
        console.log('XMLRPC Asyncronous XMLHttpRequest ('+this.call.server+
		    ') finished.....(id:'+this.callID+' rpc.id:'+rpc.async.callID+')');
      }
      else if (this.readyState == 4) {
        if ( this.status == 400 )   {
          if ( this.call.retries < 5 )   {
            ++this.call.retries;
            console.log('RETRY # '+this.call.retries+': Asyncronous XMLHttpRequest ongoing.....');
            rpc.async.call_args(this.call);
            return;
          }
        }
        var opt = '';
        if ( this.status == 400 )
          opt = ' --> You can only retry again.';
        else if ( this.status == 404 )
          opt = ' --> Are you sure the server is configured to handle your request?';
	if ( !(this.status == 502 && this.callID < rpc.async.callID) )  {
	  var status_text = ''+this.status;
	  if ( status_text == "0" || this.statusText.length == 0 )  {
	    opt = opt + 
	      '<br><b> --> There is a good chance your login token expired.</b>' +
	      '<br><b> --> You should perhaps reload this page from scratch.</b>' +
	      '<br>';
	  }
	  message_box(Ext.Msg.ERROR,
		      Ext.Msg.OK,
		      'XMLRPC Communication error',
		      '<h2>Serious communitation issue:</h2><br>',
		      'Call ID:   '+this.callID+'  Last RPC call ID: '+rpc.async.callID+'<br>',
		      'HTTP Error '+this.status+'<br>',
		      '\tStatus:\t'+this.statusText+'<br>',
		      '\tInternal Retries:\t'+this.call.retries+'<br><br>',
		      opt);
	}
	else  {
	  console.log('XMLRPC Communication error: '+this.status+'  ['+this.statusText+
		      '] Retries:'+this.call.retries);
	}
        //document.body.style.cursor = 'default';
        console.log('XMLRPC Asyncronous XMLHttpRequest ('+this.call.server+
		    ') finished.....(id:'+this.callID+' rpc.id:'+rpc.async.callID+')');
      }
    };
    rpc.async.callID = rpc.async.callID+1;
    xhttp.callID     = rpc.async.callID;
    xhttp.open('POST', '/options/RPC2', true);
    //xhttp.setRequestHeader('Content-type',   'text/xml');
    //xhttp.setRequestHeader('Connection',     'close');
    xhttp.setRequestHeader('User-Agent',     'XMLRPC-Test');
    xhttp.setRequestHeader('Dim-DNS',        args.host);
    xhttp.setRequestHeader('Dim-Server',     args.server);
    xhttp.setRequestHeader('RPC-Port',       args.port+'');
    xhttp.call = args;
    console.log('XMLRPC Asyncronous XMLHttpRequest ('+args.server+
		') started.....(id:'+this.callID+' rpc.id:'+rpc.async.callID+')');
    xhttp.send(xhttp.call.request);
    rpc.async.pending.push(xhttp);
    xhttp = null;
  };
  /// Sequence number to identify async calls
  rpc.async.callID = 0;
  /// Generic asynchronous RPC call using XMLHttpRequest objects
  rpc.async.call = function(host, port, server, request, caller, callback)   {
    var text = request.build();
    var args = {obj: caller, func: callback, host: host, server: server, port: port, request: text, retries: 0};
    //document.body.style.cursor = 'wait';
    rpc.async.call_args(args);
  };
  rpc.async.loadDomains = function(caller, callback) {  
    var request = new XmlRpcRequest('/options/RPC2', 'domains');
    rpc.async.call('ecs03', '2601', '/RPCDomainInfo', request, caller, callback);
  };
  rpc.async.loadNodes = function(dns, caller, callback) {  
    var request = new XmlRpcRequest('/options/RPC2', 'nodes');
    request.addParam(dns);
    rpc.async.call('ecs03', '2601', '/RPCDomainInfo', request, caller, callback);
  };
  rpc.async.loadServers = function(dns, node, caller, callback) {  
    var request = new XmlRpcRequest('/options/RPC2', 'tasks');
    request.addParam(dns);
    request.addParam(node);
    rpc.async.call('ecs03', '2601', '/RPCDomainInfo', request, caller, callback);
  };
  rpc.async.loadServersProperties = function(dns, server, caller, callback) {  
    var request = new XmlRpcRequest('/options/RPC2', 'allProperties');
    request.addParam(dns);
    request.addParam(server);
    rpc.async.call(dns, '2600', server, request, caller, callback);  
  };
  rpc.async.sendServersProperties = function(dns, server, props, caller, callback) {  
    var request = new XmlRpcRequest('/options/RPC2', 'setProperties');
    request.addParam(props);
    return rpc.async.call(dns, '2600', server, request, caller, callback);
  };
  ///
  /// Synchronous calls:
  rpc.sync.call = function(dns, port, server, request) {  
    request.setHeader('Content-type',   'text/xml');
    request.setHeader('User-Agent',     'XMLRPC-Test');
    request.setHeader('Dim-DNS',        dns);
    request.setHeader('Dim-Server',     server);
    request.setHeader('RPC-Port',       port+'');
    var resp = request.send();
    var para = resp[0].parseXML();
    if ( para==null || ''+para == 'undefined' )  {
      message_box(Ext.Msg.ERROR,
                  Ext.Msg.OK,
                  'XMLRPC Error','ERROR '+resp[1]);
      return null;
    } 
    return para;
  };
  rpc.sync.loadDomains = function() {  
    var request = new XmlRpcRequest('/options/RPC2', 'domains');
    var para    = rpc.sync.call('ecs03', '2601', '/RPCDomainInfo', request);
    if ( para == null )
      return emptyDomainData();
    return to_dns_tree_data(emptyDomainData(), para);
  };
  rpc.sync.loadNodes = function(dns) {  
    var request = new XmlRpcRequest('/options/RPC2', 'nodes');
    request.addParam(dns);
    var para    = rpc.sync.call('ecs03', '2601', '/RPCDomainInfo', request);
    if ( para == null )
      return emptyNodeData();
    return to_tree_data(emptyNodeData(), para);
  };
  rpc.sync.loadServers = function(dns, node) {  
    var request = new XmlRpcRequest('/options/RPC2', 'tasks');
    request.addParam(dns);
    request.addParam(node);
    var para    = rpc.sync.call('ecs03', '2601', '/RPCDomainInfo', request);
    if ( para == null )
      return emptyServerData();
    return to_tree_data(emptyServerData(), para);
  };
  rpc.sync.loadServerProperties = function(dns, server) {  
    var request = new XmlRpcRequest('/options/RPC2', 'allProperties');
    request.addParam(dns);
    request.addParam(server);
    var para = rpc.sync.call(dns, '2600', server, request);  
    return to_job_properties([],para);
  };
  rpc.sync.sendServerProperties = function(dns, server, props) {  
    var request = new XmlRpcRequest('/options/RPC2', 'setProperties');
    request.addParam(props);
    return rpc.sync.call(dns, '2600', server, request);  
  };

  // Update window title
  lbExt.setWindowTitle('LHCb Online Job Property Editor');

  /// Simple convenience function to save us some typing.
  this.header = function(opts)  {
    page_id = opts && opts.id ? opts.id : 'dummy_page_id';
    var items = [lbExt.pageHeader('LHCb',{ id: page_id+'_page_header',
            time:     false,
            sysinfo:  true,
            zoom:     true,
            left:     opts.left,
            right:    opts.right,
            center:   opts.center
            })
      ];
    page_id = null;
    return items;
  };

  ///
  lbExt.PropertyEditor.genericTreePanel = function(opts)  {
    var options = {};
    /// Setup options for the domain tree:
    options.title        = 'Tree data',
    options.width        = '100%',
    options.height       = '100%',
    options.rootVisible  = false,
    options.autoScroll   = true,
    options.collapsible  = true,
    options.iframe       = null,
    options.listeners    = {}; 
    if ( opts.get_data )  {
      options.root = opts.get_data();
    }
    options = lbExt.setOptions(options,opts);
    var panel = Ext.create('Ext.tree.Panel', options);
    options = null;
    data = null;
    if ( opts.propertyContext )  {
      panel.propertyContext = opts.propertyContext;
    }
    options = null;
    return panel;
  };

  lbExt.PropertyEditor.editor.controlPanel = function(opts)   {
    var width   = opts.hasOwnProperty('width')   ? opts.width  : '100%';
    var id      = opts.hasOwnProperty('id')      ? opts.id : 'option-edit-panel';
    var options = {
    id:           id,
    width:        '100%',
    height:       '100%',
    frame:         false,
    autoScroll:    true,
    collapsible:   false,
    minHeight:     150,
    layout:      { type: 'table', columns: 9,
                   tableAttrs:   { style: { padding: '10 0 10 10' }}},
    defaults: {  border:             true,
                 xtype:              'box',
                 style: { textAlign: 'center',
                          fontSize:  'big',
                          fontWeight:'bold',
                          padding:   '4 8 4 8'
        }
    },
    items: [  /// First line of buttons items: 0...6
      { html: 'First',  xtype: 'button', width: 100, icon: 'Images/icons/bld_undo.png', 
	handler: function() { this.panel.propertyContext.table.showFirst(); } 
      },
      { html: 'Previous',  xtype: 'button', width: 100, icon: 'Images/icons/bld_undo.png', 
	handler: function() { this.panel.propertyContext.table.showPrevious(); } 
      },
      { html: ' ', width:  50 },
      { html: ' ..... ',   cls: 'MonitorDataHeader Text-Bold', width: 350, colspan: 3 },
      { html: ' ', width:  50 },
      { html: 'Next',  xtype: 'button', width: 100, icon: 'Images/icons/bld_redo.png', 
	handler: function() { this.panel.propertyContext.table.showNext(); } 
      },
      { html: 'Last',  xtype: 'button', width: 100, icon: 'Images/icons/bld_redo.png', 
	handler: function() { this.panel.propertyContext.table.showLast(); } 
      },
      /*
      /// Second line of buttons. items 8...10
      //matchClient,
      { html: '',  xtype: 'textfield', width: 250, enableKeyEvents: true, colspan: 3,
	handler: function(field, newVal, oldVal, eOpts)  
	{  this.panel.propertyContext.table.match(newVal, this.panel.matchOption.getValue());	}
      },
      { html: 'Client-name <<< Match >>> Option-name', width: 350, cls: 'MonitorDataHeader Text-Center Text-Bold', colspan: 3 },
      //matchOption.
      { html: '',  xtype: 'textfield', width: 250, enableKeyEvents: true, colspan: 3,
	handler: function(field, newVal, oldVal, eOpts)
	{  this.panel.propertyContext.table.match(this.panel.matchClient.getValue(), newVal);	}
      },
      */
      /// Third line.  items 11...17
      { html: 'Reload',  xtype: 'button', width: 200, colspan: 2, icon: 'Images/icons/refresh.png', 
	handler: function() { this.panel.propertyContext.table.reload(); } 
      },
      { html: ' ', width:  50 },
      { html: 'Move to property',  xtype: 'button', width: 150, icon: 'Images/icons/ed_execute.png',
	handler: function()  {
	  var value = this.panel.input.getValue();
	  if ( !Ext.isEmpty(value) )  {
	    this.panel.propertyContext.table.gotoProperty(Number(value));
	  }
        }
      },
      { html: ' ', width:  20 },
      { html: '',  xtype: 'textfield', width: 200, enableKeyEvents: true,
	handler: function(field, e)  {
	  var value = this.getValue();
	  if ( e.getKey() == e.ENTER && !Ext.isEmpty(value) )  {
	    this.panel.propertyContext.table.gotoProperty(Number(value));
	  }
        }
      },
      { html: ' ', width:  50 },
      { html: 'Sent Changes to Server',  xtype: 'button', width: 200, colspan: 2, icon: 'Images/icons/bld_exit.png', 
	handler: function() { this.panel.propertyContext.table.sendChanges(); } 
      }
	      ],
    buttons: []
    };
    options = lbExt.setOptions(options,opts);
    var panel  = lbExt.create('Ext.panel.Panel',options);
    panel.label       = panel.items.get(3);
    panel.input       = panel.items.get(11);
    panel.input.on('specialKey', panel.input.handler, panel.input);
    /*  3 line option with restriction
    panel.input       = panel.items.get(14);
    panel.input.on('specialKey', panel.input.handler, panel.input);
    panel.matchClient = panel.items.get(7);
    panel.matchClient.on('change', panel.matchClient.handler, panel.matchClient);
    panel.matchOption = panel.items.get(9);
    panel.matchOption.on('change', panel.matchOption.handler, panel.matchOption);
    */
    if ( options.tooltip )  {
      elt.tooltip = new Ext.ToolTip({ target: elt.id, trackMouse: true, html: options.tooltip});
    }
    for(var i=0; i<panel.items.getCount(); ++i)
      panel.items.get(i).panel = panel;
    options = width = id = null;
    return panel;
  };

  /// Editor panel for a single option
  /**  
   *  \author  M.Frank
   *  \version 1.0
   */
  lbExt.PropertyEditor.editor.propertyEditor = function(opts)   {
    var width        = opts.hasOwnProperty('width') ? opts.width  : '100%';
    var id           = opts.hasOwnProperty('id')    ? opts.id : 'option-edit-panel';
    var reloadButton = Ext.create('Ext.Button', {
      text:  'Reload',
          width:   100,
          icon:    'Images/icons/bld_redo.png',
          handler: function()  {
          var ctx = this.panel.propertyContext;
          this.panel.edit(ctx.table.properties[this.panel.current.identifier]);
        }
      });
    var saveButton = Ext.create('Ext.Button', {
      text:  'Save',
          width:   100,
          icon:    'Images/icons/bld_save.png',
          handler: function()  {
          var ctx = this.panel.propertyContext;
          ctx.editor.current.value = ctx.editor.items.get(2).getValue();
          ctx.table.updateProperty(ctx.editor.current);
        }
      });
    var options = {
    id:           id,
    title:        'Job Options Editor: Click in the lower panel to load values',
    width:        '100%',
    height:       '100%',
    minHeight:     120,
    autoScroll:    true,
    collapsible:   true,
    layout:      { type: 'table', columns: 4,
                   tableAttrs:   { style: { padding: '10 0 10 10' }}},
    defaults: {  border:             true,
                 xtype:              'box',
                 style: { textAlign: 'left',
                          fontSize:  'big',
                          fontWeight:'bold',
                          padding:   '4 8 4 8'
        }
    },
    items: [{ html: 'DIM DNS Node',   cls: 'MonitorDataHeader Text-Bold', width: 150 }, // 0
      { html: '----',                 cls: 'MonitorDataItem   Text-Bold', width: 200 },
      { html: '',                     cls: 'MonitorDataItem   Text-Bold', rowspan: 4, height: 100, width: 500, xtype: 'textarea' },
      { html: '',                      },

      { html: 'Target Node',          cls: 'MonitorDataHeader Text-Bold' },
      { html: '----',                 cls: 'MonitorDataItem   Text-Bold' },
            reloadButton,
      { html: 'Server',               cls: 'MonitorDataHeader Text-Bold' },
      { html: '----',                 cls: 'MonitorDataItem   Text-Bold' },
            saveButton,

      { html: '..component-option..', cls: 'MonitorDataHeader Text-Bold', colspan: 2 },
      { html: '',                      }
            ],
    buttons: []
    };
    options = lbExt.setOptions(options,opts);
    var panel  = lbExt.create('Ext.panel.Panel',options);
    reloadButton.panel = panel;
    saveButton.panel   = panel;
    ///
    /// Update the basic information for the legend at the left
    panel.update = function(dns, node, server)   {
      this.items.get(1).update(dns);
      this.items.get(5).update(node);
      this.items.get(8).update(server);
    };
    ///
    /// New option to be edited. Deep copy and update the legend
    panel.edit = function(opt)   {
      this.current = instanceCopy(opt);
      this.items.get(2).setValue(opt.value);
      this.items.get(10).update('ID '+opt.identifier+':  '+opt.client+'.'+opt.name);
    };
    ///
    /// New option to be edited. Deep copy and update the legend
    panel.unedit = function()   {
      this.current = null;
      this.update('----','---','----');
      this.items.get(2).setValue('');
      this.items.get(10).update('No option associated to editor panel');
    };
    if ( options.tooltip )  {
      elt.tooltip = new Ext.ToolTip({ target: elt.id, trackMouse: true, html: options.tooltip});
    }
    saveButton = reloadButton = options = width = id = null;
    return panel;
  };


  /// The table object holding the properties of one single client
  /**  
   *  \author  M.Frank
   *  \version 1.0
   */
  lbExt.PropertyEditor.editor.propertyTable = function(opts)   {
    var store  = new Ext.data.JsonStore({
      fields: [{name: 'identifier'},
               {name: 'client'},
               {name: 'name'},
               {name: 'value'},
               {name: 'changed'}
               ],
          autoLoad: true,
          data: []
          });
    ///
    store.__load_data = function(data)   {
      var d = [], i, p, s, m, keys;
      for(i=0; i<data.length; ++i)   {
	p = instanceCopy(data[i]);
	if ( p.value )   {
	  s = p.value.replace(/</gi,'&lt;').replace(/>/gi,'&gt');
	  p.value = s;
	  d.push(p);
	  continue;
	}
	p.value = '';
	d.push(p);
	
	/*
	s = '';
	keys = Object.keys(p);
	keys.forEach(key => {
	    s = s + ' '+key+'='+p[key]
	      });
	console.log('Invalid option: no value: '+p+' = '+s);
	*/
      }
      this.loadData(d);
      keys = m = d = i = s = p = null;
    };
    var loader = opts.loader ? opts.loader : {
    id:            'hlt2RunsTableLoader',
    datapoint:     opts.datapoint,
    interval:      opts.interval ? opts.interval : 20,
    numOptions:    0,
    columns:       3,
    store:         store
    };
    var options = {
    title:        'Job property table',
    width:        '100%',
    height:       '100%',
    layout:       'fit',
    defaults:   {  width: '100%', cls: 'Text-Bold' },
    items: [{
        xtype:            'grid',
        buttonAlign:      'right',
        stripeRows:       true,
        autoExpandColumn: 'value',
        flex:             1,
        style:            {fontSize: '1em'},
        minHeight:        150,
        store:            loader.store,
        viewConfig:       {cls: 'Text-Bold' },
        defaults:         {width: '100%' },
        columns: [
      {id:'identifier', header: 'ID',       dataIndex: 'identifier', sortable: true,  width: '5%',  cls: 'Text-Left' },
      {id:'client',     header: 'Client',   dataIndex: 'client',     sortable: true,  width: '15%', cls: 'Text-Left' },
      {id:'option',     header: 'Option',   dataIndex: 'name',       sortable: true,  width: '15%', cls: 'Text-Left' },
      {id:'value',      header: 'Value',    dataIndex: 'value',      sortable: true,  width: '56%', cls: 'Text-Left' },
      {id:'changed',    header: 'Changed',  dataIndex: 'changed',    sortable: true,  width: '8%',  cls: 'Text-Center Bg-lightred' }
                  ],
        listeners:  {
          celldblclick: function(view, td, index, record, tr, row, evt, opts) {
              //alert('Double Click: \n'+index+'\n'+row+'\n'+td.innerHTML+'\n'+tr.innerHTML);
            },
          cellclick: function(view, td, index, record, tr, row, evt, opts) {
              panel.handleClick((OPTS_PANEL_NUM_ROWS*panel.optsBatch)+row, index);
            }  }
        }]};
    options = lbExt.setOptions(options, opts);
    var panel  = lbExt.create('Ext.panel.Panel', options);
    if ( !panel.id ) Ext.id(panel,'property-table');
    panel.baseTitle = options.title;
    panel.loader    = loader;
    loader.panel    = panel;
    loader.title    = opts.title;
    panel.optsBatch = -1;
    panel.handleClick = function(row, col)   {
      if ( col == 4 )   {
        if ( this.propertyContext.editor.current )   {
        }
      }
      else   {
        this.propertyContext.editor.edit(this.properties[row]);
      }
    };
    /// Control callback: reload the options using RPC
    panel.reload = function()  {
      var ctx = this.propertyContext;
      if ( ctx.optsBatch > -1 ) --ctx.table.optsBatch;
      ctx.panel.loadOptions(ctx.dns, ctx.node, ctx.server);
      ctx = null;
    };
    /// Jump to a property by ID
    panel.gotoProperty = function(which)   {
      var ctx = this.propertyContext;
      var num = Math.trunc(which/OPTS_PANEL_NUM_ROWS)-1;
      //alert(which+'   '+num);
      ctx.panel.propertyContext.table.optsBatch = num;
      ctx.panel.propertyContext.table.showNext();
      num = ctx = null;
    };
    /// Select properties using a client match
    panel.selectPropertyClient = function(cl)   {
      var ctx = this.propertyContext;
      alert('Not implemented: '+cl);
      ctx = null;
    };
    /// Select properties using a property name match
    panel.selectPropertyName = function(nam)   {
      var ctx = this.propertyContext;
      alert('Not implemented: '+nam);
      ctx = null;
    };
    /// Control callback: Send changes to target process
    panel.sendChanges = function()  {
      var changes = [];
      var ctx = this.propertyContext;
      for(ip in this.properties)   {
	var p = this.properties[ip];
        if ( p.changed != '' )  {
          changes.push({client: p.client, name: p.name, value: p.value});
        }
      }
      rpc.async.sendServersProperties(ctx.dns, ctx.server, changes, this, 'change_cb');
      changes = ctx = null;
    };
    panel.change_cb = function(num_changed)   {
      var changed = 0;
      var ctx = this.propertyContext;
      for(ip in this.properties)   {
	var p = this.properties[ip];
        if ( p.changed != '' )
          ++changed;
      }
      if ( num_changed == changed )  
        message_box(Ext.Msg.INFO,
                    Ext.Msg.OK,
                    'Status: Success',
                    '<h2>Success</h2><br>',
                    'Updated '+num_changed+' properties in server:'+ctx.server);
      /*
      else
        message_box(Ext.Msg.ERROR,
                    Ext.Msg.OK,
                    'Status: Failed',
                    '<h1>FAILURE</h1><br>',
                    'FAILED to update properties in server:'+ctx.server+'<br>',
                    'Result="'+num_changed+'"');
      */
      ctx = null;
    };
    ///
    /// Show on request the first batch of options in the table
    panel.showFirst = function()  {
      this.optsBatch = 1;
      return this.showPrevious();
    };
    ///
    /// Show on request the previous batch of options in the table
    panel.showPrevious = function()  {
      if ( this.optsBatch > 0 )
        --this.optsBatch;

      var opts = [];
      var first =  this.optsBatch*OPTS_PANEL_NUM_ROWS;
      var last  = (this.optsBatch+1)*OPTS_PANEL_NUM_ROWS;

      if ( this.properties.length == 0 )   {
        this.loader.store.__load_data(this.properties);
        return;
      }
      else if ( this.properties.length <= first )   {
        return;
      }
      else if ( last <= 0 )   {
        return;
      }
      if ( this.properties.length < last )   {
        last = this.properties.length;
      }
      for(var i=first; i < last; ++i)
        opts.push(this.properties[i]);
      if ( opts.length > 0 )
        this.propertyContext.control.label.update(' Display shows job properties '+first+' to '+last);
      else
        this.propertyContext.control.label.update(' No job properties to be displayed');
      this.loader.store.__load_data(opts);
      return this;
    };
    ///
    /// Show on request the next batch of options in the table
    panel.showNext = function()  {
      var opts = [];
      if ( (this.optsBatch+1) <= this.properties.length/OPTS_PANEL_NUM_ROWS )
        ++this.optsBatch;
      var first = this.optsBatch*OPTS_PANEL_NUM_ROWS;
      var last  = (this.optsBatch+1)*OPTS_PANEL_NUM_ROWS;

      if ( this.properties.length == 0 )   {
        this.loader.store.__load_data(this.properties);
        return;
      }
      else if ( this.properties.length <= first )   {
        return;
      }
      else if ( this.properties.length < last )   {
        last = this.properties.length;
      }

      for(var i=first; i < last; ++i)
        opts.push(this.properties[i]);
      if ( opts.length > 0 )
        this.propertyContext.control.label.update(' Display shows job properties '+first+' to '+last);
      else
        this.propertyContext.control.label.update(' No job properties to be displayed');
      this.loader.store.__load_data(opts);
    };
    ///
    /// Show on request the last batch of options in the table
    panel.showLast = function()  {
      this.optsBatch = Math.ceil(this.properties.length/OPTS_PANEL_NUM_ROWS);
      return this.showPrevious();
    };
    ///
    /// Match the displayed options according to criteria
    panel.match = function(cl, opt)  {
      if ( !Ext.isEmpty(cl) || !Ext.isEmpty(opt) )   {
	alert('NOT Implemented!  match: '+cl+' '+opt);
	this.showFirst();
      }
    };
    ///
    /// Update a property if the value changed (Called by option editor)
    panel.updateProperty = function(prop)   {
      var p = this.properties[prop.identifier];
      if ( p.value != prop.value ) prop.changed = '**Changed**';
      this.properties[prop.identifier] = instanceCopy(prop);
      //if ( this.optsBatch >= 0 ) --this.optsBatch;
      this.loadData(this.properties);
    };
    ///
    /// Load options into the table
    panel.loadData = function(props)    {
      this.properties = props;
      if ( props.length == 0 )
        this.optsBatch = -1;
      else if ( this.optsBatch >= 0 )
        --this.optsBatch;
      this.showNext();
      this.setTitle(this.baseTitle+': Total Number of options:'+this.properties.length);
    };
    ///
    /// Invoke rpc call to get client options
    panel.load = function(dns, node, server)    {
      rpc.async.loadServersProperties(dns, server, this, 'process');
    };
    ///
    /// Callback for asyncronous ajax processing
    panel.process = function(para)  {
      var props = to_job_properties([],para);
      this.loadData(props);
    };
    options = null;
    return panel;
  };

  /// Container widget to edit properties.
  /**  
   *  - The container on the top has a single option editor
   *  - In the middle there is the table to scan through the 
   *    options set of one server
   *  - At the bottom we have a controls pannel to sweep through the set
   *    reaload the options or save changes.
   *
   *  \author  M.Frank
   *  \version 1.0
   */
  lbExt.PropertyEditor.editor.panel = function(opts)   {
    var width   = opts.hasOwnProperty('width')   ? opts.width  : '100%';
    var id      = opts.hasOwnProperty('id')      ? opts.id : 'option-editor';
    var context = {
    editor:  lbExt.PropertyEditor.editor.propertyEditor({flex: 2, id: id+'-edit-panel'}),
    table:   lbExt.PropertyEditor.editor.propertyTable({flex: 6, id: id+'-options-selector-table'}),
    control: lbExt.PropertyEditor.editor.controlPanel({flex: 1, id: id+'-control-panel'})
    };
    var options = lbExt.panelDefaults({title: null,
          id:          id,
          frame:       false,
          autoHeight:  true,
          autoWidth:   true,
          autoScroll:  true,
          collapsed:   false,
          collapsible: false,
          width:       width,    
          layout:    { type:   'vbox' },
          defaults:  { width: '100%',
            height:           '100%',
            border:           true,
            align:            'top',
            collapsible:      true,
            flex:             1
            },
          items: [context.editor, context.table, context.control]
          }, opts);
    var panel = lbExt.create('Ext.panel.Panel', options);
    context.panel = panel;
    panel.propertyContext = context;
    for( var i=0; i<options.items.length; ++i )
      options.items[i].propertyContext = context;
    ///
    /// Callback to load a new batch of options into the options table
    panel.loadOptions = function(dns, node, server)   {
      this.propertyContext.dns = dns;
      this.propertyContext.node = node;
      this.propertyContext.server = server;
      this.propertyContext.table.load(dns, node, server);
      this.propertyContext.editor.update(dns, node, server);
    };
    ///
    /// Reset data and clear the options table
    panel.clear = function()  {
      this.propertyContext.table.loadData([]);
      this.propertyContext.editor.unedit();
    };
    options = null;
    return panel;
  };

  /// Widget to explore the online system on the search for proceses
  /**  
   *
   *  \author  M.Frank
   *  \version 1.0
   */
  lbExt.PropertyEditor.explorer.panel = function()  {
    var context = {items: items, dns: '', node: '', server: ''};
    var domain_reload = Ext.create('Ext.Button', {
      text:  'Reload',
          width: '100%',
          height: 30,
          icon:   'Images/icons/refresh.png',
          handler: function()  {
          var ctx = this.propertyContext;
          rpc.async.loadDomains(ctx.domains, 'process');
        }
      });
    domain_reload.propertyContext = context;
    var nodes_reload = Ext.create('Ext.Button', {
      text:  'Reload',
          width:   '100%',
          height:   30,
          icon:   'Images/icons/refresh.png',
          handler:  function()  {
          var ctx = this.propertyContext;
          rpc.async.loadNodes(ctx.dns, ctx.nodes, 'process');
        }
      });
    nodes_reload.propertyContext = context;
    var servers_reload = Ext.create('Ext.Button', {
      text:  'Reload',
          width: '100%',
          height: 30,
          icon:   'Images/icons/refresh.png',
          handler: function()  { 
          var ctx = this.propertyContext;
          rpc.async.loadServers(ctx.dns, ctx.node, ctx.servers, 'process');
        }
      });
    servers_reload.propertyContext = context;
    ///
    /// Create selection tree for all DIM domains
    var domain_tree = genericTreePanel({title: 'DIM Domains',
          id:   id+'-dim-domains',
          flex: 1,
          propertyContext: context,
          get_data:  emptyDomainData,
          listeners:  {
        itemclick: function(view, rec, item, index, evtObj) {
            if ( rec.get('leaf') == true)   {
              var ctx = this.propertyContext;
              ctx.dns = rec.raw.text;
              rpc.async.loadNodes(ctx.dns, ctx.nodes, 'process');
            }  }  }
      });
    ///
    /// Callback for asyncronous ajax processing
    domain_tree.process = function(para)   {
      var ctx = this.propertyContext;
      var domains = para == null ? emptyDomainData() : to_dns_tree_data(emptyDomainData(), para);
      ctx.domains.setRootNode(domains);
      ctx.nodes.setRootNode(emptyNodeData());
      ctx.servers.setRootNode(emptyServerData());
      ctx.table.clear();
    };
    ///
    /// Create selection tree for sunfarm nodes
    var nodes_tree = genericTreePanel({title: 'Subfarm Nodes',
          id:   id+'-dim-dns-nodes',
          propertyContext: context,
          flex: 1,
          root: {children: [{ text: 'Nodes', id: 'DNS-Nodes', expanded: true, children: []}]},
          listeners: {
        itemclick: function(view, rec, item, index, evtObj) {
            if ( rec.get('leaf') == true)   {
              var ctx = this.propertyContext;
              ctx.node = rec.raw.text;
              rpc.async.loadServers(ctx.dns, ctx.node, ctx.servers, 'process');
            } } }
      });
    ///
    /// Callback for asyncronous ajax processing
    nodes_tree.process = function(para)   {
      var ctx = this.propertyContext;
      var nodes = para==null ? emptyNodeData() : to_tree_data(emptyNodeData(), para);
      ctx.nodes.setRootNode(nodes);
      ctx.servers.setRootNode(emptyServerData());
      ctx.table.clear();
    };
    ///  
    /// Create selection tree for DIM servers
    var servers_tree = genericTreePanel({title: 'Processes',
          id:   id+'-dim-node-servers',
          flex: 1,
          propertyContext: context,
          root: {children: [{ text: 'Servers', id: 'DIM-Servers', expanded: true, children: []}]}, 
          listeners: {
        itemclick: function(view, rec, item, index, evtObj) {
            if ( rec.get('leaf') == true)   {
              var ctx    = this.propertyContext;
              ctx.server = rec.raw.text;
              ctx.table.clear();
              ctx.table.loadOptions(ctx.dns, ctx.node, ctx.server);
            } } }
      });
    ///
    /// Callback for asyncronous ajax processing
    servers_tree.process = function(para)   {
      var ctx = this.propertyContext;
      var servers = para==null ? emptyServerData() : to_tree_data(emptyServerData(), para);
      ctx.servers.setRootNode(servers);
      ctx.table.clear();
    };
    ///
    var table = lbExt.PropertyEditor.editor.panel({id:   id+'-property-editor',
          propertyContext: context,
          flex: 7});
    var items = [{xtype:       'panel',
                  framed:      false,
                  collapsible: false,
                  layout:      'vbox',
                  flex:        1,
                  height:      '100%',
                  items:       [domain_tree, domain_reload ]
      },         {xtype:       'panel',
                  framed:      false,
                  collapsible: false,
                  layout:      'vbox',
                  flex:        1,
                  height:      '100%',
                  items:       [nodes_tree, nodes_reload ]
      },         {xtype:       'panel',
                  framed:      false,
                  collapsible: false,
                  layout:      'vbox',
                  flex:        2,
                  height:      '100%',
                  items:       [servers_tree, servers_reload ]
      },
      table
      ];

    var explorer_tab = {
    id:               id+'-dim-explorer',
    xtype:            'panel',
    width:            '100%',
    height:            600,
    framed:            false,
    border:            true,
    layout:          { type: 'hbox' },
    propertyContext:   context,
    defaults:  {
      width:        '100%',
      height:       '100%',
      border:       true,
      align:        'top',
      collapsible:  true,
      flex:         1
      },
    items: items
    };
    ///
    var opts = lbExt.panelDefaults({
      xtype: 'framed-tabs',
          title:  'Job Property Manipulation',
          layout: 'fit',
          items:  [ explorer_tab ]
          },{});
    var panel = lbExt.create('Ext.panel.Panel',opts);
    context.items = [domain_tree, nodes_tree, servers_tree, table];
    context.explorer = panel;
    context.domains = domain_tree;
    context.nodes   = nodes_tree;
    context.servers = servers_tree;
    context.table   = table;
    rpc.async.loadDomains(domain_tree, 'process');
    return panel;
  };
  ///
  /// Once the body is ready, this function is called
  Ext.onReady(function()  {
      var logger   = lbExt.logger({active: messages, level: LOG_INFO, style: { height: 300 }});
      var provider = null;

      /// Create view port
      var view = null, msg = null, elt = null;
      try {
        view = lbExt.create('Ext.Viewport', {
          id:                 id,
              padding:        '6px',
              layout:         'border',
              defaults:     { bodyStyle:    'padding: 0px' },
              items: [{ id: 'hlt2-header',
                  region:   'north',
                  items:    this.header({id: id, 
                        left:   { title: lbExt.lhcb_online_picture()+'&nbsp;<B>Online Job Property Editor</B>'}})
                  },
                {   id:         'center-region',
                    region:     'center',
                    autoScroll: true,
                    layout: {   type: 'hbox' },
                    items:[ { autoScroll: true,
                        height:   '100%',
                        flex:     3,
                        layout:   {type: 'vbox' , defaults: { split: true } },
                        items: [this.explorer.panel()]}
                      ]
                    },
                {   region:   'south',
                    items:    [lbExt.LHCb_copyright({}),
                               logger]
                    }
                ],
              renderTo: Ext.getBody()
              });
        /// Attach these to the view in case we need them later....
        view.logger = logger;
      }
      catch(e)  {
        msg = 'Ext:'+Ext.version+'<br><br>Exception: '+e;
        message_box(Ext.Msg.ERROR,
                    Ext.Msg.OK,
                    'XMLRPC Error',
                    'Exception in JavaScript:',
                    msg);
        console.error('[EXCEPTION] ' + msg);
      }
      /// Initialize tool tips
      Ext.QuickTips.init();
    });
};
/// Unload entry
var jobproperties_unload = function()  {
};
///
/// Main entry point
var jobproperties_body = function()  {
  var msg  = the_displayObject['messages'];
  return LbExt_PropertyEditor('lhcb-properties-main', msg);
};
console.info('Script lhcb.display.jobproperties.cpp loaded successfully');
