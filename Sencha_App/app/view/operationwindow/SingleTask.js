Ext.define('LHCb.view.operationwindow.SingleTask', {
    extend: 'Ext.panel.Panel',
    xtype: 'singletask',

    requires: ['LHCb.model.TasksTableModel'],

    controller: 'main',

    title: '<center>Selected task overview</center>',
    bind: '{tasks}',


    // viewModel: {
    //     stores: {
    //         tasks: {
    //             model: 'LHCb.model.TasksTableModel',
    //             autoLoad: true
    //         }
    //     }
    // },

    html: 'Blablabla'

});



