
#include <Core/CoreAll.h>
#include <Fusion/FusionAll.h>
#include <CAM/CAMAll.h>


using namespace adsk::core;
using namespace adsk::fusion;
using namespace adsk::cam;

Ptr<Application> app;
Ptr<UserInterface> ui;

const std::string commandId = "CommandInputCPPCPP";
const std::string panelId = "SolidScriptsAddinsPanel";


// CommandExecuted event handler.
class OnExecuteEventHander : public CommandEventHandler
{
public:
	void notify(const Ptr<CommandEventArgs>& eventArgs) override
	{
		if (!app)
			return;

		 //do nothing, just pop out the dialog
	}
};

class MyCommandInputChange : public InputChangedEventHandler
{
public:
  void notify(const Ptr<InputChangedEventArgs>& eventArgs) override
    {
         Ptr<CommandInput> changedInput = eventArgs->input();

		 //One issue: in C++, eventArgs->inputs() is invalid if the event is fired for the embedded control within a table
         //Ptr<CommandInputs> inputs = eventArgs->inputs(); 
		 //workaround:
		 Ptr<Command> command = eventArgs->firingEvent()->sender();
		 Ptr<CommandInputs> inputs = command->commandInputs(); 
	  

		if (changedInput->id() == "tableDropdown")
        {
 			Ptr<TextBoxCommandInput>  txtBox = inputs->itemById("mytrigger"); 
			Ptr<DropDownCommandInput>  table_dropdown = inputs->itemById("tableDropdown"); 
			txtBox->text(table_dropdown->selectedItem()->name()); 
		}
		else if (changedInput->id() == "table_cell_dropdown")
        {
 			Ptr<TextBoxCommandInput>  txtBox = inputs->itemById("mytrigger");  
			Ptr<DropDownCommandInput>  table_cell_dropdown = inputs->itemById("table_cell_dropdown"); 	
			txtBox->text(table_cell_dropdown->selectedItem()->name()); 
		}
		else if (changedInput->id() == "global_dropdown")
        {
 			Ptr<TextBoxCommandInput>  txtBox = inputs->itemById("mytrigger"); 
			Ptr<DropDownCommandInput>  global_dropdown = inputs->itemById("global_dropdown"); 	
			txtBox->text(global_dropdown->selectedItem()->name());
		} 
		else if (changedInput->id() == "table_cell_txt")
        {
 			Ptr<StringValueCommandInput>  stringBox = inputs->itemById("table_cell_txt"); 
			Ptr<TextBoxCommandInput>  txtBox = inputs->itemById("mytrigger"); 
			txtBox->text(stringBox->value()); 
		}
		else if (changedInput->id() == "table_cell_spinner")
        {
 			Ptr<IntegerSpinnerCommandInput>  spinnerBox = inputs->itemById("table_cell_spinner"); 
			Ptr<TextBoxCommandInput>  txtBox = inputs->itemById("mytrigger"); 
			txtBox->text(std::to_string(spinnerBox->value())); 
		}
    }
};

// CommandCreated event handler.
class OnCommandCreatedEventHandler : public CommandCreatedEventHandler
{
public:
	void notify(const Ptr<CommandCreatedEventArgs>& eventArgs) override
	{
		if (eventArgs)
		{
			Ptr<Command> cmd = eventArgs->command();
			if (cmd)
			{
				// Connect to the CommandExecuted event.
				Ptr<CommandEvent> onExec = cmd->execute();
				if (!onExec)
					return;
				bool isOk = onExec->add(&onExecuteHander_);
				if (!isOk)
					return;				 

				// Define the inputs.
				Ptr<CommandInputs> inputs = cmd->commandInputs();
				if (!inputs)
					return;

				//display the selecting of the table
				inputs->addTextBoxCommandInput("mytrigger", "Trigger", "", 1, true);


				//global dropdown control (same level of the table)
				Ptr<DropDownCommandInput> globalDropdown = inputs->addDropDownCommandInput(
					"global_dropdown", "global dropdown", DropDownStyles::TextListDropDownStyle);

				Ptr<ListItems> global_dropdown_items = globalDropdown->listItems();
				global_dropdown_items->add("Global Dropdown Item 1", true, "");
				global_dropdown_items->add("Global Dropdown Item 2", true, "");
 			 
				//table
				Ptr<TableCommandInput> tableInput = inputs->addTableCommandInput("my_table", "Table", 3, "1:1:1");
				Ptr<CommandInputs> tableChildInputs = tableInput->commandInputs();             
 
				//dropdown control within table
				Ptr<DropDownCommandInput>  tableDropdown = tableChildInputs->addDropDownCommandInput(
					"tableDropdown", "table dropdown",
					DropDownStyles::TextListDropDownStyle);	
				Ptr<ListItems> table_dropdown_items= tableDropdown->listItems();
				table_dropdown_items->add("Table Dropdown Item1", true, "");
				table_dropdown_items->add("Table Dropdown Item2", true, "");
				// the child input will not be added into the table group until calling the table¡¯s addCommandInput or addToolbarCommandInput method
				tableInput->addToolbarCommandInput(tableDropdown);

				//dropdown control in table cell
				Ptr<DropDownCommandInput>  tableCellDropdown = tableChildInputs->addDropDownCommandInput(
					"table_cell_dropdown", "table dropdown", DropDownStyles::TextListDropDownStyle);
				 Ptr<ListItems> table_cell_dropdown_items = tableCellDropdown->listItems();
				table_cell_dropdown_items->add("Table Item 1", true, "");
				table_cell_dropdown_items->add("Table Item 2", true, "");
				tableInput->addCommandInput(tableCellDropdown,0,0);   

				//other controls in the table cell
				Ptr<StringValueCommandInput> table_cell_text = tableChildInputs->addStringValueInput("table_cell_txt", 
					"String", "table_cell_txt");
				tableInput->addCommandInput(table_cell_text,0,1);  

				Ptr<IntegerSpinnerCommandInput> tablespinner = tableChildInputs->addIntegerSpinnerCommandInput("table_cell_spinner", 
					"Integer Spinner",  2 , 9 , 2, 2);
				tableInput->addCommandInput(tablespinner,0,2);   
				 
				cmd->inputChanged()->add(&onCommandInputChangeHandler_);

			}
		}
	}
private:
	OnExecuteEventHander onExecuteHander_; 

