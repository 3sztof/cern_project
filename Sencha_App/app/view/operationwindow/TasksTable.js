Ext.define('LHCb.view.operationwindow.TasksTable', {
    extend: 'Ext.grid.Panel',
    xtype: 'taskstable',

    requires: ['LHCb.model.TasksTableModel'],

    controller: 'main',

    title: '<center>Tasks</center>',
    bind: '{tasks}',
   
    //reference: 'tasksGrid',
    //flex: 1,

    columnLines: true,
    selType: 'checkboxmodel',

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
            xtype: 'rownumberer'
        },
        {
            text: 'Unique name', dataIndex: 'task', flex: 1
        }, 
        {
            text: 'Script name', dataIndex: 'command', flex: 1
        },
        {
            text: 'Script parameters', dataIndex: 'task_parameters', flex: 1
        },
        {
            text: 'Process identifier', dataIndex: 'utgid', flex: 1
        },
        {
            text: 'PcAdd parameters', dataIndex: 'command_parameters', flex: 1
        },
        {
            xtype: 'actioncolumn',
            width: 40,
            items: 
            [
                {
                    glyph: 'f044@FontAwesome',
                    handler: function(grid, rowIndex, colindex) {
                        alert('Edit row: ' + rowIndex + ' column: ' + colindex);
                    }
                }
            ]
        }
        // {
        //     text: 'Process description', dataIndex: 'description', flex: 1.8
        // },
        
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






