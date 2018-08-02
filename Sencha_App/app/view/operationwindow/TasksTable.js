Ext.define('LHCb.view.operationwindow.TasksTable', {
    extend: 'Ext.grid.Panel',
    xtype: 'taskstable',

    requires: ['LHCb.model.TasksTableModel'],

    title: 'Tasks',
    bind: '{tasks}',
   
    //reference: 'tasksGrid',
    flex: 1,

    a: function(){debugger;},

    viewModel: {
        stores: {
            tasks: {
                model: 'LHCb.model.TasksTableModel',
                autoLoad: true
            }
        }
    },

    columns: [
        {
            text: 'Name', dataIndex: 'task', flex: 1
        }, 
        {
            text: 'Command', dataIndex: 'command', flex: 1
        }
    ],
    viewConfig: {
        emptyText: 'No tasks in the database or database API offline...',
        deferEmptyText: false
    }
});









