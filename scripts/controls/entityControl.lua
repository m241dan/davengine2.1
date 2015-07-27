local global_entity = nil
local global_input = nil

entity_command = {}

function onInterp( entity, input )
   global_entity = entity
   global_input = input
   local value
   local command = entity_command[input]
   if( command ~= nil and command[2] <= entity:getLevel() ) then
      value = command[1]()
      if( value ~= nil ) then
         entity:msg( string.format( "%s\r\n", value ) )
      end
   else
      account:msg( "Bad command, try again.\r\n" )
   end
end

function onPrompt( account )
end

entity_command["hi"] = { function()
   global_input:msg( "Hi there.\r\n" ) 
   return nil
end, 0 }
