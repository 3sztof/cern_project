Ext.define('LHCb.view.operationwindow.SingleTaskSet', {
    extend: 'Ext.panel.Panel',

    xtype: 'singletaskset',

    requires: ['LHCb.store.SelectedItemData'],

    controller: 'main',

    itemId: 'singletasksetwindow',

    bodyPadding: 10,
    layout: 'form',
    // viewModel: {
    //     stores: {
            
    //         assigned_items: {
    //             autoLoad: true,
    //             fields: [
    //                 {name: 'task', type: 'string'} 
    //             ],
            
    //             proxy: {
    //                 disableCache: false,
    //                 method: 'POST',
    //                 type: 'myproxy',
    //                 dataType: 'json',
    //                 actionMethods : {create: "POST", read: "POST", update: "POST", destroy: "POST"},
    //                 jsonData: new JSON_RPC.Request("tasksInSet", [{"task_set":LHCb.store.SelectedItemData.task_set}]),
    //                 reader: {
    //                     type: 'json',
    //                     rootProperty: 'result'
    //                 }
    //             },
                
    //         }
    //     }
    // },
    items: [{
            xtype: 'textfield',
            readOnly: true,
            fieldLabel: 'Task set\'s unique name:',
            value: '', // value property is filled in by the listener in TaskExplorer.js (on grid click)
        },
        {
            xtype: 'textareafield',
            readOnly: true,
            fieldLabel: 'Task set\'s description',
            value: ''
        },
        
    ],
    // items: [
    //     {
    //         xtype: 'grid',
    //         flex: 1,
    //         readOnly: true,
    //         bind: '{assigned_items}',

    //             columns: [{
    //                 text: 'Assigned tasks:',
    //                 dataIndex: 'task',

    //                 flex: 1,
    //                 sortable: true
    //             }]
    //     }
    // ]

});

