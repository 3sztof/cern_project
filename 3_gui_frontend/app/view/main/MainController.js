/**
 * This class is the controller for the main view for the application. It is specified as
 * the "controller" of the Main view class.
 *
 * TODO - Replace this content of this view to suite the needs of your application.
 */
Ext.define('LHCb.view.main.MainController', {
    extend: 'Ext.app.ViewController',

    requires: ['LHCb.model.TasksTableModel', 'LHCb.model.TaskSetsTableModel', 'LHCb.store.SelectedItemData'],

    alias: 'controller.main',

    showFullTasksTable: function() {
        var operationwindow = Ext.ComponentQuery.query('panel[itemId=mainoperationwindow]')[0];
        operationwindow.setActiveItem(0);
    },

    showFullTaskSetsTable: function() {
        var operationwindow = Ext.ComponentQuery.query('panel[itemId=mainoperationwindow]')[0];
        operationwindow.setActiveItem(2);
    },


    // d8888b. d88888b db      d88888b d888888b d88888b 
    // 88  `8D 88'     88      88'     `~~88~~' 88'     
    // 88   88 88ooooo 88      88ooooo    88    88ooooo 
    // 88   88 88~~~~~ 88      88~~~~~    88    88~~~~~ 
    // 88  .8D 88.     88booo. 88.        88    88.     
    // Y8888D' Y88888P Y88888P Y88888P    YP    Y88888P 


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

    onDeleteTaskSet: function() {
        Ext.Msg.confirm('Confirm', 'Are you sure?', function(btnText) {
            // console.log(btnText);
            if(btnText === "no"){
                this.destroy();
            }
            else if(btnText === "yes"){
                this.onConfirmDeleteTaskSet();
            }
        }, this);
    },

    onConfirmDeleteTaskSet: function() {
        Ext.Ajax.request({
            method: 'POST',
            // Send a JSONRPC request to the server (delete selected item)
            jsonData: new JSON_RPC.Request('deleteSet', [{'task_set':LHCb.store.SelectedItemData.task_set}]),
            dataType: 'json',
            url: 'http://localhost:8081/TDBDATA/JSONRPC',
            
            success: function(response) {
                // Switch to single task view
                var operationwindow = Ext.ComponentQuery.query('panel[itemId=mainoperationwindow]')[0];
                operationwindow.setActiveItem(2);
                // Alert that the task set has been deleted
                Ext.MessageBox.alert('Status', 'The task set has been deleted from the database.', this.showResult, this);
                // Reload store to refresh tables
                Ext.getCmp('tasksetsexplorergrid').getStore().reload();
                Ext.getCmp('tasksetstablegrid').getStore().reload();
                // console.log(response.responseText);
            },
            failure: function(response) {
                Ext.MessageBox.alert('Status', 'Request failed: the task has not been deleted from the database. Details: ' + response.responseText, this.showResult, this);
            }         
         }); 
    },

    // .88b  d88.  .d88b.  d8888b. d888888b d88888b db    db 
    // 88'YbdP`88 .8P  Y8. 88  `8D   `88'   88'     `8b  d8' 
    // 88  88  88 88    88 88   88    88    88ooo    `8bd8'  
    // 88  88  88 88    88 88   88    88    88~~~      88    
    // 88  88  88 `8b  d8' 88  .8D   .88.   88         88    
    // YP  YP  YP  `Y88P'  Y8888D' Y888888P YP         YP    


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
                    text: 'Save',
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

    onModifyTaskSet: function() {
        var taskSetModifyWindow = Ext.create('Ext.window.Window', {
            title: 'Modify the selected task set',
            itemId: 'modifytasksetform',
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
                        fieldLabel: 'Task set\'s unique name:',
                        value: '', // value property is filled in by the listener in OperationWindow.js (on card activate)
                    },
                    {
                        xtype: 'textareafield',
                        name: 'description_mod',
                        fieldLabel: 'Task set\'s description',
                        value: ''
                }],
            fbar: [{
                    text: 'Save',
                    formBind: true,
                    //itemId: 'submit',
                    handler: function() {
                        var form = this.up('form'); // get the form panel
                        var formFields = form.items;
                        Ext.Ajax.request({
                            method: 'POST',
                            // Send a JSONRPC request to the server (delete selected item)
                            jsonData: new JSON_RPC.Request('modifySet', [{
                                    'original_task_set': LHCb.store.SelectedItemData.task_set,
                                    'task_set': formFields.items[0].value,
                                    'description': formFields.items[1].value,
                                }]),
                            dataType: 'json',
                            url: 'http://localhost:8081/TDBDATA/JSONRPC',
                            
                            success: function(response) {
                                // Switch to single task set view
                                var operationwindow = Ext.ComponentQuery.query('panel[itemId=mainoperationwindow]')[0];
                                operationwindow.setActiveItem(2);
                                // Alert that the task set has been modified
                                Ext.MessageBox.alert('Status', 'The task set has been modified.', this.showResult, this);
                                // Reload store to refresh tables
                                Ext.getCmp('tasksetsexplorergrid').getStore().reload();
                                Ext.getCmp('tasksetstablegrid').getStore().reload();
                                // console.log(response.responseText);
                            },
                            failure: function(response) {
                                Ext.MessageBox.alert('Status', 'Request failed: the task set has not been modified in the database. Details: ' + response.responseText, this.showResult, this);
                            }         
                         }); 
                        Ext.ComponentQuery.query('panel[itemId=modifytasksetform]')[0].destroy();
                    }
                }]
            }]
                    }).show();
        
        // Fill the form with default data - selected item data store
        var form = taskSetModifyWindow.items.items[0].items;
        form.items[0].setValue(LHCb.store.SelectedItemData.task_set);
        form.items[1].setValue(LHCb.store.SelectedItemData.description);
    },

    // .d8b.  d8888b. d8888b. 
    // d8' `8b 88  `8D 88  `8D 
    // 88ooo88 88   88 88   88 
    // 88~~~88 88   88 88   88 
    // 88   88 88  .8D 88  .8D 
    // YP   YP Y8888D' Y8888D' 
    

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
                    text: 'Save',
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
    },

    onAddTaskSet: function() {
        var taskAddWindow = Ext.create('Ext.window.Window', {
            title: 'Add a new task set to the database',
            itemId: 'addtasksetform',
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
                        fieldLabel: 'Task set\'s unique name:',
                    },
                    {
                        xtype: 'textareafield',
                        name: 'description',
                        fieldLabel: 'Task set\'s description',
                }],
            fbar: [{
                    text: 'Save',
                    formBind: true,
                    // itemId: 'submit',
                    handler: function() {
                        var form = this.up('form'); // get the form panel
                        var formFields = form.items;
                        Ext.Ajax.request({
                            method: 'POST',
                            // Send a JSONRPC request to the server (delete selected item)
                            jsonData: new JSON_RPC.Request('addSet', [{
                                    'task_set': formFields.items[0].value,
                                    'description': formFields.items[1].value
                                }]),
                            dataType: 'json',
                            url: 'http://localhost:8081/TDBDATA/JSONRPC',
                            
                            success: function(response) {
                                // Alert that the task has been added
                                Ext.MessageBox.alert('Status', 'The task set has been added to the database.', this.showResult, this);
                                // Reload store to refresh tables
                                Ext.getCmp('tasksetsexplorergrid').getStore().reload();
                                Ext.getCmp('tasksetstablegrid').getStore().reload();
                                // console.log(response.responseText);
                            },
                            failure: function(response) {
                                Ext.MessageBox.alert('Status', 'Request failed: the task set has not been added to the the database. Details: ' + response.responseText, this.showResult, this);
                            }         
                         }); 
                        Ext.ComponentQuery.query('panel[itemId=addtasksetform]')[0].destroy();
                    }
                }]
            }]
                    }).show();
    },

    // d8888b. d88888b db      d88888b d888888b d88888b         .88b  d88.  .d8b.  d8b   db db    db 
    // 88  `8D 88'     88      88'     `~~88~~' 88'             88'YbdP`88 d8' `8b 888o  88 `8b  d8' 
    // 88   88 88ooooo 88      88ooooo    88    88ooooo         88  88  88 88ooo88 88V8o 88  `8bd8'  
    // 88   88 88~~~~~ 88      88~~~~~    88    88~~~~~         88  88  88 88~~~88 88 V8o88    88    
    // 88  .8D 88.     88booo. 88.        88    88.             88  88  88 88   88 88  V888    88    
    // Y8888D' Y88888P Y88888P Y88888P    YP    Y88888P         YP  YP  YP YP   YP VP   V8P    YP    


    onDeleteTasks: function() {
        var tasksGrid = Ext.getCmp('taskstablegrid')
        var selectionModel = tasksGrid.getSelectionModel()
        if (selectionModel.hasSelection()) {
            var selectedItems = selectionModel.getSelection()
            for (var i in selectedItems){
                Ext.Ajax.request({
                    method: 'POST',
                    // Send a JSONRPC request to the server (delete selected item)
                    jsonData: new JSON_RPC.Request('deleteTask', [{
                            'task': selectedItems[i].data["task"]
                        }]),
                    dataType: 'json',
                    url: 'http://localhost:8081/TDBDATA/JSONRPC',     
                 });
            }

            // Alert that the task has been added
            Ext.MessageBox.alert('Status', 'The selected tasks have been deleted from the database.', this.showResult, this);
            // Reload store to refresh tables
            Ext.getCmp('taskexplorergrid').getStore().reload();
            Ext.getCmp('taskstablegrid').getStore().reload();
            // console.log(response.responseText);
        }
        else{
            Ext.MessageBox.alert('Status', 'Error: no tasks were selected in the table, no action was taken.');
        }

    },

    onDeleteTaskSets: function() {
        var tasksGrid = Ext.getCmp('tasksetstablegrid')
        var selectionModel = tasksGrid.getSelectionModel()
        if (selectionModel.hasSelection()) {
            var selectedItems = selectionModel.getSelection()
            for (var i in selectedItems){
                Ext.Ajax.request({
                    method: 'POST',
                    // Send a JSONRPC request to the server (delete selected item)
                    jsonData: new JSON_RPC.Request('deleteSet', [{
                            'task_set': selectedItems[i].data["task_set"]
                        }]),
                    dataType: 'json',
                    url: 'http://localhost:8081/TDBDATA/JSONRPC',     
                 });
            }

            // Alert that the task has been added
            Ext.MessageBox.alert('Status', 'The selected task sets have been deleted from the database.', this.showResult, this);
            // Reload store to refresh tables
            Ext.getCmp('tasksetsexplorergrid').getStore().reload();
            Ext.getCmp('tasksetstablegrid').getStore().reload();
            // console.log(response.responseText);
        }
        else{
            Ext.MessageBox.alert('Status', 'Error: no task sets were selected in the table, no action was taken.');
        }

    },
    
    // .d8b.  .d8888. .d8888. d888888b  d888b  d8b   db 
    // d8' `8b 88'  YP 88'  YP   `88'   88' Y8b 888o  88 
    // 88ooo88 `8bo.   `8bo.      88    88      88V8o 88 
    // 88~~~88   `Y8b.   `Y8b.    88    88  ooo 88 V8o88 
    // 88   88 db   8D db   8D   .88.   88. ~8~ 88  V888 
    // YP   YP `8888Y' `8888Y' Y888888P  Y888P  VP   V8P 
    
    onAssignToTaskSet: function() {
        // http://examples.sencha.com/extjs/6.2.0/examples/kitchensink/?classic#dd-grid-to-grid
        var assignToTaskSetWindow = Ext.create('Ext.window.Window', {

            title: 'Assign tasks to a task set',
            controller: 'dd-grid-to-grid',
            
            itemId: 'assigntasktotaskset',

            closable: true,
            closeAction: 'destroy',
            minWidth: 250,
            border: false,
            modal: true,
            width: 650,
            height: 420,
            viewModel: {
                stores: {
                    
                    assigned_items: {
                        autoLoad: true,
                        fields: [
                            {name: 'task', type: 'string'} 
                        ],
                    
                    
                        proxy: {
                            disableCache: false,
                            method: 'POST',
                            type: 'myproxy',
                            dataType: 'json',
                            actionMethods : {create: "POST", read: "POST", update: "POST", destroy: "POST"},
                            jsonData: new JSON_RPC.Request("tasksInSet", [{"task_set":LHCb.store.SelectedItemData.task_set}]),
                            reader: {
                                type: 'json',
                                rootProperty: 'result'
                            }
                        },
                        
                    },
                    tasks: {
                        //model: 'LHCb.model.TasksTableModel',
                        autoLoad: true,

                        fields: [
                            {name: 'task', type: 'string'} 
                        ],
                                        
                        proxy: {
                            disableCache: false,
                            method: 'POST',
                            type: 'myproxy',
                            dataType: 'json',
                            actionMethods : {create: "POST", read: "POST", update: "POST", destroy: "POST"},
                            jsonData: new JSON_RPC.Request("getTask", [{"task":"*"}]),
                            reader: {
                                type: 'json',
                                rootProperty: 'result'
                            }
                        },
                        listeners: { 
                            load: function() {
                                // Only overwrite the tasks table (remove tasks that are assigned) if both tables are not empty - otherwise the algorithm will crash badly! :D
                                 if(typeof this.data.items[0] !== "undefined" &&  typeof Ext.ComponentQuery.query('panel[itemId=assigntasktotaskset]')[0].viewModel.storeInfo.assigned_items.data.items[0] !== "undefined"){
                                    // console.log("Store loading, attempting to delete some items... :DD")
                                    
                                    // Thats a crazy query! Come on!
                                    var notInSetRows = this.data.items[0].store.data.items; //working
                                    var inSetRows = Ext.ComponentQuery.query('panel[itemId=assigntasktotaskset]')[0].viewModel.storeInfo.assigned_items.data.items[0].store.data.items;                

        
                                    for(var i = 0; i < notInSetRows.length; i++){
                                        // console.log("Scanning notInSetRows")
                                        for(var k = 0; k < inSetRows.length; k++){
                                            // console.log("Scanning inSetRows")
                                            if(inSetRows[k].data["task"] == notInSetRows[i].data["task"]){
                                                // console.log("Deleting " + notInSetRows[i].data["task"] + " from Tasks to Assign table as it appears to be already assigned.")
                                                // console.log(this.data.items[0].store.find('task', inSetRows[k].data["task"]))
                                                this.removeAt(this.find('task', inSetRows[k].data["task"]))
                                            }
                                        }
                                    }
                                }
                                
                            }
                         }
                        
                    }
                }
            },
            tools: [{
                type: 'refresh',
                tooltip: 'Reset assignment',
                handler: 'onResetClick'
            }],
            layout: {
                type: 'hbox',
                pack: 'center',
                align: 'stretch'
            },  
            items: [{
                xtype: 'grid',
                // title: 'First Grid',
                reference: 'grid1',

                itemId: 'notassignedtasksgrid',

                flex: 1,

                multiSelect: false,
                margin: '0 5 0 0',

                

                viewConfig: {
                    plugins: {
                        ptype: 'gridviewdragdrop',
                        containerScroll: true,
                        dragGroup: 'dd-grid-to-grid-group1',
                        dropGroup: 'dd-grid-to-grid-group2'
                    },
                    listeners: {
                        drop: 'onDropGrid1'
                    }
                },

                bind: '{tasks}',

                columns: [{
                    text: 'Tasks',
                    dataIndex: 'task',

                    flex: 1,
                    sortable: true
                }]

                
            },
            {
                xtype: 'grid',
                reference: 'grid2',

                itemId: 'assigntasksgrid',

                flex: 1,
                stripeRows: true,

                viewConfig: {
                    plugins: {
                        ptype: 'gridviewdragdrop',
                        containerScroll: true,
                        dragGroup: 'dd-grid-to-grid-group2',
                        dropGroup: 'dd-grid-to-grid-group1',

                        // The right hand drop zone gets special styling when dragging over it.
                        dropZone: {
                            overClass: 'dd-over-gridview'
                        }
                    },

                    listeners: {
                        drop: 'onDropGrid2'
                    }
                },

                bind: '{assigned_items}',
        
                columns: [{
                    text: 'Assigned tasks',
                    dataIndex: 'task',
        
                    flex: 1,
                    sortable: true
                }]
            }
        ],

        fbar: [{
            text: 'Save',
            handler: function() {
                var assignTaskWindow = Ext.ComponentQuery.query('panel[itemId=assigntasktotaskset]')[0];
 
                // Note that those for loops are not as stupid as they seem to be (if there is multiple movement of one item in
                // the tables, the Assign/Unassign stores will contain doubled invalid entries - thanks to the for loops, the DB API
                // will throw errors only for those single items and no invalid assignment / unassignment will be made - while keeping
                // the correct ones... I know, that's not elegant... but it is way less coding than checking stores in every drag & drop handler...)

                var tasks_to_assign = LHCb.store.AssignItemsStore.tasks;

                for (var i in tasks_to_assign){
                    Ext.Ajax.request({
                        method: 'POST',
                        // Send a JSONRPC request to the server (delete selected item)
                        jsonData: new JSON_RPC.Request('assignTask', [{
                                'task': tasks_to_assign[i],
                                'task_set': LHCb.store.SelectedItemData.task_set
                            }]),
                        dataType: 'json',
                        url: 'http://localhost:8081/TDBDATA/JSONRPC',     
                     });
                }

                var tasks_to_unassign = LHCb.store.UnassignItemsStore.tasks;

                for (var i in tasks_to_unassign){
                    Ext.Ajax.request({
                        method: 'POST',
                        // Send a JSONRPC request to the server (delete selected item)
                        jsonData: new JSON_RPC.Request('unassignTask', [{
                                'task': tasks_to_unassign[i],
                                'task_set': LHCb.store.SelectedItemData.task_set
                            }]),
                        dataType: 'json',
                        url: 'http://localhost:8081/TDBDATA/JSONRPC',     
                     });
                }

                Ext.MessageBox.alert('Status', 'The tasks have been assigned to (and unassigned from) the task set.', this.showResult, this);
                assignTaskWindow.destroy();
            }
        }]
                    }).show();
    },
    }

    

);




