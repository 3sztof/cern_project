Ext.define('LHCb.model.TasksTableModel', {
    extend: 'Ext.data.Model',
    // requires: [
    //     'KitchenSink.model.field.PhoneNumber',
    //     'Ext.data.proxy.Rest'
    // ],

    alias: 'viewmodel.taskstable',

    fields: [
        {name: 'task', type: 'string'},
        {name: 'command', type: 'string'},
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
        type: 'rest',
        reader: {
            type: 'json',
            rootProperty: 'data'
        },
        url: 'http://localhost/tasks'
    },

    // validators: {
    //     name: 'presence'
    // }otepa
});