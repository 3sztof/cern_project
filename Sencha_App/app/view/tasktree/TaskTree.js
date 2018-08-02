// Ext.define('LHCb.view.tasktree.TaskTree', {
//     extend: 'Ext.Container',
//     xtype: 'tasktree',

    

//     items: [{
//         xtype: 'treepanel',
//         rootVisible: false,
//         animate: true,
//         store: {
//             type: 'tasks'
//         }
//     }]
// });







Ext.define('LHCb.view.tasktree.TaskTree', {
    extend: 'Ext.Container',
    xtype: 'tasktree',

    bla: console.log('Tree rendering'),


    controller: 'check-tree',

    items: [{

        xtype: 'treepanel',
        // Checking propagates up and down
        checkPropagation: 'both',

        store: {
            type: 'tasks'
        },
        rootVisible: false,
        useArrows: true,
        frame: true,
        width: 280,
        height: 300,
        bufferedRenderer: false,
        animate: true,
        listeners: {
            beforecheckchange: 'onBeforeCheckChange'
        },
        tbar: [{
            text: 'Get checked nodes',
            handler: 'onCheckedNodesClick'
        }]

    }]

});