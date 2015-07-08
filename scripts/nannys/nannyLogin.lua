local login_msgs = { [0] = "Hello, what is your name?",
                     [1] = "What is your password?",
                     [2] = "Please pick a password for your new account.",
                     [3] = "Please confirm your password."
}

function onNannyLoad( nanny )
   var.new( "Info", "Login Nanny", nanny );
end

function onNannyFinish( nanny )
end

function onInterp( nanny, input )
   local account
   local socket = Socket.get( nanny )
   local state = nanny:getState()
   local info = var.get( "Info", nanny );

   if( state == 0 ) then
      info["Name"] = input
      if( Account.get( input ) == nil ) then
         nanny:setState( 2 )
      else
         nanny:setState( 1 )
      end
   elseif( state == 1 ) then
      account = Account.get( info["Name"] )
      if( account:verifyPassword( input ) == true ) then
         socket:setState( socket:control( account ) )
         nanny:finish()
      else
         socket:msg( "Bad password, try again!\r\n" )
         nanny:setState( 1 )
      end
   elseif( state == 2 ) then
      if( checkPass( input ) ) then
         info["Pass"] = input
         nanny:setState( 3 )
      else
         nanny:msg( "Password is too long or too short.\r\n" )
         nanny:setState( 2 )
      end
   elseif( state == 3 ) then
      if( info["Pass"] == input ) then
         account = Account.new( info["Name"], info["Pass"] )
         if( account ~= nil ) then
            socket:setState( socket:control( account ) )
            nanny:finish()
         else
            nanny:msg( "Something has gone terribly wrong.\r\n" )
            nanny:finish()
            socket:setState( socket:control( Nanny.new( "scripts/nannys/nannyLogin.lua" ) ) )
         end
      else
         nanny:msg( "Passwords do not match, try again!!!\r\n" )
         nanny:setState( 2 )
      end
   end
end

function onPrompt( nanny )
   return nil
end

function onStateChange( nanny )
   nanny:msg( string.format( "%s\r\n", login_msgs[nanny:getState()] ) )
end

function checkPass( input )
   if( input:len() < 5 or input:len() > 20 ) then
      return false
   end
   return true
end
