Ext.define('LHCb.view.tabpanel.taskexplorer.TaskExplorer', {
    extend: 'Ext.Container',

    xtype: 'taskexplorer',
    controller: 'tabpanel',

    requires: ['LHCb.model.TasksTableModel'],
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
                handler: 'ShowAllTasks'
            }]
        },
        {
            xtype: 'grid',
            bind: '{tasks}',
            columns: [
                {
                    text: 'Select a task:', dataIndex: 'task', flex: 1
                }       
            ],
            viewConfig: {
                emptyText: 'No tasks in the database or database API offline...',
                deferEmptyText: false
            }
        }
    ]
    
});