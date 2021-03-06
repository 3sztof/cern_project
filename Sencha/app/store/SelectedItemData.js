Ext.define('LHCb.store.SelectedItemData', {
    extend: 'Ext.data.Store',
    alias: 'store.selecteditemdata',
    singleton : true,

    autoLoad: true,

    fields: [
        'task', 'description', 'utgid', 'command', 'command_parameters', 'task_parameters'
    ],

    task: '',
    task_set: '',
    node_class: '',
    regex: '',
    description: '',
    utgid: '',
    command: '',
    command_parameters: '',
    task_parameters: ''

}); 

