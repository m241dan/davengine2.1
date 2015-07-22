local global_entity = nil
local global_input = nil

entity_command = {}

function onInterp( entity, input )
end

function onPrompt( account )
end

entity_command["hi"] = { function()
   global_input:msg( "Hi there.\r\n" ) 
   return "lets shorten this a bit\r\n"
end, 0 }
