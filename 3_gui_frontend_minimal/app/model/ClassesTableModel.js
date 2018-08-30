Ext.define('LHCb.model.ClassesTableModel', {
    extend: 'Ext.data.Model',

    itemId: 'classestablemodel',

    alias: 'viewmodel.classestable',

    fields: [
        {name: 'node_class', type: 'string'},
        {name: 'description', type: 'string'}, // There is more (lookup DB table) but these descriptions are only for formatting which always is string in our case...
    ],

    proxy: {
        disableCache: false,
        method: 'POST',
        type: 'myproxy',
        dataType: 'json',
        actionMethods : {create: "POST", read: "POST", update: "POST", destroy: "POST"},
        jsonData: new JSON_RPC.Request("getClass", [{"node_class":"*"}]),
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
        url: "http://localhost:8081/TDBDATA/JSONRPC"
    },

});