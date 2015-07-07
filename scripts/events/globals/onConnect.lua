function onConnect( socket )
   local nanny = Nanny.new( "scripts/nannys/nannyLogin.lua" )
   local control_index = socket:control( nanny )
   socket:setState( control_index )
   nanny:setState( 0 )
end
