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



    layout: {
        type: 'vbox',
        pack: 'start',
        align: 'stretch'
    },

    margin: '5px 5px 5px 5px',
    //defaults: {
    //    margin: '5 5 5 5',
    //    bodyPadding: 5
    //},
    items: [
        {
            type: 'titlebar',
            title: 'LHCb Proces Boot DB Explorer',
            height: 50
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
                        title: 'Navigation'
                    },
                    {
                        xtype: 'panel',
                        title: 'Selected item description',
                        frame: true,
                        height: 150
                    }
                ]
            },
            {
                layout:{
                    type: 'vbox'
                },
                items: [{
                    xtype: 'panel',
                    html: 'blabla',
                    id: 'main-operation-window'
                },
                {
                    xtype: 'button',
                    text: 'Test',
                    listeners: {
                        click: 'testButton'
                    }
                }]
            }]
            
        }]
    });

    








