require( "scripts/libs/entity_lib" )
require( "scripts/libs/string_lib" )

local create_msgs = {
   [0] = "Please input a name for your new character",
}

function onNannyLoad( nanny )
   var.new( "Info", "Login Char Create", nanny )
end

function onNannyFinish( nanny )
   local socket = Socket.get( nanny )
   socket:prev()
end

function onInterp( nanny, input )
   local socket = Socket.get( nanny )
   local state = nanny:getState()
   local info = var.get( "Info", nanny );
   local account, character, profile, charlist

   if( state == 0 ) then
      if( Account.checkExists( input ) ~= nil ) then
         nanny:msg( "Invalid character name, an Account with that name already exists and we do not allow characters with the same name as accounts.\r\n" )
         nanny:setState( 0 )
         return;
      end
      -- create the entity
      character = Entity.new( "scripts/generics/entity.lua" )
      character:init()

      -- setup important mobile entity variables
      Entity.makePlayer( character )
      character:setType( ENTITY_MOBILE )
      profile = var.get( "profile", character )
      profile["name"] = capitalize( input )
      profile["short_descr"] = "A new player."
      profile["long_descr"] = "A new player is having a long description here."
      profile["description"] = "This entity is undescribable"

      -- retrieve account from nanny info and set this character to its characters table
      account = Account.get( info["account"] )
      charlist = var.get( "characters", account )
      charlist[profile["name"]] = character:getID()
      charlist[0] = charlist[0] + 1

      nanny:msg( "Character created.\r\n" )
      nanny:finish()
   end
end

function onPrompt( nanny )
   return nil
end

function onStateChange( nanny )
   nanny:msg( string.format( "%s\r\n", create_msgs[nanny:getState()] ) )
end
