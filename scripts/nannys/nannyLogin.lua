function onNannyLoad( nanny )
end

function onInterp( socket, input )
   socket:msg( string.format( "This is your input:\r\n%s", input ) )
end

function onPrompt( socket )
   return 1
end

function onStateChange( nanny )
   nanny:msg( "Helllloooo, login name? ", 0 )
end

