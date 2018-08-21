Ext.define('LHCb.view.operationwindow.TaskSetsTable', {
    extend: 'Ext.grid.Panel',
    xtype: 'tasksetstable',

    id: 'tasksetstablegrid',

    requires: ['LHCb.model.TaskSetsTableModel'],

    controller: 'main',

    scrollable: true,

    bind: '{tasksets}',
    
    columnLines: true,
    selType: 'checkboxmodel',

    viewModel: {
        stores: {
            tasksets: {
                model: 'LHCb.model.TaskSetsTableModel',
                autoLoad: true
            }
        }
    },


    columns: [
        {
            xtype: 'rownumberer'
        },
        {
            text: 'Unique name', dataIndex: 'task_set', flex: 1
        }, 
        {
            text: 'Description', dataIndex: 'description', flex: 1
        }       
    ],
    viewConfig: {
        emptyText: 'No task sets in the database or database API offline...',
        deferEmptyText: false
    }
});
