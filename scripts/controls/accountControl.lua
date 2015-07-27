require( "scripts/libs/interpreter_lib" )
require( "scripts/libs/string_lib" )

account_command = {}

function onInterp( account, input )
   davInterp( account, account_command, input )
end

function onPrompt( account )
   local socket = Socket.get( account )
   local width = socket:getOutBufLength( 0 )
   local header = " Account Menu "
   local length = header:len()
   local l_len, r_len, prompt

   l_len = ( width - length ) / 2
   r_len = l_len

   if( ( width - length ) % 2 ~= 0 ) then
      l_len = l_len - 1
   end

   prompt = string.format( "%s%s%s ", string.rep( "-", l_len ), header, string.rep( "-", r_len ) )
   prompt = prompt .. "Commands:\r\n"
   for i, _ in pairs( account_command ) do
      prompt = prompt .. string.format( "   %s\r\n", i )
   end
   prompt = prompt .. string.format( "%s\r\n", string.rep( "-", width ) )
   return prompt
end

account_command["create a character"] = { function( account, input )
   local nanny = Nanny.new( "scripts/nannys/nannyCharCreate.lua" )
   local socket = Socket.get( account )  
   local control_index = socket:control( nanny )
   socket:setState( control_index )
   nanny:setState( 0 )
   return nil
end, 0 }

account_command["hi"] = { function( account, input )
   account:msg( "Hi there.\r\n" ) 
   return nil
end, 0 }
