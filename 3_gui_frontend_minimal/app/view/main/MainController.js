/**
 * This class is the controller for the main view for the application. It is specified as
 * the "controller" of the Main view class.
 *
 * TODO - Replace this content of this view to suite the needs of your application.
 */
Ext.define('LHCb.view.main.MainController', {
    extend: 'Ext.app.ViewController',

    requires: ['LHCb.model.TasksTableModel', 'LHCb.model.TaskSetsTableModel', 'LHCb.store.SelectedItemData'],

    itemId: 'maincontroller',
    id: 'maincontroller',

    alias: 'controller.main',
});




