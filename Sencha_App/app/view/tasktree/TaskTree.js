Ext.define('LHCb.view.tasktree.TaskTree', {
    extend: 'Ext.Container',
    xtype: 'tasktree',


    items: [{
        xtype: 'treepanel',
        rootVisible: false,
        store: {
            type: 'tasks'
        }
    }]
});