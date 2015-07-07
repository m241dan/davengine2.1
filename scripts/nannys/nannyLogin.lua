function onNannyLoad( nanny )
end

function onInterp( socket, input )
   socket:msg( string.format( "This is your input:\r\n", input ) )
end

function onPrompt( socket )
   return ">"
end

function onStateChange( nanny )
   nanny:msg( "Helllloooo, login name? ", 0 )
end

