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
            xtype: 'panel',
            tbar: [{
                flex: 1,
                text: 'Show all task sets',
                glyph: 'f0ce@FontAwesome',
                handler: 'showFullTaskSetsTable'
            }]
        },
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
                            // Switch to single task set view
                            var operationwindow = Ext.ComponentQuery.query('panel[itemId=mainoperationwindow]')[0];
                            operationwindow.setActiveItem(3);
                            // Update the description window with the singleton store record
                            Ext.ComponentQuery.query('panel[itemId=descriptionwindow]')[0].body.update('<br><center>' + LHCb.store.SelectedItemData.task_set + ': ' + LHCb.store.SelectedItemData.description + '</center>');
                            // Update the default fields in the task overview window
                            Ext.ComponentQuery.query('panel[itemId=singletasksetwindow]')[0].items.items[0].setValue(LHCb.store.SelectedItemData.task_set);
                            Ext.ComponentQuery.query('panel[itemId=singletasksetwindow]')[0].items.items[1].setValue(LHCb.store.SelectedItemData.description);
                        }
                    }
                }       
            ],
            viewConfig: {
                emptyText: 'No tasks in the database or database API offline...',
                deferEmptyText: false
            }
        }
    ]
    
});