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
        Ext.Msg.confirm('Confirm', 'Are you sure?', 'onConfirmDeleteTask', this);
    },

    onConfirmDeleteTask: function() {
        Ext.Ajax.request({
            method: 'PUT',
            // Send a JSONRPC request to the server (delete selected item)
            jsonData: new JSON_RPC.Request('deleteTask', [{'task':LHCb.store.SelectedItemData.task}]),
            dataType: 'json',
            url: 'http://localhost:8080/tasks',
            
            success: function() {
                // Switch to single task view
                var operationwindow = Ext.ComponentQuery.query('panel[itemId=mainoperationwindow]')[0];
                operationwindow.setActiveItem(0);
                // Alert that the task has been deleted
                Ext.MessageBox.alert('Status', 'The task has been deleted from the database.', this.showResult, this);
                // Reload store to refresh tables
                Ext.getCmp('taskexplorergrid').getStore().reload();
                Ext.getCmp('taskstablegrid').getStore().reload();
                //taskexplorer.viewModel.stores.tasks.reload()
            },
            failure: function() {
                Ext.MessageBox.alert('Status', 'Request failed: the task has not been deleted from the database.', this.showResult, this);
            }         
         });
         
    }

});
