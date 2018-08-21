Ext.define('LHCb.view.operationwindow.SingleTaskSet', {
    extend: 'Ext.panel.Panel',

    xtype: 'singletaskset',

    requires: ['LHCb.store.SelectedItemData'],

    controller: 'main',

    itemId: 'singletasksetwindow',

    bodyPadding: 10,
    layout: 'form',

    items: [{
            xtype: 'textfield',
            readOnly: true,
            fieldLabel: 'Task set\'s unique name:',
            value: '', // value property is filled in by the listener in TaskExplorer.js (on grid click)
        },
        {
            xtype: 'textareafield',
            readOnly: true,
            fieldLabel: 'Task set\'s description',
            value: ''
        }
    ]

});

