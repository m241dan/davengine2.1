function onNannyLoad( nanny )
end

function onInterp( nanny, input )
end

function onPrompt( nanny )
   return ">"
end

function onStateChange( nanny )
   nanny:msg( "Helllloooo, login name? ", 0 )
end

