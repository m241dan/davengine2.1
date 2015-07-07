local login_msgs = { [0] = "Hello, what is your name?",
                     [1] = "What is your password?",
                     [2] = "Please pick a password for your new account.",
                     [3] = "Please confirm your password."
}

function onNannyLoad( nanny )
end

function onInterp( nanny, input )
end

function onPrompt( nanny )
   return nil
end

function onStateChange( nanny )
   nanny:msg( string.format( "%s\r\n", login_msgs[nanny:getState()] ) )
end

