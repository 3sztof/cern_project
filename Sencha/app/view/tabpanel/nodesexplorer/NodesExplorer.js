Ext.define('LHCb.view.tabpanel.nodesexplorer.NodesExplorer', {
    extend: 'Ext.Container',

    itemId: 'nodesexplorer',
    xtype: 'nodesexplorer',
    controller: 'main',

    requires: ['LHCb.model.NodesTableModel', 'LHCb.store.SelectedItemData'],

    viewModel: {
        stores: {
            nodes: {
                model: 'LHCb.model.NodesTableModel',
                autoLoad: true,
                sorters: [{
                    property: 'regex',
                    direction: 'ASC'
                }],
                sortRoot: 'tegex',
                sortOnLoad: true,
                remoteSort: false,
            }
        }
    },   

    layout: {
        type: 'vbox',
        pack: 'start',
        align: 'stretch'
    },
    items: [
        {   
            xtype: 'grid',
            id: 'nodesexplorergrid',
            bind: '{nodes}',
            columns: [
                {
                    text: 'Select a node:', dataIndex: 'regex', flex: 1,
                    listeners: {
                        click: function(grid, cellElement, rowIndex, cellIndex){
                            // Sweep trough the table objects
                            var gridstore = grid.getStore();
                            var rowdata = gridstore.data.items[rowIndex];
                            // Set selected item data in common singleton to be accessed by other windows
                            LHCb.store.SelectedItemData.regex = rowdata.data['regex'];
                            LHCb.store.SelectedItemData.description = rowdata.data['description'];
                            // Switch to single task set view and reassure that the assignment button is visible
                            var operationwindow = Ext.ComponentQuery.query('panel[itemId=mainoperationwindow]')[0];
                            operationwindow.setActiveItem(7);
                            Ext.ComponentQuery.query('panel[itemId=singleoperationwindowtoolbar]')[0].setVisible(true);
                            Ext.ComponentQuery.query('panel[itemId=mainoperationwindowtoolbar]')[0].setVisible(false);
                            Ext.ComponentQuery.query('panel[itemId=singleoperationwindowtoolbar]')[0].header.items.items[2].setVisible(true);
                            // Update the description window with the singleton store record
                            Ext.ComponentQuery.query('panel[itemId=descriptionwindow]')[0].body.update('<br><center>' + LHCb.store.SelectedItemData.regex + ': ' + LHCb.store.SelectedItemData.description + '</center>');
                            // Update the default fields in the task sets overview window and the assigned items table
                            Ext.ComponentQuery.query('panel[itemId=singlenodewindow]')[0].items.items[0].items.items[0].setValue(LHCb.store.SelectedItemData.regex);
                            Ext.ComponentQuery.query('panel[itemId=singlenodewindow]')[0].items.items[0].items.items[1].setValue(LHCb.store.SelectedItemData.description);
                            Ext.ComponentQuery.query('panel[itemId=singlenodewindow]')[0].items.items[1].store.proxy.jsonData = new JSON_RPC.Request("nodeclassInNode", [{"regex": rowdata.data['regex']}]);
                            Ext.ComponentQuery.query('panel[itemId=singlenodewindow]')[0].items.items[1].store.reload()
                        }
                    }
                }       
            ],
            viewConfig: {
                emptyText: 'No nodes in the database or database API offline...',
                deferEmptyText: false
            }
        }
    ]
    
});