Ext.define('LHCb.model.TasksTableModel', {
    extend: 'Ext.data.Model',
    // requires: [
    //     'KitchenSink.model.field.PhoneNumber',
    //     'Ext.data.proxy.Rest'
    // ],

    itemId: 'taskstablemodel',

    alias: 'viewmodel.taskstable',

    fields: [
        {name: 'task', type: 'string'},
        {name: 'command', type: 'string'}, // There is more (lookup DB table) but these descriptions are only for formatting which always is string in our case...
        // {name: 'phone', type: 'phonenumber' },
        // {name: 'price', type: 'float'},
        // { name: 'priceChange', type: 'float' },
        // { name: 'priceChangePct', type: 'float' },
        // { name: 'priceLastChange', type: 'date', dateReadFormat: 'n/j' },

        // // Calculated field. Depends on price value. Adds it to price history.
        // // Trend begins with the current price. Changes get pushed onto the end
        // {
        //     name: 'trend',
        //     calculate: function(data) {
        //         // Avoid circular dependency by hiding the read of trend value
        //         var trend = data['trend'] || (data['trend'] = []);

        //         trend.push(data.price);

        //         if (trend.length === 1) {
        //             //let's start the trend off with a change
        //             trend.push(data.price + data.priceChange);
        //         }

        //         if (trend.length > 10) {
        //             trend.shift();
        //         }

        //         return trend;
        //     },
    ],

    proxy: {
        disableCache: false,
        method: 'POST',
        type: 'myproxy',
        dataType: 'json',
        actionMethods : {create: "POST", read: "POST", update: "POST", destroy: "POST"},
        jsonData: new JSON_RPC.Request("getTask", [{"task":"*"}]),
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
        url: 'http://localhost:8081/TDBDATA/JSONRPC'
    },

});