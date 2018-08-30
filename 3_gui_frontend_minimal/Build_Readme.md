To build the app, you need to:

0) Do we delete workspace.json too? we actually may... then just generate it with 'sencha generate workspace /path/to/LHCb'
1) Wichtig: link the sdk: either specify the path to the SDK in workspace.json or generate ./ext folder by initializing workspace with sdk flag
2) Wichtig: generate a blank app with 'sencha -sdk /path/to/ext6 generate app MyApp /path/to/my-app', then copy .sencha folder from its directory to the root of LHCb app - .sencha is required and this is the way Sencha reccomands to generate it... see: https://docs.sencha.com/cmd/guides/extjs/cmd_app.html
