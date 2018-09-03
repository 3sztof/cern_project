Ext.define('LHCb.view.tabpanel.tasksetsexplorer.TaskSetsExplorer', {
    extend: 'Ext.Container',

    itemId: 'tasksetsexplorer',
    xtype: 'tasksetsexplorer',
    controller: 'main',

    requires: ['LHCb.model.TaskSetsTableModel', 'LHCb.store.SelectedItemData'],

    viewModel: {
        stores: {
            task_sets: {
                model: 'LHCb.model.TaskSetsTableModel',
                autoLoad: true,
                sorters: [{
                    property: 'task_set',
                    direction: 'ASC'
                }],
                sortRoot: 'task_set',
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
        // {
        //     xtype: 'panel',
        //     tbar: [{
        //         flex: 1,
        //         text: 'Show all task sets',
        //         glyph: 'f0ce@FontAwesome',
        //         handler: 'showFullTaskSetsTable'
        //     }]
        // },
        {   
            xtype: 'grid',
            id: 'tasksetsexplorergrid',
            bind: '{task_sets}',
            columns: [
                {
                    text: 'Select a task set:', dataIndex: 'task_set', flex: 1,
                    listeners: {
                        click: function(grid, cellElement, rowIndex, cellIndex){
                            // Sweep trough the table objects
                            var gridstore = grid.getStore();
                            var rowdata = gridstore.data.items[rowIndex];
                            // Set selected item data in common singleton to be accessed by other windows
                            LHCb.store.SelectedItemData.task_set = rowdata.data['task_set'];
                            LHCb.store.SelectedItemData.description = rowdata.data['description'];
                            // Switch to single task set view and reassure that the assignment button is visible
                            var operationwindow = Ext.ComponentQuery.query('panel[itemId=mainoperationwindow]')[0];
                            operationwindow.setActiveItem(3);
                            Ext.ComponentQuery.query('panel[itemId=singleoperationwindowtoolbar]')[0].setVisible(true);
                            Ext.ComponentQuery.query('panel[itemId=mainoperationwindowtoolbar]')[0].setVisible(false);
                            Ext.ComponentQuery.query('panel[itemId=singleoperationwindowtoolbar]')[0].header.items.items[2].setVisible(true);
                            // Update the description window with the singleton store record
                            Ext.ComponentQuery.query('panel[itemId=descriptionwindow]')[0].body.update('<br><center>' + LHCb.store.SelectedItemData.task_set + ': ' + LHCb.store.SelectedItemData.description + '</center>');
                            // Update the default fields in the task sets overview window and the assigned items table
                            Ext.ComponentQuery.query('panel[itemId=singletasksetwindow]')[0].items.items[0].items.items[0].setValue(LHCb.store.SelectedItemData.task_set);
                            Ext.ComponentQuery.query('panel[itemId=singletasksetwindow]')[0].items.items[0].items.items[1].setValue(LHCb.store.SelectedItemData.description);
                            Ext.ComponentQuery.query('panel[itemId=singletasksetwindow]')[0].items.items[1].store.proxy.jsonData = new JSON_RPC.Request("tasksInSet", [{"task_set": rowdata.data['task_set']}]);
                            Ext.ComponentQuery.query('panel[itemId=singletasksetwindow]')[0].items.items[1].store.reload()
                        }
                    }
                }       
            ],
            viewConfig: {
                emptyText: 'No task sets in the database or database API offline...',
                deferEmptyText: false
            }
        }
    ]
    
});