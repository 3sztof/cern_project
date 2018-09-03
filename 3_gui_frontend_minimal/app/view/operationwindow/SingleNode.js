Ext.define('LHCb.view.operationwindow.SingleNode', {
    extend: 'Ext.panel.Panel',

    xtype: 'singlenode',

    controller: 'main',

    itemId: 'singlenodewindow',

    bodyPadding: 10,

    viewModel: {
        stores: {          
            assigned_items: {
                autoLoad: true,
                fields: [
                    {name: 'task', type: 'string'} 
                ],
                sorters: [{
                    property: 'node_class',
                    direction: 'ASC'
                }],
                sortRoot: 'node_class',
                sortOnLoad: true,
                remoteSort: false,
                proxy: {
                    disableCache: false,
                    method: 'POST',
                    type: 'myproxy',
                    dataType: 'json',
                    actionMethods : {create: "POST", read: "POST", update: "POST", destroy: "POST"},
                    jsonData: new JSON_RPC.Request("classesInNode", [{"regex":LHCb.store.SelectedItemData.regex}]),
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
                    fieldLabel: 'Node\'s unique name:',
                    value: '',
                },
                {
                    xtype: 'textareafield',
                    readOnly: true,
                    fieldLabel: 'Node\'s description',
                    value: ''
                }     
            ]  
        },
        {
            xtype: 'grid',
            flex: 1,
            bind: '{assigned_items}',
            columns: [{
                text: 'Assigned node classes:',
                dataIndex: 'node_class',

                flex: 1,
                sortable: true
            }]
        }
    ],
});
