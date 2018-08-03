Ext.define('LHCb.view.tabpanel.TabPanel', {
    extend: 'Ext.tab.Panel',

    xtype: 'tabpanel',

    controller: 'tabpanel',

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
            cls: 'card',
            items: [
                {                
                    //xtype: 'tasktree' // Initial idea - task tree, replaced by regular associated table
                    xtype: 'taskexplorer'
                }
            ]
        },
        {
            title: 'Task Sets',
            html : 'Task Sets tree from DB via REST.',
            cls: 'card'
        },
        {
            title: 'Node Classes',
            html : 'Node Classes tree from DB via REST.',
            cls: 'card'
        },
        {
            title: 'Nodes',
            html : 'Node names or regular expressions from DB via REST.',
            cls: 'card'
        }
    ]
});