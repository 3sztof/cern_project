Ext.define('LHCb.view.operationwindow.TasksTable', {
    extend: 'Ext.grid.Panel',
    xtype: 'taskstable',

    requires: ['LHCb.model.TasksTableModel'],

    title: 'Tasks',
    bind: '{tasks}',
   
    //reference: 'tasksGrid',
    //flex: 1,


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


// Todo: later on, we will need to select only one task when its clicked in the hierarchy: copy the above and apply filter to the entry in the table
// Like this:
// columns: [{
//     dataIndex: 'id',
//     text: 'ID',
//     width: 50
// },{
//     dataIndex: 'show',
//     text: 'Show',
//     flex: 1,
//     filter: {
//         // required configs
//         type: 'string',
//         // optional configs
//         value: 'star',  // setting a value makes the filter active.
//         itemDefaults: {
//             // any Ext.form.field.Text configs accepted
//         }
//     }
// }]






