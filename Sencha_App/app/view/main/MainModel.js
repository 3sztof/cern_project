/**
 * This class is the view model for the Main view of the application.
 */
Ext.define('LHCb.view.main.MainModel', {
    extend: 'Ext.app.ViewModel',

    alias: 'viewmodel.main',


    // stores: {
    //     tasktreestore: {
    //         fields: ['command', 'command_parameters', 'description', 'task', 'task_parameters', 'utgid'],
    //         autoLoad: true,
    //         proxy: {
    //             type: 'ajax',
    //             url: 'app/data/tree/check-nodes.json',
    //             reader: {
    //                 type: 'json',
    //                 rootProperty: 'data',
        
    //                 // Do not attempt to load orders inline.
    //                 // They are loaded through the proxy
    //                 //implicitIncludes: false
    //             }
    //         },
    //         listeners: {
    //             load: function() {
    //                 console.log('Loadad the JSON from REST API...');
    //                 console.log(Ext.getStore('ExampleStore'));
    //             }
    //         }
    //     }
        
    // },

    data: {
        name: 'LHCb',

        loremIpsum: 'Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.'
    }

    //TODO - add data, formulas and/or methods to support your view
});
