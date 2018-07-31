Ext.define('LHCb.view.operationwindow.OperationWindow', {
    extend: 'Ext.Panel',

    layout: 'card',

    controller: 'main',


    xtype: 'mainoperationwindow',

    activeItem: 0,

    items:[ 
        {
            xtype: 'button',
            text: 'Go to view 2',
            listeners: {
                click: 'gotoView2'
            }
        }, 
        {
            xtype: 'button',
            text: 'Go back to view 1',
            listeners: {
                click: 'gotoView1'
            }
        } 
    ]

})

