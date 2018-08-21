Ext.define('LHCb.model.TaskSetsTableModel', {
    extend: 'Ext.data.Model',

    alias: 'viewmodel.tasksetstable',

    fields: [
        {name: 'task_set', type: 'string'},
        {name: 'description', type: 'string'}, 
    ],

    proxy: {
        disableCache: false,
        method: 'POST',
        type: 'myproxy',
        dataType: 'json',
        actionMethods : {create: "POST", read: "POST", update: "POST", destroy: "POST"},
        jsonData: new JSON_RPC.Request("getSet", [{"task_set":"*"}]),
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
        url: 'http://localhost:8081/TDBDATA/JSONRPC'
    },

});