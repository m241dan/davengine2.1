local global_account = nil
local global_input = nil

account_command = {}

function onInterp( account, input )
   global_account = account
   global_input = input
   local value
   local command = account_command[input]
   if( command ~= nil and command[2] <= account:getLevel() ) then
      value = command[1]()
      if( value ~= nil ) then
         account:msg( string.format( "%s", value ) )
      end
   else
      account:msg( "Bad command, try again.\r\n" )
   end
end

function onPrompt( account )
   local socket = Socket.get( account )

   local prompt
   return "<>"
end

account_command["hi"] = { function()
   global_account:msg( "Hi there.\r\n" ) 
   return "lets shorten this a bit\r\n"
end, 0 }
