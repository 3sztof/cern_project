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
                    },
                    items: [
                        {
                            title: '<center><strong>LHCb Online Farm Process Explorer</strong></center>',
                            flex: 1,
                            header: {
                                titlePosition: 1,
                                items: [
                                    {
                                        glyph: 'f129@FontAwesome',
                                        tooltip: 'About',
                                        handler: function(){
                                            Ext.MessageBox.alert('About', '<center><b>LHCb Online Farm Process Explorer</b><br><br>Author: Krzysztof Wilczynski<ul list-style-position: inside; margin-top: 0;><li><a href="mailto:krzysztofwilczynski@mail.com">krzysztofwilczynski@mail.com</a></li><li><a href="https://www.linkedin.com/in/3sztof">www.linkedin.com/in/3sztof/</a></li><li><a href="tel:+48668876202">+48 669 876 202</a></li></ul>Supervisor: Markus Frank<ul list-style-position: inside; margin-top: 0;><li><a href="mailto:markus.frank@cern.ch">markus.frank@cern.ch</a></li></ul></center>', this.showResult, this);
                                        }
                                    }
                                ]
                            }
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
                    // defaults: {
                    //     scrollable: true
                    // },
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
                                    title: '<center>Navigation</center>',
                                    margin: '0 0 5 0'
                                },
                                {
                                    xtype: 'panel',
                                    title: '<center>Selected item\'s description</center>',
                                    itemId: 'descriptionwindow',
                                    frame: true,
                                    scrollable: true,
                                    collapsible: true,
                                    height: 150
                                }
                            ]
                        },
                        {
                            flex: 1,
                            layout: {
                                type: 'vbox',
                                pack: 'start',
                                align: 'stretch'
                            },
                            items: [
                                {
                                    title: '<center>Operation</center>'
                                },
                                {
                                    autoScroll: true,
                                    flex: 1,
                                    items: [{
                                        xtype: 'mainoperationwindow',
                                        itemId: 'mainoperationwindow'
                                    }]  
                                }
                            ]
                                                        
                        }
                    ]
                }]
        }
    ]

    
});

    








