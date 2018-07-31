Ext.define('LHCb.store.TaskTreeStore', {
    extend: 'Ext.data.TreeStore',
    alias: 'store.tasks',

    rootData: {
        text: 'Ext JS',
        expanded: true,
        children: [
            {
                text: 'Tasks',
                expanded: true,
                children: [
                    { leaf:true, text: 'Button.js' },
                    { leaf:true, text: 'Cycle.js' },
                    { leaf:true, text: 'Split.js' },
                    { leaf:true, text: 'Application.js' }
                ]
            }]
    },

    constructor: function (config) {
        // Since records claim the data object given to them, clone the data
        // for each instance.
        config = Ext.apply({
            root: Ext.clone(this.rootData)
        }, config);

        this.callParent([config]);
    }
});