	MyCommandInputChange onCommandInputChangeHandler_;

 } cmdCreated_;
	

extern "C" XI_EXPORT bool run(const char* context)
{
	const std::string commandName = "Command Input Test in C++";
	const std::string commandDescription = "This is to test command input in C++";
	const std::string commandResources = "./resources";

	app = Application::get();
	if (!app)
		return false;

	ui = app->userInterface();
	if (!ui)
		return false;

	// add a command on create panel in modeling workspace
	Ptr<Workspaces> workspaces = ui->workspaces();
	if (!workspaces)
		return false;
	Ptr<Workspace> modelingWorkspace = workspaces->itemById("FusionSolidEnvironment");
	if (!modelingWorkspace)
		return false;

	Ptr<ToolbarPanels> toolbarPanels = modelingWorkspace->toolbarPanels();
	if (!toolbarPanels)
		return false;
	Ptr<ToolbarPanel> toolbarPanel = toolbarPanels->itemById(panelId);
	if (!toolbarPanel)
		return false;

	Ptr<ToolbarControls> toolbarControls = toolbarPanel->controls();
	if (!toolbarControls)
		return false;
	Ptr<ToolbarControl> toolbarControl = toolbarControls->itemById(commandId);
	if (toolbarControl)
	{
 		ui->messageBox("[Mouse Test in C++] command is already loaded.");
		adsk::terminate();
		return true;
	}
	else
	{
		Ptr<CommandDefinitions> commandDefinitions = ui->commandDefinitions();
		if (!commandDefinitions)
			return false;
		Ptr<CommandDefinition> commandDefinition = commandDefinitions->itemById(commandId);
		if (!commandDefinition)
		{
			commandDefinition = commandDefinitions->addButtonDefinition(commandId, commandName, commandDescription, commandResources);
			if (!commandDefinition)
				return false;
		}

		Ptr<CommandCreatedEvent> commandCreatedEvent = commandDefinition->commandCreated();
		if (!commandCreatedEvent)
			return false;
		commandCreatedEvent->add(&cmdCreated_);
		toolbarControl = toolbarControls->addCommand(commandDefinition);
		if (!toolbarControl)
			return false;
		toolbarControl->isVisible(true);
	} 

	//ui->messageBox("in run");

	return true;
}

extern "C" XI_EXPORT bool stop(const char* context)
{
	
	if (!app)
		return false;

	if (!ui)
		return false;

	// Get toolbar control to delete
	Ptr<Workspaces> workspaces = ui->workspaces();
	if (!workspaces)
		return false;
	Ptr<Workspace> modelingWorkspace = workspaces->itemById("FusionSolidEnvironment");
	if (!modelingWorkspace)
		return false;

	Ptr<ToolbarPanels> toolbarPanels = modelingWorkspace->toolbarPanels();
	if (!toolbarPanels)
		return false;
	Ptr<ToolbarPanel> toolbarPanel = toolbarPanels->itemById(panelId);
	if (!toolbarPanel)
		return false;

	Ptr<ToolbarControls> toolbarControls = toolbarPanel->controls();
	if (!toolbarControls)
		return false;
	Ptr<ToolbarControl> toolbarControl = toolbarControls->itemById(commandId);

	// Get command definition to delete
	Ptr<CommandDefinitions> commandDefinitions = ui->commandDefinitions();
	if (!commandDefinitions)
		return false;
	Ptr<CommandDefinition> commandDefinition = commandDefinitions->itemById(commandId);

	if (toolbarControl)
		toolbarControl->deleteMe();

	if (commandDefinition)
		commandDefinition->deleteMe();

	if (ui)
	{
		//ui->messageBox("in stop");
		ui = nullptr;
	}

	return true;
}


#ifdef XI_WIN

#include <windows.h>

BOOL APIENTRY DllMain(HMODULE hmodule, DWORD reason, LPVOID reserved)
{
	switch (reason)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

#endif // XI_WIN
