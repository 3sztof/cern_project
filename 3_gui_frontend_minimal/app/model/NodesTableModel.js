Ext.define('LHCb.model.NodesTableModel', {
    extend: 'Ext.data.Model',

    itemId: 'nodestablemodel',

    alias: 'viewmodel.nodestable',

    fields: [
        {name: 'regex', type: 'string'},
        {name: 'description', type: 'string'}, 
    ],

    proxy: {
        disableCache: false,
        method: 'POST',
        type: 'myproxy',
        dataType: 'json',
        actionMethods : {create: "POST", read: "POST", update: "POST", destroy: "POST"},
        jsonData: new JSON_RPC.Request("getNode", [{"regex":"*"}]),
        reader: {
            type: 'json',
            rootProperty: 'result'
        },
        writer: {
            method:'POST',
            type: 'json',
            writeAllFields: true,
        },
        useDefaultXhrHeader : false,
        noCache: false,
        limitParam: undefined,
        pageParam: undefined,
        startParam: undefined,
        //url: 'http://localhost:8080/tasks'
        url: LHCb.variables.GlobalVariables.request_path
    },

});