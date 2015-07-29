require( "scripts/libs/interpreter_lib" )
require( "scripts/libs/string_lib" )

account_command = {}

function init( account )
   var.new( "characters", 0, account )
end

function onInterp( account, input )
   local characters, socket, control_index, charid

   if( davInterp( account, account_command, input ) == true ) then
      return
   end

   characters = var.get( "characters", account )
   charid = characters[capitalize( input )]
   if( charid ~= nil ) then
      socket = Socket.get( account );
      control_index = socket:control( Entity.get( charid ) )
      socket:setState( control_index )
      return
   end
   account:msg( "Bad command, try again." )
end

function onPrompt( account )
   local socket = Socket.get( account )
   local width = socket:getOutBufWidth( 0 )
   local header = " Account Menu "
   local length = header:len()
   local l_len, r_len, prompt, characters

   l_len = ( width - length ) / 2
   r_len = l_len

   if( ( width - length ) % 2 ~= 0 ) then
      l_len = l_len - 1
   end

   prompt = string.format( "%s%s%s ", string.rep( "-", l_len ), header, string.rep( "-", r_len ) )

   prompt = prompt .. "Commands:\r\n"
   for i, c in pairs( account_command ) do
      prompt = prompt .. string.format( "   %s - %s\r\n", i, c[2] )
   end

   characters = var.get( "characters", account )
   if( characters[0] ~= 0 ) then
      prompt = prompt .. "\r\nCharacters:\r\n"
      for i, c in var.iterate( characters ) do
         if(  i ~= 0 ) then
            prompt = prompt .. string.format( "   %s\r\n", i )
         end
      end
   end

   prompt = prompt .. string.format( "%s\r\n", string.rep( "-", width ) )

   return prompt
end

account_command["create"] = { function( account, input )
   local nanny = Nanny.new( "scripts/nannys/charCreate.lua" )
   local socket = Socket.get( account )  
   local control_index = socket:control( nanny )
   local info = var.get( "Info", nanny )

   info["account"] = account:getID()
   socket:setState( control_index )
   nanny:setState( 0 )
   return nil
end, "Use this command to create a new character", 0 }

account_command["config"] = { function( account, input )
   
end, "Use the config command to edit your configurable options", 0 }

account_command["hi"] = { function( account, input )
   account:msg( "Hi there.\r\n" ) 
   return nil
end, "Say hi to... well no one", 0 }
