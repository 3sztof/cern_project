Ext.define('LHCb.store.SelectedItemData', {
    extend: 'Ext.data.Store',
    alias: 'store.selecteditemdata',
    singleton : true,

    autoLoad: true,

    fields: [
        'task', 'description', 'utgid', 'command', 'command_parameters', 'task_parameters'
    ],

    task: '',
    description: '',
    utgid: '',
    command: '',
    command_parameters: '',
    task_parameters: ''

}); 


// // Association with data key before somehow also works
// Ext.define('LHCb.store.SelectedItemData', {
//     extend: 'Ext.data.Model',
//     alias: 'viewmodel.selecteditemdata',
//     singleton : true,

//     autoLoad: true,

//     fields: [
//         'task', 'description', 'utgid', 'command', 'command_parameters', 'task_parameters'
//     ],

//     data: [
//         {
//             task: 'bla'
//         },
//         {
//             descritpion: ''
//         },
//         {
//             utgid: ''
//         },
//         {
//             command: ''
//         },
//         {
//             command_parameters: ''
//         },
//         {
//             task_parameters: ''
//         }
//     ]

// }); 