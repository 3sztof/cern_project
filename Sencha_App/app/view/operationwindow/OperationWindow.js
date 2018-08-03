Ext.define('LHCb.view.operationwindow.OperationWindow', {
    extend: 'Ext.Panel',

    layout: 'card',

    controller: 'main',

    alias: 'mainoperationwindow',

    xtype: 'mainoperationwindow',

    activeItem: 0,

    // View switcher - wichtig
    items:[
        // View 0 - Tasks full table
        {
            layout: {
                type: 'vbox',
                align: 'stretch'
            },
            items: [
                {
                    layout: {
                        type: 'fit',
                    },
                    items: [
                        {
                            xtype: 'taskstable',
                            flex: 1
                        }
                    ]
                },
                {
                    height: '100',
                    xtype: 'button',
                    text: 'Go to view 2 (to be replaced by table click events)',
                    listeners: {
                        click: 'gotoView2'
                    }
                },
                {
                    bbar: [
                        {
                            flex: 1,
                            text: 'Add task',
                            glyph: 'f055@FontAwesome',
                            handler: 'addTask'
                        },
                        {
                            flex: 1,
                            text: 'Delete task',
                            glyph: 'f056@FontAwesome',
                            handler: 'deleteTask'
                        }
                    ],
                    align: 'bottom'
                }
            ]
        }, 
        // View 1 - clicked (navigation) task details? - TODO
        {
            xtype: 'button',
            text: 'Go back to view 1',
            listeners: {
                click: 'gotoView1'
            }
        } 
    ]

})

