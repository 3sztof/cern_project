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
                            handler: 'onDeleteTasks'
                            // handler: function() {
                            //     alert('Delete selected tasks trough REST api, optional: ask if the user is sure')
                            // }
                        }
                    ],
                },
                {
                    xtype: 'taskstable'
                }
                
            ]
        }, 
        // View 1 - clicked (navigation) task details
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
        },
        // View 2 - Task Sets full table
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
                            text: 'Add task set',
                            glyph: 'f055@FontAwesome',
                            handler: 'onAddTaskSet'
                        },
                        {
                            flex: 1,
                            text: 'Delete task set(s)',
                            glyph: 'f056@FontAwesome',
                            handler: 'onDeleteTaskSets'
                        }
                    ],
                },
                {
                    xtype: 'tasksetstable'
                }
                
            ]
        },
        // View 3 - clicked (navigation) task set details
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
                            xtype: 'singletaskset',
                            flex: 1
                        }
                    ]
                },
                {
                    bbar: [
                        {
                            flex: 1,
                            text: 'Modify task set',
                            glyph: 'f044@FontAwesome',
                            handler: 'onModifyTaskSet'
                        },
                        {
                            flex: 2,
                            text: 'Assign & unassign tasks',
                            iconCls: 'x-fa fa-sign-in',
                            handler: 'onAssignToTaskSet'
                        },
                        {
                            flex: 1,
                            text: 'Delete task set',
                            glyph: 'f056@FontAwesome',
                            handler: 'onDeleteTaskSet'
                        }
                    ],
                    align: 'bottom'
                }
            ]
        }  
    ]

})

