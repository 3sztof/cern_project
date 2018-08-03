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
    }

});
