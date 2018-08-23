Ext.define('LHCb.view.main.GridToGridController', {
    extend: 'Ext.app.ViewController',

    alias: 'controller.dd-grid-to-grid',

    beforeRender: function () {
        // var store = this.lookup('grid1').store,
        //     data = (this.myData = []),
        //     obj;

        this.onResetClick();
        

        // Keep a copy of the original data for reset:
        // store.each(function (rec) {
        //     data.push(obj = Ext.apply({}, rec.data));
        //     delete obj.id;
        // });
    },


    onDrop: function (onRec, rec, dropPosition, title) {
        // var dropOn = onRec ? ' ' + dropPosition + ' ' + onRec.get('name') : ' on empty view';

        console.log('Items to assign:');
        console.log(LHCb.store.AssignItemsStore.tasks);
        console.log('Items to unassign:');
        console.log(LHCb.store.UnassignItemsStore.tasks);

        // KitchenSink.toast(title, 'Dropped ' + rec.get('name') + dropOn);
    },

    onDropGrid1: function (node, data, dropRec, dropPosition) {
        // this.onDrop(dropRec, data.records[0], dropPosition, 'Drag from right to left');

        // Assign Logic
        itemtoremove = data.event.item.innerText;
        delete LHCb.store.AssignItemsStore.tasks[itemtoremove];

        // Unassign Logic (keys and values to prevent the overlapping of records indexed by the table index... from previous version)
        key = data.event.item.innerText;
        value = data.event.item.innerText;
        LHCb.store.UnassignItemsStore.tasks[key] = value;
        this.onDrop();
    },

    onDropGrid2: function (node, data, dropRec, dropPosition) {
        // this.onDrop(dropRec, data.records[0], dropPosition, 'Drag from left to right');

        // Assign Logic
        key = data.event.item.innerText;
        value = data.event.item.innerText;
        LHCb.store.AssignItemsStore.tasks[key] = value;

        // Unassign Logic
        itemtoremove = data.event.item.innerText;
        delete LHCb.store.UnassignItemsStore.tasks[itemtoremove];
        this.onDrop();
    },

    onResetClick: function () {
        LHCb.store.AssignItemsStore.tasks = [];
        LHCb.store.UnassignItemsStore.tasks = [];
        this.lookup('grid1').getStore().reload();
        this.lookup('grid2').getStore().reload();
        this.onDrop();
    }

        
    }
);