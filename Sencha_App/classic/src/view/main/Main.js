/**
 * This class is the main view for the application. It is specified in app.js as the
 * "mainView" property. That setting automatically applies the "viewport"
 * plugin causing this view to become the body element (i.e., the viewport).
 *
 * TODO - Replace this content of this view to suite the needs of your application.
 */
Ext.define('LHCb.view.main.Main', {
    extend: 'Ext.Container',
    xtype: 'app-main',

    controller: 'main',
    viewModel: 'main',


    //margin: '5px 5px 5px 5px',
    defaults: {
        margin: '3px 3px 3px 3px',
        //padding: 5
    },
    layout: {
        type: 'fit',
    },
    items: [
        {
            layout: {
                type: 'vbox',
                pack: 'start',
                align: 'stretch'
            },
        
            frame: true,
            
            items: [
                {
                    layout: {
                        type: 'hbox',
                        // pack: 'start',
                        // align: 'stretch'
                    },
                    items: [
                        {
                            title: '<center><strong>LHCb Online Farm Process Explorer</strong></center>',
                            flex: 1
                        },
                        {
                            xtype: 'image',
                            src: 'resources/LHCb_mod.png',
                            flex: 0.12,
                            height: 88
                        }
                    ]
                },
                {
                    layout: {
                        type: 'hbox',
                        pack: 'start',
                        align: 'stretch'
                    },
                    flex: 1,
                    items:[
                    {
                        layout: {
                            type: 'vbox',
                            pack: 'start',
                            align: 'stretch'
                        },
                        items: [
                            {
                                xtype: 'tabpanel',
                                flex: 1,
                                width: 350,
                                title: 'Navigation',
                                margin: '0 0 5 0'
                            },
                            {
                                xtype: 'panel',
                                title: 'Selected item\'s description',
                                frame: true,
                                height: 150,
                                tbar: [{
                                    text: 'Load store data',
                                    handler: 'onClickLoadData'
                                }]
                            }
                        ]
                    },
                    {
                        flex: 1,
                        items: [{
                            xtype: 'mainoperationwindow',
                            itemId: 'mainoperationwindow'
                        }]
                    }]
                    
                }]
        }
    ]

    
});

    








