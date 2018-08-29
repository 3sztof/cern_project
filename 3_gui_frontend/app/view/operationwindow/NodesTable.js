Ext.define('LHCb.view.operationwindow.NodesTable', {
    extend: 'Ext.grid.Panel',
    xtype: 'nodestable',

    id: 'nodestablegrid',

    requires: ['LHCb.model.NodesTableModel'],

    controller: 'main',

    scrollable: true,

    bind: '{nodes}',
    
    columnLines: true,
    selType: 'checkboxmodel',

    viewModel: {
        stores: {
            nodes: {
                model: 'LHCb.model.NodesTableModel',
                autoLoad: true
            }
        }
    },


    columns: [
        {
            xtype: 'rownumberer'
        },
        {
            text: 'Unique name', dataIndex: 'regex', flex: 1
        }, 
        {
            text: 'Description', dataIndex: 'description', flex: 1
        }       
    ],
    viewConfig: {
        emptyText: 'No nodes in the database or database API offline...',
        deferEmptyText: false
    }
});
