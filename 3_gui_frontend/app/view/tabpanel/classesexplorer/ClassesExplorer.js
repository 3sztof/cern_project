Ext.define('LHCb.view.tabpanel.classesexplorer.ClassesExplorer', {
    extend: 'Ext.Container',

    itemId: 'classesexplorer',
    xtype: 'classesexplorer',
    controller: 'main',

    requires: ['LHCb.model.ClassesTableModel', 'LHCb.store.SelectedItemData'],

    viewModel: {
        stores: {
            node_classes: {
                model: 'LHCb.model.ClassesTableModel',
                autoLoad: true
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
            id: 'classesexplorergrid',
            bind: '{node_classes}',
            columns: [
                {
                    text: 'Select a node class:', dataIndex: 'node_class', flex: 1,
                    listeners: {
                        click: function(grid, cellElement, rowIndex, cellIndex){
                            // Sweep trough the table objects
                            var gridstore = grid.getStore();
                            var rowdata = gridstore.data.items[rowIndex];
                            // Set selected item data in common singleton to be accessed by other windows
                            LHCb.store.SelectedItemData.task_set = rowdata.data['node_class'];
                            LHCb.store.SelectedItemData.description = rowdata.data['description'];
                            // Switch to single task set view and reassure that the assignment button is visible
                            var operationwindow = Ext.ComponentQuery.query('panel[itemId=mainoperationwindow]')[0];
                            operationwindow.setActiveItem(5);
                            Ext.ComponentQuery.query('panel[itemId=singleoperationwindowtoolbar]')[0].setVisible(true);
                            Ext.ComponentQuery.query('panel[itemId=mainoperationwindowtoolbar]')[0].setVisible(false);
                            Ext.ComponentQuery.query('panel[itemId=singleoperationwindowtoolbar]')[0].header.items.items[2].setVisible(true);
                            // Update the description window with the singleton store record
                            Ext.ComponentQuery.query('panel[itemId=descriptionwindow]')[0].body.update('<br><center>' + LHCb.store.SelectedItemData.task_set + ': ' + LHCb.store.SelectedItemData.description + '</center>');
                            // Update the default fields in the task sets overview window and the assigned items table
                            Ext.ComponentQuery.query('panel[itemId=singleclasswindow]')[0].items.items[0].items.items[0].setValue(LHCb.store.SelectedItemData.task_set);
                            Ext.ComponentQuery.query('panel[itemId=singleclasswindow]')[0].items.items[0].items.items[1].setValue(LHCb.store.SelectedItemData.description);
                            Ext.ComponentQuery.query('panel[itemId=singleclasswindow]')[0].items.items[1].store.proxy.jsonData.params[0]["node_class"] = rowdata.data['node_class'];
                            Ext.ComponentQuery.query('panel[itemId=singleclasswindow]')[0].items.items[1].store.reload()
                        }
                    }
                }       
            ],
            viewConfig: {
                emptyText: 'No node classes in the database or database API offline...',
                deferEmptyText: false
            }
        }
    ]
    
});