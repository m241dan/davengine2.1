require( "scripts/libs/entity_lib" )

function onCall()
   local entity = Entity.get( 1 )
   local profile = var.get( "Profile",  entity )
   profile["name"] = "Davenge"
end
