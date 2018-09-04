To build the app, you need to:

----------------------------------------------------------------------

1) Link the sdk: modify the workspace.json file specifying a path to 
   Sencha Ext JS SKD as follows:

	"frameworks": {
		"ext": {
		    "path":"path to SDK",
		    "version":"full version number of the SDK"
		}
        
        
    	}

----------------------------------------------------------------------

2) Generate the .sencha folder by executing the following commands
   (consult https://docs.sencha.com/cmd/guides/extjs/cmd_app.html 
   for details):

   Note that you need to update the SKD path to yours!
   Execute the commands outside of the app directory!
   Version of sencha cmd used when building this way: 6.6.0.13


	mkdir temp
	sencha -sdk /home/leon/.Sencha_Ext_JS/ext-6.2.0 generate app MyApp temp
	cp -r temp/.sencha/ <Sencha app folder from the repo>
	rm -r temp
 

----------------------------------------------------------------------

3) Build the app by running (in the app folder):

	sencha app build (production / testing - defaults to production)

   Or start the app in debug mode on port 1841:

	sencha app watch (also needs ext/ folder which can be generated
			 using the existing SDK and placed in root of 
			 the app)

----------------------------------------------------------------------

4) The built app can be found under ./build/(production / testing)/LHCb/
   Build path can be also modified in workspace.json file.
