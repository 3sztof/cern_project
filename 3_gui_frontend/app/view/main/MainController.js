/**
 * This class is the controller for the main view for the application. It is specified as
 * the "controller" of the Main view class.
 *
 * TODO - Replace this content of this view to suite the needs of your application.
 */
Ext.define('LHCb.view.main.MainController', {
    extend: 'Ext.app.ViewController',

    alias: 'controller.main',

    // // Temporary - test
    // gotoView1: function() {
    //     var operationwindow = Ext.ComponentQuery.query('panel[itemId=mainoperationwindow]')[0];
    //     operationwindow.setActiveItem(0);
    // },

    // // Temporary - test
    // gotoView2: function() {
    //     var operationwindow = Ext.ComponentQuery.query('panel[itemId=mainoperationwindow]')[0];
    //     operationwindow.setActiveItem(1);
    // },

    showFullTasksTable: function() {
        var operationwindow = Ext.ComponentQuery.query('panel[itemId=mainoperationwindow]')[0];
        operationwindow.setActiveItem(0);
    },

    onDeleteTask: function() {
        Ext.Msg.confirm('Confirm', 'Are you sure?', function(btnText) {
            // console.log(btnText);
            if(btnText === "no"){
                this.destroy();
            }
            else if(btnText === "yes"){
                this.onConfirmDeleteTask();
            }
        }, this);
    },

    onConfirmDeleteTask: function() {
        Ext.Ajax.request({
            method: 'POST',
            // Send a JSONRPC request to the server (delete selected item)
            jsonData: new JSON_RPC.Request('deleteTask', [{'task':LHCb.store.SelectedItemData.task}]),
            dataType: 'json',
            url: 'http://localhost:8081/TDBDATA/JSONRPC',
            
            success: function(response) {
                // Switch to single task view
                var operationwindow = Ext.ComponentQuery.query('panel[itemId=mainoperationwindow]')[0];
                operationwindow.setActiveItem(0);
                // Alert that the task has been deleted
                Ext.MessageBox.alert('Status', 'The task has been deleted from the database.', this.showResult, this);
                // Reload store to refresh tables
                Ext.getCmp('taskexplorergrid').getStore().reload();
                Ext.getCmp('taskstablegrid').getStore().reload();
                // console.log(response.responseText);
            },
            failure: function(response) {
                Ext.MessageBox.alert('Status', 'Request failed: the task has not been deleted from the database. Details: ' + response.responseText, this.showResult, this);
            }         
         }); 
    },

    onModifyTask: function() {
        var taskModifyWindow = Ext.create('Ext.window.Window', {
            title: 'Modify the selected task',
            itemId: 'modifytaskform',
            closable: true,
            closeAction: 'destroy',
            width: 350,
            minWidth: 250,
            border: false,
            modal: true,
            items: [{
                xtype: 'form',
                layout: {
                    type: 'vbox',
                    align: 'center',
                    pack: 'center'
                },
                items: [{
                        xtype: 'textfield',
                        name: 'task_mod',
                        fieldLabel: 'Task\'s unique name:',
                        value: '', // value property is filled in by the listener in OperationWindow.js (on card activate)
                    },
                    {
                        xtype: 'textfield',
                        name: 'script_mod',
                        fieldLabel: 'Task\'s script name:',
                        value: '', // value property is filled in by the listener in OperationWindow.js (on card activate)
                    }, 
                    {
                        xtype: 'textfield',
                        name: 'script_par_mod',
                        fieldLabel: 'Parameters for the script:',
                        value: '', // value property is filled in by the listener in OperationWindow.js (on card activate)
                    },
                    {
                        xtype: 'textfield',
                        name: 'utgid_mod',
                        fieldLabel: 'Process identifier (utgid):',
                        value: '', // value property is filled in by the listener in OperationWindow.js (on card activate)
                    },
                    {
                        xtype: 'textfield',
                        name: 'pcadd_par_mod',
                        fieldLabel: 'Parameters for the pcAdd command:',
                        value: '', // value property is filled in by the listener in OperationWindow.js (on card activate)
                    },  
                    {
                        xtype: 'textareafield',
                        name: 'description_mod',
                        fieldLabel: 'Task\'s description',
                        value: ''
                }],
            fbar: [{
                    text: 'Submit',
                    formBind: true,
                    itemId: 'submit',
                    handler: function() {
                        var form = this.up('form'); // get the form panel
                        var formFields = form.items;
                        Ext.Ajax.request({
                            method: 'POST',
                            // Send a JSONRPC request to the server (delete selected item)
                            jsonData: new JSON_RPC.Request('modifyTask', [{
                                    'original_task': LHCb.store.SelectedItemData.task,
                                    'task': formFields.items[0].value,
                                    'command': formFields.items[1].value,
                                    'task_parameters': formFields.items[2].value,
                                    'utgid': formFields.items[3].value,
                                    'command_parameters': formFields.items[4].value,
                                    'description': formFields.items[5].value
                                }]),
                            dataType: 'json',
                            url: 'http://localhost:8081/TDBDATA/JSONRPC',
                            
                            success: function(response) {
                                // Switch to single task view
                                var operationwindow = Ext.ComponentQuery.query('panel[itemId=mainoperationwindow]')[0];
                                operationwindow.setActiveItem(0);
                                // Alert that the task has been modified
                                Ext.MessageBox.alert('Status', 'The task has been modified.', this.showResult, this);
                                // Reload store to refresh tables
                                Ext.getCmp('taskexplorergrid').getStore().reload();
                                Ext.getCmp('taskstablegrid').getStore().reload();
                                // console.log(response.responseText);
                            },
                            failure: function(response) {
                                Ext.MessageBox.alert('Status', 'Request failed: the task has not been modified in the database. Details: ' + response.responseText, this.showResult, this);
                            }         
                         }); 
                        Ext.ComponentQuery.query('panel[itemId=modifytaskform]')[0].destroy();
                    }
                }]
            }]
                    }).show();
        
        // Fill the form with default data - selected item data store
        var form = taskModifyWindow.items.items[0].items;
        form.items[0].setValue(LHCb.store.SelectedItemData.task);
        form.items[1].setValue(LHCb.store.SelectedItemData.command);
        form.items[2].setValue(LHCb.store.SelectedItemData.task_parameters);
        form.items[3].setValue(LHCb.store.SelectedItemData.utgid);
        form.items[4].setValue(LHCb.store.SelectedItemData.command_parameters);
        form.items[5].setValue(LHCb.store.SelectedItemData.description);
    },

    onAddTask: function() {
        var taskAddWindow = Ext.create('Ext.window.Window', {
            title: 'Add a new task to the database',
            itemId: 'addtaskform',
            closable: true,
            closeAction: 'destroy',
            width: 350,
            minWidth: 250,
            border: false,
            modal: true,
            items: [{
                xtype: 'form',
                layout: {
                    type: 'vbox',
                    align: 'center',
                    pack: 'center'
                },
                items: [{
                        xtype: 'textfield',
                        name: 'task',
                        fieldLabel: 'Task\'s unique name:',
                    },
                    {
                        xtype: 'textfield',
                        name: 'command',
                        fieldLabel: 'Task\'s script name:',
                    }, 
                    {
                        xtype: 'textfield',
                        name: 'command_parameters',
                        fieldLabel: 'Parameters for the script:',
                    },
                    {
                        xtype: 'textfield',
                        name: 'utgid',
                        fieldLabel: 'Process identifier (utgid):',
                    },
                    {
                        xtype: 'textfield',
                        name: 'script_parameters',
                        fieldLabel: 'Parameters for the pcAdd command:',
                    },  
                    {
                        xtype: 'textareafield',
                        name: 'description',
                        fieldLabel: 'Task\'s description',
                }],
            fbar: [{
                    text: 'Submit',
                    formBind: true,
                    itemId: 'submit',
                    handler: function() {
                        var form = this.up('form'); // get the form panel
                        var formFields = form.items;
                        Ext.Ajax.request({
                            method: 'POST',
                            // Send a JSONRPC request to the server (delete selected item)
                            jsonData: new JSON_RPC.Request('addTask', [{
                                    'task': formFields.items[0].value,
                                    'command': formFields.items[1].value,
                                    'task_parameters': formFields.items[2].value,
                                    'utgid': formFields.items[3].value,
                                    'command_parameters': formFields.items[4].value,
                                    'description': formFields.items[5].value
                                }]),
                            dataType: 'json',
                            url: 'http://localhost:8081/TDBDATA/JSONRPC',
                            
                            success: function(response) {
                                // Alert that the task has been added
                                Ext.MessageBox.alert('Status', 'The task has been added to the database.', this.showResult, this);
                                // Reload store to refresh tables
                                Ext.getCmp('taskexplorergrid').getStore().reload();
                                Ext.getCmp('taskstablegrid').getStore().reload();
                                // console.log(response.responseText);
                            },
                            failure: function(response) {
                                Ext.MessageBox.alert('Status', 'Request failed: the task has not been added to the the database. Details: ' + response.responseText, this.showResult, this);
                            }         
                         }); 
                        Ext.ComponentQuery.query('panel[itemId=addtaskform]')[0].destroy();
                    }
                }]
            }]
                    }).show();
    }

});
