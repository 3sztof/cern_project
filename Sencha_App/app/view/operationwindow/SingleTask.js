Ext.define('LHCb.view.operationwindow.SingleTask', {
    extend: 'Ext.panel.Panel',

    xtype: 'singletask',

    requires: ['LHCb.store.SelectedItemData'],

    controller: 'main',

    itemId: 'singletaskwindow',

    title: '<center>Selected task overview</center>',

    bodyPadding: 10,
    layout: 'form',

    items: [{
            xtype: 'textfield',
            itemId: 'singletasktask',
            name: 'task_mod',
            fieldLabel: 'Task\'s unique name:',
            value: 'blabla', // value property is filled in by the listener in OperationWindow.js (on card activate)
        }, 
        {
            xtype: 'textareafield',
            name: 'description_mod',
            fieldLabel: 'Task\'s description',
            value: ''
        }
    ]

});

