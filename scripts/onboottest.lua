require( "scripts/libs/entity_lib" )

function onCall()
   local entity = Entity.get( 1 )
   local profile = var.new( "Profile", nil, entity )
   profile["name"] = "Davenge"
end
