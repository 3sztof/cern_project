Ext.define('LHCb.view.operationwindow.OperationWindow', {
    extend: 'Ext.Panel',

    layout: 'card',

    controller: 'main',

    alias: 'mainoperationwindow',

    xtype: 'mainoperationwindow',

    activeItem: 0,


    items:[ 
        {
            // View 0
            layout: {
                type: 'vbox',
                //pack: 'start',
                align: 'stretch'
            },
            items: [
                {
                    xtype: 'taskstable',
                    flex: 1
                },
                {
                    xtype: 'button',
                    text: 'Go to view 2',
                    listeners: {
                        click: 'gotoView2'
                    }
                }
            ]
            
        }, 
        {
            // View 1
            xtype: 'button',
            text: 'Go back to view 1',
            listeners: {
                click: 'gotoView1'
            }
        } 
    ]

})

