Ext.define('LHCb.view.operationwindow.TasksTable', {
    extend: 'Ext.grid.Panel',
    xtype: 'taskstable',

    id: 'taskstablegrid',

    itemId: 'taskstable',

    requires: ['LHCb.model.TasksTableModel'],

    controller: 'main',

    //height: 720,
    scrollable: true,

    bind: '{tasks}',
    
    columnLines: true,
    selType: 'checkboxmodel',

    viewModel: {
        stores: {
            tasks: {
                model: 'LHCb.model.TasksTableModel',
                autoLoad: true,
                sorters: [{
                    property: 'task',
                    direction: 'ASC'
                }],
                sortRoot: 'task',
                sortOnLoad: true,
                remoteSort: false,
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
        }
        // {
        //     xtype: 'actioncolumn',
        //     flex: 1,
        //     items: 
        //     [
        //         {
        //             glyph: 'f044@FontAwesome',
        //             handler: function(grid, rowIndex, colindex) {
        //                 console.log(grid)
        //                 alert('Edit row: ' + rowIndex + ' column: ' + colindex);
        //             }
        //         }
        //     ]
        // }
        // {
        //     text: 'Process description', dataIndex: 'description', flex: 1.8
        // },
        
    ],
    viewConfig: {
        emptyText: 'No tasks in the database or database API offline...',
        deferEmptyText: false
    }
});






// Ext.define('LHCb.view.operationwindow.TasksTable', {
//     extend: 'Ext.grid.Panel',
//     xtype: 'taskstable',

//     requires: ['LHCb.model.TasksTableModel'],

//     controller: 'main',

    
//     bind: '{tasks}',
    
//     columnLines: true,
//     selType: 'checkboxmodel',

//     viewModel: {
//         stores: {
//             tasks: {
//                 model: 'LHCb.model.TasksTableModel',
//                 autoLoad: true
//             }
//         }
//     },


//     columns: [
//         {
//             xtype: 'rownumberer'
//         },
//         {
//             text: 'Unique name', dataIndex: 'task', flex: 1
//         }, 
//         {
//             text: 'Script name', dataIndex: 'command', flex: 1
//         },
//         {
//             text: 'Script parameters', dataIndex: 'task_parameters', flex: 1
//         },
//         {
//             text: 'Process identifier', dataIndex: 'utgid', flex: 1
//         },
//         {
//             text: 'PcAdd parameters', dataIndex: 'command_parameters', flex: 1
//         },
//         {
//             xtype: 'actioncolumn',
//             width: 40,
//             items: 
//             [
//                 {
//                     glyph: 'f044@FontAwesome',
//                     handler: function(grid, rowIndex, colindex) {
//                         alert('Edit row: ' + rowIndex + ' column: ' + colindex);
//                     }
//                 }
//             ]
//         }
//         // {
//         //     text: 'Process description', dataIndex: 'description', flex: 1.8
//         // },
        
//     ],
//     viewConfig: {
//         emptyText: 'No tasks in the database or database API offline...',
//         deferEmptyText: false
//     }
// });









