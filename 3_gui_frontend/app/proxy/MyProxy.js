Ext.define('LHCb.proxy.MyProxy', {
    extend: 'Ext.data.proxy.Ajax',
    alias: 'proxy.myproxy',
    doRequest: function(operation, callback, scope) {
        var writer  = this.getWriter(),
            request = this.buildRequest(operation, callback, scope);
            
        if (operation.allowWrite()) {
            request = writer.write(request);
        }
        
        // console.log('Making a request using custom proxy: LHCb.proxy.MyProxy');

        Ext.apply(request, {
            url           : this.url,
            headers       : this.headers,
            timeout       : this.timeout,
            jsonData      : this.jsonData,
            // jsonData      : request.params,
            scope         : this,
            callback      : this.createRequestCallback(request, operation, callback, scope),
            method        : this.getMethod(request),
            disableCaching: false // explicitly set it to false, ServerProxy handles caching
        });
        
        delete request.params;
        
        Ext.Ajax.request(request);
        
        // console.log('jsonData: ' + request.jsonData);

        return request;
    },
    constructor: function() {
        LHCb.proxy.MyProxy.superclass.constructor.apply(this, arguments);
        this.actionMethods.read = 'POST';
    }
});