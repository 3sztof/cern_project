Ext.define('LHCb.view.operationwindow.SingleTask', {
    extend: 'Ext.panel.Panel',

    xtype: 'singletask',

    requires: ['LHCb.store.SelectedItemData'],

    controller: 'main',

    itemId: 'singletaskwindow',

    //title: '<center>Selected task overview</center>',

    bodyPadding: 10,
    layout: 'form',

    items: [{
            xtype: 'textfield',
            readOnly: true,
            name: 'task_mod',
            fieldLabel: 'Task\'s unique name:',
            value: '', // value property is filled in by the listener in OperationWindow.js (on card activate)
        },
        {
            xtype: 'textfield',
            readOnly: true,
            name: 'script_mod',
            fieldLabel: 'Task\'s script name:',
            value: '', // value property is filled in by the listener in OperationWindow.js (on card activate)
        }, 
        {
            xtype: 'textfield',
            readOnly: true,
            name: 'script_par_mod',
            fieldLabel: 'Parameters for the script:',
            value: '', // value property is filled in by the listener in OperationWindow.js (on card activate)
        },
        {
            xtype: 'textfield',
            readOnly: true,
            name: 'utgid_mod',
            fieldLabel: 'Process identifier (utgid):',
            value: '', // value property is filled in by the listener in OperationWindow.js (on card activate)
        },
        {
            xtype: 'textfield',
            readOnly: true,
            name: 'pcadd_par_mod',
            fieldLabel: 'Parameters for the pcAdd command:',
            value: '', // value property is filled in by the listener in OperationWindow.js (on card activate)
        },  
        {
            xtype: 'textareafield',
            name: 'description_mod',
            readOnly: true,
            fieldLabel: 'Task\'s description',
            value: ''
        }
    ]

});

