require( "scripts/libs/string_lib" )

local global_account = nil
local global_input = nil

account_command = {}

function onInterp( account, input )
   local value, cmd, command, found
   global_account = account

   cmd, input = oneArg( input )
   command = account_command[cmd]
   global_input = input
   
   if( command == nil ) then
      for i, command in pairs( account_command ) do
         if( isPrefix( i, cmd ) == true ) then
            found = true
            break
         end
      end
      if( found == false )
         account:msg( "Bad command, try again.\r\n" )
         return
      end
   end
   if( command[2] <= account:getLevel() ) then
      value = command[1]()
      if( value ~= nil ) then
         account:msg( string.format( "%s", value ) )
      end
   else

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

account_command["create a character"] = { function()
   local nanny = Nanny.new( "scripts/nannys/nannyCharCreate.lua" )
   local socket = Socket.get( global_account )  
   local control_index = socket:control( nanny )
   socket:setState( control_index )
   nanny:setState( 0 )
   return nil
end, 0 }

account_command["hi"] = { function()
   global_account:msg( "Hi there.\r\n" ) 
   return nil
end, 0 }
