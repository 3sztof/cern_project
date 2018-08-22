// Ext.define('LHCb.model.AssignedItems', {
//     extend: 'Ext.data.Model',

//     alias: 'viewmodel.assigneditems',

//     itemId: 'assigneditemsstore',

//     fields: [
//         {name: 'task', type: 'string'} 
//     ],


//     proxy: {
//         disableCache: false,
//         method: 'POST',
//         type: 'myproxy',
//         dataType: 'json',
//         actionMethods : {create: "POST", read: "POST", update: "POST", destroy: "POST"},
//         jsonData: new JSON_RPC.Request("tasksInSet", [{"task_set":LHCb.store.SelectedItemData.task_set}]),
//         a: console.log('store loading'),
//         reader: {
//             type: 'json',
//             rootProperty: 'result'
//         },
//         url: 'http://localhost:8081/TDBDATA/JSONRPC'
//     },

// });