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
                            flex: 1,
                            header: {
                                titlePosition: 1,
                                items: [
                                    {
                                        glyph: 'f129@FontAwesome',
                                        handler:function(){
                                            Ext.MessageBox.alert('About', '<center><b>LHCb Online Farm Process Explorer</b><br><br>Author: Krzysztof Wilczynski<br>Contact me:<uk><li><a href="mailto:krzysztofwilczynski@mail.com">krzysztofwilczynski@mail.com</a></li><li><a href="https://www.linkedin.com/in/3sztof">www.linkedin.com/in/3sztof/</a></li><li><a href="tel:+48668876202">+48 669 876 202</a></li></ul><br>Supervisor: Markus Frank</center>', this.showResult, this);
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
                                height: 150
                                //calc: function () {return '<br>' +  LHCb.store.SelectedItemData.description},
                                //html: this.calc
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

    








