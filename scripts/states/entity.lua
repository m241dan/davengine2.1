entity_command = {}

function onInterp( entity, input )
   davInterp( entity, entity_command, input )
end

function onPrompt( entity )
   entity:msg( "<Prompt>" )
   
end

entity_command["hi"] = { function( entity, input )
   entity:msg( "Hi there.\r\n" ) 
   return nil
end, "the hi command", 0 }
