/**
 * This class is the main view for the application. It is specified in app.js as the
 * "mainView" property. That setting automatically applies the "viewport"
 * plugin causing this view to become the body element (i.e., the viewport).
 *
 * TODO - Replace this content of this view to suite the needs of your application.
 */
Ext.define('LHCb.view.main.Main', {
    extend: 'Ext.Container',
    xtype: 'app-main',

    controller: 'main',
    viewModel: 'main',


    //margin: '5px 5px 5px 5px',
    defaults: {
        margin: '3px 3px 3px 3px',
        //padding: 5
    },
    layout: {
        type: 'fit',
    },
    items: [
        {
            layout: {
                type: 'vbox',
                pack: 'start',
                align: 'stretch'
            },
        
            frame: true,
            
            items: [
                {
                    layout: {
                        type: 'hbox',
                    },
                    items: [
                        {
                            title: '<center><strong>LHCb Online Farm Process Explorer</strong></center>',
                            flex: 1,
                            header: {
                                titlePosition: 1,
                                items: [
                                    {
                                        glyph: 'f129@FontAwesome',
                                        tooltip: 'About',
                                        handler: function() {
                                            Ext.MessageBox.alert('About', '<center><b>LHCb Online Farm Process Explorer</b><br><br>Author: Krzysztof Wilczynski<ul list-style-position: inside; margin-top: 0;><li><a href="mailto:krzysztofwilczynski@mail.com">krzysztofwilczynski@mail.com</a></li><li><a href="https://www.linkedin.com/in/3sztof">www.linkedin.com/in/3sztof/</a></li><li><a href="tel:+48668876202">+48 669 876 202</a></li></ul>Supervisor: Markus Frank<ul list-style-position: inside; margin-top: 0;><li><a href="mailto:markus.frank@cern.ch">markus.frank@cern.ch</a></li></ul></center>', this.showResult, this);
                                        }
                                    }
                                ]
                            }
                        },
                        {
                            xtype: 'image',
                            src: 'resources/LHCb_mod.png',
                            flex: 0.12,
                            height: 88
                        }
                    ]
                },
                {
                    layout: {
                        type: 'hbox',
                        pack: 'start',
                        align: 'stretch'
                    },
                    // defaults: {
                    //     scrollable: true
                    // },
                    flex: 1,
                    items:[
                        {
                            layout: {
                                type: 'vbox',
                                pack: 'start',
                                align: 'stretch'
                            },
                            items: [
                                {
                                    xtype: 'tabpanel',
                                    flex: 1,
                                    width: 350,
                                    title: 'Navigation',
                                    margin: '0 0 5 0',
                                    header: {
                                        items: [
                                            {
                                                xtype: 'button',
                                                text: 'Show all ',
                                                tooltip: 'Show a table containing all items of selected type',
                                                glyph: 'f0ce@FontAwesome',
                                                handler: function() {
                                                    var rpcController = LHCb.app.getController("LHCb.controller.RPCController");
                                                    var tabName = Ext.ComponentQuery.query('tabpanel[itemId=tabpanel]')[0].activeTab.tabname;
                                                    if(tabName == 'tasks'){
                                                        rpcController.showFullTasksTable();
                                                    }
                                                    if(tabName == 'task sets'){
                                                        rpcController.showFullTaskSetsTable();
                                                    }
                                                    if(tabName == 'node classes'){
                                                        rpcController.showFullClassesTable();
                                                    }
                                                    if(tabName == 'nodes'){
                                                        rpcController.showFullNodesTable();
                                                    }
                                                }
                                            }
                                        ]
                                    }
                                },
                                {
                                    xtype: 'panel',
                                    title: '<center>Selected item\'s description</center>',
                                    itemId: 'descriptionwindow',
                                    frame: true,
                                    scrollable: true,
                                    collapsible: true,
                                    height: 150
                                }
                            ]
                        },
                        {
                            flex: 1,
                            layout: {
                                type: 'vbox',
                                pack: 'start',
                                align: 'stretch'
                            },
                            items: [
                                // {
                                //     title: '<center>Operation</center>'
                                // },
                                {
                                    // Default tools panel (for the main table view) - checks on which tab it is and handles operations
                                    itemId: 'mainoperationwindowtoolbar',
                                    title: 'Operation',
                                    header: {
                                        items: [
                                            {
                                                xtype: 'button',
                                                text: 'Create',
                                                glyph: 'f055@FontAwesome',
                                                tooltip: 'Create a new item',
                                                handler: function() {
                                                    var rpcController = LHCb.app.getController("LHCb.controller.RPCController");
                                                    var activeWindow = Ext.ComponentQuery.query('panel[itemId=mainoperationwindow]')[0].getLayout().getActiveItem().toolbaridentifier;
                                                    if(activeWindow == 'taskstable'){
                                                        rpcController.onAddTask();
                                                    }
                                                    if(activeWindow == 'setstable'){
                                                        rpcController.onAddTaskSet();
                                                    }
                                                    if(activeWindow == 'classestable'){
                                                        rpcController.onAddClass();
                                                    }
                                                    if(activeWindow == 'nodestable'){
                                                        rpcController.onAddNode();
                                                    }
                                                }
                                            },
                                            {
                                                xtype: 'button',
                                                glyph: 'f056@FontAwesome',
                                                tooltip: 'Delete the selected item(s)',
                                                text: 'Delete',
                                                handler: function() {
                                                    var rpcController = LHCb.app.getController("LHCb.controller.RPCController");
                                                    var activeWindow = Ext.ComponentQuery.query('panel[itemId=mainoperationwindow]')[0].getLayout().getActiveItem().toolbaridentifier;
                                                    if(activeWindow == 'taskstable'){
                                                        rpcController.onDeleteTasks();
                                                    }
                                                    if(activeWindow == 'setstable'){
                                                        rpcController.onDeleteTaskSets();
                                                    }
                                                    if(activeWindow == 'classestable'){
                                                        rpcController.onDeleteClasses();
                                                    }
                                                    if(activeWindow == 'nodestable'){
                                                        rpcController.onDeleteNodes();
                                                    }
                                                }
                                            }
                                        ]
                                    }
                                },
                                {
                                    // Single item toolbar - same as above but for single item views :P
                                    itemId: 'singleoperationwindowtoolbar',
                                    // hidemebydefault: Ext.ComponentQuery.query('panel[itemId=singleoperationwindowtoolbar]')[0].setVisible(false),
                                    title: 'Operation',
                                    hidden: true,
                                    header: {
                                        items: [
                                            {
                                                xtype: 'button',
                                                text: 'Modify',
                                                glyph: 'f044@FontAwesome',
                                                tooltip: 'Modify the selected item',
                                                handler: function() {
                                                    var rpcController = LHCb.app.getController("LHCb.controller.RPCController");
                                                    var activeWindow = Ext.ComponentQuery.query('panel[itemId=mainoperationwindow]')[0].getLayout().getActiveItem().toolbaridentifier;
                                                    if(activeWindow == 'singletask'){
                                                        rpcController.onModifyTask();
                                                    }
                                                    if(activeWindow == 'singleset'){
                                                        rpcController.onModifyTaskSet();
                                                    }
                                                    if(activeWindow == 'singleclass'){
                                                        rpcController.onModifyClass();
                                                    }
                                                    if(activeWindow == 'singlenode'){
                                                        rpcController.onModifyNode();
                                                    }
                                                }
                                            },
                                            {
                                                xtype: 'button',
                                                text: 'Assign / unassign items',
                                                iconCls: 'x-fa fa-sign-in',
                                                tooltip: 'Assign items to the selected set',
                                                handler: function() {
                                                    var rpcController = LHCb.app.getController("LHCb.controller.RPCController");
                                                    var activeWindow = Ext.ComponentQuery.query('panel[itemId=mainoperationwindow]')[0].getLayout().getActiveItem().toolbaridentifier;
                                                    if(activeWindow == 'singleset'){
                                                        rpcController.onAssignToTaskSet();
                                                    }
                                                    if(activeWindow == 'singleclass'){
                                                        rpcController.onAssignToClass();
                                                    }
                                                    if(activeWindow == 'singlenode'){
                                                        rpcController.onAssignToNode();
                                                    }
                                                }
                                            },
                                            {
                                                xtype: 'button',
                                                glyph: 'f056@FontAwesome',
                                                tooltip: 'Delete the selected item',
                                                text: 'Delete',
                                                handler: function() {
                                                    var rpcController = LHCb.app.getController("LHCb.controller.RPCController");
                                                    var activeWindow = Ext.ComponentQuery.query('panel[itemId=mainoperationwindow]')[0].getLayout().getActiveItem().toolbaridentifier;
                                                    if(activeWindow == 'singletask'){
                                                        rpcController.onDeleteTask();
                                                    }
                                                    if(activeWindow == 'singleset'){
                                                        rpcController.onDeleteTaskSet();
                                                    }
                                                }
                                            }
                                        ]
                                    }
                                },
                                {
                                    autoScroll: true,
                                    flex: 1,
                                    items: [{
                                        xtype: 'mainoperationwindow',
                                        itemId: 'mainoperationwindow'
                                    }]  
                                }
                                
                            ]
                                                        
                        }
                    ]
                }]
        }
    ]

    
});

    








