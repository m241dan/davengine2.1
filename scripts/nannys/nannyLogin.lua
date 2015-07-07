function onNannyLoad( nanny )
end

function onInterp( nanny, input )
end

function onPrompt( nanny )
   return ">"
end

function onStateChange( nanny )
   print( "Being called\n" )
   nanny:msg( "Helllloooo, login name?", 0 )
end

