Ext.define('LHCb.view.operationwindow.SingleClass', {
    extend: 'Ext.panel.Panel',

    xtype: 'singleclass',

    controller: 'main',

    itemId: 'singleclasswindow',

    bodyPadding: 10,

    viewModel: {
        stores: {          
            assigned_items: {
                autoLoad: false,
                fields: [
                    {name: 'task', type: 'string'} 
                ],
                sorters: [{
                    property: 'task_set',
                    direction: 'ASC'
                }],
                sortRoot: 'task_set',
                sortOnLoad: true,
                remoteSort: false,
                proxy: {
                    disableCache: false,
                    method: 'POST',
                    type: 'myproxy',
                    dataType: 'json',
                    actionMethods : {create: "POST", read: "POST", update: "POST", destroy: "POST"},
                    jsonData: new JSON_RPC.Request("taskSetsInClass", [{"node_class":"Error on purpose: cant stop SingleClass store autoload"}]),
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
                    value: '', // value property is filled in by the listener in TaskExplorer.js (on grid click)
                },
                {
                    xtype: 'textareafield',
                    readOnly: true,
                    fieldLabel: 'Node class description:',
                    value: ''
                }     
            ]  
        },
        {
            xtype: 'grid',
            flex: 1,
            bind: '{assigned_items}',
            columns: [{
                text: 'Assigned tasks:',
                dataIndex: 'task_set',

                flex: 1,
                sortable: true
            }]
        }
    ],
});