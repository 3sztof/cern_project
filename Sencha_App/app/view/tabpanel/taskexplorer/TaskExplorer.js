Ext.define('LHCb.view.tabpanel.taskexplorer.TaskExplorer', {
    extend: 'Ext.Container',

    xtype: 'taskexplorer',
    controller: 'main',

    requires: ['LHCb.model.TasksTableModel', 'LHCb.store.SelectedItemData'],
    viewModel: {
        stores: {
            tasks: {
                model: 'LHCb.model.TasksTableModel',
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
                text: 'Show all tasks',
                glyph: 'f0ce@FontAwesome',
                handler: 'showFullTasksTable'
            }]
        },
        {
            xtype: 'grid',
            bind: '{tasks}',
            columns: [
                {
                    text: 'Select a task:', dataIndex: 'task', flex: 1,
                    listeners: {
                        click: function(grid, cellElement, rowIndex, cellIndex){
                            // Sweep trough the table objects
                            var gridstore = grid.getStore();
                            var rowdata = gridstore.data.items[rowIndex];
                            // Set selected item data in common singleton to be accessed by other windows
                            LHCb.store.SelectedItemData.task = rowdata.data['task'];
                            LHCb.store.SelectedItemData.description = rowdata.data['description'];
                            LHCb.store.SelectedItemData.task_parameters = rowdata.data['task_parameters'];
                            LHCb.store.SelectedItemData.command_parameters = rowdata.data['command_parameters'];
                            LHCb.store.SelectedItemData.utgid = rowdata.data['utgid'];
                            LHCb.store.SelectedItemData.command = rowdata.data['command'];
                            // Switch to single task view
                            var operationwindow = Ext.ComponentQuery.query('panel[itemId=mainoperationwindow]')[0];
                            operationwindow.setActiveItem(1);
                            // Update the description window with the singleton store record
                            Ext.ComponentQuery.query('panel[itemId=descriptionwindow]')[0].body.update('<br><center>' + LHCb.store.SelectedItemData.description + '</center>');
                            // Update the default fields in the task overview window
                            Ext.ComponentQuery.query('panel[itemId=singletaskwindow]')[0].items.items[0].setValue(LHCb.store.SelectedItemData.task);
                            Ext.ComponentQuery.query('panel[itemId=singletaskwindow]')[0].items.items[1].setValue(LHCb.store.SelectedItemData.description);
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