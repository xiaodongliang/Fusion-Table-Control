#Author-Autodesk Inc.
#Description-Demo command input examples
import adsk.core, adsk.fusion, traceback

app = None
ui  = None
commandId = 'CommandInputGallery'
commandName = 'Command Input Gallery'
commandDescription = 'Demo command input examples.'
rowNumber = 0

# Global set of event handlers to keep them referenced for the duration of the command
handlers = [] 

class MyCommandInputChangedHandler(adsk.core.InputChangedEventHandler):
    def __init__(self):
        super().__init__()
    def notify(self, args):
        #ui.messageBox('aaa')
        
        try:
           command = args.firingEvent.sender   
           cmdInput = args.input                   
           inputs = command.commandInputs
          
           if cmdInput.id == commandId +'tableDropdown':
               tableDropdown = inputs.itemById(commandId +'tableDropdown')
               txt = inputs.itemById(commandId +'mytrigger')
               txt.text = tableDropdown.selectedItem.name
           if cmdInput.id == commandId +'global_dropdown':
               tableDropdown = inputs.itemById(commandId +'global_dropdown')
               txt = inputs.itemById(commandId +'mytrigger')
               txt.text = tableDropdown.selectedItem.name
           if cmdInput.id == commandId +'table_cell_dropdown':
               tableDropdown = inputs.itemById(commandId +'table_cell_dropdown')
               txt = inputs.itemById(commandId +'mytrigger')
               txt.text = tableDropdown.selectedItem.name
           if cmdInput.id == commandId +'table_cell_txt':
               tableDropdown = inputs.itemById(commandId +'table_cell_txt')
               txt = inputs.itemById(commandId +'mytrigger')
               txt.text = tableDropdown.value
           if cmdInput.id == commandId +'table_cell_spinner':
               tableDropdown = inputs.itemById(commandId +'table_cell_spinner')
               txt = inputs.itemById(commandId +'mytrigger')
               txt.text = str(tableDropdown.value)
               
        except:
            if ui:
                ui.messageBox('Failed:\n{}'.format(traceback.format_exc()))
                
class MyCommandDestroyHandler(adsk.core.CommandEventHandler):
    def __init__(self):
        super().__init__()
    def notify(self, args):
        try:
            # When the command is done, terminate the script
            # This will release all globals which will remove all event handlers
            adsk.terminate()
        except:
            if ui:
                ui.messageBox('Failed:\n{}'.format(traceback.format_exc()))

class MyCommandCreatedHandler(adsk.core.CommandCreatedEventHandler):
    def __init__(self):
        super().__init__()
    def notify(self, args):
        try:
            cmd = args.command
            onDestroy = MyCommandDestroyHandler()
            cmd.destroy.add(onDestroy)
            # Keep the handler referenced beyond this function
            handlers.append(onDestroy)
            
            onInputChanged = MyCommandInputChangedHandler()
            cmd.inputChanged.add(onInputChanged)
            handlers.append(onInputChanged)    
            
            inputs = cmd.commandInputs
            global commandId 
            
            #display the selecting of the table
            inputs.addTextBoxCommandInput(commandId +'mytrigger', 'Trigger', '', 1, True)
            
            #global dropdown control (same level of the table)
            globalDropdown = inputs.addDropDownCommandInput(commandId +'global_dropdown', 'global dropdown', adsk.core.DropDownStyles.TextListDropDownStyle)

            global_dropdown_items = globalDropdown.listItems
            global_dropdown_items.add('Global Dropdown Item 1', True, '') 
            global_dropdown_items.add('Global Dropdown Item 2', True, '') 
    
             #table
            tableInput = inputs.addTableCommandInput(commandId +'my_table', 'Table', 3, '1:1:1') 
            tableChildInputs = tableInput.commandInputs            
             
            				#dropdown control within table
            tableDropdown = tableChildInputs.addDropDownCommandInput(commandId +'tableDropdown', 'table dropdown',adsk.core.DropDownStyles.TextListDropDownStyle) 	
            table_dropdown_items= tableDropdown.listItems
            table_dropdown_items.add('Table Dropdown Item1', True, '') 
            table_dropdown_items.add('Table Dropdown Item2', True, '') 
            				#the child input will not be added into the table group until calling the table’s addCommandInput or addToolbarCommandInput method
            tableInput.addToolbarCommandInput(tableDropdown) 
            
            				#dropdown control in table cell
            tableCellDropdown = tableChildInputs.addDropDownCommandInput(commandId +'table_cell_dropdown', 'table dropdown', adsk.core.DropDownStyles.TextListDropDownStyle)
            table_cell_dropdown_items = tableCellDropdown.listItems 
            table_cell_dropdown_items.add("Table Item 1", True, '') 
            table_cell_dropdown_items.add("Table Item 2", True, '') 
            tableInput.addCommandInput(tableCellDropdown,0,0)    
            
            				#other controls in the table cell
            table_cell_text = tableChildInputs.addStringValueInput(commandId +'table_cell_txt','String', 'table_cell_txt');
            tableInput.addCommandInput(table_cell_text,0,1)   
            
            tablespinner = tableChildInputs.addIntegerSpinnerCommandInput(commandId +'table_cell_spinner', 'Integer Spinner',  2 , 9 , 2, 2)
            tableInput.addCommandInput(tablespinner,0,2)   
          
            
        except:
            if ui:
                ui.messageBox('Failed:\n{}'.format(traceback.format_exc()))

def run(context):
    ui = None
    try:
        global app
        app = adsk.core.Application.get()
        global ui
        ui = app.userInterface

        global commandId
        global commandName
        global commandDescription

        # Create command defintion
        cmdDef = ui.commandDefinitions.itemById(commandId)
        if not cmdDef:
            cmdDef = ui.commandDefinitions.addButtonDefinition(commandId, commandName, commandDescription)

        # Add command created event
        onCommandCreated = MyCommandCreatedHandler()
        cmdDef.commandCreated.add(onCommandCreated)
        # Keep the handler referenced beyond this function
        handlers.append(onCommandCreated)

        # Execute command
        cmdDef.execute()

        # Prevent this module from being terminate when the script returns, because we are waiting for event handlers to fire
        adsk.autoTerminate(False)

    except:
        if ui:
            ui.messageBox('Failed:\n{}'.format(traceback.format_exc()))