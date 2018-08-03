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
                    bbar: [
                        {
                            flex: 1,
                            text: 'Add task',
                            glyph: 'f055@FontAwesome',
                            handler: function() {
                                alert('Switch to "Add task" popup, PUT to DB via REST api');
                            }
                        },
                        {
                            flex: 1,
                            text: 'Delete task',
                            glyph: 'f056@FontAwesome',
                            handler: function() {
                                alert('Delete selected tasks trough REST api, optional: ask if the user is sure')
                            }
                        }
                    ],
                    align: 'bottom'
                }
            ]
        }, 
        // View 1 - clicked (navigation) task details? - TODO
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
                            xtype: 'singletask',
                            flex: 1
                        }
                    ]
                },
                {
                    bbar: [
                        {
                            flex: 1,
                            text: 'Modify task',
                            glyph: 'f044@FontAwesome',
                            handler: function() {
                                alert('Switch to modifying popup - store the task\'s data in the store.SelectedItemData singleton.');
                            }
                        },
                        {
                            flex: 1,
                            text: 'Delete task',
                            glyph: 'f056@FontAwesome',
                            handler: function() {
                                alert('Delete selected task trough REST api, optional: ask if the user is sure')
                            }
                        }
                    ],
                    align: 'bottom'
                }
            ]
        } 
    ]

})

