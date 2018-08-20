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
            layout: {
                type: 'vbox',
                pack: 'start',
                align: 'stretch'
            },
            
            items: [
                // {
                //     xtype: 'panel',
                //     title: '<center>Tasks</center>'
                // },
                {
                    tbar: [
                        {
                            flex: 1,
                            text: 'Add task',
                            glyph: 'f055@FontAwesome',
                            handler: 'onAddTask'
                            // function() {
                            //     alert('Switch to "Add task" popup, PUT to DB via REST api');
                            // }
                        },
                        {
                            flex: 1,
                            text: 'Delete task(s)',
                            glyph: 'f056@FontAwesome',
                            handler: function() {
                                alert('Delete selected tasks trough REST api, optional: ask if the user is sure')
                            }
                        }
                    ],
                },
                {
                    xtype: 'taskstable'
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
                            handler: 'onModifyTask'
                        },
                        {
                            flex: 1,
                            text: 'Delete task',
                            glyph: 'f056@FontAwesome',
                            handler: 'onDeleteTask'
                            // handler: function() {
                            //     alert('Delete selected task trough REST api, optional: ask if the user is sure')
                            // }
                        }
                    ],
                    align: 'bottom'
                }
            ]
        } 
    ]

})

