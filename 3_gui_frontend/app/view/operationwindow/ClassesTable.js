Ext.define('LHCb.view.operationwindow.ClassesTable', {
    extend: 'Ext.grid.Panel',
    
    xtype: 'classestable',

    id: 'classestablegrid',

    requires: ['LHCb.model.ClassesTableModel'],

    controller: 'main',

    scrollable: true,
    
    columnLines: true,
    selType: 'checkboxmodel',

    viewModel: {
        stores: {
            node_classes: {
                model: 'LHCb.model.ClassesTableModel',
                autoLoad: true
            }
        }
    },

    bind: '{node_classes}',

    columns: [
        {
            xtype: 'rownumberer'
        },
        {
            text: 'Unique name', dataIndex: 'node_class', flex: 1
        }, 
        {
            text: 'Description', dataIndex: 'description', flex: 1
        }       
    ],
    viewConfig: {
        emptyText: 'No node classes in the database or database API offline...',
        deferEmptyText: false
    }
});
