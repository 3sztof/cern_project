Ext.define('LHCb.model.AssignItems', {
    extend: 'Ext.data.Model',

    alias: 'viewmodel.assignitems',

    itemId: 'assigneditemsstore',

    fields: [
        {name: 'task', type: 'string'} 
    ],

});