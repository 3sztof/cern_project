Ext.define('LHCb.view.operationwindow.SingleClass', {
    extend: 'Ext.panel.Panel',

    xtype: 'singleclass',

    controller: 'main',

    itemId: 'singleclasswindow',

    bodyPadding: 10,

    viewModel: {
        stores: {          
            assigned_items: {
                autoLoad: true,
                fields: [
                    {name: 'node_class', type: 'string'} 
                ],
                proxy: {
                    disableCache: false,
                    method: 'POST',
                    type: 'myproxy',
                    dataType: 'json',
                    actionMethods : {create: "POST", read: "POST", update: "POST", destroy: "POST"},
                    jsonData: new JSON_RPC.Request("taskSetsInClass", [{"task_set":LHCb.store.SelectedItemData.node_class}]),
                    reader: {
                        type: 'json',
                        rootProperty: 'result'
                    }
                }        
            }
        }
    },

    layout: {
        type: 'vbox',
        align: 'stretch',
        pack: 'start'
    },
    items: [

        {
            layout: 'form',
            items: [
                {
                    xtype: 'textfield',
                    readOnly: true,
                    fieldLabel: 'Node class unique name:',
                    value: '',
                },
                {
                    xtype: 'textareafield',
                    readOnly: true,
                    fieldLabel: 'Node class description',
                    value: ''
                }     
            ]  
        },
        {
            xtype: 'grid',
            flex: 1,
            bind: '{assigned_items}',
            columns: [{
                text: 'Assigned task sets:',
                dataIndex: 'task_set',

                flex: 1,
                sortable: true
            }]
        }
    ],
});