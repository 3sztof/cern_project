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

    

    layout: {
        type: 'vbox',
        pack: 'center',
        align: 'stretch'
    },
    margin: '0 10',
    defaults: {
        margin: '0 0 10 0',
        bodyPadding: 10
    },
    items: [
        {
            layout: {
                type: 'hbox',
                pack: 'center',
                align: 'stretch'
            },
            defaults: {
                flex: 1,
                bodyPadding: 10,
                html: 'test'
            },
            items: [
                {
                    xtype: 'panel',
                    margin: '0 5 0 0'
                },
                {
                    xtype: 'panel',
                    title: 'Title',
                    margin: '0 0 0 5'
                }
            ]
        },
        {
            xtype: 'panel',
            title: 'Built in Tools',
            html: 'Lorem ipsum',
            tools: [
                {type: 'minimize'},
            ]
        }]
    });