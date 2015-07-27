local create_msgs = { [0] = "Please input a name for your new character",
}

function onNannyLoad( nanny )
   var.new( "Info", "Login Char Create", nanny );
end

function onNannyFinish( nanny )
end

function onInterp( nanny, input )
   local socket = Socket.get( nanny )
   local state = nanny:getState()
   local info = var.get( "Info", nanny );
   local character

   if( state == 0 ) then
      if( Account.checkExists( input ) ~= nil ) then
         nanny:msg( "Invalid character name, an Account with that name already exists and we do not allow characters with the same name as accounts.\r\n" )
         nanny:setState( 0 )
      else
         info["Name"] = input
         nanny:setState( 1 )
      end
   end

end

function onPrompt( nanny )
   return nil
end

function onStateChange( nanny )
   nanny:msg( string.format( "%s\r\n", create_msgs[nanny:getState()] ) )
end

function checkPass( input )
   if( input:len() < 5 or input:len() > 20 ) then
      return false
   end
   return true
end
