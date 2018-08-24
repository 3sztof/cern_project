Ext.define('LHCb.view.operationwindow.OperationWindow', {
    extend: 'Ext.Panel',

    layout: 'card',

    requires: ['LHCb.view.operationwindow.SingleTask'],

    controller: 'main',

    alias: 'mainoperationwindow',

    xtype: 'mainoperationwindow',

    activeItem: 0,

    // View switcher - wichtig
    items:[
        // View 0 - Tasks full table
        {
            toolbaridentifier: 'taskstable',

            layout: {
                type: 'vbox',
                pack: 'start',
                align: 'stretch'
            },
            
            items: [
                {
                    xtype: 'taskstable'
                }       
            ]
        }, 
        // View 1 - clicked (navigation) task details
        {
            toolbaridentifier: 'singletask',

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
                            xtype: 'singletask',
                            flex: 1
                        }
                    ]
                }
            ]
        },
        // View 2 - Task Sets full table
        {
            toolbaridentifier: 'setstable',

            layout: {
                type: 'vbox',
                pack: 'start',
                align: 'stretch'
            },
            
            items: [
                {
                    autoScroll: true,
                    xtype: 'tasksetstable'
                }
                
            ]
        },
        // View 3 - clicked (navigation) task set details
        {
            toolbaridentifier: 'singleset',
         
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
                            xtype: 'singletaskset',
                            flex: 1
                        }
                    ]
                }
            ]
        }  
    ]

})

