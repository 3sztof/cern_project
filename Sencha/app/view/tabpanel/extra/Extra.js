Ext.define('LHCb.view.tabpanel.extra.Extra', {
    extend: 'Ext.Container',

    itemId: 'extra',
    xtype: 'extra',
    controller: 'main',

    requires: ['LHCb.store.SelectedItemData'],

    layout: {
        type: 'vbox',
        pack: 'center',
        align: 'stretch'
    },

    items   : [
        {
            xtype: 'button',
            text : 'Show assigned tasks',
            listeners: {
                click: function() {
                    if(LHCb.store.SelectedItemData.regex == ""){return}
                    var showAssignedTasksWindow = Ext.create('Ext.window.Window', {
                        title: 'Tasks started on ' + LHCb.store.SelectedItemData.regex,
                        itemId: 'addtaskform',
                        closable: true,
                        closeAction: 'destroy',
                        width: 350,
                        minWidth: 250,
                        border: false,
                        modal: true,
                        

                        viewModel: {
                            stores: {
                                
                                assigned_items: {
                                    autoLoad: true,
                                    fields: [
                                        {name: 'task', type: 'string'} 
                                    ],
                                
                                
                                    proxy: {
                                        disableCache: false,
                                        method: 'POST',
                                        type: 'myproxy',
                                        dataType: 'json',
                                        actionMethods : {create: "POST", read: "POST", update: "POST", destroy: "POST"},
                                        jsonData: new JSON_RPC.Request("getTasksByNode", [{"node":LHCb.store.SelectedItemData.regex}]),
                                        reader: {
                                            type: 'json',
                                            rootProperty: 'result'
                                        }
                                    },
                                    
                                }
                                
                            }
                        },
            
                        items: [{
                            xtype: 'grid',
                            // title: 'First Grid',
            
                            itemId: 'notassignedtasksgrid',
            
                            flex: 1,
            
                            multiSelect: false,
                            margin: '0 5 0 0',

            
                            bind: '{assigned_items}',
            
                            columns: [{
                                text: 'Tasks',
                                dataIndex: 'task',
            
                                flex: 1,
                                sortable: true
                            }]
                            
                        }
                    ]

                        
                    }).show();
                }
            }
        }
    ]

});