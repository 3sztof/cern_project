/*
 * This file is generated and updated by Sencha Cmd. You can edit this file as
 * needed for your application, but these edits will have to be merged by
 * Sencha Cmd when upgrading.
 */
Ext.application({
    name: 'LHCb',

    extend: 'LHCb.Application',

    requires: [
        'LHCb.view.main.Main',
        //'LHCb.store.TaskTreeStore',
        'LHCb.view.operationwindow.TasksTable',
        'LHCb.model.TasksTableModel',
        'LHCb.view.operationwindow.SingleTask'
    ],


    toolkit: 'classic',

    // The name of the initial view to create. With the classic toolkit this class
    // will gain a "viewport" plugin if it does not extend Ext.Viewport. With the
    // modern toolkit, the main view will be added to the Viewport.
    //
    mainView: 'LHCb.view.main.Main'

    //-------------------------------------------------------------------------
    // Most customizations should be made to LHCb.Application. If you need to
    // customize this file, doing so below this section reduces the likelihood
    // of merge conflicts when upgrading to new versions of Sencha Cmd.
    //-------------------------------------------------------------------------
});