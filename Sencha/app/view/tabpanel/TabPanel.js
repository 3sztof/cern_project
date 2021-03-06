Ext.define('LHCb.view.tabpanel.TabPanel', {
    extend: 'Ext.tab.Panel',

    xtype: 'tabpanel',

    controller: 'main',

    itemId: 'tabpanel',

    shadow: true,
    frame: true,
    cls: 'demo-solid-background',
    tabBar: {
        layout: {
            pack: 'center'
        }
    },
    activeTab: 0, // Active by default
    defaults: {
        scrollable: true
    },
    items: [
        {
            title: 'Tasks',
            id: 'tasktreeID',
            tabname: 'tasks',
            cls: 'card',
            items: [
                {                
                    xtype: 'taskexplorer'
                }
            ]
        },
        {
            title: 'Task Sets',
            cls: 'card',
            tabname: 'task sets',
            items: [
                {                
                    xtype: 'tasksetsexplorer'
                }
            ]
        },
        {
            title: 'Node Classes',
            tabname: 'node classes',
            cls: 'card',
            items: [
                {                
                    xtype: 'classesexplorer'
                }
            ]
        },
        {
            title: 'Nodes',
            tabname: 'nodes',
            cls: 'card',
            items: [
                {                
                    xtype: 'nodesexplorer'
                }
            ]
        },
        {
            title: 'Extra',
            tabname: 'extra',
            cls: 'card',
            items: [
                {                
                    xtype: 'extra'
                }
            ]
        }
    ]
});